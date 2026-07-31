#include <ZXSpectrum/Datasette.hpp>
#include <FZ80/CP.hpp>

const MCHEmul::Attributes ZXSPECTRUM::DatasetteInjection::_ATTRIBUTES =
		{ { "Name", "Datasette Injection ZXSpectrum" },
		  { "Manufacturer", "ICF to inject the code directly into the memory" } };

// ---
ZXSPECTRUM::DatasetteInjection::DatasetteInjection (ZXSPECTRUM::Type t)
	: MCHEmul::DatasettePeripheral (_ID, _ATTRIBUTES),
	  _type (t),
	  _loadTrap { 
		0, 
		"LOAD", 
		// Both the standard entry at 0x0556 and custom loaders entering at 0x0562
		// converge here after the CP A / RET NZ sequence has been executed.
		MCHEmul::Address ({ 0x6c, 0x05 }, false),
		MCHEmul::Address ({ 0xe2, 0x05 }, false), // The classical return!
	    {  } },
	  _blockRead (0)
{
	setClassName ("ZXSpectrumDatasetteInjection");
}

// ---
bool ZXSPECTRUM::DatasetteInjection::connectData (MCHEmul::FileData* dt)
{
	// There are two types of files admitted: 
	// TAP that is most standard way of injecting info into the memory...
	// ...and TZX when all blocks are like TAP!
	if (dynamic_cast <ZXSPECTRUM::TAPFileData*> (dt) == nullptr &&
		dynamic_cast <SINCLAIR::TZXFileData*> (dt) == nullptr)
		return (false); // That type of info is not valid from the datasette...
	if (dynamic_cast <SINCLAIR::TZXFileData*> (dt) != nullptr &&
		!static_cast <SINCLAIR::TZXFileData*> (dt) -> isOnlyLikeTAPInfo ())
		return (false);

	_data = std::move (dt -> asMemoryBlocks ());

	// Starting from the beginning...
	_blockRead = 0;
	
	return (true); 
}

// ---
bool ZXSPECTRUM::DatasetteInjection::executeCommand (int cId, const MCHEmul::Strings& prms)
{
	bool result = true;

	switch (cId)
	{
		case _KEYFOWARD:
			{
				if (_blockRead < (_data._data.size () - 1))
					_blockRead++;
			}

			break;

		case _KEYREWIND:
			{
				if (_blockRead > 0)
					_blockRead--;
			}

			break;

		case _KEYEJECT:
			{
				_data._data.clear ();

				_blockRead = 0;
			}

			break;

		// Command not valid, but the status doesn't change...
		default:
			result = false; 
			break;
	}

	return (result);
}

// ---
bool ZXSPECTRUM::DatasetteInjection::simulate (MCHEmul::CPU* cpu)
{
	if (cpu -> programCounter ().internalRepresentation () != _loadTrap._addressIn.value ())
		return (true);

	if (!validLoadTrapContext (cpu))
	{
		_IFDEBUG debugTrapNotApplied ("Unsupported ROM load routine");

		return (true);
	}

	if (_data._data.empty () || _blockRead >= _data._data.size ())
	{
		_IFDEBUG debugNothingToRead ();

		return (true);
	}

	_IFDEBUG debugSimulation (cpu);

	if (simulateTrap (cpu) == TrapResult::_NOTAPPLIED)
		return (true);

	// A simulated tape error still consumes the physical block, as the ROM
	// would already have read it before returning with carry reset.
	_blockRead++;

	// A standard ROM block ends with the cassette input at the high level.
	// This matters to custom loaders which sample EAR immediately afterwards.
	setRead (true);

	cpu -> programCounter ().setAddress (_loadTrap._addressOut);

	return (true);
}

// ---
bool ZXSPECTRUM::DatasetteInjection::validLoadTrapContext (MCHEmul::CPU* cpu) const
{
	if (_type != ZXSPECTRUM::Type::_STANDARD)
		return (false);

	const MCHEmul::UBytes loadSignature (std::vector <MCHEmul::UByte>
		{ 0xdb, 0xfe, 0x1f, 0xe6, 0x20, 0xf6, 0x02,
		  0x4f, 0xbf, 0xc0, 0xcd, 0xe7, 0x05 });

	return (cpu -> memoryRef () -> values
			(MCHEmul::Address ({ 0x62, 0x05 }, false), loadSignature.size ()) == loadSignature &&
		cpu -> memoryRef () -> value
			(MCHEmul::Address ({ 0xe2, 0x05 }, false)) == 0xc9);
}

// ---
MCHEmul::InfoStructure ZXSPECTRUM::DatasetteInjection::getInfoStructure () const
{
	MCHEmul::InfoStructure result =
		std::move (MCHEmul::DatasettePeripheral::getInfoStructure ());

	result.add ("TYPE", std::to_string ((int) _type)); // The type of computer...
	result.add ("NUMBERBLOCKS", _data._data.size ());
	result.add ("BLOCKTOREAD", _blockRead);
	result.add ("TRAP", _loadTrap.asString ());

	return (result);
}

ZXSPECTRUM::DatasetteInjection::TrapResult 
	ZXSPECTRUM::DatasetteInjection::simulateTrap (MCHEmul::CPU* cpu)
{
	struct CP1 final : public FZ80::CP_General
	{
		CP1 ()
			: FZ80::CP_General (0, 1, 1, { }, "CP1") // This info is not important...
				{  }

		bool justDo (FZ80::CZ80* cpu)
							{ _lastExecutionData._cpu = cpu; return (executeWith (MCHEmul::UByte::_1)); }

		// This instruction is never used, but it has to be defined...
		virtual bool executeImpl (bool &f) override
							{ return (false); }
	};

	if (_data._data.empty () || _data._data.size () <= _blockRead)
		return (TrapResult::_NOTAPPLIED);

	FZ80::CZ80* c = static_cast <FZ80::CZ80*> (cpu);
	MCHEmul::Register& aR		= c -> aRegister ();
	MCHEmul::Register& bR		= c -> bRegister ();
	MCHEmul::Register& cR		= c -> cRegister ();
	MCHEmul::RefRegisters& deR	= c -> deRegister ();
	MCHEmul::RefRegisters& ixR	= c -> ixRegister ();
	MCHEmul::RefRegisters& afpR	= c -> afpRegister ();
	MCHEmul::StatusRegister& st = c -> statusRegister ();

	_IFDEBUG debugStatus ("Entering Routine Simulation", c);

	const std::vector <MCHEmul::UByte>& dMB = _data._data [_blockRead].bytes ();
	unsigned short length = c -> valueFromRegisters (deR);
	if (dMB.size () != ((size_t) length + 2))
	{
		_IFDEBUG debugTrapNotApplied ("Partial or non-ROM tape block");

		return (TrapResult::_NOTAPPLIED);
	}

	MCHEmul::Address where = c -> addressFromRegisters (ixR);
	MCHEmul::UByte flag = afpR [0] -> values ()[0];
	bool verify = !afpR [1] -> values ()[0].bit (FZ80::CZ80::_CARRYFLAG);
	MCHEmul::UByte parity = dMB [0];
	MCHEmul::UByte lastByte = dMB [0];

	aR.set ({ 0x00 });
	c -> lRegister ().set ({ lastByte });

	// The original 48K ROM reads the flag but treats a zero length request
	// specially. Fuse preserves this ROM defect in its accelerated loader.
	if (length == 0)
	{
		bR.set ({ 0xb0 });
		aR.set ({ parity });
		CP1 ().justDo (c);
		cR.set ({ 0x01 });
		c -> hRegister ().set ({ parity });

		_IFDEBUG debugStatus ("Exiting zero-length ROM load simulation", c);

		return (TrapResult::_BLOCKCONSUMED);
	}

	// This is the alternate register state reached by the standard routine
	// after accepting the first flag byte.
	afpR [0] -> set ({ 0x01 });
	afpR [1] -> set ({ 0x45 });

	if (flag != dMB [0])
	{
		st.setBitStatus (FZ80::CZ80::_CARRYFLAG, false);
		cR.set ({ 0x01 });
		c -> hRegister ().set ({ parity });

		_IFDEBUG debugStatus ("Exiting simulation with a block flag mismatch", c);

		return (TrapResult::_BLOCKCONSUMED);
	}

	unsigned short processed = 0;
	bool verifyFailed = false;
	for (; processed < length; processed++)
	{
		MCHEmul::UByte tapeByte = dMB [(size_t) processed + 1];
		parity = parity ^ tapeByte;
		lastByte = tapeByte;

		if (verify)
		{
			if (c -> memoryRef () -> value (where + processed) != tapeByte)
			{
				verifyFailed = true;

				break;
			}
		}
		else
			c -> memoryRef () -> put (where + processed, tapeByte);
	}

	c -> setValueInRegisters (deR, length - processed);
	c -> setValueInRegisters (ixR, (unsigned short) (where + processed).value ());
	cR.set ({ 0x01 });
	c -> hRegister ().set ({ parity });
	c -> lRegister ().set ({ lastByte });

	if (verifyFailed)
	{
		st.setBitStatus (FZ80::CZ80::_CARRYFLAG, false);

		_IFDEBUG debugStatus ("Exiting simulation with a VERIFY mismatch", c);

		return (TrapResult::_BLOCKCONSUMED);
	}

	parity = parity ^ dMB [(size_t) length + 1];
	aR.set ({ parity });
	CP1 ().justDo (c);
	bR.set ({ 0xb0 });
	c -> hRegister ().set ({ parity });

	_IFDEBUG debugStatus (parity == 0x00
		? "Exiting ROM load simulation successfully"
		: "Exiting ROM load simulation with a checksum error", c);

	return (TrapResult::_BLOCKCONSUMED);
}

// ---
void ZXSPECTRUM::DatasetteInjection::debugSimulation (MCHEmul::CPU* cpu)
{
	assert (_deepDebugFile != nullptr);

	FZ80::CZ80* c = static_cast <FZ80::CZ80*> (cpu);
	const std::vector <MCHEmul::UByte>& dMB = _data._data [_blockRead].bytes ();
	MCHEmul::Address ix = c -> addressFromRegisters (c -> ixRegister ());
	MCHEmul::Address de (2, c -> valueFromRegisters (c -> deRegister ()));

	_deepDebugFile -> writeCompleteLine 
		("ZXSPECTRUMDN", 
		 cpu -> clockCycles (), 
		 "Reading block:",
			{ { "BLOCK:", std::to_string (_blockRead) },
			  { "NAME:", _data._data [_blockRead].name () },
			  { "TYPE:", _data._data [_blockRead].attribute ("TYPE") },
			  { "LENGTH:", _data._data [_blockRead].attribute ("DATALENGTH") },
			  { "ENTRY:", _loadTrap._addressIn.asString
				(MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2) },
			  { "EXPECTEDFLAG:", c -> apRegister ().values ()[0].asString
				(MCHEmul::UByte::OutputFormat::_HEXA, 2) },
			  { "ACTUALFLAG:", dMB.empty () ? "--" : dMB [0].asString
				(MCHEmul::UByte::OutputFormat::_HEXA, 2) },
			  { "MODE:", c -> fpRegister ().values ()[0].bit (FZ80::CZ80::_CARRYFLAG)
				? "LOAD" : "VERIFY" },
			  { "IX:", ix.asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2) },
			  { "DE:", de.asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2) } });
}

// ---
void ZXSPECTRUM::DatasetteInjection::debugStatus (const std::string& where, FZ80::CZ80* cpu)
{
	auto allRgsAsAttrs = [&](const MCHEmul::Registers& rgs) -> MCHEmul::Attributes
		{ MCHEmul::Attributes result;
		  for (const auto& i : rgs) 
			  result.insert (MCHEmul::Attributes::value_type (i.name (), i.asString ()));
		  return (result); };

	_deepDebugFile -> writeSimpleLine (where);
	_deepDebugFile -> writeCompleteLine ("ZXSPECTRUMDN", cpu -> clockCycles (), 
		"Registers:", allRgsAsAttrs (cpu -> internalRegisters ()));
}

// ---
void ZXSPECTRUM::DatasetteInjection::debugTrapNotApplied (const std::string& reason)
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeLineData ("Tape trap not applied at block " +
		std::to_string (_blockRead) + ": " + reason);
}

// ---
void ZXSPECTRUM::DatasetteInjection::debugNothingToRead ()
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeLineData ("No data to read...");
}
