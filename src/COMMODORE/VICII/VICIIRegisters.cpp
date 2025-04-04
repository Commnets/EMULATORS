#include <COMMODORE/VICII/VICIIRegisters.hpp>

// ---
COMMODORE::VICIIRegisters::VICIIRegisters (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: MCHEmul::ChipRegisters (_VICREGS_SUBSET, ps, pp, a, s),
	  _numberPositionsNextInstruction (0),
	  _backgroundColor (4, 0x00),
	  _spriteInfo (8, COMMODORE::VICIIRegisters::SpriteInfo ()),
	  _spriteSharedColor (2, 0x00),
	  _lastValueRead (MCHEmul::PhysicalStorage::_DEFAULTVALUE),
	  _raster (nullptr), // It is initialized later...
	  _interruptsEnabledBack (false)
	  // At this point the rest internal variables will have random values...
	  // The vector are initialized just to given them a default size!
{
	setClassName ("VICIIRegisters");

	initializeInternalValues ();
}

// ---
void COMMODORE::VICIIRegisters::initialize ()
{
	MCHEmul::PhysicalStorageSubset::initialize ();

	initializeInternalValues ();
}

// ---
MCHEmul::InfoStructure COMMODORE::VICIIRegisters::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::ChipRegisters::getInfoStructure ());

	result.add ("ACTIVE",			!_videoResetActive);
	result.add ("BLANK",			_blankEntireScreen);
	result.add ("MODE",				(unsigned int) _graphicModeActive);
	result.add ("40C",				_textDisplay40ColumnsActive);
	result.add ("25R",				_textDisplay25RowsActive);
	result.add ("SCROLLX",			_horizontalScrollPosition);
	result.add ("SCROLLY",			_verticalScrollPosition);
	result.add ("FORECOLOR",		_foregroundColor);
	result.add ("BKCOLOR1",			_backgroundColor [0]);
	result.add ("BKCOLOR2",			_backgroundColor [1]);
	result.add ("BKCOLOR3",			_backgroundColor [2]);
	result.add ("BKCOLOR4",			_backgroundColor [3]);
	result.add ("IRQ",				_rasterIRQActive);
	result.add ("IRQLINE",			_IRQRasterLineAt);
	result.add ("BANK",				_bank);
	result.add ("CHARADDRESS",		charDataMemory ());
	result.add ("SCREENADDRESS",	screenMemory ());
	result.add ("BITMAPADDRESS",	bitmapMemory ());
	result.add ("SPRITEADDRESS",	spritePointersMemory ());
	result.add ("LIGHTPENX",		_currentLightPenHorizontalPosition);
	result.add ("LIGHTPENY",		_currentLightPenVerticalPosition);

	// Info for the sprites...
	MCHEmul::InfoStructure sInfo;
	for (size_t i = 0; i < 8; i++)
	{ 
		MCHEmul::InfoStructure sInfoD = 
			std::move (_spriteInfo [i].getInfoStructure ()); 
		sInfoD.add ("ID", i);

		sInfo.add (std::to_string (i), std::move (sInfoD));
	}

	result.add ("SPRITES", std::move (sInfo));

	return (result);
}

// ---
void COMMODORE::VICIIRegisters::setValue (size_t p, const MCHEmul::UByte& v)
{
	size_t pp = p % 0x40;

	// The 64 first bytes will keep the right value...
	MCHEmul::PhysicalStorageSubset::setValue (pp, v);

	// Depending on the registerr the action will be one or another...
	switch (pp)
	{
		// SPnX: Sprite n Horizontal Position
		case 0x0:
		case 0x2:
		case 0x4:
		case 0x6:
		case 0x8:
		case 0xa:
		case 0xc:
		case 0xe:
			{
				_spriteInfo [pp >> 1]._spriteXCoord = ((unsigned short) v.value ()) | 
					(_spriteInfo [pp >> 1]._spriteXCoord & 0x0100);
			}

			break;

		// SPnY: Sprite n Vertical Position 
		case 0x1:
		case 0x3:
		case 0x5:
		case 0x7:
		case 0x9:
		case 0xb:
		case 0xd:
		case 0xf:
			{
				_spriteInfo [pp >> 1]._spriteYCoord = v.value ();
			}

			break;

		// MSIGX: Most significant bits of sprites Horizontal Positions
		case 0x10:
			{
				for (size_t i = 0; i < 8; i++)
					_spriteInfo [i]._spriteXCoord = (_spriteInfo [i]._spriteXCoord & 0x00ff) | 
						(v.bit (i) ? 0x0100 : 0x0000); 
			}

			break;

		// SCROLY: Vertical Fine Scrolling and Control Register
		case 0x11:
			{
				_verticalScrollPosition = v.value () & 0x07; // 3 LSBits
				if ((_textDisplay25RowsActive = v.bit (3))) { _minRasterV = 51; _maxRasterV = 251; }
				else { _minRasterV = 55; _maxRasterV = 247; }
				_blankEntireScreen = !v.bit (4);
				_graphicBitModeActive = v.bit (5);
				_graphicExtendedColorTextModeActive = v.bit (6);
				_IRQRasterLineAt = (_IRQRasterLineAt & 0x00ff) | (v.bit (7) ? 0x0100 : 0x0000); // The MSB of the raster position

				setGraphicModeActive ();
			}

			break;

		// RASTER: Write to compare with raster IRQ
		case 0x12:
			{
				_IRQRasterLineAt = (unsigned short) v.value () | (_IRQRasterLineAt & 0x0100);
			}

			break;

		// LPENX/LPENY: Light Pen Horizontal/Vertical Positions (it must be multiplied by 2 after read)
		case 0x13:
		case 0x14:
			/** Only makes sense when reading. */
			break;

		// SPENA: Sprite enable register
		case 0x15:
			{
				for (size_t i = 0; i < 8; i++)
					_spriteInfo [i]._spriteEnabled = v.bit (i);
			}

			break;
		
		// SCROLX: Horizontal Fine Scrolling and Control Register
		case 0x16:
			{
				_horizontalScrollPosition = v.value () & 0x07; // 3 LSBits
				_textDisplay40ColumnsActive = v.bit (3);
				_graphicMulticolorTextModeActive = v.bit (4);
				_videoResetActive = v.bit (5);
				/** bites 6 - 7 are not used. */

				setGraphicModeActive ();
			}

			break;

		// YXPAND Sprite Vertical Expansion Register
		case 0x17:
			{
				for (size_t i = 0; i < 8; i++)
					if (!(_spriteInfo [i]._spriteDoubleHeight = v.bit (i)))
						_expansionYFlipFlop [i] = true; // As long as YXEXPAND is cleared, the expansion flip flop is set...
					
			}

			break;
	
		// VMCSB: VICII Chip Memory Control Register
		case 0x18:
			{
				/* bits ----xxx- */
				_charDataMemory = MCHEmul::Address (2, // always 2 bytes long...
					((unsigned int) (v.value () & 0x0e)) << 10 /** multiply 1024 */);
				/* bits xxxx---- */
				_screenMemory = MCHEmul::Address (2,
					((unsigned int) (v.value () & 0xf0)) << 6 /** to make it simpler. Really >> 4 << 10 = multiply by 1024 */);
				/* bit ----x--- (also used above) */
				_bitmapMemory = MCHEmul::Address (2,
					((unsigned int) (v.value () & 0x08)) << 10 /** multiple by 1024. */);
				/** bit 0 is not used. */

				calculateMemoryPositions ();
			}

			break;

		// VICIRQ: VIC Interrrupt Flag Register
		// When setting it could be useful to clean up IRQ launched (if any)
		case 0x19:
			{
				// Just one bit to 1 is enough to reset all of them...
				if ((v.value () & 0x0f) != 0x00)
				{
					_rasterIRQHappened = false; // clean up the latches...
					_spriteCollisionWithDataIRQHappened = false;
					_spriteCollisionsIRQHappened = false;
					_lightPenIRQHappened = false;
				}

				/** bits from 4 to 7 are not used. */
				_interruptsEnabledBack = true;
			}

			break;
	
		// IRQMSK: IRQ Mask Register
		case 0x1a:
			{
				_rasterIRQActive = v.bit (0);
				_spriteCollisionWithDataIRQActive = v.bit (1);
				_spriteCollisionsIRQActive = v.bit (2);
				_lightPenIRQActive = v.bit (3);
				/** bites 4 - 6 are not used. */
				/** bit 7 is always "ora" of bites from 0 to 3. */
			}

			break;
	
		// SPBGPR: Sprite to Foreground Display Priority Register
		case 0x1b:
			{
				for (size_t i = 0; i < 8; i++)
					_spriteInfo [i]._spriteToForegroundPriority = v.bit (i);
			}

			break;

		// SPMC: Sprite Multicolor Registers
		case 0x1c:
			{
				for (size_t i = 0; i < 8; i++)
					_spriteInfo [i]._spriteMulticolor = v.bit (i);
			}

			break;
	
		// XXPAND: Sprite Horizontal Expansion Register
		case 0x1d:
			{
				for (size_t i = 0; i < 8; i++)
					_spriteInfo [i]._spriteDoubleWidth = v.bit (i);
			}

			break;
	
		// SPSPCL: Sprite to Sprite Collision Register
		case 0x1e:
			/* Only makes sense when reading. */
			break;
		
		// EXTCOLOR: Border Color Register
		case 0x20:
			{
				_foregroundColor = v.value () & 0x0f;
			}

			break;
	
		// BGCOL0, BGCOL1, BGCOL2, BGCOL3 : Background Color Registers
		case 0x21:
		case 0x22:
		case 0x23:
		case 0x24:
			{
				_backgroundColor [pp - 0x21] = v.value () & 0x0f;
			}

			break;

		// SPMCOL0, SPMCOL1: Sprite Multicolor Registers
		case 0x25:
		case 0x26:
			{
				_spriteSharedColor [pp - 0x25] = v.value () & 0x0f;
			}

			break;

		// SPnCOL: Sprite Color Registers
		case 0x27:
		case 0x28:
		case 0x29:
		case 0x2a:
		case 0x2b:
		case 0x2c:
		case 0x2d:
		case 0x2e:
			{
				_spriteInfo [pp - 0x27]._spriteColor = v.value () & 0x0f;
			}

			break;
	
		// Not connected
		// Ignored when writing...
		case 0x2f:
		case 0x30:
		case 0x31:
		case 0x32:
		case 0x33:
		case 0x34:
		case 0x35:
		case 0x36:
		case 0x37:
		case 0x38:
		case 0x39:
		case 0x3a:
		case 0x3b:
		case 0x3c:
		case 0x3d:
		case 0x3e:
		case 0x3f:
		default:
			break;
	}
}       

// ---
const MCHEmul::UByte& COMMODORE::VICIIRegisters::readValue (size_t p) const
{
	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	size_t pp = p % 0x40;

	switch (pp)
	{
		// The value is read as it is...
		case 0x0:
		case 0x1:
		case 0x2:
		case 0x3:
		case 0x4:
		case 0x5:
		case 0x6:
		case 0x7:
		case 0x8:
		case 0x9:
		case 0xa:
		case 0xb:
		case 0xc:
		case 0xd:
		case 0xe:
		case 0xf:
		case 0x10:
		case 0x15:
		case 0x17:
		case 0x18:
		case 0x1b:
		case 0x1c:
		case 0x1d:
			{
				result = MCHEmul::PhysicalStorageSubset::readValue (pp);
			}

			break;

		// SCROLY: Vertical Fine Scrolling and Control Register
		// Just to consider that when reading the raster MSB bit shows where the raster is now
		case 0x11:
			{
				bool oP = false;
				unsigned short rL = currentRasterLine ();
				_raster -> simulateMoveCycles (_numberPositionsNextInstruction, oP);
				if (oP) rL++;
				result = (MCHEmul::PhysicalStorageSubset::readValue (pp).value () & 0x7f) | 
					(((rL & 0xff00) != 0) ? 0x80 : 0x00);
			}

			break;

		// RASTER: When reading get the current raster postion (except the MSB that is in the previous)
		case 0x12:
			{
				// The real "read" happens in the very last cycle of the instruction
				// The same is with any other register but this one might be the very important one (and $d011)
				// So if is necessary to know whether the execution of the instruction will imply a 
				// change in the position of the raster or not!
				bool oP = false;
				unsigned short rL = currentRasterLine ();
				_raster -> simulateMoveCycles (_numberPositionsNextInstruction, oP); 
				if (oP) rL++;
				result = MCHEmul::UByte ((unsigned char) (rL & 0x00ff));
			}

			break;

		// LPENX: Light Pen Horizontal Position
		// The position is adjusted very two pixels...
		case 0x13:
			{
				result = _lightPenActive // Only when the lightpen is active...
					? MCHEmul::UByte (_currentLightPenHorizontalPosition) : 0;
			}

			break;

		// LPENY: Light Pen Vertical Position
		// The position is adjusted very two pixels...
		case 0x14:
			{
				result = _lightPenActive // only when the lightpen is active...
					? MCHEmul::UByte (_currentLightPenVerticalPosition) : 0;
			}

			break;
		
		// SCROLX: Horizontal Fine Scrolling and Control Register
		case 0x16:
			{
				/** bit 6 & 7 always on. */
				result = MCHEmul::UByte (MCHEmul::PhysicalStorageSubset::readValue (pp).value () & 0x3f | 0xc0);
			}

			break;

		// VICIRQ: VIC Interrrupt Flag Register
		// It is enought to set up a flag when the condition is reached...
		case 0x19:
			{
				result = MCHEmul::UByte::_FF; 
				result.setBit (0, _rasterIRQHappened);
				result.setBit (1, _spriteCollisionWithDataIRQHappened);
				result.setBit (2, _spriteCollisionsIRQHappened);
				result.setBit (3, _lightPenIRQHappened);
				/** bits 4, 5, and 6 are not used, and always to 1. */
				result.setBit (7, launchIRQ ());
				// Notice that this bit only sets when a interruption has really happened...
				// ...and not only when any of the conditions to launch it has been reched,
				// as it is the case of any of the previous bits.
			}

			break;
	
		// IRQMSK: IRQ Mask Register
		case 0x1a:
			{
				result = MCHEmul::UByte::_FF; 
				result.setBit (0, _rasterIRQActive);
				result.setBit (1, _spriteCollisionWithDataIRQActive);
				result.setBit (2, _spriteCollisionsIRQActive);
				result.setBit (3, _lightPenIRQActive);
				/** bites 4 - 7 are not used, and always to 1. */
			}

			break;
	
		// SPSPCL: Sprite - Sprite Collision Register
		case 0x1e:
			{
				result = MCHEmul::UByte::_0;
				for (size_t i = 0; i < 8; i++)
					result.setBit (i, _spriteCollisionHappened [i]);
				_spriteCollisionHappened = std::vector <bool> (8, false); // false back when reading...
			}

			break;

		// SPBGCL: Sprite - Data Collision Register
		case 0x1f:
			{
				result = MCHEmul::UByte::_0;
				for (size_t i = 0; i < 8; i++)
					result.setBit (i, _spriteCollisionWithDataHappened [i]);
				_spriteCollisionWithDataHappened = std::vector <bool> (8, false); // false back when reading...
			}

			break;

		case 0x20:
		case 0x21:
		case 0x22:
		case 0x23:
		case 0x24:
		case 0x25:
		case 0x26:
		case 0x27:
		case 0x28:
		case 0x29:
		case 0x2a:
		case 0x2b:
		case 0x2c:
		case 0x2d:
		case 0x2e:
			{
				/** The MSB to 1 always. */
				result = MCHEmul::UByte (MCHEmul::PhysicalStorageSubset::readValue (pp).value () & 0x0f | 0xf0);
			}

			break;
	
		// Not connected
		case 0x2f:
		case 0x30:
		case 0x31:
		case 0x32:
		case 0x33:
		case 0x34:
		case 0x35:
		case 0x36:
		case 0x37:
		case 0x38:
		case 0x39:
		case 0x3a:
		case 0x3b:
		case 0x3c:
		case 0x3d:
		case 0x3e:
		case 0x3f:
		default:
			{
				result = MCHEmul::UByte::_FF;
			}

			break;
	}

	return (_lastValueRead = result);
}

// ---
void COMMODORE::VICIIRegisters::initializeInternalValues ()
{
	// The internal variables are initialized direcly throught the data in the register...

	// _spriteXCoord = std::vector <unsigned short> (8, 0x0000);
	setValue (0x00, MCHEmul::UByte::_0);
	setValue (0x02, MCHEmul::UByte::_0);
	setValue (0x04, MCHEmul::UByte::_0);
	setValue (0x06, MCHEmul::UByte::_0);
	setValue (0x08, MCHEmul::UByte::_0);
	setValue (0x0a, MCHEmul::UByte::_0);
	setValue (0x0c, MCHEmul::UByte::_0);
	setValue (0x0e, MCHEmul::UByte::_0);
	// _spriteYCoord = std::vector <unsigned short> (8, 0x0000);
	setValue (0x01, MCHEmul::UByte::_0);
	setValue (0x03, MCHEmul::UByte::_0);
	setValue (0x05, MCHEmul::UByte::_0);
	setValue (0x07, MCHEmul::UByte::_0);
	setValue (0x09, MCHEmul::UByte::_0);
	setValue (0x0b, MCHEmul::UByte::_0);
	setValue (0x0d, MCHEmul::UByte::_0);
	setValue (0x0f, MCHEmul::UByte::_0);
	
	// MSIGX: Most significant bits of sprites Horizontal Positions
	setValue (0x10, MCHEmul::UByte::_0);

	// _verticalScrollPosition = 0x0000;
	// _textDisplay25RowsActive = true;
	// _blankEntireScreen = false;
	// _graphicBitModeActive = false;
	// _graphicExtendedColorTextModeActive = false;
	// _IRQRasterLineAt = 0x00;
	setValue (0x11, MCHEmul::UByte (0x18));

	// _IRQRasterLineAt = 0x00;
	setValue (0x12, MCHEmul::UByte::_0);

	// _spriteEnabled = std::vector <bool> (8, false);
	setValue (0x15, MCHEmul::UByte::_0);
		
	// _horizontalScrollPosition = 0x0000;
	// _textDisplay40ColumnsActive = true;
	// _graphicMulticolorTextModeActive = false;
	// _videoResetActive = false;
	setValue (0x16, MCHEmul::UByte (0x08));

	// _spriteDoubleHeight = std::vector <bool> (8, false);
	setValue (0x17, MCHEmul::UByte::_0);
	
	// _charDataMemory	= MCHEmul::Address ({ 0x00, 0x10 }, false);
	// _screenMemory	= MCHEmul::Address ({ 0x00, 0x04 }, false); 
	// _bitmapMemory	= MCHEmul::Address ({ 0x00, 0x00 }, false);
	// + bit 0 = 1...
	setValue (0x18, MCHEmul::UByte (0x15));

	// No IRQ have been launched!
	setValue (0x19, MCHEmul::UByte::_FF);

	// _rasterIRQActive = false;
	// _spriteCollisionWithDataIRQActive = false;
	// _spriteCollisionsIRQActive = false;
	// _lightPenIRQActive = false;
	setValue (0x1a, MCHEmul::UByte::_0);
	
	// _spriteToForegroundPriority = std::vector <bool> (8, false);
	setValue (0x1b, MCHEmul::UByte::_0);

	//_spriteMulticolor = std::vector <bool> (8, false);
	setValue (0x1c, MCHEmul::UByte::_0);
	
	// _spriteDoubleWidth = std::vector <bool> (8, false);
	setValue (0x1c, MCHEmul::UByte::_0);
		
	// _foregroundColor = 0x0000;
	setValue (0x20, MCHEmul::UByte::_0);
	
	// _backgroundColor = std::vector <unsigned short> (4, 0x0000);
	setValue (0x21, MCHEmul::UByte::_0);
	setValue (0x22, MCHEmul::UByte::_0);
	setValue (0x23, MCHEmul::UByte::_0);
	setValue (0x24, MCHEmul::UByte::_0);

	// _spriteSharedColor = std::vector <unsigned char> (2, 0x00);
	setValue (0x25, MCHEmul::UByte::_0);
	setValue (0x26, MCHEmul::UByte::_0);

	// _spriteColor = std::vector <unsigned char> (8, 0x00);
	setValue (0x27, MCHEmul::UByte::_0);
	setValue (0x28, MCHEmul::UByte::_0);
	setValue (0x29, MCHEmul::UByte::_0);
	setValue (0x2a, MCHEmul::UByte::_0);
	setValue (0x2b, MCHEmul::UByte::_0);
	setValue (0x2c, MCHEmul::UByte::_0);
	setValue (0x2d, MCHEmul::UByte::_0);
	setValue (0x2e, MCHEmul::UByte::_0);
	setValue (0x2e, MCHEmul::UByte::_0); // Duplicated, because buffer...

	// Managed direclty by the VICII Chip...
	_currentLightPenHorizontalPosition = _latchLighPenHorizontalPosition = 0x00;
	_currentLightPenVerticalPosition = _latchLightPenVerticalPosition = 0x00;
	_lightPenLatched = false; _lightPenActive = false;
	_spriteCollisionWithDataHappened = std::vector <bool> (8, false), 
	_spriteCollisionHappened = std::vector <bool> (8, false);
	// Reasons for IRQ
	_rasterIRQHappened = false;
	_spriteCollisionWithDataIRQHappened = false;
	_spriteCollisionsIRQHappened = false;
	_lightPenIRQHappened = false;

	// This variable will be set from CIA2 Registers...
	_bank = 0;

	// The status of the expansion Y flip flop is true...
	// Meaning that, initially drawing the sprite will happen every raster line...
	for (size_t i = 0; i < 8; _expansionYFlipFlop [i++] = true);

	_interruptsEnabledBack = false;
}

// ---
void COMMODORE::VICIIRegisters::setGraphicModeActive ()
{
	_graphicModeActive = COMMODORE::VICIIRegisters::GraphicMode::_INVALIDBITMAPMODE2; // All on...

	if (!graphicExtendedColorTextModeActive () && !graphicBitModeActive () && !graphicMulticolorTextModeActive ())
		_graphicModeActive = COMMODORE::VICIIRegisters::GraphicMode::_CHARMODE;
	else if (!graphicExtendedColorTextModeActive () && !graphicBitModeActive () && graphicMulticolorTextModeActive ())
		_graphicModeActive = COMMODORE::VICIIRegisters::GraphicMode::_MULTICOLORCHARMODE;
	else if (!graphicExtendedColorTextModeActive () && graphicBitModeActive () && !graphicMulticolorTextModeActive ())
		_graphicModeActive = COMMODORE::VICIIRegisters::GraphicMode::_BITMAPMODE;
	else if (!graphicExtendedColorTextModeActive () && graphicBitModeActive () && graphicMulticolorTextModeActive ())
		_graphicModeActive = COMMODORE::VICIIRegisters::GraphicMode::_MULTICOLORBITMAPMODE;
	else if (graphicExtendedColorTextModeActive () && !graphicBitModeActive () && !graphicMulticolorTextModeActive ())
		_graphicModeActive = COMMODORE::VICIIRegisters::GraphicMode::_EXTENDEDBACKGROUNDMODE;
	else if (graphicExtendedColorTextModeActive () && !graphicBitModeActive () && graphicMulticolorTextModeActive ())
		_graphicModeActive = COMMODORE::VICIIRegisters::GraphicMode::_INVALIDTEXMODE;
	else if (graphicExtendedColorTextModeActive () && graphicBitModeActive () && !graphicMulticolorTextModeActive ())
		_graphicModeActive = COMMODORE::VICIIRegisters::GraphicMode::_INVALIDBITMAPMODE1;

	_textMode = (_graphicModeActive == COMMODORE::VICIIRegisters::GraphicMode::_CHARMODE ||
				 _graphicModeActive == COMMODORE::VICIIRegisters::GraphicMode::_MULTICOLORCHARMODE ||
				 _graphicModeActive == COMMODORE::VICIIRegisters::GraphicMode::_EXTENDEDBACKGROUNDMODE ||
				 _graphicModeActive == COMMODORE::VICIIRegisters::GraphicMode::_INVALIDTEXMODE);
}

// ---
MCHEmul::InfoStructure COMMODORE::VICIIRegisters::SpriteInfo::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	result.add ("X",			_spriteXCoord);
	result.add ("Y",			_spriteYCoord);
	result.add ("COLOR",		_spriteColor);
	result.add ("MULTICOLOR",	_spriteMulticolor);
	result.add ("ENABLED",		_spriteEnabled);
	result.add ("DOUBLEW",		_spriteDoubleWidth);
	result.add ("DOUBLEH",		_spriteDoubleHeight);
	result.add ("PRIORITY",		!_spriteToForegroundPriority); // Priority over the background...

	return (result);
}
