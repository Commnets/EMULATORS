#include <COMMODORE/SID/SID.hpp>

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
		for (unsigned int i = (cpu -> clockCycles () - _lastClockCycles); i > 0 ; i--)
		{
			_IFDEBUG debugSIDCycle (cpu, i);

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

	return (result);
}

// ---
void COMMODORE::SID::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	switch (evnt.id ())
	{
		// The movement of the joystick can simulate a potenciometer!
		case MCHEmul::InputOSSystem::_JOYSTICKMOVED:
			{
				if (_SIDRegisters -> potenciometerGroupActive () == MCHEmul::_S0)
					break; // No sense to spend time in reading something that is not "connected"...

				std::shared_ptr <MCHEmul::InputOSSystem::JoystickMovementEvent> jm = 
					std::static_pointer_cast <MCHEmul::InputOSSystem::JoystickMovementEvent> (evnt.data ());
				if ((jm -> _joystickId != 0 && jm -> _joystickId != 1) || jm -> _axisValues.size () > 2)
					break; // Only joysticks 0 y 1 are allowed and never more than 2 axis each!
						   // This matched what is defined in SIDRegisters! (@see SIDRegisters)

				size_t ct = 0; // The number of axis managed...
				for (auto i : jm -> _axisValues)
				{
					float tadd = (i > 0) ? 0.25f : ((i < 0) ? -0.25f : 0.0f);
					_SIDRegisters -> setPotenciometerValue // To 0 when reach 255...
						((size_t) jm -> _joystickId, ct, 
							_SIDRegisters -> potenciometerValue ((size_t) jm -> _joystickId, ct) + tadd);

					ct++;
				}
			}

		default:
			break;

		break;
	}
}

// ---
void COMMODORE::SID::debugSIDCycle (MCHEmul::CPU* cpu, unsigned int i)
{
	assert (_deepDebugFile != nullptr);

	MCHEmul::InfoStructure sidStr = std::move (soundWrapper () -> getInfoStructure ());
	MCHEmul::InfoStructure voicesStr = std::move (sidStr.infoStructure ("VOICES"));
	
	// Create the structure of attributes...
	MCHEmul::Attributes voicesAttrs;
	for (const auto& e : voicesStr.infoStructures ())
	{ 
		std::string w;
		MCHEmul::InfoStructure wStr = std::move (e.second.infoStructure ("WAVES"));
		for (size_t ct = 0; ct < 3; ct++)
			w += ((ct != 0) ? "," : "") + std::string ("Wave ") + std::to_string (ct) + "=" +
				wStr.infoStructure (std::to_string (ct)).attribute ("TYPEANDFREQUENCY");
		std::string v = e.second.attribute ("ADSR");
		voicesAttrs.insert (MCHEmul::Attributes::value_type ("Voice " + e.first, v + "," + w));
	}

	_deepDebugFile -> writeCompleteLine (className (), 
		cpu -> clockCycles () - i, "Info Cycle", voicesAttrs);
}
