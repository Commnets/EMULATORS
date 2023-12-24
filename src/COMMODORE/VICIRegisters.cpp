#include <COMMODORE/VICIRegisters.hpp>

// ---
COMMODORE::VICIRegisters::VICIRegisters (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: MCHEmul::ChipRegisters (_VICREGS_SUBSET, ps, pp, a, s),
	  _lastValueRead (MCHEmul::PhysicalStorage::_DEFAULTVALUE),
	  _soundWrapper (nullptr)
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

	// TODO

	return (result);
}

// ---
void COMMODORE::VICIRegisters::setValue (size_t p, const MCHEmul::UByte& v)
{
	size_t pp = p % 0x10;

	// The 64 first bytes will keep the right value...
	MCHEmul::PhysicalStorageSubset::setValue (p, v);

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
				_soundWrapper -> setValue (p, v);
			}
			
			break;

		// VICCRE
		case 0x0e:
			{
				_auxiliarColor = (v.value () & 0xf0) >> 4;

				_soundWrapper -> setValue (p, v);
			}

			break;

		// VICCRF
		case 0x0f:
			{
				_screenColor = (v.value ()  & 0xf0) >> 4;

				_inverseMode = (v.value () & 0x08) != 0x00;

				_borderColor = (v.value () & 0x07);
			}

			break;

		default:
			// It shouldn't be here...
			assert (false);
	}
}       

// ---
const MCHEmul::UByte& COMMODORE::VICIRegisters::readValue (size_t p) const
{
	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	size_t pp = p % 0x10;

	switch (pp)
	{
		case 0x00:
			{
				result = (_interlaced ? 0x80 : 0x00) | (_offsetXScreen & 0x7f);
			}

			break;

		case 0x01:
			{
				result = _offsetYScreen;
			}

			break;

		case 0x02:
			{
				result = (_charsWidthScreen & 0x7f) | 
						 (((unsigned char) _b9ScreenColorMemory) << 7);
			}

			break;

		case 0x03:
			{
				result = (((_currentRasterLine & 0x0f00) != 0x00) ? 0x80 : 0x00) |
						 ((_charsHeightScreen << 1) & 0x7e) | 
						 (_charsExpanded ? 0x01 : 0x00);
			}

		case 0x04:
			{
				result = (unsigned char) (_currentRasterLine & 0x00ff);
			}

			break;

		case 0x05:
			{
				result = (unsigned char) (_b10to13CharDatamemory) |	
						 (unsigned char) (_b10to13ScreenMemory << 4);
			}

		case 0x06:
			{
				result = _lightPenActive // Only when the lightpen is active...
					? MCHEmul::UByte ((unsigned char) (_currentLightPenHorizontalPosition >> 1)) : 0;
			}

			break;

		case 0x07:
			{
				result = _lightPenActive // only when the lightpen is active...
					? MCHEmul::UByte ((unsigned char) (_currentLightPenVerticalPosition >> 1)) : 0;
			}

			break;

		case 0x08:
			// TODO
			break;

		case 0x09:
			// TODO
			break;

		case 0x0a:
		case 0x0b:
		case 0x0c:
		case 0x0d:
			{
				result = _soundWrapper -> readValue (p);
			}

			break;

		case 0x0e:
			{
				result = (_auxiliarColor << 4) | _soundWrapper -> readValue (0x0e).value ();
			}

			break;

		case 0x0f:
			{
				result = (_screenColor << 4) |
						 (_inverseMode ? 0x08 : 0x00) |
						 (_borderColor & 0x07);
			}

			break;

		default:
			// It shouldn't be here...
			assert (false);
	}

	return (_lastValueRead = result);
}

// ---
void COMMODORE::VICIRegisters::initializeInternalValues ()
{
	setValue (0x00, 0x05); // No interlaced & horizontal adjust = 5 (for PAL systems)
	setValue (0x01, 0x0e); // Vertical adjust = 14 (for both PAL & NTSC systems);
	setValue (0x02, 0x96); // 9 bit screen (true) & color memory = true (color map l ocation = 0x9600) & 22 columns = 0x16 (see also 0x05)
	setValue (0x03, 0x2e); // 23 rows, not exapnded
	setValue (0x04, 0x00); // Has no effect...
	setValue (0x05, 0xf0); // screen map location = this value (bot 13,12,11,10) + bit 7 0x02 (bit 9) = $1e00 (7680), 
						   // colour map location = 0x9600
	setValue (0x06, 0x00); // No important...
	setValue (0x07, 0x00); // No important...
	setValue (0x08, 0xff); // No important...
	setValue (0x09, 0xff); // No important...
	setValue (0x0a, 0x00);
	setValue (0x0b, 0x00);
	setValue (0x0c, 0x00);
	setValue (0x0d, 0x00);
	setValue (0x0e, 0x00);
	setValue (0x0f, 0x1b); // Background color = 1 (white), inverse active, border color = 3 (cyan)
}
