#include <ZXSpectrum/PortManager.hpp>
#include <ZXSpectrum/ULA.hpp>
#include <ZXSpectrum/ULARegisters.hpp>

const std::string ZXSPECTRUM::PortManager::_NAME = "ZX81 PortManager";

// ---
ZXSPECTRUM::PortManager::PortManager ()
	: FZ80::Z80Port (_ID, _NAME,
		{ { "Name", "General Port Manager" },
		  { "Description", "Class to manage all ZXSpectrum port iteractions" },
		  { "Detail", "FE (to read the keyboard status), FE (to enable NMI generation), FD (to disable NMI generation)" }
		}),
	  _ULA (nullptr),
	  _ULARegisters (nullptr)
{
	setClassName ("PortManager");
}

// ---
void ZXSPECTRUM::PortManager::setValue (unsigned short ab, unsigned char id, const MCHEmul::UByte& v)
{
	// Any port with A0 = 0 is ULA
	// However, 0xfe is the ZXSpectrum common one, but many others will behave similar...
	if ((id & 0b00000001) == 0b00000000) 
	{
		// The three lowest significant bits defines the border color
		// Bear in mind than in ZXSpectrum the border can not have bright!
		_ULARegisters -> setBorderColor (v.value () & 0x07);

		// The bit 3 (when 0) activates the MIC socket...
		// ...and the 4 (when 1) activates the EAR one and also the internal speaker...
		// But one one of then is activated, then the other is also activated, 
		// because both are connected to resistor to the same entry point as it is described in:
		// http://fizyka.umk.pl/~jacek/zx/faq/reference/48kreference.htm
		bool aME = !v.bit (3) || v.bit (4);
		_ULARegisters -> setMICSignal (aME);
		_ULARegisters -> setEARSignal (aME);
	}
}

// ---
MCHEmul::UByte ZXSPECTRUM::PortManager::getValue (unsigned short ab, unsigned char id, bool ms) const
{
	MCHEmul::UByte result = MCHEmul::UByte::_0;

	// Any port with A0 = 0 is ULA
	// However, 0xfe is the ZXSpectrum common one, but many others will behave similar...
	if ((id & 0b00000001) == 0b00000000) // The post 254 is the typical one...
	{ 
		// The bit 6 of the final result will be the value in the EAR socket...
		/** The EAR signal can be used to identify which is the ZXSpectrum issue (1,2 or 3),
			as it is described in: http://fizyka.umk.pl/~jacek/zx/faq/reference/48kreference.htm \n
			However it is not important for an emulator, so the first signal the port will read will be always false, 
			genarating a final result 0f 0xbf with no keys pressed. */
		result = 0b10100000; // = 0xa0

		MCHEmul::UByte pR = MCHEmul::UByte::_FF;
		// What row to read is determined by the value of the register B...
		MCHEmul::UByte bVal = (unsigned char) ((ab & 0xff00) >> 8);
		// If no row is selected...
		if (bVal.value () != MCHEmul::UByte::_FF)
		{
			for (size_t i = 0; i < 8; i++) // Several keys can be pressed simultaneously...
				if (!bVal.bit (i)) 
					pR &= ~_ULARegisters -> keyboardStatus (i); // ...and all of them are added!
		}

		result |= pR & 0x1f; // but at the end only the lowest 5 bits are important!
	}

	// Any port with A5 = 0 and A0 = 1 is Kempston Joystick...
	if ((id & 0b00100001) == 0b00000001) // the port 31 is the typical one...
	{
		result = MCHEmul::UByte::_0;

		// Set the bits of the result...
		result.setBit (0, _ULARegisters -> joystickStatus 
			(ZXSPECTRUM::ULARegisters::JoystickElement::_RIGHT));
		result.setBit (1, _ULARegisters -> joystickStatus 
			(ZXSPECTRUM::ULARegisters::JoystickElement::_LEFT));
		result.setBit (2, _ULARegisters -> joystickStatus 
			(ZXSPECTRUM::ULARegisters::JoystickElement::_DOWN));
		result.setBit (3, _ULARegisters -> joystickStatus 
			(ZXSPECTRUM::ULARegisters::JoystickElement::_UP));
		result.setBit (4, _ULARegisters -> joystickStatus 
			(ZXSPECTRUM::ULARegisters::JoystickElement::_FIRE));

		// The bits 5 - 7 are not used...
	}

	return (result);
}

// ---
void ZXSPECTRUM::PortManager::linkToULA (ZXSPECTRUM::ULA* ula)
{
	assert (ula != nullptr);

	_ULA = ula;
	_ULARegisters = _ULA -> registers ();
}

// ---
void ZXSPECTRUM::PortManager::initialize ()
{
	_ULARegisters -> initialize ();
}
