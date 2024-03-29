#include <ZX81/PortManager.hpp>
#include <ZX81/ULA.hpp>
#include <ZX81/ULARegisters.hpp>

const std::string ZX81::PortManager::_NAME = "ZX81 PortManager";

// ---
ZX81::PortManager::PortManager ()
	: FZ80::Z80Port (_ID, _NAME,
		{ { "Name", "General Port Manager" },
		  { "Description", "Class to manage all ZX81 port iteractions" },
		  { "Detail", "FE (to read the keyboard status), FE (to enable NMI generation), FD (to disable NMI generation)" }
		}),
	  _ULA (nullptr),
	  _ULARegisters (nullptr)
{
	setClassName ("PortManager");
}

// ---
void ZX81::PortManager::setValue (unsigned char id, const MCHEmul::UByte& v)
{
	// The VSYNC pukse is released...
	// ...and the LNCTRL will start to be incremented...
	_ULARegisters -> setVSYNCPulse (false);
	// ...and the casette signal is put back into high...
	_ULARegisters -> setCasetteSignal (true);

	// specific behaviour by specific ports!
	// Desactive the generation of NMI interrupts!
	if (id == 0xfd)
		_ULARegisters -> setNMIGenerator (false);
	// Active the generation of NMI interrupts!
	if (id == 0xfe)
		_ULARegisters -> setNMIGenerator (true);
}

// ---
void ZX81::PortManager::linkToULA (ZX81::ULA* ula)
{
	assert (ula != nullptr);

	_ULA = ula;
	_ULARegisters = _ULA -> registers ();
}

// ---
MCHEmul::UByte ZX81::PortManager::getValue (unsigned char id, bool ms) const
{
	MCHEmul::UByte result = MCHEmul::UByte::_0;

	// Any port with A0 = 0... (portFE to read the port)
	if ((id & 0b00000001) == 0b00000000)
	{ 
		result = 0b0010000 |		// But 5 is always set...
			(_ULARegisters -> NTSC () ? 0b00000000 : 0b01000000);	// Bit 6 set when 50Hz = PAL = !NTSC...

		// What row to read is determined by the value of the register B...
		MCHEmul::UByte bVal = 
			static_cast <const FZ80::CZ80*> (cpu ()) -> bRegister ().values ()[0];
		for (size_t i = 0; i < 7; i++) // Can be pressed simultaneously...
			if (!bVal.bit (i)) result |= _ULARegisters -> keyboardStatus (i); // Only the bits 0,1,2 and 3 can be set!

		// If the NMI generator is off...
		if (ms && // ...and it is wanted to modify the status...
			!_ULARegisters -> NMIGenerator ())
		{ 
			// ...and the vertical sync as well...
			_ULARegisters -> setVSYNCPulse (true);
			// ...the vertical line counter is restarted,
			// ...but it wouldn't increment if VSYNC were on!
			_ULARegisters -> setLINECNTRL (0);
			// ...and the casette signal is moved into low...
			_ULARegisters -> setCasetteSignal (false);
		}
	}

	return (result);
}
