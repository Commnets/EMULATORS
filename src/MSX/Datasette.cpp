#include <MSX/Datasette.hpp>
#include <FZ80/incs.hpp>

const MCHEmul::Attributes MSX::DatasetteInjection::_ATTRIBUTES =
	{ { "Name", "MSX Datasette Injection" },
	  { "Manufacturer", "ICF Software Simulation" } };;

// ---
MSX::DatasetteInjection::DatasetteInjection ()
	: MCHEmul::StandardDatasette (_ID,
		new MCHEmul::StandardDatasette::NilImplementation, 
		true, /** Motor controlled internally. */
		_ATTRIBUTES),
	  _traps
		{
		  { 
			_TAPIONTRAP,
			"TAPION",	
			MCHEmul::Address ({ 0xe1, 0x00 }, false), 
			MCHEmul::Address ({ 0x00, 0x00 }, false), // Not used...
			{ 0xc3, 0x63, 0x1a }
		  },
		  { 
			_TAPINTRAP,
			"TAPIN",
			MCHEmul::Address ({ 0xe4, 0x00 }, false), 
			MCHEmul::Address ({ 0x00, 0x00 }, false), // Not used...
			{ 0xc3, 0xbc, 0x1a }
		  },
		  { 
			_TAPIOFTRAP,
			"TAPIOF",
			MCHEmul::Address ({ 0xe7, 0x00 }, false),
			MCHEmul::Address ({ 0x00, 0x00 }, false), // Not used...
			{ 0xc3, 0xe9, 0x19 }
		  },
		  {
			_TAPOONTRAP,
			"TAPOON",
			MCHEmul::Address ({ 0xea, 0x00 }, false),
			MCHEmul::Address ({ 0x00, 0x00 }, false), // Not used...
			{ 0xc3, 0xf1, 0x19 }
		  },
		  { 
			_TAPOUTRAP,
			"TAPOUT",
			MCHEmul::Address ({ 0xed, 0x00 }, false),
			MCHEmul::Address ({ 0x00, 0x00 }, false), // Not used...
			{ 0xc3, 0x19, 0x1a }
		  },
		  {
			_TAPOOFTRAP,
			"TAPOOF",
			MCHEmul::Address ({ 0xf0, 0x00 }, false),
			MCHEmul::Address ({ 0x00, 0x00 }, false), // Not used...
			{ 0xc3, 0xdd, 0x19 }
		  }
		}
{ 
	setClassName ("MSXDatasetteInjection");
}

// ---
bool MSX::DatasetteInjection::connectData (MCHEmul::FileData* dt)
{
	if (dynamic_cast <MSX::CASFileData*> (dt) == nullptr)
		return (false); // These formats are the only ones accepted only...

	_data = dt -> asMemoryBlocks ();

	_dataCounter = 0;
	_elementCounter = 0;

	return (true);
}

// ---
bool MSX::DatasetteInjection::simulate (MCHEmul::CPU* cpu)
{
	// What to do will depend on the simulation is...
	// Te definition can be in many different traps
	// Detect first whether the simulation is or not in one of them and then execute the right thing...
	// There can not be more than 1 trap with the same name
	bool nF = false;
	for (MCHEmul::Traps::const_iterator i = _traps.begin ();
		i != _traps.end () && !nF; i++)
	{
		if (nF = (cpu -> programCounter ().internalRepresentation () == (*i)._addressIn.value () && 
			(*i).verifyFingerTipAgainst (cpu -> memoryRef ()))) // The fingertip has to be also "found"...
		{
			// Empty any pending set action...
			// Because a trap is breaking the normal flow of the code...
			// so no set commands pendings can be left!
			MCHEmul::Memory::configuration ().executeMemorySetCommandsBuffered ();

			// Execute the trap...
			// ...and if it was executed, change the PC to the out address...
			// The direction where to return can be different depending on the trap executed...
			// and also on the version of the BIOS executed, so the direction where to return
			// is stored at the top of the stack...
			MCHEmul::Address rStr (cpu -> memoryRef () -> values 
				(cpu -> memoryRef () -> stack () -> currentAddress (), 2), false);
			if (executeTrap ((*i), cpu))
				cpu -> programCounter ().setAddress (rStr); // Where to return...
		}
	}

	// There are no errors by default...
	// It doesn't mean that the trap was o not executed actually, 
	// just that the routine eorked with no errors...
	return (true);
}

// ---
MCHEmul::InfoStructure MSX::DatasetteInjection::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::StandardDatasette::getInfoStructure ());

	// TODO

	return (result);
}

// ---
bool MSX::DatasetteInjection::executeTrap (const MCHEmul::Trap& t, MCHEmul::CPU* cpu)
{
	// The CPU has to be from the F6500 family!
	assert (dynamic_cast <FZ80::CZ80*> (cpu) != nullptr);

	_IFDEBUG debugStatus (t.asString (), cpu);

	bool result = true;
	switch (t._id)
	{
		case _TAPIONTRAP:
			{
				result = executeTAPIONTrap (cpu); 
			}

			break;

		case _TAPINTRAP:
			{
				result = executeTAPINTrap (cpu);
			}

			break;

		case _TAPIOFTRAP:
			{
				result = executeTAPIOFTrap (cpu);
			}
			break;

		case _TAPOONTRAP:
			{
				result = executeTAPOONTrap (cpu);
			}
			break;

		case _TAPOUTRAP:
			{
				result = executeTAPOUTTrap (cpu);
			}
			break;

		case _TAPOOFTRAP:
			{
				result = executeTAPOOFrap (cpu);
			}
			break;

		default:
			{
				_LOG ("Trap type not supported yet:" + 
					std::to_string (t._id));
				assert (false); // Just for compiling with debug on...
			}

			break;
	}

	return (result);
}

bool MSX::DatasetteInjection::executeTAPIONTrap (MCHEmul::CPU* cpu)
{
	// TODO

	return (true);
}

bool MSX::DatasetteInjection::executeTAPINTrap (MCHEmul::CPU* cpu)
{
	// TODO

	return (true);
}

bool MSX::DatasetteInjection::executeTAPIOFTrap	(MCHEmul::CPU* cpu)
{
	// TODO

	return (true);
}

bool MSX::DatasetteInjection::executeTAPOONTrap	(MCHEmul::CPU* cpu)
{
	// TODO

	return (true);
}

bool MSX::DatasetteInjection::executeTAPOUTTrap (MCHEmul::CPU* cpu)
{
	// TODO

	return (true);
}

bool MSX::DatasetteInjection::executeTAPOOFrap (MCHEmul::CPU* cpu)
{
	// TODO

	return (true);
}

// ---
void MSX::DatasetteInjection::debugStatus (const std::string& where, MCHEmul::CPU* cpu)
{
	auto allRgsAsAttrs = [&](const MCHEmul::Registers& rgs) -> MCHEmul::Attributes
		{ MCHEmul::Attributes result;
		  for (const auto& i : rgs) 
			  result.insert (MCHEmul::Attributes::value_type (i.name (), i.asString ()));
		  return (result); };

	_deepDebugFile -> writeSimpleLine (where);
	_deepDebugFile -> writeCompleteLine ("MSXDatasetteInjection", cpu -> clockCycles (), 
		"Registers:", allRgsAsAttrs (cpu -> internalRegisters ()));
}

// ---
void MSX::DatasetteInjection::debugErrorTrap ()
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeLineData ("Error reading block:" + std::to_string (_dataCounter));
}

// ---
void MSX::DatasetteInjection::debugNothingToRead ()
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeLineData ("No data to read...");
}
