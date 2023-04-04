#include <COMMODORE/SID.hpp>

// ---
COMMODORE::SID::SID (unsigned int cF)
	: MCHEmul::SoundChip (_ID,
		{ { "Name", "SID" },
		  { "Code", "6581/8580" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Year", "1982" } }),
		  _SIDRegisters (nullptr),
	  _resid_sid (), // The sampling parameters are adjusted later...
	  _lastClockCycles (0)
{ 
	setClassName ("SID");

	_resid_sid.set_sampling_parameters ((double) cF, SAMPLE_FAST, (double) samplingFrecuency ());
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

	_resid_sid.reset ();

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
	for (unsigned int i = 0; i < (cpu -> clockCycles () - _lastClockCycles); i++)
	{
		_resid_sid.clock ();
		short data = (short) _resid_sid.output (); // The output is between -32768 and 32767 it is 16 bits...
		for (int j = 0; j < 3; j++)
			if (soundMemory () -> addSampleData ((unsigned char*) &data, sampleSize ()))
				notify (MCHEmul::Event (_SOUNDREADY));
	}
	
	_lastClockCycles = cpu -> clockCycles ();

	return (true);
}

// ---
MCHEmul::InfoStructure COMMODORE::SID::getInfoStructure () const
{
	MCHEmul::InfoStructure result = MCHEmul::SoundChip::getInfoStructure ();

	result.remove ("Memory"); // This is not neccesary...
	result.add ("Registers",	_SIDRegisters -> getInfoStructure ());

	return (result);
}
