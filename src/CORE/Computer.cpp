#include <CORE/Computer.hpp>
#include <CORE/StdCommands.hpp>
#include <CORE/FmterBuilder.hpp>
#include <CORE/Formatter.hpp>
#include <thread>
#include <sstream>

// ---
MCHEmul::Computer::Computer (MCHEmul::CPU* cpu, const MCHEmul::Chips& c, 
		MCHEmul::Memory* m, const MCHEmul::IODevices& d, unsigned int cs, const MCHEmul::Attributes& attrs)
	: MCHEmul::InfoClass ("Computer"),
	  _cpu (cpu), _chips (c), _memory (m), _devices (d), _attributes (attrs), 
	  _actionsAt (), _status (_STATUSRUNNING), _actionForNextCycle (_ACTIONNOTHING),
	  _exit (false), 
	  _debugLevel (MCHEmul::_DEBUGNOTHING),
	  _error (MCHEmul::_NOERROR),
	  _screen (nullptr), _inputOSSystem (nullptr), _graphicalChip (nullptr),
	  _clock (cs), 
	  _lastAction (_ACTIONNOTHING)
{ 
	assert (_cpu != nullptr);
	assert (_memory != nullptr && _memory -> stack () != nullptr);

	_cpu -> setMemoryRef (_memory);

	for (const auto& i : _chips)
	{
		if (_graphicalChip == nullptr)
			_graphicalChip = dynamic_cast <MCHEmul::GraphicalChip*> (i.second);

		i.second -> setMemoryRef (_memory);
	}

	// To define a graphical chip is mandatory...
	assert (_graphicalChip != nullptr);

	for (const auto& i : _devices)
	{
		if (_screen == nullptr) 
			_screen = dynamic_cast <MCHEmul::Screen*> (i.second);
		if (_inputOSSystem == nullptr) 
			_inputOSSystem = dynamic_cast <MCHEmul::InputOSSystem*> (i.second);

		// In general...
		i.second -> linkToChips (_chips);
	}

	// These are mandatory...
	assert (_screen != nullptr && _inputOSSystem != nullptr);
}

// ---
MCHEmul::Computer::~Computer ()
{ 
	delete (_cpu); 

	delete (_memory);

	for (const auto& i : _chips)
		delete (i.second);

	for (const auto& i : _devices)
		delete (i.second);
}

// ---
bool MCHEmul::Computer::initialize ()
{
	_error = MCHEmul::_NOERROR;

	if (_screen == nullptr || _inputOSSystem == nullptr ||
		_graphicalChip == nullptr)
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	if (!_cpu -> initialize ())
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	if (!_memory -> initialize ())
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	bool resultChips = true;
	for (const auto& i : chips ())
		resultChips &= i.second -> initialize ();
	if (!resultChips)
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	bool resultIO = true;
	for (const auto& i : devices ())
		resultIO &= i.second -> initialize ();
	if (!resultIO)
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	_status = _STATUSRUNNING;
	_actionForNextCycle = _ACTIONNOTHING;
	_lastAction = _ACTIONNOTHING;

	return (true);
}

// ---
bool MCHEmul::Computer::run ()
{
	// It has to be initialized before...

	_exit = false;
	_error = MCHEmul::_NOERROR;
	_status = _STATUSRUNNING;
	_actionForNextCycle = _ACTIONNOTHING;
	_lastAction = _ACTIONNOTHING;

	startsComputerClock ();

	bool ok = true;
	while (ok && !_exit)
	{
		ok &= runComputerCycle (/** no action. */);
		ok &= runIOCycle ();
		
		finishCycle ();
	}

	return (_error != MCHEmul::_NOERROR);
}

// ---
bool MCHEmul::Computer::runComputerCycle (unsigned int a)
{
	MCHEmul::Computer::MapOfActions::const_iterator at =
		_actionsAt.find (cpu () -> programCounter ().asAddress ());
	if (!executeAction (
		_lastAction /* can be modified within the method. */,
		(at == _actionsAt.end ()) ? _ACTIONNOTHING : (*at).second, /** What is defined in the address where the PC is on. */
		(_actionForNextCycle != _ACTIONNOTHING) ? _actionForNextCycle : a /** If there is something pending to be processed it has priority. */))
	{
		_actionForNextCycle = _ACTIONNOTHING;

		return (true); // It has decided not to execute the cycle...
	}

	_actionForNextCycle = _ACTIONNOTHING;

	if (!_cpu -> executeNextInstruction ())
	{
		_exit = true;

		_error = MCHEmul::_CPU_ERROR;

		if (_debugLevel >= MCHEmul::_DEBUGERRORS)
			std::cout << "Error executing instruction" << std::endl;

		return (false); // Error...
	}

	if (_debugLevel >= MCHEmul::_DEBUGALL)
	{
		MCHEmul::InfoStructure rst;
		MCHEmul::CPUSimpleStatusCommand stCmd; stCmd.execute (nullptr, this, rst);
		std::cout << MCHEmul::FormatterBuilder::instance () -> formatter ("DEBUGCPUINFO") -> format (rst) << std::endl;
	}

	for (const auto& i : _chips)
	{
		if (!i.second -> simulate (_cpu))
		{
			_exit = true;

			_error = MCHEmul::_CHIP_ERROR;

			if (_debugLevel >= MCHEmul::_DEBUGERRORS)
				std::cout << "Error simulating chip: " << std::endl << i.second << std::endl;

			return (false); // Error...
		}
	}

	return (true);
}

// ---
bool MCHEmul::Computer::runIOCycle ()
{
	for (const auto& i : _devices)
	{
		if (!i.second -> simulate ())
		{
			_exit = true;

			_error = MCHEmul::_DEVICE_ERROR;

			if (_debugLevel >= MCHEmul::_DEBUGERRORS)
				std::cout << "Error in IO device:" << std::endl << i.second << std::endl;

			return (false); // Error...
		}
	}

	_exit = _inputOSSystem -> quitRequested ();

	return (true);
}

// ---
void MCHEmul::Computer::addAction (const MCHEmul::Address& at, unsigned int a)
{
	MCHEmul::Computer::MapOfActions::iterator i = _actionsAt.find (at);
	if (i == _actionsAt.end ()) _actionsAt.insert (MCHEmul::Computer::MapOfActions::value_type (at, a));
	else (*i).second = a;
}

// ---
void MCHEmul::Computer::removeAction (const MCHEmul::Address& at)
{
	MCHEmul::Computer::MapOfActions::iterator i = _actionsAt.find (at);
	if (i != _actionsAt.end ()) 
		_actionsAt.erase (i);
}

// ---
void MCHEmul::Computer::removeAllActions (unsigned int a)
{
	for (MCHEmul::Computer::MapOfActions::iterator i = _actionsAt.begin (); i != _actionsAt.end ();)
	{
		if ((*i).second == a)
			_actionsAt.erase (i++);
		else
			++i;
	}
}

// ---
MCHEmul::InfoStructure MCHEmul::Computer::getInfoStructure () const
{
	MCHEmul::InfoStructure result;

	result.add ("ATTRS", _attributes);
	result.add ("CPU", _cpu -> getInfoStructure ());

	MCHEmul::InfoStructure chps;
	for (const auto& i : _chips)
		chps.add (std::to_string (i.first), i.second -> getInfoStructure ());
	result.add ("CHIPS", chps);

	MCHEmul::InfoStructure dvcs;
	for (const auto& i : _devices)
		dvcs.add (std::to_string (i.first), i.second -> getInfoStructure ());
	result.add ("DEVICES", dvcs);

	return (result);
}

// ---
bool MCHEmul::Computer::executeAction (unsigned int& lA, unsigned int at, unsigned int a)
{
	// The action passed as parameter to the runCycle method has priority 
	// over the one defined at the address where the program counter is now on
	unsigned int act = (a != _ACTIONNOTHING) ? a : at;

	unsigned int lS = _status;

	switch (lA)
	{
		// Nothing was executed before...
		case _ACTIONNOTHING:
			switch (act)
			{
				case _ACTIONSTOP:
					_status = _STATUSSTOPPED;
					break;

				case _ACTIONCONTINUE:
				case _ACTIONNEXT:
					_status = _STATUSRUNNING;
					break;

				default:
					break;
			}

			break;

		// The last requested action was to stop...
		case _ACTIONSTOP:
			switch (act)
			{
				case _ACTIONNOTHING:
				case _ACTIONSTOP:
					_status = _STATUSSTOPPED;
					break;

				case _ACTIONCONTINUE:
				case _ACTIONNEXT:
					_status = _STATUSRUNNING;
					break;
			}

			break;

		// The last requested action was to go on...
		case _ACTIONCONTINUE:
			switch (act)
			{
				case _ACTIONNOTHING:
				case _ACTIONCONTINUE:
				case _ACTIONNEXT:
					_status = _STATUSRUNNING;
					break;

				case _ACTIONSTOP:
					_status = _STATUSSTOPPED;
					break;
			}

			break;

		case _ACTIONNEXT:
			switch (act)
			{
				case _ACTIONNOTHING:
				case _ACTIONSTOP:
					_status = _STATUSSTOPPED;
					break;

				case _ACTIONCONTINUE:
				case _ACTIONNEXT:
					_status = _STATUSRUNNING;
			}
			break;
	}

	// Keep track of the last action executed...
	lA = act;

	// The execution of the next instruction is allowed if the status is = _STATUSRUNNING...
	return (_status == _STATUSRUNNING);
}

// ---
void MCHEmul::Computer::Clock::start (unsigned int cC)
{
	_initialClockCycles = cC;

	_realCyclesPerSecond = 0;

	_iClock = std::chrono::steady_clock ().now ();
}

// ---
void MCHEmul::Computer::Clock::waitFor (unsigned int cC)
{
	static const long long nanosc = (long long) 1.0e9;

	long long elapsed = (std::chrono::steady_clock::now () - _iClock).count ();

	// If after 1 second or more...
	if (elapsed > nanosc)
	{
		// The number of clock cycles executed was bigger 
		// than the max number of cycles allowed for this CPU
		// It would be needed to wait for a while...
		_realCyclesPerSecond = cC - _initialClockCycles;
		if (_realCyclesPerSecond > _cyclesPerSecond)
			std::this_thread::sleep_for (std::chrono::nanoseconds ((long long) 
				(_realCyclesPerSecond - _cyclesPerSecond) * (elapsed - nanosc) / _realCyclesPerSecond));

		// ...and then starts back to count...
		_iClock = std::chrono::steady_clock::now ();
		_initialClockCycles = cC;
	}
}
