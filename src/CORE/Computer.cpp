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
	  _lastError (MCHEmul::_NOERROR),
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
	_lastError = MCHEmul::_NOERROR;

	if (_screen == nullptr || _inputOSSystem == nullptr ||
		_graphicalChip == nullptr)
	{
		_lastError = MCHEmul::_INIT_ERROR;

		return (false);
	}

	if (!_cpu -> initialize ())
	{
		_lastError = MCHEmul::_INIT_ERROR;

		return (false);
	}

	bool resultChips = true;
	for (const auto& i : chips ())
		resultChips &= i.second -> initialize ();
	if (!resultChips)
	{
		_lastError = MCHEmul::_INIT_ERROR;

		return (false);
	}

	bool resultIO = true;
	for (const auto& i : devices ())
		resultIO &= i.second -> initialize ();
	if (!resultIO)
	{
		_lastError = MCHEmul::_INIT_ERROR;

		return (false);
	}

	if (!_memory -> initialize ())
	{
		_lastError = MCHEmul::_INIT_ERROR;

		return (false);
	}

	_status = _STATUSRUNNING;
	_actionForNextCycle = _ACTIONNOTHING;

	return (true);
}

// ---
bool MCHEmul::Computer::run ()
{
	// It has to be initialized before...

	_exit = false;
	_lastError = MCHEmul::_NOERROR;

	bool ok = true;
	while (ok && !_exit)
	{
		startsCycle ();

		ok &= runComputerCycle (/** no action. */);
		ok &= runIOCycle ();
		
		finishCycle ();
	}

	return (_lastError != MCHEmul::_NOERROR);
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

		_lastError = MCHEmul::_CPU_ERROR;

		if (_debugLevel >= MCHEmul::_DEBUGERRORS)
			std::cout << "Error executing instruction" << std::endl;

		return (false); // Error...
	}

	if (_debugLevel >= MCHEmul::_DEBUGALL)
	{
		if (_cpu -> lastInstruction () != nullptr)
			std::cout << "->" << *_cpu -> lastInstruction () << std::endl;
		MCHEmul::CPUStatusCommand stCmd; MCHEmul::InfoStructure rst; stCmd.execute (nullptr, this, rst);
		std::cout << rst << std::endl;
	}

	for (const auto& i : _chips)
	{
		if (!i.second -> simulate (_cpu))
		{
			_exit = true;

			_lastError = MCHEmul::_CHIP_ERROR;

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

			_lastError = MCHEmul::_DEVICE_ERROR;

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
	_iClock = std::chrono::steady_clock ().now ();
}

// ---
void MCHEmul::Computer::Clock::waitFor (unsigned int cC)
{
	static const long long nanosc = (long long) 1.0e9;

	long long elapsed = std::chrono::duration_cast <std::chrono::nanoseconds> (std::chrono::steady_clock::now () - _iClock).count ();
	long long tElapsed = (long long) ((double) (cC - _initialClockCycles) / (double) _cyclesPerSecond * (double) nanosc);
	if (tElapsed > elapsed)
		std::this_thread::sleep_for (std::chrono::nanoseconds (tElapsed - elapsed));
}
