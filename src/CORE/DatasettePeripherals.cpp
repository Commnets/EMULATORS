#include <CORE/DatasettePeripherals.hpp>
#include <CORE/CPU.hpp>
#include <CORE/LogChannel.hpp>

// ---
MCHEmul::DatasettePeripheral::DatasettePeripheral (int id, const MCHEmul::Attributes& attrs)
	: MCHEmul::IOPeripheral (id, attrs),
	  _valueRead (true),
	  _valueToWrite (false),
	  _motorOff (true),
	  _noKeyPressed (true),
	  _data (),
	  _motorChangeStatusRequest (false),
	  _readChangeValueRequest (false),
	  _writeChangeValueRequest (false),
	  _keysChangedStatusRequest (false)
{
	setClassName ("Datasette");

	// A very basic empty datasette is added at construction time...
	// ...with this name for the data!
	_data._name = "COMMTYNETS";
}

// ---
bool MCHEmul::DatasettePeripheral::initialize ()
{
	_valueRead = true;
	_valueToWrite = false;
	_motorOff = true;
	_noKeyPressed = true;

	clearData ();

	// Notice that the data is not initialized...
	// After being loaded it could be also usefull to keep it here many things...
	// even if the computer restarts!

	_motorChangeStatusRequest = false;
	_readChangeValueRequest = false;
	_writeChangeValueRequest = false;
	_keysChangedStatusRequest = false;

	return (true);
}

// ---
bool MCHEmul::DatasettePeripheral::connectData (MCHEmul::FileData* dt)
{
	if (dynamic_cast <MCHEmul::RawFileData*> (dt) == nullptr)
		return (false); // That type of info is not valid from the datasette...

	_data = std::move (dt -> asMemoryBlocks ());
	
	return (true); 
}

// ---
MCHEmul::FileData* MCHEmul::DatasettePeripheral::retrieveData () const
{
	// The format understood by this retrive method is just raw...
	MCHEmul::FileData* result = new MCHEmul::RawFileData; 
	MCHEmul::RawFileData* tap = dynamic_cast <MCHEmul::RawFileData*> (result);
	// If is not needed to ckeck it because it has been the one loaded at the beginning...

	// Later, when saving if any, the size in the name will be limited...
	tap -> _signature = _data._name;
	// ..the number of blocks...
	tap -> _dataBlocks = (unsigned int) _data._data.size ();
	// ...and the data...
	for (const auto& i : _data._data)
	{
		MCHEmul::RawFileData::Block dB;
		dB._name = "";
		dB._dataSize = (unsigned int) i.size ();
		dB._bytes = i.bytes ();
		tap -> _blocks.emplace_back (std::move (dB));
	}

	return (result);
}

// ---
MCHEmul::InfoStructure MCHEmul::DatasettePeripheral::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::IOPeripheral::getInfoStructure ());

	result.add ("KEYS",		!_noKeyPressed);
	result.add ("MOTOR",	!_motorOff);
	result.add ("DATANAME", (_data._name == "") ? "-" : _data._name);
	result.add ("DATASIZE", _data._data.size ());

	MCHEmul::InfoStructure dS;
	for (size_t i = 0; i < (size_t) _data._data.size (); i++)
	{
		MCHEmul::InfoStructure dSA;

		dSA.add ("ID",		i); // The id...
		dSA.add ("SIZE",	_data._data [i].size ());
		if (_data._data [i].bytes ().size () > 256)
		{
			dSA.add ("FBYTES", _data._data [i].bytes ()); // All bytes...
			dSA.add ("BYTES", std::vector <MCHEmul::UByte> 
				(_data._data [i].bytes ().begin (), _data._data [i].bytes ().begin () + 256));
		}
		else // This way because the type of method used varies...
			dSA.add ("BYTES", _data._data [i].bytes ());

		dS.add (MCHEmul::fixLenStr (std::to_string (i), 4, true, MCHEmul::_CEROS), 
			std::move (dSA));
	}

	result.add ("DATA", dS);

	return (result);
}

// ---
MCHEmul::StandardDatasette::StandardDatasette 
		(int id, MCHEmul::StandardDatasette::Implementation* i, 
		 bool mI, const MCHEmul::Attributes& attrs)
	: MCHEmul::DatasettePeripheral (id, attrs),
	  _implementation (i),
	  _motorControlledInternally (mI),
	  _status (Status::_STOPPED),
	  _dataCounter (std::numeric_limits <size_t>::max ()), 
	  _elementCounter (0),
	  _lastCPUCycles (0),
	  _firstTimeReading (false)
{
	assert (_implementation != nullptr);

	setClassName ("StdDatasette");
}

// ---
MCHEmul::StandardDatasette::~StandardDatasette ()
{
	delete (_implementation);
}

// ---
bool MCHEmul::StandardDatasette::initialize ()
{
	_status = Status::_STOPPED;

	_dataCounter = (std::numeric_limits <size_t>::max ()); // No data loaded at the beginning (@see parent class method)
	_elementCounter = 0;
	_lastCPUCycles = 0;
	_firstTimeReading = false;

	_implementation -> initialize (_status);

	return (MCHEmul::DatasettePeripheral::initialize ());
}

// ---
bool MCHEmul::StandardDatasette::connectData (MCHEmul::FileData* dt)
{
	bool result = MCHEmul::DatasettePeripheral::connectData (dt);

	if (result)
		_dataCounter = 
			(_data._data.empty () ? std::numeric_limits <size_t>::max () : 0); 
	// at the beginning or pointing to nowhere if there is no data finally loaded...

	return (result);
}

// ---
bool MCHEmul::StandardDatasette::executeCommand (int id, const MCHEmul::Strings& prms)
{
	bool result = true;

	// Only the allowed possibilities...
	// Most of them are possibe only when the datasette is stopped
	switch (id)
	{
		// FORWARD...
		case _KEYFOWARD:
			{
				if (_status == Status::_STOPPED && 
					!_data._data.empty ()) // There must be data inside...
				{
					// ...move the pointer to the next element in the list...
					// It can pointer to the one "after" the last!
					if (++_dataCounter > _data._data.size ())
						_dataCounter = 0; // ...or to the first one if there is none else to point to...
					_elementCounter = 0; // Always pointing to the first element of the block...

					// Just to be ready when the new block is reached!
					_implementation -> 
						whenReadingNewBlock (_data._data [_dataCounter]);

					// No keys is supossed to be pressed...
					// if the user wanted to move to the next / previous element
					// the same command would have to be executed...
					setNoKeyPressed (true);
				}
				else
				{
					result = false;

					_LOG ("No forward possible until datasette is stopped and has data inside");
				}
			}

			break;

		// REWIND...
		case _KEYREWIND:
			{
				if (_status == Status::_STOPPED &&
					!_data._data.empty ())
				{
					// ...move the pointer to the previous element in the list...
					// It can pointer to the one after the last too!
					if (--_dataCounter > _data._data.size ()) // it is an unsigned short...
						_dataCounter = (_data._data.size () - 1); //...or to after the last one if there is none else to point to...
					_elementCounter = 0; // Always pointing to the first element of the block...

					// Just to be ready when the new block is reached!
					_implementation -> 
						whenReadingNewBlock (_data._data [_dataCounter]);

					setNoKeyPressed (true);
				}
				else
				{
					result = false;

					_LOG ("No rewind possible until datasette is stopped and has data inside");
				}
			}

			break;

		// STOPPED...
		case _KEYSTOP:
			{
				// If it was saving data, the counter is moved one position forward...
				if (_status == Status::_SAVING)
				{
					_dataCounter++;
					// It will be then pointing to the next element
					// ...or to an empty space (@see _PLAY and _PLAY + _RECORD command)
					_elementCounter = 0;
				}

				_status = Status::_STOPPED;

				setNoKeyPressed (true);
			}

			break;

		// PLAY...
		case _KEYPLAY:
			{
				if (_status == Status::_STOPPED &&
					_dataCounter < _data._data.size ())
				{
					// Notice that the varible _elementCounter is not set back to 0
					// It means, that the casette continues reading from the last element...

					_status = Status::_READING;

					_implementation -> initialize (_status);

					_firstTimeReading = true;

					// If there is no any further internal signal expected to start...
					if (!_motorControlledInternally)
						setMotorOff (false); // The motor starts...

					// Just to be ready when the new block is reached!
					_implementation -> 
						whenReadingNewBlock (_data._data [_dataCounter]);

					setNoKeyPressed (false);
				}
				else
				{
					result = false;

					_LOG ("No play possible until datasette is stopped and has data inside");
				}

				// Notice that, if the pointer is set after the last one,
				// the reading operation won't work at all!
			}

			break;

		// RECORD...
		case (_KEYPLAY + _KEYRECORD):
			{
				// The change in the status is only possible when stopped previously...
				if (_status == Status::_STOPPED)
				{
					_elementCounter = 0;

					_status = Status::_SAVING;

					// If the counter is pointing at the end, 
					// a null data element is added...
					if (_dataCounter >= _data._data.size ())
					{ 
						_data._data.push_back (MCHEmul::DataMemoryBlock ());

						_dataCounter = _data._data.size () -1;
					}

					// ...and, any case, the element affected is fully clearead...
					// There shouldn't be here anything, but just in case!
					_data._data [_dataCounter].clear ();

					_implementation -> initialize (_status);

					// If there is no any further internal signal expected to start...
					if (!_motorControlledInternally)
						setMotorOff (false); // The motor starts...

					// In this case, the invocation to _implementation -> whenReadingNewBlock is not needed...
					// The system will have to take care of saving all variables associated to the block too!

					setNoKeyPressed (false);
				}
				else
				{
					result = false;

					_LOG ("No play + record possible until datasette is stopped");
				}
			}

			break;

		// EJECT...
		case _KEYEJECT:
			{
				// Data is forgetted...
				clearData ();

				// ...always stopped...
				_status = Status::_STOPPED;

				// and starts back from the beginning...
				_dataCounter = std::numeric_limits <size_t>::max ();

				_elementCounter = 0;

				setNoKeyPressed (true);
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
bool MCHEmul::StandardDatasette::simulate (MCHEmul::CPU* cpu)
{
	// First time?
	if (_lastCPUCycles == 0)
	{ 
		_lastCPUCycles = cpu -> clockCycles (); // Nothing to do...

		return (true);
	}

	// If the motor doesn't run, nothing to do...
	if (motorOff ())
	{
		_lastCPUCycles = cpu -> clockCycles ();

		return (true);
	}

	bool result = true; // Can be changed when reading...
	for (unsigned int i = 0; 
		 i < (cpu -> clockCycles () - _lastCPUCycles) && result; i++)
	{
		_implementation -> clock ();

		switch (_status)
		{
			case Status::_STOPPED:
				// Nothing else to do...
				break;

			// The Datasette is reading...
			case Status::_READING:
				{
					bool rNB = false; // Read a new value fromn the data file?
					if (_firstTimeReading) // Becomes false once it is used!
						rNB = true;
					else
					{
						// If it was time to read (tr), the value read will be in rv...
						// This value can be either 0 or 1 (false or true), and it will depend on the implementation!
						// The variable rV will be set to true when it were also time to read a new byte from the file.
						bool tr = false;
						bool vr = false;
						std::tie (tr, vr, rNB) = 
							_implementation -> timeToReadValue (_lastCPUCycles + i);
						if (tr)
							// Set the value read...
							// The IODevice simulation will use it to sent an event...
							// The way that event is managed will depend on the computer/element connected...
							setRead (vr);
					}

					// Reads a new value?
					if (rNB)
					{
						// Read the next value from the data file...
						bool e = false; // No error at the beginning...
						size_t pDC = _dataCounter;
						MCHEmul::UByte nVR = readFromData (e);
						// If there was a change in the block managed...
						if (_dataCounter != pDC)
							_implementation -> whenReadingNewBlock (_data._data [_dataCounter]);
						// The error variable will point out when the end of the data file was reached
						if (e) _LOG ("Error while reading data from the file");
						// The value is managed...
						_implementation -> whenValueRead (_lastCPUCycles + i, nVR);
					}
				}

				break;

			// The datasette is saving...
			case Status::_SAVING:
				{
					bool tw = false; // Time to write a new value?
					MCHEmul::UByte vw = MCHEmul::UByte::_0; // Which value (to write) in the file in that case (when previous = true)?
					std::tie (tw, vw) = 
						_implementation -> timeToWriteValue (_lastCPUCycles + i, 
							peekWriteChangeValueRequested (), clockCyclesWhenWriteAction (), valueToWrite ());
					if (tw)
					{
						// The change in the value has been already taken into account...
						writeChangeValueRequested ();
						// Store the value in the data file...
						storeInData (vw);
						// ...and what is necessary to do after the action...
						_implementation -> whenValueWritten (_lastCPUCycles + i, vw);
					}
				}

				break;
		}

		additionalSimulateActions (cpu, _lastCPUCycles + i);
	}

	_lastCPUCycles = cpu -> clockCycles ();

	return (result);
}

// ---
MCHEmul::InfoStructure MCHEmul::StandardDatasette::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::DatasettePeripheral::getInfoStructure ());

	result.add ("DATACOUNTER",		_dataCounter);
	result.add ("ELEMENTCOUNTER",	_elementCounter);

	return (result);
}
