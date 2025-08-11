#include <ZXSpectrum/Datasette.hpp>
#include <FZ80/CP.hpp>
#include <FZ80/XOR.hpp>

// ---
std::tuple <bool, bool, bool> ZXSPECTRUM::Datasette::DirectRecordingImplementation::timeToReadValue (unsigned int cC) 
{
	// Time to read?
	bool tr = false;
	// Which value is read?
	bool vr = false;
	// Get a new value from the file?
	bool nVR  = false;

	// Is is time for a new value?
	if (tr = (clockValue () >= _cyclesToAction))
	{
		// The value to read is codified into the _byteInfo. From MSB to LSB
		vr = _byteInfo.bit (_bitManaged);
		// ...but all bits have been processing,
		// and is time to read a neew value from the file?
		nVR = (_bitManaged == 0);
		// The bit to managed for the next iteration...
		_bitManaged = nVR ? 7 : _bitManaged - 1;

		// Time to restart...
		resetClock ();
	}

	return { tr, vr, nVR };
}

// ---
void ZXSPECTRUM::Datasette::DirectRecordingImplementation::whenReadingNewBlock (const MCHEmul::DataMemoryBlock& dB)
{
	// New parameters to sample...
	_cyclesToAction = (unsigned int) 
		MCHEmul::getAttributeAsInt ("TSTATESSAMPLE", dB.attributes ());
	// New information about the last bit in this block...
	_bitsLastByte = (unsigned int) 
		MCHEmul::getAttributeAsInt ("BITSUSEDLAST", dB.attributes ());

	// Restart the clock...
	resetClock ();
}

// ---
std::tuple <bool, MCHEmul::UByte> ZXSPECTRUM::Datasette::DirectRecordingImplementation::timeToWriteValue 
	(unsigned int cC, bool vc, unsigned int ccvc, bool vs)
{
	// Write a new value to the file?
	bool tw = false;
	// Which value must be written in that case?
	MCHEmul::UByte vw = MCHEmul::UByte::_0;

	// Is time to action?
	if (clockValue () >= _cyclesToAction)
	{
		// Shift left 1 and set the value of the bit 0 witjh the value vs...
		// As you notice, it doesn't mather whether the value in the EAR was or changed...
		// It saves at a specific pace!
		_byteInfo.shiftLeft (1).setBit (0, vs);
		// If the number of iterations (bits managed) is already 8 (or bigger)
		// it is time to write the value...
		// ...and the value is the one that has been managed for a while...
		if (tw = (++_bitManaged >= 8))
			vw = _byteInfo;

		// Restart the clock!
		resetClock ();
	}

	return { tw, vw };
}


// ---
void ZXSPECTRUM::Datasette::DirectRecordingImplementation::initialize 
	(MCHEmul::StandardDatasette::Status st)
{
	MCHEmul::StandardDatasette::Implementation::initialize (st);

	_byteInfo = MCHEmul::UByte::_0;

	_bitManaged = (st == MCHEmul::StandardDatasette::Status::_READING) 
		? 7 /** The MSB when reading. */ : 0; // The LSB in other cases...
}

// ---
ZXSPECTRUM::Datasette::Datasette (unsigned int rS)
	: MCHEmul::StandardDatasette (_ID, 
		new MCHEmul::StandardDatasette::NilImplementation, false /** Controlled externally. */,
		{ { "Name", "Datasette ZXSpectrum" },
		  { "Manufacturer", "Almost anyone, No special connector needed" } }),
	  _originalData () // Assigned when connectdata is executed...
{
	setClassName ("ZXSpectrumDatasette");
}

// ---
bool ZXSPECTRUM::Datasette::connectData (MCHEmul::FileData* dt)
{
	// The parent class moves RAW type, 
	// that is not going to be valid for this datasette...

	// Only TZX is admitted...
	// ...and everything is transformed into DirectRecording type of records...
	if (dynamic_cast <ZXSPECTRUM::TZXFileData*> (dt) == nullptr)
		return (false); // That type of info is not valid from the datasette...
	// But TZX is only admitted when all blocks talks about sampling info...
	if (!static_cast <ZXSPECTRUM::TZXFileData*> (dt) -> isSamplingInfo ())
		return (false);

	// MOves the data...
	_data = std::move (dt -> asMemoryBlocks ());

	// And sets the counter to start...
	_dataCounter = 
		(_data._data.empty () ? std::numeric_limits <size_t>::max () : 0 /** At the first element. */);
	_elementCounter = 0;
	
	return (true); 
}

// ---
MCHEmul::FileData* ZXSPECTRUM::Datasette::retrieveData () const
{
	ZXSPECTRUM::TZXFileData* result = 
		ZXSPECTRUM::TZXFileData::createFromMemoryBlock (_data);

	// Adds the name of the file...
	result -> _attributes ["FNAME"] = 
		MCHEmul::getAttribute ("FNAME", _originalData._attributes);

	// ...and returns it!
	return (result);
}

// ---
ZXSPECTRUM::DatasetteInjection::DatasetteInjection (ZXSPECTRUM::Type t)
	: MCHEmul::DatasettePeripheral (_ID, 
		{ { "Name", "Datasette Injection ZXSpectrum" },
		  { "Manufacturer", "ICF to inject the code directly into the memory" } }),
	  _type (t),
	  _loadTrap { 
		0, 
		"LOAD", 
		// The LDA-Bytes routine starts at 0x0556. 
	    // At this positión is when returning if BREAK key was pressed...
	    // As the data is going to be loaded quick!, pushing BREAK makes no sense...
		MCHEmul::Address ({ 0x56, 0x05 }, false), 
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
		dynamic_cast <ZXSPECTRUM::TZXFileData*> (dt) == nullptr)
		return (false); // That type of info is not valid from the datasette...
	if (dynamic_cast <ZXSPECTRUM::TZXFileData*> (dt) &&
		!static_cast <ZXSPECTRUM::TZXFileData*> (dt) -> isLikeTAPInfo ())
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
	if ((cpu -> programCounter ().internalRepresentation () == _loadTrap._addressIn.value ()) &&
		!_data._data.empty () && // there must be data inside...
		_blockRead < _data._data.size ()) // ...and the block to read must be right...
	{
		_IFDEBUG debugSimulation (cpu);

		// If everything was ok, then it is time for the next block!
		if (simulateTrap (cpu))
			_blockRead++;
		else
			_IFDEBUG debugErrorTrap ();
			
		cpu -> programCounter ().setAddress (_loadTrap._addressOut);
	}
	else
		_IFDEBUG debugNothingToRead ();

	return (true);
}

// ---
MCHEmul::InfoStructure ZXSPECTRUM::DatasetteInjection::getInfoStructure () const
{
	MCHEmul::InfoStructure result =
		std::move (MCHEmul::DatasettePeripheral::getInfoStructure ());

	result.add ("TYPE", std::to_string ((int) _type));
	result.add ("NUMBERBLOCKS", _data._data.size ());
	result.add ("BLOCKTOREAD", _blockRead);
	result.add ("TRAP", _loadTrap.asString ());

	return (result);
}

bool ZXSPECTRUM::DatasetteInjection::simulateTrap (MCHEmul::CPU* cpu)
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

	struct XORV final : public FZ80::XOR_General
	{
		XORV ()
			: FZ80::XOR_General (0, 1, 1, { }, "XOR") // This info is not important...
				{ }

		bool justDo (FZ80::CZ80* cpu, const MCHEmul::UByte& v)
							{ _lastExecutionData._cpu = cpu; return (executeWith (v)); }

		// This instruction is never used, but it has to be defined...
		virtual bool executeImpl (bool& f) override
							{ return (false); }
	};

	if (_data._data.empty () ||
		_data._data.size () <= _blockRead ||
		_data._data [_blockRead].size () < 2)
		return (true); // There is no data to load...

	// The main registers used here...
	FZ80::CZ80* c = static_cast <FZ80::CZ80*> (cpu);
	MCHEmul::Register& aR		= c -> aRegister ();
	MCHEmul::Register& bR		= c -> bRegister ();
	MCHEmul::Register& cR		= c -> cRegister ();
	MCHEmul::RefRegisters& deR	= c -> deRegister ();
	MCHEmul::RefRegisters& hlR	= c -> hlRegister ();
	MCHEmul::RefRegisters& ixR	= c -> ixRegister ();
	MCHEmul::RefRegisters& afpR	= c -> afpRegister ();
	MCHEmul::StatusRegister& st = c -> statusRegister ();

	_IFDEBUG debugStatus ("Entering Routine Simulation", c);

	const std::vector <MCHEmul::UByte>& dMB = 
		_data._data [_blockRead].bytes ();
	unsigned short lMB		= (unsigned short) dMB.size ();
	MCHEmul::UByte fMB		= dMB [0];
	unsigned short length	= c -> valueFromRegisters (deR);
	MCHEmul::Address where	= c -> addressFromRegisters (ixR);
	MCHEmul::UByte flag		= aR.values ()[0];

	if (flag != fMB)
	{
		XORV ().justDo (c, fMB);

		st.setBitStatus (FZ80::CZ80::_CARRYFLAG, false);

		_IFDEBUG debugStatus ("Exiting Routine with error. \
			The type of file requested and the one in the file don't match", c);
	}
	else
	{
		aR.set ({ flag });

		unsigned short ct = 0; // Counting the data loaded...
		MCHEmul::Address fA = where; // The address where the data will be stored from...
		for (; ct < length && ct < (lMB - 1); ct++)
		{
			c -> memoryRef () -> put (fA, dMB [(size_t) (ct + 1)]);
			XORV ().justDo (c, dMB [(size_t) (ct + 1)]); // Calculating the parity!
			fA = fA + 1;
		}

		// If the number of bytes loaded are just the ones indicated from the ROM...
		if (ct == length)
		{
			// The parity is calculated...
			// ...and the number finally stored in the a register should be 0!
			XORV ().justDo (c, dMB [(size_t) (ct + 1)]);
			// ...and this comparation is done when everything was ok...
			CP1 ().justDo (c);

			_IFDEBUG debugStatus ("Exiting Routine OK. \
				The data were well loaded", c);
		}
		// But if not...
		else
		{
			st.set ({ 0x50 }); // The flags are set to the error status...

			_IFDEBUG debugStatus ("Exiting Routine OK. \
				But not all data were well loaded", c);
		}

		c -> setValueInRegisters (deR, length - ct);
		c -> setValueInRegisters (ixR, (unsigned short) fA.value ());
	}

	return (true);
}

// ---
void ZXSPECTRUM::DatasetteInjection::debugSimulation (MCHEmul::CPU* cpu)
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeCompleteLine 
		("ZXSPECTRUMDN", 
		 cpu -> clockCycles (), 
		 "Reading block:",
			{ { "BLOCK:", std::to_string (_blockRead) },
			  { "NAME:", _data._data [_blockRead].name () },
			  { "TYPE:", _data._data [_blockRead].attribute ("TYPE") },
			  { "LENGTH:", _data._data [_blockRead].attribute ("DATALENGTH") } });
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
void ZXSPECTRUM::DatasetteInjection::debugErrorTrap ()
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeLineData ("Error reading block:" + std::to_string (_blockRead));
}

// ---
void ZXSPECTRUM::DatasetteInjection::debugNothingToRead ()
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeLineData ("No data to read...");
}
