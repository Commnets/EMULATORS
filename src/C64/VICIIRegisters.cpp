#include <C64/VICIIRegisters.hpp>
#include <C64/C64.hpp>

// ---
C64::VICIIRegisters::VICIIRegisters (int id, MCHEmul::PhysicalStorage* ps)
	: ChipRegisters (id, ps, 0xd000, MCHEmul::Address ({ 0x00, 0xd0 }, false), 0x0400),
	  _lastValueRead (MCHEmul::PhysicalStorage::_DEFAULTVALUE),
	  _backgroundColor (4, 0x00),
	  _spriteXCoord (8, 0x0000), _spriteYCoord (8, 0x0000),
	  _spriteColor (8, 0x0000),
	  _spriteSharedColor (2, 0x0000), _spriteMulticolor (8, false),
	  _spriteEnabled (8, false),
	  _spriteDoubleWidth (8, false), _spriteDoubleHeight (8, false),
	  _spriteToForegroundPriority (8, false)
	  // At this point the rest internal variables will have random values...
	  // The vector are initialized just to given them a default size!
{
	setClassName ("VICIIRegisters");

	initializeInternalValues ();
}


// ---
void C64::VICIIRegisters::initialize ()
{
	MCHEmul::PhysicalStorageSubset::initialize ();

	initializeInternalValues ();
}

// ---
void C64::VICIIRegisters::setValue (size_t p, const MCHEmul::UByte& v)
{
	MCHEmul::PhysicalStorageSubset::setValue (p, v);

	size_t pp = p % 0x40;

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
			_spriteXCoord [pp >> 1] = ((unsigned short) v.value ()) | (_spriteXCoord [pp >> 1] & 0x0100);
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
			_spriteYCoord [pp >> 1] = v.value ();
			break;

		// MSIGX: Most significant bits of sprites Horizontal Positions
		case 0x10:
			for (size_t i = 0; i < 8; i++)
				_spriteXCoord [i] = (_spriteXCoord [i] & 0x00ff) | (v.bit (i) ? 0x0100 : 0x0000); 
			break;

		// SCROLY: Vertical Fine Scrolling and Control Register
		case 0x11:
			_verticalScrollPosition = v.value () & 0x07; // 3 LSBits
			_textDisplay25RowsActive = v.bit (3);
			_screenSameColorBorderActive = v.bit (4);
			_graphicBitModeActive = v.bit (5);
			_graphicExtendedColorTextModeActive = v.bit (6);
			_IRQRasterLineAt = (_IRQRasterLineAt & 0x00ff) | (v.bit (7) ? 0x0100 : 0x0000); // The MSB of the raster position
			setGraphicModeActive ();
			break;

		// RASTER: Write to compare with raster IRQ
		case 0x12:
			_IRQRasterLineAt = (unsigned short) v.value () | (_IRQRasterLineAt & 0x0100);
			break;

		// LPENX/LPENY: Light Pen Horizontal/Vertical Positions (it must be multiplied by 2 after read)
		case 0x13:
		case 0x14:
			/** Only makes sense when reading. */
			break;

		// SPENA: Sprite enable register
		case 0x15:
			for (size_t i = 0; i < 7; i++)
				_spriteEnabled [i] = v.bit (i);
			break;
		
		// SCROLX: Horizontal Fine Scrolling and Control Register
		case 0x16:
			_horizontalScrollPosition = v.value () & 0x07; // 3 LSBits
			_textDisplay40ColumnsActive = v.bit (3);
			_graphicMulticolorTextModeActive = v.bit (4);
			_videoResetActive = v.bit (5);
			/** bites 6 - 7 are not used. */
			setGraphicModeActive ();
			break;

		// YXPAND Sprite Vertical Expansion Register
		case 0x17:
			for (size_t i = 0; i < 7; i++)
				_spriteDoubleHeight [i] = v.bit (i);
			break;
	
		// VMCSB: VICII Chip Memory Control Register
		case 0x18:
			/* bits ----xxx- */
			_charDataMemory = MCHEmul::Address (MCHEmul::UInt::fromUnsignedInt 
				(((unsigned int) (v.value () & 0x0e)) << 10 /** multiply 1024 */));
			/* bits xxxx---- */
			_screenMemory = MCHEmul::Address (MCHEmul::UInt::fromUnsignedInt 
				(((unsigned int) (v.value () & 0xf0)) << 6 /** multiply by 64 = /16*1024 */));
			/* bit ----x--- (also used above) */
			_bitmapMemory = MCHEmul::Address (MCHEmul::UInt::fromUnsignedInt 
				(((unsigned int) (v.value () & 0x08)) << 10 /** multiple by 1024. */));
			/** bit 0 is not used. */
			break;

		// VICIRQ: VIC Interrrupt Flag Register
		case 0x19:
			/** Only make sense when reading. */
			break;
	
		// IRQMSK: IRQ Mask Register
		case 0x1a:
			_rasterIRQActive = v.bit (0);
			_spriteCollisionWithDataIRQActive = v.bit (1);
			_spriteCollisionsIRQActive = v.bit (2);
			_lightPenIRQActive = v.bit (3);
			/** bites 4 - 7 are not used. */
			break;
	
		// SPBGPR: Sprite to Foreground Display Priority Register
		case 0x1b:
			for (size_t i = 0; i < 8; i++)
				_spriteToForegroundPriority [i] = v.bit (i);
			break;

		// SPMC: Sprite Multicolor Registers
		case 0x1c:
			for (size_t i = 0; i < 8; i++)
				_spriteMulticolor [i] = v.bit (i);
			break;
	
		// XXPAND: Sprite Horizontal Expansion Register
		case 0x1d:
			for (size_t i = 0; i < 8; i++)
				_spriteDoubleWidth [i] = v.bit (i);
			break;
	
		// SPSPCL: Sprite to Sprite Collision Register
		case 0x1e:
			/* Only makes sense when reading. */
			break;
		
		// EXTCOLOR: Border Color Register
		case 0x20:
			_borderColor = v.value ();
			break;
	
		// BGCOL0, BGCOL1, BGCOL2, BGCOL3 : Background Color Registers
		case 0x21:
		case 0x22:
		case 0x23:
		case 0x24:
			_backgroundColor [pp - 0x21] = v.value ();
			break;

		// SPMCOL0, SPMCOL1: Sprite Multicolor Registers
		case 0x25:
		case 0x26:
			_spriteSharedColor [pp - 0x25] = v.value ();
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
			_spriteColor [pp - 0x27] = v.value ();
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
const MCHEmul::UByte& C64::VICIIRegisters::readValue (size_t p) const
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
			result = MCHEmul::PhysicalStorageSubset::readValue (pp);
			break;

		// SCROLY: Vertical Fine Scrolling and Control Register
		// Just to consider that when reading the raster MSB bit shows where the raster is now
		case 0x11:
			result = MCHEmul::UByte 
				((MCHEmul::PhysicalStorageSubset::readValue (pp).value () & 0x7f) | 
				 (((_currentRasterLine & 0xff00) != 0) ? 0x80 : 0x00));
			break;

		// RASTER: When reading get the current raster postion (except the MSB that is in the previous)
		case 0x12:
			result = MCHEmul::UByte ((unsigned char) _currentRasterLine & 0x00ff);
			break;

		// LPENX: Light Pen Horizontal Position
		case 0x13:
			result = MCHEmul::UByte ((unsigned char) _currentLightPenHorizontalPosition);
			break;

		// LPENY: Light Pen Vertical Position
		case 0x14:
			result = MCHEmul::UByte ((unsigned char) _currentLightPenVerticalPosition);
			break;
		
		// SCROLX: Horizontal Fine Scrolling and Control Register
		case 0x16:
			result = MCHEmul::UByte (MCHEmul::PhysicalStorageSubset::readValue (pp).value () & 0x3f | 0xd0); /** bit 6 & 7 always on. */
			break;

		// VICIRQ: VIC Interrrupt Flag Register
		case 0x19:
			result = MCHEmul::UByte::_1; 
			result.setBit (0, _rasterAtIRQLine);
			result.setBit (1, _spritesCollisionWithDataHappened);
			result.setBit (2, _spritesCollisionHappened);
			result.setBit (3, _lightPenOnScreenHappened);
			/** bits 4, 5, and 6 are not used, and always to 1. */
			result.setBit (7, _vicIItoGenerateIRQ);
			break;
	
		// IRQMSK: IRQ Mask Register
		case 0x1a:
			result = MCHEmul::UByte::_1; 
			result.setBit (0, _rasterIRQActive);
			result.setBit (1, _spriteCollisionWithDataIRQActive);
			result.setBit (2, _spriteCollisionsIRQActive);
			result.setBit (3, _lightPenIRQActive);
			/** bites 4 - 7 are not used, and always to 1. */
			break;
	
		// SPSPCL: Sprite - Sprite Collision Register
		case 0x1e:
			result = MCHEmul::UByte::_0;
			for (size_t i = 0; i < 8; i++)
				result.setBit (i, _spriteCollisionHappened [i]);
			_spriteCollisionHappened = std::vector <bool> (8, false); // false back when reading...
			break;

		// SPBGCL: Sprite - Data Collision Register
		case 0x1f:
			result = MCHEmul::UByte::_0;
			for (size_t i = 0; i < 8; i++)
				result.setBit (i, _spriteCollisionWithDataHappened [i]);
			_spriteCollisionWithDataHappened = std::vector <bool> (8, false); // false back when reading...
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
			/** The MSB to 1 always. */
			result = MCHEmul::UByte (MCHEmul::PhysicalStorageSubset::readValue (pp).value () & 0x0f | 0xf0);
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
			result = MCHEmul::UByte::_FF;
			break;
	}

	return (_lastValueRead = result);
}

// ---
void C64::VICIIRegisters::initializeInternalValues ()
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
	// _screenSameColorBorderActive = false;
	// _graphicBitModeActive = false;
	// _graphicExtendedColorTextModeActive = false;
	// _IRQRasterLineAt = 0x00;
	setValue (0x11, MCHEmul::UByte (0x08));

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
	setValue (0x18, MCHEmul::UByte (0x14));

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
		
	// _borderColor = 0x0000;
	setValue (0x20, MCHEmul::UByte::_0);
	
	// _backgroundColor = std::vector <unsigned short> (4, 0x0000);
	setValue (0x21, MCHEmul::UByte::_0);
	setValue (0x22, MCHEmul::UByte::_0);
	setValue (0x23, MCHEmul::UByte::_0);
	setValue (0x24, MCHEmul::UByte::_0);

	// _spriteSharedColor = std::vector <unsigned short> (2, 0x0000);
	setValue (0x25, MCHEmul::UByte::_0);
	setValue (0x26, MCHEmul::UByte::_0);

	// _spriteColor = std::vector <unsigned short> (8, 0x0000);
	setValue (0x27, MCHEmul::UByte::_0);
	setValue (0x28, MCHEmul::UByte::_0);
	setValue (0x29, MCHEmul::UByte::_0);
	setValue (0x2a, MCHEmul::UByte::_0);
	setValue (0x2b, MCHEmul::UByte::_0);
	setValue (0x2c, MCHEmul::UByte::_0);
	setValue (0x2d, MCHEmul::UByte::_0);
	setValue (0x2e, MCHEmul::UByte::_0);

	// Managed direclty by the VICII Chip...
	_currentRasterLine = 0x0000;
	_currentLightPenHorizontalPosition = 0x0000;
	_currentLightPenVerticalPosition = 0x0000;
	_rasterAtIRQLine = false;
	_spritesCollisionWithDataHappened = false;
	_spriteCollisionWithDataHappened = std::vector <bool> (8, false), 
	_spritesCollisionHappened = false;
	_spriteCollisionHappened = std::vector <bool> (8, false);
	_lightPenOnScreenHappened = false;
	_vicIItoGenerateIRQ = false;

	// This variable will be set from CIAII simulation method...
	_bank = 0;
}

// ---
void C64::VICIIRegisters::setGraphicModeActive ()
{
	_graphicModeActive = C64::VICIIRegisters::GraphicMode::_ILLEGALMODE;

	if (!graphicExtendedColorTextModeActive () && !graphicBitModeActive () && !graphicMulticolorTextModeActive ())
		_graphicModeActive = C64::VICIIRegisters::GraphicMode::_CHARMODE;
	else if (!graphicExtendedColorTextModeActive () && !graphicBitModeActive () && graphicMulticolorTextModeActive ())
		_graphicModeActive = C64::VICIIRegisters::GraphicMode::_MULTICOLORCHARMODE;
	else if (!graphicExtendedColorTextModeActive () && graphicBitModeActive () && !graphicMulticolorTextModeActive ())
		_graphicModeActive = C64::VICIIRegisters::GraphicMode::_BITMAPMODE;
	else if (!graphicExtendedColorTextModeActive () && graphicBitModeActive () && graphicMulticolorTextModeActive ())
		_graphicModeActive = C64::VICIIRegisters::GraphicMode::_MULTICOLORBITMAPMODE;
	else if (graphicExtendedColorTextModeActive () && !graphicBitModeActive () && !graphicMulticolorTextModeActive ())
		_graphicModeActive = C64::VICIIRegisters::GraphicMode::_EXTENDEDBACKGROUNDMODE;
}
