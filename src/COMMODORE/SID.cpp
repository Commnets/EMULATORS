#include <COMMODORE/SID.hpp>

// ---
COMMODORE::SID::SID (unsigned int cF)
	: MCHEmul::SoundChip (_ID,
		{ { "Name", "SID" },
		  { "Code", "6581/8580" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Year", "1982" } }),
		  _SIDRegisters (nullptr),
	  _lastClockCycles (0)
{ 
	setClassName ("SID");

//	setSoundWrapper (new COMMODORE::SoundRESIDWrapper (cF, SAMPLE_FAST, samplingFrecuency ()));
	setSoundWrapper (new COMMODORE::SoundSimpleWrapper (cF, samplingFrecuency ()));
}

// ---
bool COMMODORE::SID::initialize ()
{
	assert (memoryRef () != nullptr);

	// Gets the memory block dedicated to the SID
	if (!(_SIDRegisters = 
		dynamic_cast <COMMODORE::SIDRegisters*> (memoryRef () -> subset (COMMODORE::SIDRegisters::_SIDREGS_SUBSET))))
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	if (soundWrapper () != nullptr)
		soundWrapper () -> initialize ();

	_SIDRegisters -> setSIDLibWrapper (static_cast <COMMODORE::SIDLibWrapper*> (soundWrapper ()));

	_lastClockCycles = 0;

	return (MCHEmul::SoundChip::initialize ());
}

// ---
bool COMMODORE::SID::simulate (MCHEmul::CPU* cpu)
{
	// First time?
	if (_lastClockCycles == 0)
	{ 
		_lastClockCycles = cpu -> clockCycles (); // Nothing to do...

		return (true);
	}

	// Add the output to the sound memory...
	if (soundWrapper () != nullptr)
	{
		for (unsigned int i = 0; i < (cpu -> clockCycles () - _lastClockCycles); i++)
		{
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
	
	_lastClockCycles = cpu -> clockCycles ();

	return (true);
}

// ---
MCHEmul::InfoStructure COMMODORE::SID::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::SoundChip::getInfoStructure ());

	result.remove ("Memory"); // This is not neccesary...
	result.add ("Registers",	std::move (_SIDRegisters -> getInfoStructure ()));
	if (soundWrapper () != nullptr)
		result.add ("Wrapper",	std::move (soundWrapper () -> getInfoStructure ()));

	return (result);
}
