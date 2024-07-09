#include <COMMODORE/TED/TEDRegisters.hpp>
#include <COMMODORE/TED/TEDTimer.hpp>
#include <COMMODORE/C6529B/C6529B.hpp>

// ---
MCHEmul::InfoStructure COMMODORE::TEDRegisters::Color::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	result.add ("COLOR",		_color);
	result.add ("LUMINANCE",	_luminance);

	return (result);
}

// ---
MCHEmul::InfoStructure COMMODORE::TEDRegisters::GraphicalInfo::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	result.add ("RASTERY",		_currentRasterLine);
	result.add ("RASTERX",		_currentRasterColumn);
	result.add ("CHARPOS",		_currentCharacterPosition);
	result.add ("FLASHCOUNTER",	_flashCounter);

	return (result);
}

// ---
COMMODORE::TEDRegisters::TEDRegisters (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: MCHEmul::ChipRegisters (_TEDREGS_SUBSET, ps, pp, a, s),
	  _lastValueRead (MCHEmul::PhysicalStorage::_DEFAULTVALUE),
	  _backgroundColor (4, { 0x00, 0x00 })
	  // At this point the rest internal variables will have random values...
	  // The vector are initialized just to given them a default size!
{
	setClassName ("TEDRegisters");

	initializeInternalValues ();
}

// ---
void COMMODORE::TEDRegisters::initialize ()
{
	MCHEmul::PhysicalStorageSubset::initialize ();

	initializeInternalValues ();
}

// ---
MCHEmul::InfoStructure COMMODORE::TEDRegisters::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::ChipRegisters::getInfoStructure ());

	result.add ("MODE",				(unsigned int) _graphicModeActive);
	result.add ("40C",				_textDisplay40ColumnsActive);
	result.add ("25R",				_textDisplay25RowsActive);
	result.add ("SCROLLX",			_horizontalScrollPosition);
	result.add ("SCROLLY",			_verticalScrollPosition);
	result.add ("BkColor1",			std::move (_backgroundColor [0].getInfoStructure ()));
	result.add ("BkColor2",			std::move (_backgroundColor [1].getInfoStructure ()));
	result.add ("BkColor3",			std::move (_backgroundColor [2].getInfoStructure ()));
	result.add ("BkCOlor4",			std::move (_backgroundColor [3].getInfoStructure ()));
	result.add ("BorderColor",		std::move (_borderColor.getInfoStructure ()));
	result.add ("IRQ",				_rasterIRQActive);
	result.add ("IRQLINE",			_IRQRasterLineAt);
	result.add ("CHARADDRESS",		charDataMemory ());
	result.add ("SCREENADDRESS",	screenMemory ());
	result.add ("BITMAPADDRESS",	bitmapMemory ());
	result.add ("GraphicalInfo",	std::move (_graphicalInfo.getInfoStructure ()));

	return (result);
}

// ---
void COMMODORE::TEDRegisters::setValue (size_t p, const MCHEmul::UByte& v)
{
	if (_soundWrapper == nullptr ||
		_T1 == nullptr ||
		_T2 == nullptr)
		return;

	size_t pp = p % 0x20;

	// The 64 first bytes will keep the right value...
	MCHEmul::PhysicalStorageSubset::setValue (p, v);

	switch (pp)
	{
		// Timer 1 Low Byte
		case 0x00:
			{
				_T1 -> setInitialValue ((_T1 -> initialValue () & 0xff00) | v.value ());

				// Stop counting until the hight byte is loaded...
				_T1 -> stop ();
			}

		// Timer High Byte
		case 0x01:
			{
				_T1 -> setInitialValue ((_T1 -> initialValue () & 0x00ff) | (v.value () << 8));

				// Once the hight byte is loaded, the timer starts to count...
				_T1 -> start ();
			}

			break;

		// Timer 2 Low Byte
		case 0x02:
			{
				_T2 -> setInitialValue ((_T1 -> initialValue () & 0xff00) | v.value ());
			}

			break;

		// Timer 2 High Byte
		case 0x03:
			{
				_T2 -> setInitialValue ((_T1 -> initialValue () & 0x00ff) | (v.value () << 8));
			}

			break;

		// Timer 3 Low Byte
		case 0x04:
			{
				_T3 -> setInitialValue ((_T1 -> initialValue () & 0xff00) | v.value ());
			}

			break;

		// Timer 3 High Byte
		case 0x05:
			{
				_T3 -> setInitialValue ((_T1 -> initialValue () & 0x00ff) | (v.value () << 8));
			}

			break;

		// Vertical scroll and size, and graphic modes...
		case 0x06:
			{
				_verticalScrollPosition = v.value () & 0x07; // 3 LSBits
				_textDisplay25RowsActive = v.bit (3);
				_blankEntireScreen = !v.bit (4); // When happens the CPU clocks moves twice the speed...
				_graphicBitModeActive = v.bit (5);
				_graphicExtendedColorTextModeActive = v.bit (6);

				// Pending to implement bit 7...

				setGraphicModeActive ();
			}
			
			break;

		// Horizontal scroll and size, and mode graphic modes...
		case 0x07:
			{
				_horizontalScrollPosition = v.value () & 0x07; // 3 LSBits
				_textDisplay40ColumnsActive = v.bit (3);
				_graphicMulticolorTextModeActive = v.bit (4);
				_freezeActive = v.bit (5);
				_PALActive = !v.bit (6);
				_reverseVideoActive = !v.bit (7);

				setGraphicModeActive ();
			}

			break;

		// Scan Keyboard... 
		// When writting the value at the keyboard pins is latched...
		case 0x08:
			{
				_keyboardLatch = _keyboardPins;
			}

			break;

		// IRQ Status Register...
		case 0x09:
			{
				if (v.bit (1)) _rasterIRQHappened = false; // clean up the latches...
				if (v.bit (2)) _lightPenIRQHappened = false;
				if (v.bit (3)) _T1 -> interruptRequested (); // clen up the lacth...
				if (v.bit (4)) _T2 -> interruptRequested ();
				if (v.bit (6)) _T3 -> interruptRequested ();
				/** bits 0, 5, and 7 are not used. */
			}

			break;

		// IRQ Mask Register
		case 0x0a:
			{
				_IRQRasterLineAt = (_IRQRasterLineAt & 0x00ff) | 
					(v.bit (0) ? 0x0100 : 0x0000); // The MSB of the raster position...
				_rasterIRQActive = v.bit (1);
				_lightPenIRQActive = v.bit (2);
				_T1 -> setInterruptEnabled (v.bit (3));
				_T2 -> setInterruptEnabled (v.bit (4));
				_T3 -> setInterruptEnabled (v.bit (6));
				/** bits 5 and 7 are not used. */
			}

			break;

		// LSB Raster Position
		case 0x0b:
			{
				_IRQRasterLineAt = ((unsigned short) v.value ()) | (_IRQRasterLineAt & 0x0100);
			}

			break;

		// MSB Cursor Position
		case 0x0c:
			{
				// Only the bits 0 and 1 are used...
				_cursorPosition = ((unsigned short) ((v.value () & 0x03) << 8)) | (_cursorPosition & 0x00ff);
			}

			break;

		// LSB Cursor Position
		case 0x0d:
			{
				_cursorPosition = (_cursorPosition & 0xff00) | ((unsigned short) v.value ());
			}

			break;

		// Sound Wrapper Registers
		case 0x0e:
		case 0x0f:
		case 0x10:
		case 0x11:
			{
				_soundWrapper -> setValue (pp, v);
			}

			break;

		// Bit Map Address...
		// ROM as source for reading the character info?
		// But also the MSB of the Voice 1 Frequency
		case 0x12:
			{
				// The bits 0 y 1 are MSB of the Voice 1 Frequency...
				_soundWrapper -> setValue (pp, v);

				_ROMSourceActive = v.bit (2);
				_bitmapMemory = MCHEmul::Address (2, 
					(unsigned int) ((v.value () & 0x38 /** bits 3, 4 & 5. */) << 10
						/** 3 + 10 to become the bits 13, 14 & 15 of the address. */));
				/** bits 6 & 7 are not used. */
			}

			break;

		// Char Data Address
		// Also whether the clock mode is twice or single...
		case 0x13:
			{
				/** The bit 0 is reading. */
				_singleClockModeActive = v.bit (1);
				_charDataMemory = MCHEmul::Address (2, 
					(unsigned int) ((v.value () & 0xfc /** bits 2 - 7. */) << 8 
						/** 2 + 8 to become the bits 10 - 15 of the address. */));
			}

			break;

		// Screen Memory Address
		case 0x14:
			{
				/** Bits 0, 1 & 2 are not used. */
				_screenMemory = MCHEmul::Address (2,
					(unsigned int) ((v.value () & 0xf8 /** bits 3 - 7. */) << 8 
						/** 3 + 8 to become the bits 11 - 15 of the address. */) | 0x400 /** The bit 10 is always set. */);
				_attributeMemory = MCHEmul::Address (2,
					(unsigned int) ((v.value () & 0xf8 /** bits 3 - 7. */) << 8 
						/** 3 + 8 to become the bits 11 - 15 of the address. */));
				// The difference between the _screenMemory location and the _attributeMemory location
				// is just the bit 10 is set in _screeMemory and off in _attributeMemory...
			}

			break;

		// Background Color
		case 0x15:
			{
				_backgroundColor [0] = COMMODORE::TEDRegisters::Color 
					(v.value () & 0x0f, (v.value () & 0x70) >> 4);
				/** bit 7 is not used. */
			}

			break;

		// MultiColor/Extended Color 1
		case 0x16:
			{
				_backgroundColor [1] = COMMODORE::TEDRegisters::Color 
					(v.value () & 0x0f, (v.value () & 0x70) >> 4);
				/** bit 7 is not used. */
			}

			break;

		// MultiColor/Extended Color 2
		case 0x17:
			{
				_backgroundColor [2] = COMMODORE::TEDRegisters::Color 
					(v.value () & 0x0f, (v.value () & 0x70) >> 4);
				/** bit 7 is not used. */
			}

			break;

		// Extended Color 3
		case 0x18:
			{
				_backgroundColor [3] = COMMODORE::TEDRegisters::Color
					(v.value () & 0x0f, (v.value () & 0x70) >> 4);
				/** bit 7 is not used. */
			}

			break;

		// Border Color
		case 0x19:
			{
				_borderColor = COMMODORE::TEDRegisters::Color 
					(v.value () & 0x0f, (v.value () & 0x70) >> 4);
				/** bit 7 is not used. */
			}

			break;

		// Take care when writting to these registers...
		// They control how the screen is drawn, 
		// so flicking effects can happen when writting
		// Current Character Position MSB
		case 0x1a:
			{
				_graphicalInfo._currentCharacterPosition = 
					(_graphicalInfo._currentCharacterPosition & 0x00ff) | ((v.value () & 0x03) /** bits 0 & 1. */ << 8);
			}

			break;

		// Current Charecter Position LSB
		case 0x1b:
			{
				_graphicalInfo._currentCharacterPosition =
					(_graphicalInfo._currentCharacterPosition & 0xff00) | v.value ();
			}

			break;

		// Vertical Raster Position MSB
		case 0x1c:
			{
				_graphicalInfo._currentRasterLine = 
					(_graphicalInfo._currentRasterLine & 0x00ff) | ((v.value () & 0x01) /** bits 0. */ << 8);
			}

			break;

		// Vertical Raster Position LSB
		case 0x1d:
			{
				_graphicalInfo._currentRasterLine =
					(_graphicalInfo._currentRasterLine & 0xff00) | v.value ();
			}

			break;

		// Horizontal Raster Position MSB
			// But only the 8 most significant bits...
		case 0x1e:
			{
				_graphicalInfo._currentRasterColumn = 
					_graphicalInfo._currentRasterColumn | ((v.value () & 0xfe) /** bit 0 is not used. */ << 1);
			}

			break;

		// Control of the character scanning row...
		// ...and the status of the counter...
		case 0x1f:
			{
				_graphicalInfo._currentCharacterRow = v.value () & 0x07;
				_graphicalInfo._flashCounter = (v.value () & 0b01111000) >> 3;
			}

			break;

		default:
			// It should be here anyway
			assert (false);
			break;
	}
}       

// ---
const MCHEmul::UByte& COMMODORE::TEDRegisters::readValue (size_t p) const
{
	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	if (_soundWrapper == nullptr ||
		_T1 == nullptr ||
		_T2 == nullptr ||
		_T3 == nullptr)
		return (_lastValueRead = result);

	size_t pp = p % 0x20;

	switch (pp)
	{
		case 0x00:
			{
				result = (unsigned char) (_T1 -> currentValue () & 0x00ff);
			}

			break;

		case 0x01:
			{
				result = (unsigned char) ((_T1 -> currentValue () & 0xff00) >> 8);
			}

			break;

		case 0x02:
			{
				result = (unsigned char) (_T2 -> currentValue () & 0x00ff);
			}

			break;

		case 0x03:
			{
				result = (unsigned char) ((_T2 -> currentValue () & 0xff00) >> 8);
			}

			break;

		case 0x04:
			{
				result = (unsigned char) (_T3 -> currentValue () & 0x00ff);
			}

			break;

		case 0x05:
			{
				result = (unsigned char) ((_T3 -> currentValue () & 0xff00) >> 8);
			}

			break;

		case 0x06:
			{
				result = MCHEmul::UByte::_FF;
				result |= (MCHEmul::PhysicalStorageSubset::readValue (pp).value () & 0x7f);

				// Pending to implement bit 7...
			}

			break;

		case 0x07:
			{
				result = MCHEmul::PhysicalStorageSubset::readValue (pp);
			}

		// Read the value latched in TED
		// after executing setValue (0x08)...
		case 0x08:
			{
				result = _keyboardLatch;
			}

			break;

		case 0x09:
			{
				result = MCHEmul::UByte::_FF; 
				/** bit 0 not used, and always to 1. */
				result.setBit (1, _rasterIRQHappened);
				/** There is no lightpen in TED systems, 
					however it is left for future expansions. */
				result.setBit (2, _lightPenIRQHappened);
				result.setBit (3, _T1 -> peekInterruptRequested ());
				result.setBit (4, _T2 -> peekInterruptRequested ());
				/** bit 5 not used, and always to 1. */
				result.setBit (6, _T3 -> peekInterruptRequested ());
				result.setBit (7, launchIRQ ());
				// Notice that this bit only sets when a interruption has really happened...
				// ...and not only when any of the conditions to launch it has been reched,
				// as it is the case of any of the previous bits.
			}

			break;

		case 0x0a:
			{
				result = MCHEmul::UByte::_FF; // The rest of the bits are up!
				result.setBit (0, (_graphicalInfo._currentRasterLine & 0xff00) != 0); // The MSB of the raster position...
				result.setBit (1, _rasterIRQActive);
				result.setBit (2, _lightPenIRQActive);
				result.setBit (3, _T1 -> interruptEnabled ());
				result.setBit (4, _T2 -> interruptEnabled ());
				result.setBit (6, _T3 -> interruptEnabled ());
				/** bits 5 & 7 are not used. */
			}

			break;

		case 0x0b:
			{
				result = (unsigned char) (_graphicalInfo._currentRasterLine & 0x00ff);
			}

			break;

		case 0x0c:
			{
				result = ((unsigned char) ((_cursorPosition & 0xff00) >> 8)) | 0xfc;
			}

		case 0x0d:
			{
				result = (unsigned char) (_cursorPosition & 0x00ff);
			}

			break;

		// Sound Wrapper Registers
		case 0x0e:
		case 0x0f:
		case 0x10:
		case 0x11:
			{
				result = _soundWrapper -> readValue (pp);
			}

			break;

		case 0x12:
			{
				result = ((unsigned char) ((_bitmapMemory.value () & 0xff00) >> 10
					/** to become the bits 3, 4 & 5 of the address. */)) | ~0b00111000; // The rest of the bits to 1 initially...
				result.setBit (2, _ROMSourceActive);
			}

			break;

		case 0x13:
			{
				result = ((unsigned char) ((_charDataMemory.value () & 0xff00) >> 8 /** The become the bits from 7 to 2. */)) 
					| ~0b11111100; // The rest of the bits to 1 initially...
				result.setBit (2, _singleClockModeActive);
			}

			break;

		case 0x14:
			{
				result = ((unsigned char) ((_screenMemory.value () & 0xff00) >> 8 /** The become the bits from 7 to 3. */)) 
					| ~0b11111000; // The rest of the bits to 1....
			}

			break;

		case 0x15:
		case 0x16:
		case 0x17:
		case 0x18:
		case 0x19:
			{
				result = MCHEmul::PhysicalStorageSubset::readValue (pp);
				result.setBit (7, true); // The non used bits always to 1... (the luminance only takes 3 bits)
			}

			break;
		
		case 0x1a:
			{
				result = ((unsigned char) ((_graphicalInfo._currentCharacterPosition & 0xff00) >> 8)) 
					| ~0b00000001; // The bits not used must be to 1...
			}

			break;

		case 0x1b:
			{
				result = (unsigned char) (_graphicalInfo._currentCharacterPosition & 0x00ff);
			}

			break;

		case 0x1c:
			{
				result = ((unsigned char) ((_graphicalInfo._currentRasterLine & 0xff00) >> 8)) 
					| ~0b00000001; // The bits not used are 1...
			}

			break;

		case 0x1d:
			{
				result = (unsigned char) (_graphicalInfo._currentRasterLine & 0x00ff);
			}

			break;

		case 0x1e:
			{
				result = ((unsigned char) ((_graphicalInfo._currentRasterColumn >> 1) & 0x00ff)) 
					| ~0b11111110; // The bits not used are set to 1...
			}

			break;

		case 0x1f:
			{
				result = ((_graphicalInfo._currentCharacterRow & 0x0f) |
						 ((_graphicalInfo._flashCounter & 0x0f) << 3)) | ~0b01111111; // The bit 7 to 1!
			}

			break;

		// It shouldn't be here...
		default:
			assert (false);
			break;
	}

	return (_lastValueRead = result);
}

// ---
void COMMODORE::TEDRegisters::initializeInternalValues ()
{
	if (_soundWrapper == nullptr ||
		_T1 == nullptr ||
		_T2 == nullptr ||
		_T3 == nullptr)
		return; // Not time still to initialize anything...

	setValue (0x00, MCHEmul::UByte::_0);
	setValue (0x01, MCHEmul::UByte::_0);
	setValue (0x02, MCHEmul::UByte::_0);
	setValue (0x03, MCHEmul::UByte::_0);
	setValue (0x04, MCHEmul::UByte::_0);
	setValue (0x05, MCHEmul::UByte::_0);
	setValue (0x06, 0x1b);					// scroll y = 3, 25 rows, no blank screem normal graphic mode
	setValue (0x07, 0x88);					// scroll x = 0, 40 columns, no freeze, PAL, no reverse
	setValue (0x08, MCHEmul::UByte::_0);
	setValue (0x09, MCHEmul::UByte::_FF);	// Clear up all interrupts...
	setValue (0x0a, MCHEmul::UByte::_0);	// No interrups enabled.
	setValue (0x0b, MCHEmul::UByte::_0);	// Raster IRQ, when any, at the beginning...
	setValue (0x0c, MCHEmul::UByte::_0);
	setValue (0x0d, MCHEmul::UByte::_0);	// Cursor at the beginning...
	setValue (0x0e, MCHEmul::UByte::_0);
	setValue (0x0f, MCHEmul::UByte::_0);
	setValue (0x10, MCHEmul::UByte::_0);
	setValue (0x11, MCHEmul::UByte::_0);	// No sound...
	setValue (0x12, MCHEmul::UByte::_0);	// ROM as source for character information, and bitMemory at $000...
	setValue (0x13, MCHEmul::UByte::_0);	// Single clock mode no active, and char data memory at $0000...
	setValue (0x14, MCHEmul::UByte::_0);	// Screen memory and Attribute memory at $0000...
	setValue (0x15, MCHEmul::UByte::_0);
	setValue (0x16, MCHEmul::UByte::_0);
	setValue (0x17, MCHEmul::UByte::_0);
	setValue (0x18, MCHEmul::UByte::_0);
	setValue (0x19, MCHEmul::UByte::_0);	// Background & foreground colors in black...
	// From 0x1a to 0x1f control who screen behaves...
	// better if nothing is written. Look at the next line instead...

	// Managed direclty by the TED Chip...
	_graphicalInfo = COMMODORE::TEDRegisters::GraphicalInfo ();
	// Reasons for IRQ
	_rasterIRQHappened = false;
	_lightPenIRQHappened = false;
}

// ---
void COMMODORE::TEDRegisters::setGraphicModeActive ()
{
	_graphicModeActive = COMMODORE::TEDRegisters::GraphicMode::_INVALIDBITMAPMODE2; // All on...

	if (!graphicExtendedColorTextModeActive () && !graphicBitModeActive () && !graphicMulticolorTextModeActive ())
		_graphicModeActive = COMMODORE::TEDRegisters::GraphicMode::_CHARMODE;
	else if (!graphicExtendedColorTextModeActive () && !graphicBitModeActive () && graphicMulticolorTextModeActive ())
		_graphicModeActive = COMMODORE::TEDRegisters::GraphicMode::_MULTICOLORCHARMODE;
	else if (!graphicExtendedColorTextModeActive () && graphicBitModeActive () && !graphicMulticolorTextModeActive ())
		_graphicModeActive = COMMODORE::TEDRegisters::GraphicMode::_BITMAPMODE;
	else if (!graphicExtendedColorTextModeActive () && graphicBitModeActive () && graphicMulticolorTextModeActive ())
		_graphicModeActive = COMMODORE::TEDRegisters::GraphicMode::_MULTICOLORBITMAPMODE;
	else if (graphicExtendedColorTextModeActive () && !graphicBitModeActive () && !graphicMulticolorTextModeActive ())
		_graphicModeActive = COMMODORE::TEDRegisters::GraphicMode::_EXTENDEDBACKGROUNDMODE;
	else if (graphicExtendedColorTextModeActive () && !graphicBitModeActive () && graphicMulticolorTextModeActive ())
		_graphicModeActive = COMMODORE::TEDRegisters::GraphicMode::_INVALIDTEXMODE;
	else if (graphicExtendedColorTextModeActive () && graphicBitModeActive () && !graphicMulticolorTextModeActive ())
		_graphicModeActive = COMMODORE::TEDRegisters::GraphicMode::_INVALIDBITMAPMODE1;

	_textMode = (_graphicModeActive == COMMODORE::TEDRegisters::GraphicMode::_CHARMODE ||
				 _graphicModeActive == COMMODORE::TEDRegisters::GraphicMode::_MULTICOLORCHARMODE ||
				 _graphicModeActive == COMMODORE::TEDRegisters::GraphicMode::_EXTENDEDBACKGROUNDMODE ||
				 _graphicModeActive == COMMODORE::TEDRegisters::GraphicMode::_INVALIDTEXMODE);
}
