#include <ZXSpectrum/PortManager.hpp>
#include <ZXSpectrum/ULA.hpp>
#include <ZXSpectrum/ULARegisters.hpp>
#include <FZ80/Instruction.hpp>

const std::string ZXSPECTRUM::PortManager::_NAME = "ZX81 PortManager";

// ---
ZXSPECTRUM::PortManager::PortManager ()
	: FZ80::Z80Port (_ID, _NAME,
		{ { "Name", "General Port Manager" },
		  { "Description", "Class to manage all ZXSpectrum port iteractions" },
		  { "Detail", "FE (to read the keyboard status), FE (to enable NMI generation), FD (to disable NMI generation)" }
		}),
	  _ULA (nullptr),
	  _ULARegisters (nullptr),
	  _portWriteNotLinked (), _portReadNotLinked ()
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

		// Bit 3 drives the cassette MIC output and bit 4 drives the EAR/speaker output latch.
		// Neither output overwrites the independent signal arriving through the EAR socket.
		// This emulator uses the Issue 3 digital read model:
		// Value output to bit 4 (EAR)  3 (MIC)  |  Iss 2  Iss 3   Iss 2 V    Iss 3 V <= Output when read EAR signal
		//						  1			1	 |    1      1       3.79       3.70
        //						  1			0	 |    1      1       3.66       3.56
        //						  0			1	 |    1      0       0.73       0.66
        //						  0			0	 |    0      0       0.39       0.34		
		// http://fizyka.umk.pl/~jacek/zx/faq/reference/48kreference.htm
		_ULARegisters -> setMICOutputSignal (v.bit (3));
		_ULARegisters -> setEAROutputSignal (v.bit (4));
		// Both output signals affect the simplified digital buzzer.
		_ULARegisters -> alignBuzzerSignal ();
	}
	else
	{
		// First access to a no codified port might be interesting 
		// to force a contention without affecting the ULA work
		// T o avoid continuous entries in the log, just the fist one is kept...
		if (std::find (_portWriteNotLinked.begin (), 
				_portWriteNotLinked.end (), id) == _portWriteNotLinked.end ())
		{
			_portWriteNotLinked.push_back (id);

			_LOG ("PortManager::setValue: Unattached output port " +
				std::to_string ((int) id) + ". It might generate contention");
		}
	}

	// Accessing to a no codified port might be instesting to force
	// a contention without affecting the ULA work
	// It would depend on the type of OUT sentence used and the value of the registers B, C and A...
}

// ---
unsigned int ZXSPECTRUM::PortManager::additionalClockCyclesForIO
	(unsigned short ab, unsigned int cC) const
{
	assert (_ULA != nullptr);

	return (_ULA -> IOContentionDelayAt (ab, cC));
}

// ---
unsigned int ZXSPECTRUM::PortManager::IOAccessClockCycle () const
{
	assert (cpu () != nullptr);
	assert (cpu () -> currentInstruction () != nullptr);

	const FZ80::Instruction* i =
		dynamic_cast <const FZ80::Instruction*> (cpu () -> currentInstruction ());
	if (i != nullptr)
		return (i -> IOAccessClockCycle ());

	const FZ80::InstructionUndefined* uI =
		dynamic_cast <const FZ80::InstructionUndefined*> (cpu () -> currentInstruction ());
	assert (uI != nullptr);

	return (uI -> IOAccessClockCycle ());
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

		// In the Issue 3 model, D6 is derived from the EAR input and the D4 output latch.
		// With D4 low, as used by the standard ROM loader, D6 follows the cassette input.
		result.setBit (6, _ULARegisters -> EARReadSignal ());
	}
	// Any port with A5 = 0 and A0 = 1 is Kempston Joystick...
	else
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
	// An unattached input samples the ULA-side bus at the end of its I/O cycle.
	// A peek is not an instruction execution, so it uses the current CPU time.
	else
	{
		if (std::find (_portReadNotLinked.begin (), 
				_portReadNotLinked.end (), id) == _portReadNotLinked.end ())
		{
			_portReadNotLinked.push_back (id); // It is mutable...

			_LOG ("PortManager::getValue: Unattached input port " +
				std::to_string ((int) id) + ". It might generate contention");
		}

		result = _ULA -> floatingBusValueAt
			(ms ? IOAccessClockCycle () : cpu () -> clockCycles ());
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
