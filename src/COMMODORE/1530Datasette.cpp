#include <COMMODORE/1530Datasette.hpp>
#include <F6500/incs.hpp>

// ---
void COMMODORE::Datasette1530::TAPFileFormatImplementation::whenValueRead 
	(unsigned int cC, const MCHEmul::UByte& v)
{
	_cyclesToRead = (v == valueForNoMoreData ()) 
		? std::numeric_limits <unsigned int>::max () // No more 1 signals from the tape!
		: ((unsigned int) v.value ()) << 3;

	// To start back to count!
	resetClock ();
}

// ---
COMMODORE::Datasette1530::Datasette1530 (unsigned int cR)
	: MCHEmul::StandardDatasette (_ID, 
		new COMMODORE::Datasette1530::TAPFileFormatImplementation (cR), true /** Motor controlled internaly. */,
		{ { "Name", "Commodore 1530/1531 Synchronous (CN2)" },
		  { "Manufacturer", "Commodore Business Machines CBM" } })
{
	setClassName ("C2N1530");
}

// ---
bool COMMODORE::Datasette1530::connectData (MCHEmul::FileData* dt)
{
	// The parent class moves RAW type, 
	// that is not going to be valid for this datasette...

	if (dynamic_cast <COMMODORE::TAPFileData*> (dt) == nullptr)
		return (false); // That type of info is not valid from the datasette...

	_data = std::move (dt -> asMemoryBlocks ());
	_dataCounter = 
		(_data._data.empty () ? std::numeric_limits <size_t>::max () : 0 /** At the first element. */);
	_elementCounter = 0;
	
	return (true); 
}

// ---
MCHEmul::FileData* COMMODORE::Datasette1530::retrieveData () const
{
	COMMODORE::TAPFileData* result = 
		COMMODORE::TAPFileData::createFromMemoryBlock (_data);

	// Adds the name of the file...
	result -> _attributes ["FNAME"] = 
		MCHEmul::getAttribute ("FNAME", _data._attributes);

	// ...and return it!
	return (result);
}

// ---
MCHEmul::InfoStructure COMMODORE::Datasette1530Injection::Definition::getInfoStructure () const
{
	MCHEmul::InfoStructure result;

	result.add ("BUFFER", _bufferAddr);
	result.add ("STATUS", _statusAddr);
	result.add ("VERIFYFLAG", _verifyFlagAddr);
	result.add ("STARTPRG", _startProgramAddr);
	result.add ("ENDPRG", _endProgramAddr);
	result.add ("KEYBOARDBUFFER", _keyboardBufferAddr);
	result.add ("KEYBOARDPENDING", _keyboardPendingAddr);

	MCHEmul::InfoStructure trps;
	for (size_t i = 0; i < _traps.size (); i++)
		trps.add (std::to_string (i), _traps [i].asString ());
	result.add ("TRAPS", std::move (trps));

	return (result);
}

// ---
COMMODORE::Datasette1530Injection::Datasette1530Injection (const COMMODORE::Datasette1530Injection::Definition& def)
	: MCHEmul::StandardDatasette (_ID,
		new MCHEmul::StandardDatasette::NilImplementation, true,
		{ { "Name", "Datasette Injection 1530 (CN2)" },
		  { "Manufacturer", "ICF to inject the code directly into the memory" } }),
	  _definition (def)
{ 
	setClassName ("C2N1530Injection");
}

// ---
bool COMMODORE::Datasette1530Injection::connectData (MCHEmul::FileData* dt)
{
	if (dynamic_cast <COMMODORE::T64FileData*> (dt) == nullptr &&
		dynamic_cast <COMMODORE::PRGFileData*> (dt) == nullptr)
		return (false); // These formats are the only ones accepted only...

	_data = dt -> asMemoryBlocks ();

	_dataCounter = 0;
	_elementCounter = 0;

	return (true);
}

// ---
bool COMMODORE::Datasette1530Injection::simulate (MCHEmul::CPU* cpu)
{
	bool result = true;

	// What to do will depend on the simulation is...
	// Te definition can be in many different trap(
	// Detect first whether the simulation is or not in one of them and then execute the right thing...
	// There can not be more than 1 trap with the same name
	bool nF = false;
	for (MCHEmul::Traps::const_iterator i = _definition._traps.begin ();
		i != _definition._traps.end () && !nF; i++)
	{
		if (nF = (cpu -> programCounter ().internalRepresentation () == (*i)._addressIn.value () && 
			(*i).verifyFingerTipAgainst (cpu -> memoryRef ()))) // The fingertip has to be also "found"...
		{
			// Empty any pending set action...
			// Because a trap is breaking the normal flow of the code...
			// so no set commands pendings can be left!
			MCHEmul::Memory::configuration ().executeMemorySetCommandsBuffered ();

			// Execute the trap...
			result = executeTrap ((*i), cpu);

			cpu -> programCounter ().setAddress ((*i)._addressOut);
		}
	}

	return (result);
}

// ---
MCHEmul::InfoStructure COMMODORE::Datasette1530Injection::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::StandardDatasette::getInfoStructure ());

	result.add ("D1530InjectionDefinition", std::move (_definition.getInfoStructure ()));

	return (result);
}

// ---
bool COMMODORE::Datasette1530Injection::executeTrap (const MCHEmul::Trap& t, MCHEmul::CPU* cpu)
{
	bool result = true;

	// The CPU has to be from the F6500 family!
	assert (dynamic_cast <F6500::C6500*> (cpu) != nullptr);

	_IFDEBUG debugStatus (t.asString (), cpu);

	switch (t._id)
	{
		// When the loader is looking for a header...
		case _FINDHEADERTRAP:
			{
				_IFDEBUG debugHeaderFileFound (cpu);

				int e = 0;

				// Gets with the buffer is really located...
				// using and indirect address mode...
				// This info will be used many times later...
				MCHEmul::Address ctteBuffer (cpu -> memoryRef () -> values (_definition._bufferAddr, 2).reverse ());

				// If there is no data, makes no sense to progress...
				// ..ot the data counter is at the end of the datasette file...
				if (_data._data.empty () ||
					_dataCounter > (_data._data.size () - 1))
					e = 1;
				else
				{
					// Or if there is no any type of data of the type
					// that this traps needs...
					while (_data._data [_dataCounter].attribute ("TYPE") != "1" /** File record normal = 1 */
						&& _dataCounter < _data._data.size ())
						_dataCounter++;
					if (_dataCounter > _data._data.size ())
					{
						_dataCounter = 0; // Start back next time...

						e = 2;

						break;
					}
				}

				// Only if something usefull was found, the method continues...
				if (e == 0)
				{
					// Storing the header of the info found in the cassette buffer...
					MCHEmul::DataMemoryBlock& dtM = _data._data [_dataCounter];
					cpu -> memoryRef () -> put (ctteBuffer, 0x01 /** Machine type default value. */);
					cpu -> memoryRef () -> put (ctteBuffer + 1, MCHEmul::UBytes (dtM.startAddress ().bytes (), 
						false /** little - endian. */).bytes ());
					cpu -> memoryRef () -> put (ctteBuffer + 3, MCHEmul::UBytes (dtM.endAddress ().bytes (), false).bytes ());
					for (size_t i = 0; i < 16 /** No more than 16 bytes. */; i++)
						cpu -> memoryRef () -> put (ctteBuffer + 5 + i, 
							(i < dtM.name ().size ()) ? dtM.name ()[i] : MCHEmul::UByte::_0);
				}
				else
				if (e == 1)
				{
					cpu -> memoryRef () -> put (ctteBuffer, 0x05 /** End of casette. */);

					_IFDEBUG debugNothingToRead ();
				}
				else
					_IFDEBUG debugErrorTrap ();

				// Cleans up everthing...
				cpu -> memoryRef () -> put (_definition._statusAddr, MCHEmul::UByte::_0);
				cpu -> memoryRef () -> put (_definition._verifyFlagAddr, MCHEmul::UByte::_0);
				if (_definition._irqTmpAddr != MCHEmul::Address ())
					cpu -> memoryRef () -> put (_definition._irqTmpAddr, 
						{ (unsigned char) (_definition._irqVal & 0x00ff), 
						  (unsigned char) ((_definition._irqVal & 0xff00) >> 8) } /** Already in little - endian. */);

				// Before finishing, check whether STOP key was or not pressed
				// before this trap wwas reached...
				// If it was, the carry flag of the cpu is set to true, otherwise it is false...
				cpu -> statusRegister ().setBitStatus (F6500::C6500::_CARRYFLAG, false);
				for (unsigned char i = cpu -> memoryRef () ->
					value (_definition._keyboardPendingAddr).value (); i > 0; i--)
				{
					if (cpu -> memoryRef () -> value (_definition._keyboardBufferAddr + i - 1) == 0x03)
					{
						cpu -> statusRegister ().setBitStatus (F6500::C6500::_CARRYFLAG, true);

						break;
					}
				}

				// Finally the ZERO flag is set to true...
				// It is how the original kernal routine worked!
				cpu -> statusRegister ().setBitStatus (F6500::C6500::_ZEROFLAG, true);
			}

			break;

		// When there is data about to be loaded...
		case _RECEIVEDATATRAP:
			{
				_IFDEBUG debugDataFileFound (cpu);

				MCHEmul::Address start (cpu -> memoryRef () -> 
					values (_definition._startProgramAddr, 2).reverse ());
				MCHEmul::Address end (cpu -> memoryRef () -> 
					values (_definition._endProgramAddr, 2).reverse ());

				switch (static_cast <F6500::C6500*> (cpu) -> xRegister ().values () [0].value ())
				{
					case 0x0e:
						// This methd has been defined to read always the same number 
						// of bytes that it was expected!
						// VOther implementatoions (VICE e.g.) are prepared to read even files with defect...
						loadDataBlockInRAM (_data._data [_dataCounter], cpu);
						break;

					default:
						{
							_LOG ("Trap function type not supported yet:" + 
								std::to_string ((int) static_cast <F6500::C6500*> (cpu) -> xRegister ().values () [0].value ()));
							assert (false); // Just for compiling with debug on...
						}

						break;
				}

				if (_definition._irqTmpAddr != MCHEmul::Address ())
					cpu -> memoryRef () -> put (_definition._irqTmpAddr, 
						{ (unsigned char) (_definition._irqVal & 0x00ff), 
						  (unsigned char) ((_definition._irqVal & 0xff00) >> 8) } /** Already in little - endian. */);

				cpu -> memoryRef () -> put (_definition._statusAddr, 
					cpu -> memoryRef () -> value (_definition._statusAddr) | 0x40 /** EOF. */);

				cpu -> statusRegister ().setBitStatus (F6500::C6500::_CARRYFLAG, false);
				cpu -> statusRegister ().setBitStatus (F6500::C6500::_IRQFLAG, false);
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

// ---
void COMMODORE::Datasette1530Injection::debugHeaderFileFound (MCHEmul::CPU* cpu)
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeCompleteLine 
		("C2N1530DN", 
		 cpu -> clockCycles (), 
		 "Block Found:",
			{ { "BLOCK:", std::to_string (_dataCounter) },
			  { "TYPE:", _data._data [_dataCounter].attribute ("TYPE") } });
}

// ---
void COMMODORE::Datasette1530Injection::debugDataFileFound (MCHEmul::CPU* cpu)
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeCompleteLine 
		("C2N1530DN", 
		 cpu -> clockCycles (), 
		 "Block Read:",
			{ { "BLOCK:", std::to_string (_dataCounter) },
			  { "TYPE:", _data._data [_dataCounter].attribute ("TYPE") } });
}

// ---
void COMMODORE::Datasette1530Injection::debugStatus (const std::string& where, MCHEmul::CPU* cpu)
{
	auto allRgsAsAttrs = [&](const MCHEmul::Registers& rgs) -> MCHEmul::Attributes
		{ MCHEmul::Attributes result;
		  for (const auto& i : rgs) 
			  result.insert (MCHEmul::Attributes::value_type (i.name (), i.asString ()));
		  return (result); };

	_deepDebugFile -> writeSimpleLine (where);
	_deepDebugFile -> writeCompleteLine ("C2N1530DN", cpu -> clockCycles (), 
		"Registers:", allRgsAsAttrs (cpu -> internalRegisters ()));
}

// ---
void COMMODORE::Datasette1530Injection::debugErrorTrap ()
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeLineData ("Error reading block:" + std::to_string (_dataCounter));
}

// ---
void COMMODORE::Datasette1530Injection::debugNothingToRead ()
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeLineData ("No data to read...");
}
