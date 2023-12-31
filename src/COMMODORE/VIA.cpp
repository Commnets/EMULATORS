#include <COMMODORE/VIA.hpp>

COMMODORE::VIA::VIA (int id, int rId, unsigned int intId)
	: MCHEmul::Chip (id,
		{ { "Name", "VIA" },
		  { "Code", "6526/6526A/8521" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Year", "1980" } }),
	  _registersId (rId),
	  _VIARegisters (nullptr),
	  _interruptId (intId),
	  _lastClockCycles (0)
{ 
	setClassName ("VIA"); 
}

// ---
bool COMMODORE::VIA::initialize ()
{
	assert (memoryRef () != nullptr);

	// Gets the memory block dedicated to the CIA
	if (!(_VIARegisters = 
		dynamic_cast <COMMODORE::VIARegisters*> (memoryRef () -> subset (_registersId))))
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	_VIARegisters -> initializeInternalValues ();

	_VIARegisters -> initialize ();

	_lastClockCycles = 0;

	return (true);
}

// ---
bool COMMODORE::VIA::simulate (MCHEmul::CPU* cpu)
{
	// First time?
	if (_lastClockCycles == 0)
	{ 
		_lastClockCycles = cpu -> clockCycles (); // Nothing to do...

		return (true);
	}

	for (unsigned int i = cpu -> clockCycles () - _lastClockCycles; i > 0; i--)
	{
		// TODO
	}

	_lastClockCycles = cpu -> clockCycles ();

	return (true);
}

// ---
MCHEmul::InfoStructure COMMODORE::VIA::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::Chip::getInfoStructure ());

	result.remove ("Memory"); // This is not neccesary...
	result.add ("Registers",	_VIARegisters -> getInfoStructure ());
	result.add ("PortA",		_VIARegisters -> portA ());
	result.add ("PortB",		_VIARegisters -> portB ()); // This is already a Ubyte...
	result.add ("CIDDRA",		MCHEmul::UByte (_VIARegisters -> dataPortADir ())); // To write doen it in hexadecimal form...
	result.add ("CIDDRB",		MCHEmul::UByte (_VIARegisters -> dataPortBDir ()));

	return (result);
}

//---
void COMMODORE::VIA::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	// TODO
}
