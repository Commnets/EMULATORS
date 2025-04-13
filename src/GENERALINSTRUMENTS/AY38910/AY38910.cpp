#include <GENERALINSTRUMENTS/AY38910/AY38910.hpp>

// ---
GENERALINSTRUMENTS::AY38910::AY38910 (GENERALINSTRUMENTS::AY38910Registers* reg)
	: MCHEmul::Chip (GENERALINSTRUMENTS::AY38910::_ID,
		 { { "Name", "PPI 8255" },
		   { "Manufacturer", "Intel" },
		   { "Year", "1970" } }),
	  _AY38910Registers (reg),
	  _lastCPUCycles (0),
	  _internalRegisters (nullptr)
{
	// If nullptr a temporal one is created that it will be deleted when the object is destroyed...
	if (_AY38910Registers == nullptr)
		_internalRegisters = _AY38910Registers = new GENERALINSTRUMENTS::AY38910Registers;

}

// ---
GENERALINSTRUMENTS::AY38910::~AY38910 ()
{
	// That could be nullptr, it they would have been created externally!
	delete (_internalRegisters); // ...and in that case, nothing will happen...
}

// ---
bool GENERALINSTRUMENTS::AY38910::simulate (MCHEmul::CPU* cpu)
{
	// First time?
	if (_lastCPUCycles == 0)
	{ 
		_lastCPUCycles = cpu -> clockCycles (); // Nothing to do...

		return (true);
	}

	// Simulate the visulization...
	for (unsigned int i = 
			((cpu -> clockCycles  () - _lastCPUCycles)); 
		i > 0; i--)
	{
		_IFDEBUG debugAY38910Cycle (cpu, i);

		// TODO
	}

	_lastCPUCycles = cpu -> clockCycles ();

	return (true);
}

// ---
MCHEmul::InfoStructure GENERALINSTRUMENTS::AY38910::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::Chip::getInfoStructure ());

	// TODO

	return (result);
}

// ---
void GENERALINSTRUMENTS::AY38910::debugAY38910Cycle (MCHEmul::CPU* cpu, unsigned int i)
{
	assert (_deepDebugFile != nullptr);

	// TODO
}