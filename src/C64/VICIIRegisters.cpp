#include <C64/VICIIRegisters.hpp>

// ---
C64::VICIIRegisters::VICIIRegisters ()
	: MCHEmul::Memory (MCHEmul::Address ({ 0x00, 0xd0 }, false), 0x0400),
	  _borderColor (0x0000),
	  _backgroundColor (4, 0x0000),
	  _spriteXCoord (8, 0x0000),
	  _spriteYCoord (8, 0x0000),
	  _spriteColor (8, 0x0000),
	  _spriteSharedColor (2, 0x0000),
	  _spriteMulticolor (8, false),
	  _spriteEnabled (8, false),
	  _spriteDoubleWidth (8, false),
	  _spriteDoubleHeight (8, false),
	  _spriteToForegroundPriority (8, false),
	  _verticalScrollPosition (0x0000),
	  _horizontalScrollPosition (0x0000),
	  _textDisplay25RowsActive (true),
	  _textDisplay40ColumnsActive (true),
	  _screenSameColorBorderActive (false),
	  _videoResetActive (false),
	  _graphicBitModeActive (false),
	  _graphicExtendedColorTextModeActive (false),
	  _graphicMulticolorTextModeActive (false),
	  _graphicModeActive (C64::VICIIRegisters::GraphicMode::_CHARMODE),
	  _rasterIRQActive (false),
	  _spriteCollisionWithDataIRQActive (false),
	  _spriteCollisionsIRQActive (false),
	  _lightPenIRQActive (false),
	  _IRQRasterLineAt (0x00),
	  _charMemory (MCHEmul::Address ({ 0xd0, 0x00 })),
	  _screenMemory (MCHEmul::Address ({ 0x10, 0x00 })),
	  _bitmapMemory (MCHEmul::Address ({ 0xdd, 0x00 })),
	  // Used when reading and for VICII to kep internal values...
	  _currentRasterLine (0x0000), 
	  _currentLightPenHorizontalPosition (0x0000), 
	  _currentLightPenVerticalPosition (0x0000),
	  _rasterAtIRQLine (false), 
	  _spritesCollisionWithDataHappened (false), 
	  _spriteCollisionWithDataHappened (8, false), 
	  _spritesCollisionHappened (false), 
	  _spriteCollisionHappened (8, false), 
	  _lightPenOnScreenHappened (false),
	  _vicIItoGenerateIRQ (false)
{
	// Nothing else to do apart of the initialization...
}

// ---
void C64::VICIIRegisters::setValue (size_t p, const MCHEmul::UByte& v)
{
	MCHEmul::Memory::setValue (p, v);

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
			_spriteYCoord [pp >> 1] = (unsigned short) v.value ();
			break;

		// MSIGX: Most significant bits of sprites Horizontal Positions
		case 0x10:
			for (size_t i = 0; i < 8; i++)
				_spriteXCoord [i] = (_spriteXCoord [i] & 0x00ff) | (v.bit (i) ? 0x0100 : 0x0000); 
			break;

		// SCROLY: Vertical Fine Scrolling and Control Register
		case 0x11:
			_verticalScrollPosition = (unsigned short) (v.value () & 0x07); // 3 LSBits
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
			_horizontalScrollPosition = (unsigned short) (v.value () & 0x07); // 3 LSBits
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
			_charMemory = MCHEmul::Address (MCHEmul::UInt::fromUnsignedInt 
				(((unsigned int) (v.value () & 0x0e)) << 10 /** multiply 1024 */));
			/* bits xxxx---- */
			_screenMemory = MCHEmul::Address (MCHEmul::UInt::fromUnsignedInt 
				(((unsigned int) (v.value () & 0xf0)) << 6 /** multiply by 64. */));
			/* bit ----x--- (also used above) */
			_bitmapMemory = MCHEmul::Address (MCHEmul::UInt::fromUnsignedInt 
				(((unsigned int) (v.value () & 0x80)) << 10 /** multiple by 1024. */));
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
			_borderColor = (unsigned short) v.value ();
			break;
	
		// BGCOL0, BGCOL1, BGCOL2, BGCOL3 : Background Color Registers
		case 0x21:
		case 0x22:
		case 0x23:
		case 0x24:
			_backgroundColor [p - 0x21] = (unsigned short) v.value ();
			break;

		// SPMCOL0, SPMCOL1: Sprite Multicolor Registers
		case 0x25:
		case 0x26:
			_spriteSharedColor [p - 0x25] = (unsigned short) v.value ();
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
			_spriteColor [p - 0x27] = (unsigned short) v.value ();
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
MCHEmul::UByte C64::VICIIRegisters::readValue (size_t p) const
{
	MCHEmul::UByte result;

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
			result = MCHEmul::Memory::readValue (pp);
			break;

		// SCROLY: Vertical Fine Scrolling and Control Register
		// Just to consider that when reading the raster MSB bit shows where the raster is now
		case 0x11:
			result = MCHEmul::UByte 
				((MCHEmul::Memory::readValue (pp).value () & 0x7f) | ((_currentRasterLine & 0xff00) != 0) ? 0x80 : 0x00);
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
			result = MCHEmul::UByte (MCHEmul::Memory::readValue (pp).value () & 0x3f | 0xd0); /** bit 6 & 7 always on. */
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
			result = MCHEmul::UByte (MCHEmul::Memory::readValue (pp).value () & 0x0f | 0xf0);
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
			result = MCHEmul::UByte::_F;
			break;
	}

	return (result);
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
