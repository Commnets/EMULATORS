#include <CORE/DatasettePeripherals.hpp>
#include <CORE/CPU.hpp>

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
	// If is not needed to ckeckit because it has been the one loaded at the beginning...

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
		dS.add (std::to_string (i), std::move (dSA));
	}

	result.add ("DATA", dS);

	return (result);
}

// ---
MCHEmul::InfoStructure MCHEmul::StandardDatasette::IOSimulation::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	result.add ("IOCOMMANDS", MCHEmul::concatenateStrings (commandDescriptions (), ", "));

	return (result);
}

// ---
void MCHEmul::StandardDatasette::IOSynchronous::initialize ()
{
	_firstCycleSimulation = true;

	_lastCPUCycles = _clockCycles = 0; 
}

// ---
bool MCHEmul::StandardDatasette::IOSynchronous::io (MCHEmul::StandardDatasette& dS, MCHEmul::CPU* cpu)
{
	if (_firstCycleSimulation)
	{
		_firstCycleSimulation = false;

		_lastCPUCycles = cpu -> clockCycles () - cpu -> lastCPUClockCycles ();
	}

	for (unsigned int i = (cpu -> clockCycles  () - _lastCPUCycles); 
		i > 0; i--)
	{
		if (++_clockCycles <= _runningSpeed)
			continue; // Not the time...
	
		// Start back...
		_clockCycles = 0; 

		dS.io ();
	}

	_lastCPUCycles = cpu -> clockCycles ();

	return (true);
}

// ---
bool MCHEmul::StandardDatasette::IOSynchronous::executeCommand
	(int id, const MCHEmul::Strings& prms)
{
	if (id == _KEYIOBASE)
		setRunningSpeed (std::atoi (prms [0].c_str ()));

	return (true);
}

// ---
MCHEmul::InfoStructure MCHEmul::StandardDatasette::IOSynchronous::getInfoStructure () const
{
	MCHEmul::InfoStructure result = 
		std::move (MCHEmul::StandardDatasette::IOSimulation::getInfoStructure ());

	result.add ("SPEED", _runningSpeed);

	return (result);
}

// ---
bool MCHEmul::StandardDatasette::IOASynchronous::io (MCHEmul::StandardDatasette& dS, MCHEmul::CPU* cpu)
{
	if (_process != nullptr)
		return (true);

	// If there was no process already running, 
	// creates and on and start the simulation in paralell...
	_thread = std::thread (&MCHEmul::StandardDatasette::IOASynchronous::Process::run, 
		_process = new MCHEmul::StandardDatasette::IOASynchronous::Process (dS, _motorSpeed));
	// The process continues because the thread variable is persistent!

	return (true);
}

// ---
void MCHEmul::StandardDatasette::IOASynchronous::stop ()
{
	if (_process != nullptr)
	{
		_process -> finish (); // The process finishes...
		
		_thread.join (); // The current thread (main), waits for the operation really finishes...
		
		delete (_process); // Now is safe to delete the process...
		
		_process = nullptr; // ...and prepare the system for the next!
	}
}

// ---
bool MCHEmul::StandardDatasette::IOASynchronous::executeCommand
	(int id, const MCHEmul::Strings& prms)
{
	if (id == _KEYIOBASE)
		setMotorSpeed (std::atoi (prms [0].c_str ()));

	return (true);
}

// ---
MCHEmul::InfoStructure MCHEmul::StandardDatasette::IOASynchronous::getInfoStructure () const
{
	MCHEmul::InfoStructure result = 
		std::move (MCHEmul::StandardDatasette::IOSimulation::getInfoStructure ());

	result.add ("SPEED", _motorSpeed);

	return (result);
}

// ---
MCHEmul::StandardDatasette::StandardDatasette 
		(int id, MCHEmul::StandardDatasette::IOSimulation* s, 
		 MCHEmul::StandardDatasette::IOEncoderDecoder* e, bool mI, const MCHEmul::Attributes& attrs)
	: MCHEmul::DatasettePeripheral (id, attrs),
	  _ioSimulation (s),
	  _ioEncoderDecoder (e),
	  _motorControlledInternally (mI),
	  _status (Status::_STOPPED),
	  _dataCounter (std::numeric_limits <size_t>::max ()), 
	  _elementCounter (0)
{
	assert (_ioSimulation != nullptr && 
			_ioEncoderDecoder != nullptr);

	setClassName ("StdDatasette");
}

// ---
MCHEmul::StandardDatasette::~StandardDatasette ()
{
	delete (_ioSimulation);

	delete (_ioEncoderDecoder);
}

// ---
bool MCHEmul::StandardDatasette::initialize ()
{
	_status = Status::_STOPPED;

	_dataCounter = (std::numeric_limits <size_t>::max ()); // No data loaded at the beginning (@see parent class method)
	_elementCounter = 0;

	_ioSimulation -> initialize ();

	return (MCHEmul::DatasettePeripheral::initialize ());
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
					if (++_dataCounter > _data._data.size ())
						_dataCounter = 0; // ...or to the first one if there is none else to point to...

					// No keys is supossed to be pressed...
					// if the user wanted to move to the next / previous element
					// the same command would have to be executed...
					setNoKeyPressed (true);
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
					if (--_dataCounter > _data._data.size ()) // it is an unsigned short...
						_dataCounter = _data._data.size (); //...or to after the last one if there is none else to point to...

					setNoKeyPressed (true);
				}
			}

			break;

		// STOPPED...
		case _KEYSTOP:
			{
				// If it was saving data, the counter is moved one position...
				if (_status == Status::_SAVING)
					_dataCounter++;

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
					_elementCounter = 0;

					_status = Status::_READING;

					_ioSimulation -> initialize ();

					// If there is no any further internal signal expected to start...
					if (!_motorControlledInternally)
						setMotorOff (false); // The motor starts...

					setNoKeyPressed (false);
				}
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

					// If the counter is pointing at the end, a null data element is added...
					if (_dataCounter > _data._data.size ())
					{ 
						_data._data.push_back (MCHEmul::DataMemoryBlock ());

						_dataCounter = _data._data.size () -1;
					}

					// ...and, any case, the element affected is fully clearead...
					// There shouldn't be here anything, but just in case!
					_data._data [_dataCounter].clear ();

					_ioSimulation -> initialize ();

					// If there is no any further internal signal expected to start...
					if (!_motorControlledInternally)
						setMotorOff (false); // The motor starts...

					setNoKeyPressed (false);
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

	// The simulation can also been affected!
	_ioSimulation -> executeCommand (id, prms);

	return (result);
}

// ---
bool MCHEmul::StandardDatasette::simulate (MCHEmul::CPU* cpu)
{
	if (motorOff ())
	{ 
		_ioSimulation -> stop ();

		return (true);
	}

	bool result = true;
	if (_status == Status::_READING ||
		_status == Status::_SAVING)
		result = _ioSimulation -> io (*this, cpu);
	else
		_ioSimulation -> stop ();

	additionalSimulateActions (cpu);

	return (result);
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
MCHEmul::InfoStructure MCHEmul::StandardDatasette::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::DatasettePeripheral::getInfoStructure ());

	result.add ("IOEMULATION", // All attributes together...
		std::move (_ioSimulation -> getInfoStructure ().asString ()));
	result.add ("DATACOUNTER", _dataCounter);

	return (result);
}
