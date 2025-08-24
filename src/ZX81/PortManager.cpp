#include <ZX81/PortManager.hpp>
#include <ZX81/ULA.hpp>
#include <ZX81/ULARegisters.hpp>

const std::string ZX81::PortManager::_NAME = "ZX81 PortManager";

// ---
ZX81::PortManager::PortManager (ZX81::Type t)
	: FZ80::Z80Port (_ID, _NAME,
		{ { "Name", "General Port Manager" },
		  { "Description", "Class to manage all ZX81 port iteractions" },
		  { "Detail", "FE (to read the keyboard status), FE (to enable NMI generation), FD (to disable NMI generation)" }
		}),
	  _type (t),
	  _ULA (nullptr),
	  _ULARegisters (nullptr)
{
	setClassName ("PortManager");
}

// ---
void ZX81::PortManager::setValue (unsigned short ab, unsigned char id, const MCHEmul::UByte& v)
{
	// Mark the event in the screen...
	_ULA -> markWritePortAction ();

	// Any out really does this set of actions...
	// As it is defined in the ZX81 ports documentation!:

	// Any time an output is done...
	// The Vsync finishes...
	_ULARegisters -> setVSync (false);

	// If the port id has the bit 1 off 
	// (FD is the normal ZX81 but many others will behave equal)...
	if ((id & 0b00000010) == 0x00 && _type != ZX81::Type::_ZX80) // In the ZX80 there is no this port...
	{
		_ULARegisters -> setNMIGenerator (false); // the NMI generator is disconnected...

		_ULA -> markNMIGeneratorOff ();
	}

	// If the port id has the bit 0 off (FE is the normal ZX81, 
	// but many others will behave in the same way)...
	if ((id & 0b00000001) == 0x00)
	{
		// In the ZX80 there is no even a wire connected to NMI pint...
		if (_type != Type::_ZX80)
			_ULARegisters -> setNMIGenerator (true); // The NMI generator is connected...

		_ULA -> markNMIGeneratorOn ();
	}

	// Any output to any odd port manages the casette signal...
	// Always put it high!
	if ((id & 0b00000001) == 0b00000001) // Any odd port...
		_ULARegisters -> setMICSignal (true); 
}

// ---
MCHEmul::UByte ZX81::PortManager::getValue (unsigned short ab, unsigned char id, bool ms) const
{
	MCHEmul::UByte result = MCHEmul::UByte::_0;

	// Any port with A0 = 0... (FE is the ZX81 common one, but many others will behave similar)...
	if ((id & 0b00000001) == 0b00000000) // The most typical port is the 254...
	{ 
		// Mark the event in the screen...
		_ULA -> markReadPortFEAction ();

		result = 0b00100000 | // Bit 5 is always set...
			(_ULARegisters -> NTSC () ? 0b00000000 : 0b01000000); // Bit 6 set when 50Hz = PAL = !NTSC...

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

		// but at the end only the lowest 5 bits are important!
		result |= pR & 0x1f; 

		// If the NMI generator is off...
		if (ms && // ...and it is wanted to modify the status...
			!_ULARegisters -> NMIGenerator ())
		{
			// ...the VSYNC starts, if it didn't before...
			if (!_ULARegisters -> inVSync ())
			{ 
				_ULARegisters -> setVSync (true);

				// The raster is initialized. Both, the horizontal and the vertical ones...
				_ULA -> raster ().initialize ();
			}
		}

		// Any read to the port FE put the MIC signal low...
		_ULARegisters -> setMICSignal (false);
		// ...and gets the status of the EAR signal in the bit 6!
		result.setBit (6, _ULARegisters -> EARSignal ());
	}

	return (result);
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
	_ULARegisters -> initialize ();
}
