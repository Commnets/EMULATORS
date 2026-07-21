#include <MSX/Datasette.hpp>
#include <FZ80/incs.hpp>

const MCHEmul::Attributes MSX::DatasetteInjection::_ATTRIBUTES =
	{ { "Name", "MSX Datasette Injection" },
	  { "Manufacturer", "ICF Software Simulation" } };

// ---
MSX::DatasetteInjection::DatasetteInjection ()
	: MCHEmul::DatasettePeripheral (_ID, _ATTRIBUTES),
	  _traps
		{
		  {
			_TAPIONTRAP,
			"TAPION",
			MCHEmul::Address ({ 0xe1, 0x00 }, false),
			MCHEmul::Address ({ 0x00, 0x00 }, false),
			{ 0xc3 }
		  },
		  {
			_TAPINTRAP,
			"TAPIN",
			MCHEmul::Address ({ 0xe4, 0x00 }, false),
			MCHEmul::Address ({ 0x00, 0x00 }, false),
			{ 0xc3 }
		  },
		  {
			_TAPIOFTRAP,
			"TAPIOF",
			MCHEmul::Address ({ 0xe7, 0x00 }, false),
			MCHEmul::Address ({ 0x00, 0x00 }, false),
			{ 0xc3 }
		  },
		  {
			_TAPOONTRAP,
			"TAPOON",
			MCHEmul::Address ({ 0xea, 0x00 }, false),
			MCHEmul::Address ({ 0x00, 0x00 }, false),
			{ 0xc3 }
		  },
		  {
			_TAPOUTRAP,
			"TAPOUT",
			MCHEmul::Address ({ 0xed, 0x00 }, false),
			MCHEmul::Address ({ 0x00, 0x00 }, false),
			{ 0xc3 }
		  },
		  {
			_TAPOOFTRAP,
			"TAPOOF",
			MCHEmul::Address ({ 0xf0, 0x00 }, false),
			MCHEmul::Address ({ 0x00, 0x00 }, false),
			{ 0xc3 }
		  }
		},
	  _mode (Mode::_IDLE),
	  _blockCounter (0),
	  _byteCounter (0)
{
	setClassName ("MSXDatasetteInjection");
}

// ---
bool MSX::DatasetteInjection::initialize ()
{
	bool result = MCHEmul::DatasettePeripheral::initialize ();

	_mode = Mode::_IDLE;
	_blockCounter = 0;
	_byteCounter = 0;

	return (result);
}

// ---
bool MSX::DatasetteInjection::connectData (MCHEmul::FileData* dt)
{
	if (dynamic_cast <MSX::CASFileData*> (dt) == nullptr)
		return (false);

	_data = dt -> asMemoryBlocks ();

	_mode = Mode::_IDLE;
	_blockCounter = 0;
	_byteCounter = 0;

	return (true);
}

// ---
MCHEmul::FileData* MSX::DatasetteInjection::retrieveData () const
{
	MSX::CASFileData* result = new MSX::CASFileData;

	for (const auto& dataBlock : _data._data)
	{
		MSX::CASFileData::Block block;
		block._bytes = dataBlock.bytes ();
		block._fileName = dataBlock.existAttribute ("FILENAME")
			? dataBlock.attribute ("FILENAME") : "";

		std::string type = dataBlock.existAttribute ("TYPE")
			? dataBlock.attribute ("TYPE") : "DATA";
		if (type == "BINARYHEADER")
			block._type = MSX::CASFileData::BlockType::_BINARYHEADER;
		else if (type == "BASICHEADER")
			block._type = MSX::CASFileData::BlockType::_BASICHEADER;
		else if (type == "ASCIIHEADER")
			block._type = MSX::CASFileData::BlockType::_ASCIIHEADER;

		result -> _blocks.emplace_back (std::move (block));
	}

	return (result);
}

// ---
bool MSX::DatasetteInjection::simulate (MCHEmul::CPU* cpu)
{
	assert (dynamic_cast <FZ80::CZ80*> (cpu) != nullptr);

	bool found = false;
	for (MCHEmul::Traps::const_iterator i = _traps.begin ();
		i != _traps.end () && !found; i++)
	{
		found = (cpu -> programCounter ().internalRepresentation () == (*i)._addressIn.value () &&
			(*i).verifyFingerTipAgainst (cpu -> memoryRef ()));
		if (!found)
			continue;

		// A trap skips the BIOS routine, therefore pending memory writes must be visible
		// before its observable cassette effects are applied.
		MCHEmul::Memory::configuration ().executeMemorySetCommandsBuffered ();

		if (executeTrap ((*i), cpu))
		{
			// Emulate the RET which would finish the trapped BIOS routine.
			MCHEmul::UBytes returnBytes = cpu -> memoryRef () -> stack () -> pull (2);
			if (returnBytes.size () != 2)
				return (false);

			cpu -> programCounter ().setAddress (
				MCHEmul::Address (returnBytes, false));
		}
	}

	return (true);
}

// ---
MCHEmul::InfoStructure MSX::DatasetteInjection::getInfoStructure () const
{
	MCHEmul::InfoStructure result =
		std::move (MCHEmul::DatasettePeripheral::getInfoStructure ());

	std::string mode = _mode == Mode::_READING
		? "READING" : (_mode == Mode::_WRITING ? "WRITING" : "IDLE");
	result.add ("MODE",		mode);
	result.add ("BLOCKS",	_data._data.size ());
	result.add ("BLOCK",	_blockCounter);
	result.add ("OFFSET",	_byteCounter);

	MCHEmul::InfoStructure traps;
	for (const auto& trap : _traps)
		traps.add (std::to_string (trap._id), trap.asString ());
	result.add ("TRAPS", std::move (traps));

	return (result);
}

// ---
bool MSX::DatasetteInjection::executeTrap (
	const MCHEmul::Trap& t, MCHEmul::CPU* cpu)
{
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
				result = executeTAPOOFTrap (cpu);
			}

			break;

		default:
			{
				_LOG ("Trap type not supported:" + std::to_string (t._id));
				assert (false); // To fail in debug mode...

				result = false;
			}

			break;
	}

	return (result);
}

// ---
bool MSX::DatasetteInjection::executeTAPIONTrap (MCHEmul::CPU* cpu)
{
	// Calling TAPION again while a block is open scans forward to the next pilot.
	if (_mode == Mode::_READING)
		_blockCounter++;

	_byteCounter = 0;
	if (_mode == Mode::_WRITING ||
		_blockCounter >= _data._data.size ())
	{
		_mode = Mode::_IDLE;
		setMotorOff (true);
		setCassetteError (cpu, true);

		_IFDEBUG debugNothingToRead ();

		return (true);
	}

	_mode = Mode::_READING;
	setMotorOff (false);
	setCassetteError (cpu, false);

	return (true);
}

// ---
bool MSX::DatasetteInjection::executeTAPINTrap (MCHEmul::CPU* cpu)
{
	if (_mode != Mode::_READING ||
		_blockCounter >= _data._data.size () ||
		_byteCounter >= _data._data [_blockCounter].size ())
	{
		setCassetteError (cpu, true);

		_IFDEBUG debugErrorTrap ();

		return (true);
	}

	FZ80::CZ80* z80 = static_cast <FZ80::CZ80*> (cpu);
	z80 -> aRegister ().set (
		{ _data._data [_blockCounter].byte (_byteCounter++).value () });
	setCassetteError (cpu, false);

	return (true);
}

// ---
bool MSX::DatasetteInjection::executeTAPIOFTrap (MCHEmul::CPU* cpu)
{
	(void) cpu;

	if (_mode == Mode::_READING)
		_blockCounter++;

	_mode = Mode::_IDLE;
	_byteCounter = 0;
	setMotorOff (true);

	return (true);
}

// ---
bool MSX::DatasetteInjection::executeTAPOONTrap (MCHEmul::CPU* cpu)
{
	if (_mode == Mode::_READING)
		_blockCounter++;

	MCHEmul::DataMemoryBlock newBlock;
	newBlock.setName ("CASBLOCK_" + std::to_string (_data._data.size ()));
	newBlock.setAttribute ("TYPE", "DATA");

	_data._data.emplace_back (std::move (newBlock));
	_blockCounter = _data._data.size () - 1;
	_byteCounter = 0;
	_mode = Mode::_WRITING;
	setMotorOff (false);

	// A selects a short or long physical pilot. CAS stores the same marker for both.
	setCassetteError (cpu, false);

	return (true);
}

// ---
bool MSX::DatasetteInjection::executeTAPOUTTrap (MCHEmul::CPU* cpu)
{
	if (_mode != Mode::_WRITING ||
		_blockCounter >= _data._data.size ())
	{
		setCassetteError (cpu, true);

		return (true);
	}

	FZ80::CZ80* z80 = static_cast <FZ80::CZ80*> (cpu);
	_data._data [_blockCounter].addByte (z80 -> aRegister ().values ()[0]);
	_byteCounter++;
	setCassetteError (cpu, false);

	return (true);
}

// ---
bool MSX::DatasetteInjection::executeTAPOOFTrap (MCHEmul::CPU* cpu)
{
	(void) cpu;

	_mode = Mode::_IDLE;
	_blockCounter = _data._data.size ();
	_byteCounter = 0;
	setMotorOff (true);

	return (true);
}

// ---
void MSX::DatasetteInjection::setCassetteError (
	MCHEmul::CPU* cpu, bool error)
{
	FZ80::CZ80* z80 = static_cast <FZ80::CZ80*> (cpu);
	z80 -> statusRegister ().setBitStatus (FZ80::CZ80::_CARRYFLAG, error);
}

// ---
void MSX::DatasetteInjection::debugStatus (
	const std::string& where, MCHEmul::CPU* cpu)
{
	assert (_deepDebugFile != nullptr);

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

	_deepDebugFile -> writeLineData (
		"Error reading block:" + std::to_string (_blockCounter));
}

// ---
void MSX::DatasetteInjection::debugNothingToRead ()
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeLineData ("No data to read...");
}
