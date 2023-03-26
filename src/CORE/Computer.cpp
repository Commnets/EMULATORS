#include <CORE/Computer.hpp>
#include <CORE/StdCommands.hpp>
#include <CORE/FmterBuilder.hpp>
#include <CORE/Formatter.hpp>
#include <sstream>

// ---
MCHEmul::Computer::Computer (MCHEmul::CPU* cpu, const MCHEmul::Chips& c, 
		MCHEmul::Memory* m, const MCHEmul::IODevices& d, unsigned int cs, const MCHEmul::Attributes& attrs)
	: MCHEmul::InfoClass ("Computer"),
	  _cpu (cpu), _chips (c), _memory (m), _devices (d), _attributes (attrs), 
	  _actionsAt (), _status (_STATUSRUNNING), _actionForNextCycle (_ACTIONNOTHING),
	  _exit (false), _restartAfterExit (false), _restartLevel (0), // Meaning full!
	  _debugLevel (MCHEmul::_DEBUGNOTHING),
	  _error (MCHEmul::_NOERROR),
	  _screen (nullptr), _sound (nullptr), _inputOSSystem (nullptr), _graphicalChip (nullptr),
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
		if (_sound == nullptr)
			_sound = dynamic_cast <MCHEmul::SoundSystem*> (i.second);
		if (_inputOSSystem == nullptr) 
			_inputOSSystem = dynamic_cast <MCHEmul::InputOSSystem*> (i.second);

		// In general...
		i.second -> linkToChips (_chips);
	}

	// These are mandatory...
	// But the sound system is not mandatory (there are many old computers with no sound...really? but possible)
	assert (_screen != nullptr && _inputOSSystem != nullptr);
}

// ---
MCHEmul::Computer::~Computer ()
{ 
	for (const auto& i : _devices)
		delete (i.second);

	for (const auto& i : _chips)
		delete (i.second);

	delete (_cpu); 

	delete (_memory);
}

// ---
MCHEmul::IOPeripherals MCHEmul::Computer::peripherals () const
{
	MCHEmul::IOPeripherals result;
	for (const auto& i : _devices)
	{
		MCHEmul::IOPeripherals r = i.second -> peripherals ();
		result.insert (r.begin (), r.end ());
	}

	return (result);
}

// ---
const MCHEmul::IOPeripheral* MCHEmul::Computer::peripheral (int id) const
{ 
	MCHEmul::IOPeripherals prh = std::move (peripherals ()); 
	MCHEmul::IOPeripherals::const_iterator i = prh.find (id); 
	return (i == prh.end () ? nullptr : (*i).second); 
}

// ---
bool MCHEmul::Computer::connectPeripheral (MCHEmul::IOPeripheral* p)
{
	assert (p != nullptr);

	bool result = false;
	
	for (MCHEmul::IODevices::const_iterator i = _devices.begin (); 
			i != _devices.end () && !result; i++)
		result = (*i).second -> connectPeripheral (p);
	
	return (result);
}

// ---
bool MCHEmul::Computer::disconnectPeripheral (int id)
{ 
	bool result = true; 
	for (const auto& i : _devices) 
		result &= i.second -> disconnectPeripheral (id); /** Try in all. */ 
	return (result);
}

// ---
bool MCHEmul::Computer::disconnectAllPeripherals ()
{ 
	bool result = true;
	for (const auto& i : _devices) 
		result &= i.second -> disconnectAllPeripherals (); 
	return (result);
}

// ---
bool MCHEmul::Computer::initialize (bool iM)
{
	_error = MCHEmul::_NOERROR;

	if (_screen == nullptr || _inputOSSystem == nullptr ||
		_graphicalChip == nullptr)
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	// Init the memory is optional...
	if (iM && !_memory -> initialize ())
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	if (!_cpu -> initialize ())
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

	startsComputerClock ();

	return (true);
}

// ---
bool MCHEmul::Computer::restart ()
{ 
	_exit = false; // No more...
	if (_restartLevel == 0)
		return (cpu () -> restart ());
	else if (_restartLevel == 1)
		return (initialize (false /** withou memory. */));
	return (initialize (true /** with memory. */));
}

// ---
bool MCHEmul::Computer::run ()
{
	_restartAfterExit = false;

	// This loop repeats while a restart order (_restartAfterExit == true) is defined...
	do 
	{
		_exit = false;

		// Is the computer commning from a restarting?
		if (_restartAfterExit)
		{
			// Once is enough...
			_restartAfterExit = false; 
			// To really continue the emulation the initialization has to be ok..
			_exit = !restart (); // This method should take into account the level
		}

		_error = MCHEmul::_NOERROR;
		_status = _STATUSRUNNING;
		_actionForNextCycle = _ACTIONNOTHING;
		_lastAction = _ACTIONNOTHING;

		startsComputerClock ();

		bool ok = true;
		while (ok && !_exit)
		{
			// Every cycle will be acountable of their own speed...
			ok &= runComputerCycle (/** no action. */);
			ok &= runIOCycle ();
		}
	} 
	while (_restartAfterExit);

	return (_error != MCHEmul::_NOERROR);
}

// ---
bool MCHEmul::Computer::runComputerCycle (unsigned int a)
{
	if (_exit)
	{ 
		_error = MCHEmul::_NOERROR;

		return (false); // Leave... exit is defined!
	}

	// If the Computer is running too quick, then the cycle is lost...
	if (_clock.tooQuick ())
	{ 
		countClockCycles (0);

		return (true); // The cycle was not executed, but everything went ok...
	}

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

	// After excuting the cycle, the number of cpu cycles are counted...
	countClockCycles (_cpu -> lastClockCycles ());

	return (true);
}

// ---
bool MCHEmul::Computer::runIOCycle ()
{
	for (const auto& i : _devices)
	{
		if (!i.second -> simulate (_cpu))
		{
			_exit = true;

			_error = MCHEmul::_DEVICE_ERROR;

			if (_debugLevel >= MCHEmul::_DEBUGERRORS)
				std::cout << "Error in IO device:" << std::endl << i.second << std::endl;

			return (false); // Error...
		}
	}

	_exit |= _inputOSSystem -> quitRequested ();

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
	MCHEmul::InfoStructure result = MCHEmul::InfoClass::getInfoStructure ();

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

