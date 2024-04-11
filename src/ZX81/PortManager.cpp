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
	// Any out really does this set of actions...
	// As it is defined in the ZX81 ports documentation!:

	// First of all, when VYSNC situation happens, a read to the port FE is done.
	// When this is done, the video signal is un - clamped and the LINECTRLN counter is un - blocked.
	// During this period, the ZX81 reads the keyboard for a maximm of 400us.
	// After that period the video signal is unclamped, and same with the LINECTRLN counter.
	_ULARegisters -> clampVideoSignal (false);
	_ULARegisters -> setLINECTRLNBlocked (false);
	_ULARegisters -> setCasetteSignal (true);

	// If the port id has the bit 1 off (FD is the normal ZX81 but many others will behave equal)...
	if ((id & 0b00000010) == 0x00)
		_ULARegisters -> setNMIGenerator (false); // the NMI gneerator is disconnected...

	// If the port id has the bit 0 off (FE is the normal ZX81, but many others will behave in the same way)...
	if (id == 0xfe)
		_ULARegisters -> setNMIGenerator (true); // The NMI generator is connected...
}

// ---
void ZX81::PortManager::linkToULA (ZX81::ULA* ula)
{
	assert (ula != nullptr);

	_ULA = ula;
	_ULARegisters = _ULA -> registers ();
}

// ---
void ZX81::PortManager::initialize ()
{
	_ULARegisters -> clampVideoSignal (true);
	_ULARegisters -> setLINECNTRL (0);
	_ULARegisters -> setLINECTRLNBlocked (true);
	_ULARegisters -> setCasetteSignal (false);
}

// ---
MCHEmul::UByte ZX81::PortManager::getValue (unsigned char id, bool ms) const
{
	MCHEmul::UByte result = MCHEmul::UByte::_0;

	// Any port with A0 = 0... (FE is the ZX81 common one, but many others will behave similar)...
	if ((id & 0b00000001) == 0b00000000)
	{ 
		result = 0b0010000 | // Bit 5 is always set...
			(_ULARegisters -> NTSC () ? 0b00000000 : 0b01000000); // Bit 6 set when 50Hz = PAL = !NTSC...

		// What row to read is determined by the value of the register B...
		MCHEmul::UByte bVal = 
			static_cast <const FZ80::CZ80*> (cpu ()) -> bRegister ().values ()[0];
		for (size_t i = 0; i < 7; i++) // Can be pressed simultaneously...
			if (!bVal.bit (i)) result |= _ULARegisters -> keyboardStatus (i); // Only the bits 0,1,2 and 3 can be set!

		// If the NMI generator is off...
		if (ms && // ...and it is wanted to modify the status...
			!_ULARegisters -> NMIGenerator ())
		{ 
			// This is to enter the VSYNC routine...
			_ULARegisters -> clampVideoSignal (true);		// Clamps the video signal...
			_ULARegisters -> setLINECNTRL (0);				// puts the counter to 0
			_ULARegisters -> setLINECTRLNBlocked (true);	// and also block it
			_ULARegisters -> setCasetteSignal (false);		// ...and finally the casette signal is down!
		}
	}

	return (result);
}
