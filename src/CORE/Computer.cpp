#include <CORE/Computer.hpp>
#include <CORE/StdCommands.hpp>
#include <CORE/FmterBuilder.hpp>
#include <CORE/Formatter.hpp>
#include <sstream>

// ---
MCHEmul::Computer::Computer (
		MCHEmul::CPU* cpu, 
		const MCHEmul::Chips& c, 
		MCHEmul::Memory* m, 
		const MCHEmul::IODevices& d, 
		unsigned int cs,
		const MCHEmul::Buses& bs,
		const MCHEmul::Wires& ws,
		const MCHEmul::Attributes& attrs, unsigned short sL)
	: MCHEmul::InfoClass ("Computer"),
	  _cpu (cpu), _chips (c), _memory (m), _devices (d), _attributes (attrs), 
	  _templateActions (), _actionsAt (), _status (_STATUSRUNNING), _actionForNextCycle (_ACTIONNOTHING),
	  _deepDebug (), // No active at all...
	  _exit (false), _restartAfterExit (false), _restartLevel (0), // Meaning full!
	  _debugLevel (MCHEmul::_DEBUGNOTHING),
	  _stabilizationLoops (sL),
	  _error (MCHEmul::_NOERROR),
	  _screen (nullptr), _sound (nullptr), _inputOSSystem (nullptr), _graphicalChip (nullptr),
	  _clock (cs), 
	  _lastAction (_ACTIONNOTHING),
	  _stabilized (false), _currentStabilizationLoops (0)
{ 
	assert (_cpu != nullptr);
	assert (_memory != nullptr && _memory -> stack () != nullptr);

	// Let's put all MotherboardElements with a list of them...
	// To add all of them to the buses and wires (if they accept!)...
	MotherboardElements mbE;
	
	mbE.emplace_back (_memory);

	_cpu -> setMemoryRef (_memory);

	for (const auto& i : _chips)
	{
		mbE.emplace_back (i.second);

		if (_graphicalChip == nullptr)
			_graphicalChip = dynamic_cast <MCHEmul::GraphicalChip*> (i.second);

		i.second -> setMemoryRef (_memory);
	}

	// To define a graphical chip is mandatory...
	assert (_graphicalChip != nullptr);

	for (const auto& i : _devices)
	{
		mbE.emplace_back (i.second);

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

	// Connect the motherboard elements to both the buses and wires...
	for (const auto& i : _buses)
		i.second -> connectElements (mbE);
	for (const auto& i : _wires)
		i.second -> connectElements (mbE);

	// Create the template of actions more common...
	// More actions can be added in later instances of the class computer!
	_templateActions =
		{ 
			{  _ACTIONNOTHING,	new MCHEmul::Computer::NoAction }, 
			{  _ACTIONCONTINUE,	new MCHEmul::Computer::ContinueAction }, 
			{  _ACTIONSTOP,		new MCHEmul::Computer::StopAction }, 
			{  _ACTIONNEXT,		new MCHEmul::Computer::NextCommandAction}
		};
}

// ---
MCHEmul::Computer::~Computer ()
{ 
	for (const auto& i : _buses)
		delete (i.second);

	for (const auto& i : _wires)
		delete (i.second);

	for (const auto& i : _devices)
		delete (i.second);

	for (const auto& i : _chips)
		delete (i.second);

	for (const auto& i : _templateActions)
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

	_clock.start ();

	_currentStabilizationLoops = 0;
	_stabilized = (_stabilizationLoops == _currentStabilizationLoops); // Can be stable from the beginning...

	// Put in a plain format the list of actions...
	unsigned int maxTAId = 0;
	_templateListActions = { };
	for (auto& i : _templateActions)
		if (i.second -> id () > maxTAId)
			maxTAId = i.second -> id ();
	_templateListActions = std::vector <MCHEmul::Computer::Action*> (maxTAId + 1, nullptr);
	for (auto& i : _templateActions)
		_templateListActions [i.second -> id ()] = i.second;

	return (true);
}

// ---
bool MCHEmul::Computer::restart ()
{ 
	_exit = false; // No more...
	if (_restartLevel == 0)
		return (cpu () -> restart ());
	else if (_restartLevel == 1)
		return (initialize (false /** without memory. */));
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

		_clock.start ();

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
		_clock.countCycles (0);

		return (true); // The cycle was not executed, but everything went ok...
	}

	unsigned int cI = 
		(cpu () -> ticksCounter () == nullptr) 
			? cpu () -> clockCycles ()
			: 0; // It doesn't matter... 

	// Maybe there will be some to be execute at the point where the CPU is now stopped
	// The action received in the loop is passed to the method to 
	// be considered in the decision in case of conflict!
	if (!executeActionAtPC (a))
	{
		_actionForNextCycle = _ACTIONNOTHING;

		return (true); // It has decided not to execute the cycle...
	}

	_actionForNextCycle = _ACTIONNOTHING;

	// The CPU is executed only when the computer is stable...
	if (_stabilized)
	{ 
		// The desativation of the deep debug is forbidden when
		// a instruction is about to be executed...
		_deepDebug.setBlockDesactivate ();
		bool rInst = _cpu -> executeNextCycle ();
		// If some instruction tried to desactivate the debug, now it is the moment...
		if (_deepDebug.desactivateTryWhenBlockDesativate ())
			_deepDebug.desactivate ();

		if (!rInst)
		{
			_exit = true;

			_error = _cpu -> error ();

			if (_debugLevel >= MCHEmul::_DEBUGERRORS)
				std::cout << "Error executing CPU Cycle:" << _error << std::endl;

			return (false); // Error...
		}
	}
	else
		_stabilized = (++_currentStabilizationLoops >= _stabilizationLoops);

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
				std::cout << "Error simulating chip: " << std::endl << *(i.second) << std::endl;

			return (false); // Error...
		}
	}

	// After excuting the cycle, 
	// the number of speed cycles are counted...
	_clock.countCycles ((_cpu -> ticksCounter () == nullptr) 
		? _cpu -> clockCycles () - cI
		: ((_cpu -> lastCPUClockCycles () == 0) 
			? 1 : _cpu -> lastCPUClockCycles ())
				/** When the clock is external to the CPU the incrementl in one by one (usually), or 
					if the last instruction took more than 1 (is very strange). */);

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
				std::cout << "Error in IO device:" << std::endl << *(i.second) << std::endl;

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
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	result.add ("ATTRS",	_attributes);
	result.add ("CPU",		std::move (_cpu -> getInfoStructure ()));

	MCHEmul::InfoStructure chps;
	for (const auto& i : _chips)
		chps.add (std::to_string (i.first), std::move (i.second -> getInfoStructure ()));
	result.add ("CHIPS", std::move (chps));

	MCHEmul::InfoStructure dvcs;
	for (const auto& i : _devices)
		dvcs.add (std::to_string (i.first), std::move (i.second -> getInfoStructure ()));
	result.add ("DEVICES", std::move (dvcs));

	return (result);
}

// ---
bool MCHEmul::Computer::executeActionAtPC (unsigned int a)
{
	// Select where there is an action at the address where the CPU is sopped now
	MCHEmul::Computer::MapOfActions::const_iterator at =
		_actionsAt.find (cpu () -> programCounter ().asAddress ());
	// Internal actions received has priority over the external ones...
	unsigned int na = (_actionForNextCycle != _ACTIONNOTHING) ? _actionForNextCycle : a;
	// ..and even over the ones defined at one specific point...
	unsigned int act = 
		(na != _ACTIONNOTHING)
			? na
			: (at == _actionsAt.end ()) 
				? _ACTIONNOTHING
				: (*at).second;

	// Get the action from the template...
	bool result = false;
	MCHEmul::Computer::Action* actPtr = nullptr;
	if (act < _templateListActions.size () && // Defined?
		(actPtr = _templateListActions [act]) != nullptr)
		result = actPtr -> execute (this);

	// Keep track of the last action executed...
	_lastAction = act;
	
	return (result);
}

