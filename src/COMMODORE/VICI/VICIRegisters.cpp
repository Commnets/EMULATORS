#include <COMMODORE/VICI/VICIRegisters.hpp>

// ---
COMMODORE::VICIRegisters::VICIRegisters (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: MCHEmul::ChipRegisters (_VICREGS_SUBSET, ps, pp, a, s),
	  _latchLightPenHorizontalPosition (0x00),
	  _latchLightPenVerticalPosition (0x00),
	  _lightPenActive (false),
	  _soundWrapper (nullptr),
	  _raster (nullptr),
	  _lastValueRead (MCHEmul::PhysicalStorage::_DEFAULTVALUE),
	  _screenMemoryVIC (0),
	  _charDataMemoryVIC (0),
	  _mousePositionX (-1),
	  _mousePositionY (-1)
{
	setClassName ("VICIRegisters");

	initializeInternalValues ();
}

// ---
void COMMODORE::VICIRegisters::initialize ()
{
	MCHEmul::PhysicalStorageSubset::initialize ();

	initializeInternalValues ();
}

// ---
MCHEmul::InfoStructure COMMODORE::VICIRegisters::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::ChipRegisters::getInfoStructure ());

	result.add ("AUXCOLOR",			_auxiliarColor);
	result.add ("BKCOLOR",			_backgroundColor);
	result.add ("BRDCOLOR",			_borderColor);
	result.add ("CHARADDRESS",		charDataMemory ());
	result.add ("SCREENADDRESS",	screenMemory ());
	result.add ("COLOURADDRESS",	colourMemory ());
	result.add ("LIGHTPENX",		_latchLightPenHorizontalPosition);
	result.add ("LIGHTPENY",		_latchLightPenVerticalPosition);

	return (result);
}

// ---
void COMMODORE::VICIRegisters::setValue (size_t p, const MCHEmul::UByte& v)
{
	size_t pp = p % 0x10;

	// Mirrored accesses share the canonical byte used by register reads.
	MCHEmul::PhysicalStorageSubset::setValue (pp, v);

	switch (pp)
	{
		// VICCR0
		case 0x00:
			{
				_interlaced = (v.value () & 0x80) != 0x00;

				_offsetXScreen = v.value () & 0x7f; // The 7 LSB bits...
			}

			break;

		// VICCR1
		case 0x01:
			{
				_offsetYScreen = v.value ();
			}

			break;

		// VICCR2
		case 0x02:
			{
				_charsWidthScreen = v.value () & 0x7f; // The 7 LSB bits...

				_b9ScreenColorMemory = (v.value () & 0x80) >> 7; // 0x00 or 0x01!

				calculateMemoryPositions ();
			}

			break;

		// VICCR3
		case 0x03:
			{
				_charsHeightScreen = (v.value () & 0x7e) >> 1; // bits 7..1 (6 bits)

				_charsExpanded = (v.value () & 0x01) != 0x00;
			}

			break;

		// VICCR4
		case 0x04:
			// Has not effect when setting the raster line in VICI...
			break;

		// VICCR5
		case 0x05:
			{
				_b10to13ScreenMemory = (v.value () & 0xf0) >> 4; // From 0x00 to 0x0f

				_b10to13CharDatamemory = (v.value () & 0x0f); // From 0x00 to 0x0f...

				calculateMemoryPositions ();
			}

			break;

		// VICCR6 & VICCR7
		// Lightpen position
		case 0x06:
		case 0x07:
			/** Only makes sense when reading. */
			break;

		// VICCR8 & VICCR9
		// Paddle position
		case 0x08:
		case 0x09:
			/** Only makes sense when reading. */
			break;

		// VICCRA, VICCRB, VICCRC, VICCRD
		// Managing the sound...
		case 0x0a:
		case 0x0b:
		case 0x0c:
		case 0x0d:
			{
				if (_soundWrapper != nullptr)
					_soundWrapper -> setValue (pp, v);
			}
			
			break;

		// VICCRE
		case 0x0e:
			{
				_auxiliarColor = (v.value () & 0xf0) >> 4;

				if (_soundWrapper != nullptr)
					_soundWrapper -> setValue (pp, v);
			}

			break;

		// VICCRF
		case 0x0f:
			{
				_backgroundColor = (v.value ()  & 0xf0) >> 4;

				_inverseMode = (v.value () & 0x08) == 0x00; // When bit 3 no active, the inverse mode is active!

				_borderColor = (v.value () & 0x07);
			}

			break;

		// This situation doesn't happen...
		default:
			break;
	}
}       

// ---
const MCHEmul::UByte& COMMODORE::VICIRegisters::readValue (size_t p) const
{
	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	size_t pp = p % 0x10;

	switch (pp)
	{
		// For this registers it is enough the info saved!
		case 0x00:
		case 0x01:
		case 0x02:
		case 0x05:
		case 0x0f:
			{
				result = MCHEmul::ChipRegisters::readValue (pp);
			}

			break;

		case 0x03:
			{
				result = MCHEmul::UByte ((unsigned char)
					(((currentRasterLine () & 0x0001) << 7) |
					 ((_charsHeightScreen << 1) & 0x7e) |
					 (_charsExpanded ? 0x01 : 0x00)));
			}

			break;

		case 0x04:
			{
				result = MCHEmul::UByte
					((unsigned char) ((currentRasterLine () >> 1) & 0x00ff));
			}

			break;

		case 0x06:
			{
				result = MCHEmul::UByte (_latchLightPenHorizontalPosition);
			}

			break;

		case 0x07:
			{
				result = MCHEmul::UByte (_latchLightPenVerticalPosition);
			}

			break;

		case 0x08:
			{
				result = MCHEmul::UByte::_FF;
			}

			break;

		case 0x09:
			{
				result = MCHEmul::UByte::_FF;
			}

			break;

		case 0x0a:
		case 0x0b:
		case 0x0c:
		case 0x0d:
			{
				if (_soundWrapper != nullptr)
					result = _soundWrapper -> readValue (pp);
			}

			break;

		case 0x0e:
			{
				result = (_auxiliarColor << 4);
				if (_soundWrapper != nullptr)
					result |= (_soundWrapper -> readValue (pp).value () & 0x0f);
			}

			break;

		// This situation doesn't happen...
		default:
			break;
	}

	return (_lastValueRead = result);
}

// ---
void COMMODORE::VICIRegisters::initializeInternalValues ()
{
	// Initialize retained input state directly: CPU register writes cannot change it.
	_latchLightPenHorizontalPosition = 0x00;
	_latchLightPenVerticalPosition = 0x00;

	// Writes to either address register recalculate all memory positions.
	// Define every selector before applying the register presets.
	_b9ScreenColorMemory = 0;
	_b10to13ScreenMemory = 0;
	_b10to13CharDatamemory = 0;

	setValue (0x00, 0x0e); // Non-interlaced; horizontal origin 14.
	setValue (0x01, 0x27); // Vertical origin 39.
	setValue (0x02, 0x96); // 22 columns; matrix address bit 9 set.
	setValue (0x03, 0x2e); // 23 rows; 8x8 characters.
	setValue (0x04, 0x00); // Raster position is read-only.
	setValue (0x05, 0xf0); // Matrix $1e00; characters $8000; color RAM $9600.
	setValue (0x06, 0x00);
	setValue (0x07, 0x00);
	setValue (0x08, 0xff);
	setValue (0x09, 0xff);
	setValue (0x0a, 0x00);
	setValue (0x0b, 0x00);
	setValue (0x0c, 0x00);
	setValue (0x0d, 0x00);
	setValue (0x0e, 0x00);
	setValue (0x0f, 0x1b); // White background; normal hires; cyan border.
}
