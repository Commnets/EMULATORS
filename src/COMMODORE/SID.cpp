#include <COMMODORE/SID.hpp>

// ---
MCHEmul::UBytes COMMODORE::SoundRESIDWrapper::getData (MCHEmul::CPU* cpu)
{
	_resid_sid.clock ();
	short rData = (short) _resid_sid.output ();
	return (MCHEmul::UBytes ({ *(((char*)&rData) + 0), *(((char*)&rData) + 1) }));
}

// ---
void COMMODORE::SoundPulseWrapper::setParameters (const MCHEmul::Attributes& attrs)
{ 
	MCHEmul::Attributes::const_iterator i = attrs.find ("COUNTER");
	if (i != attrs.end ()) 
		_maxCounter = std::atoi ((*i).second.c_str ()); 

	i = attrs.find ("VALUE");
	if (i != attrs.end ()) 
		_pulseValue = std::atoi ((*i).second.c_str ()); 
}

// ---
MCHEmul::UBytes COMMODORE::SoundPulseWrapper::getData (MCHEmul::CPU *cpu)
{ 
	if (++_counter > _maxCounter)
	{
		if (++_status > 2) _status = 0;
		_counter = 0;
		_data = MCHEmul::UBytes 
			({ (_status == 0) ? 0 : ((_status == 1) ? _pulseValue : -_pulseValue) });
	}

	return (_data);
}

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

	setSoundWrapper (new COMMODORE::SoundRESIDWrapper (cF, SAMPLE_FAST, samplingFrecuency ()));
/**
	For testing. 
	setSoundWrapper (new COMMODORE::SoundPulseWrapper ());
*/
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

	_SIDRegisters -> setRESID_SID (&_resid_sid);

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
			// The number of bytes that can come from the warpper is not fixed...
			MCHEmul::UBytes data = soundWrapper () -> getData (cpu);
			for (size_t j = 0; j < data.size (); j++)
			{
				char dt = data [j].value ();
				if (soundMemory () -> addSampleData (&dt, sizeof (char)))
					notify (MCHEmul::Event (_SOUNDREADY));
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
