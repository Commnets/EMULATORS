#include <COMMODORE/SID.hpp>

// ---
COMMODORE::SID::SID (unsigned int cF, MCHEmul::SoundLibWrapper* sW)
	: MCHEmul::SoundChip (_ID,
		{ { "Name", "SID" },
		  { "Code", "6581/8580" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Year", "1982" } }, 
		sW),
	  _SIDRegisters (nullptr),
	  _lastClockCycles (0)
{ 
	// Take care that the sound emulation library was not null at all...
	// ...and also belonging to the right type..
	assert (dynamic_cast <COMMODORE::SIDLibWrapper*> (soundWrapper ()) != nullptr);

	setClassName ("SID");
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

	result.remove ("Memory"); // This attribute is not neccesary...
	result.add ("Registers",		
		std::move (_SIDRegisters -> getInfoStructure ())); // Maybe they will all appear as 0...
	result.add ("SoundLibWrapper",	
		std::move (soundWrapper () -> getInfoStructure ())); // To know which library is behing...

	// Information about the voices...
	MCHEmul::InfoStructure vDt;
	for (unsigned char i = 0; i < 3; i++)
		vDt.add (std::to_string (i), 
				 std::move (static_cast <const COMMODORE::SIDLibWrapper*> (soundWrapper ()) -> getVoiceInfoStructure (i)));
	result.add ("VOICES", vDt);

	return (result);
}
