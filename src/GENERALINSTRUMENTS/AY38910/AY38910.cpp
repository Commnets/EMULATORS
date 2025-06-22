#include <GENERALINSTRUMENTS/AY38910/AY38910.hpp>

// ---
GENERALINSTRUMENTS::AY38910::AY38910 
	(GENERALINSTRUMENTS::AY38910Registers* reg, GENERALINSTRUMENTS::AY38910LibWrapper* w)
	: MCHEmul::SoundChip (GENERALINSTRUMENTS::AY38910::_ID,
		 { { "Name", "PPI 8255" },
		   { "Manufacturer", "Intel" },
		   { "Year", "1970" } }, w),
	  _AY38910Registers (reg),
	  _lastCPUCycles (0),
	  _internalRegisters (nullptr)
{
	// If nullptr a temporal one is created that it will be deleted when the object is destroyed...
	if (_AY38910Registers == nullptr)
		_internalRegisters = _AY38910Registers = new GENERALINSTRUMENTS::AY38910Registers;
	_AY38910Registers -> setAY38910LibWrapper (w); // To link it to it...

	// The warpper cannot be nullptr never...
	assert (soundWrapper () != nullptr);
}

// ---
GENERALINSTRUMENTS::AY38910::~AY38910 ()
{
	// That could be nullptr, it they would have been created externally!
	delete (_internalRegisters); // ...and in that case, nothing will happen...
	// The wrapper is deleted in the parent class...
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

	if (soundWrapper () != nullptr)
	{
		// Simulate the sound...
		for (unsigned int i = 
				((cpu -> clockCycles  () - _lastCPUCycles) >> 1); 
			i > 0; i--)
		{
			_IFDEBUG debugAY38910Cycle (cpu, i);

			// The number of bytes that can come from the wrapper might not be fixed...
			MCHEmul::UBytes data;
			if (soundWrapper () -> getData (cpu, data))
			{
				for (size_t j = 0; j < data.size (); j++)
				{
					char dt = data [j].value ();
					if (soundMemory () -> addSampleData (&dt, sizeof (char)))
						notify (MCHEmul::Event (_SOUNDREADY));
				}
			}
		}
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