#include <MSX/PPI8255.hpp>

// ---
MSX::PPI8255::PPI8255 (MSX::PPI8255Registers* reg)
	: MCHEmul::Chip (MSX::PPI8255::_ID,
		 { { "Name", "PPI 8255" },
		   { "Manufacturer", "Intel" },
		   { "Year", "1970" } }),
	  _PPI8255Registers (reg),
	  _lastCPUCycles (0),
	  _internalRegisters (nullptr)
{
	// If nullptr a temporal one is created that it will be deleted when the object is destroyed...
	if (_PPI8255Registers == nullptr)
		_internalRegisters = _PPI8255Registers = new MSX::PPI8255Registers;

}

// ---
MSX::PPI8255::~PPI8255 ()
{
	// That could be nullptr, it they would have been created externally!
	delete (_internalRegisters); // ...and in that case, nothing will happen...
}

// ---
bool MSX::PPI8255::simulate (MCHEmul::CPU* cpu)
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
		_IFDEBUG debugPPI8255Cycle (cpu, i);

		// TODO
	}

	_lastCPUCycles = cpu -> clockCycles ();

	return (true);
}

// ---
MCHEmul::InfoStructure MSX::PPI8255::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::Chip::getInfoStructure ());

	// TODO

	return (result);
}

// ---
void MSX::PPI8255::debugPPI8255Cycle (MCHEmul::CPU* cpu, unsigned int i)
{
	assert (_deepDebugFile != nullptr);

	// TODO
}