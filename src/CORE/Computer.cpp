#include <CORE/Computer.hpp>
#include <CORE/StdCommands.hpp>
#include <CORE/FmterBuilder.hpp>
#include <CORE/Formatter.hpp>
#include <CORE/LogChannel.hpp>
#include <CORE/Stack.hpp>
#include <sstream>

// ---
bool MCHEmul::Computer::CompositeAction::execute (MCHEmul::Computer* c)
{
	bool result = true;
	for (auto& i : _actions)
		result &= i -> execute (c);
	return (result);
}

// ---
bool MCHEmul::Computer::NoAction::execute (MCHEmul::Computer* c)
{
	// What to do now will depend on was the last action was...
	switch (c -> _lastAction)
	{
		// The status doesn't change as there is nothing else to do...
		case MCHEmul::Computer::_ACTIONNOTHING:
			break;

		// After stopping or executing a "next command" the CPU maintains stopped...
		// This situation will become the first in the next cycle....
		case MCHEmul::Computer::_ACTIONSTOP:
		case MCHEmul::Computer::_ACTIONNEXT:
			{
				c -> _status = MCHEmul::Computer::_STATUSSTOPPED;
			}

			break;

		// In this case, if after executing the "next stack command" the position of the
		// stack is the same than after the beginning, the CPU stops, any other case
		// it continues working executing another action...
		case MCHEmul::Computer::_ACTIONNEXTSTACK:
			{
				MCHEmul::Computer::NextStackCommandAction* act = 
					static_cast <MCHEmul::Computer::NextStackCommandAction*> 
						(c -> _templateListActions [_ACTIONNEXTSTACK]);
				if (act -> initialPosition () == c -> memory () -> stack () -> position ())
				{
					c -> _status = MCHEmul::Computer::_STATUSSTOPPED;

					// Ready for a similar command...
					act -> init ();
				}
				else
				{ 
					c -> _status = MCHEmul::Computer::_STATUSRUNNING;

					// Bear in mind, that this is the only situation
					// where the _actionForNextCycle is set to another value...
					c -> _actionForNextCycle = MCHEmul::Computer::_ACTIONNEXTSTACK;
				}
			}

			break;

		// In this case, keep running.
			// This situation will become the first in the next cycle...
		case MCHEmul::Computer::_ACTIONCONTINUE:
			{ 
				c -> _status = MCHEmul::Computer::_STATUSRUNNING;
			}

			break;
	}

	return (c -> _status == MCHEmul::Computer::_STATUSRUNNING);
}

// ---
bool MCHEmul::Computer::StopAction::execute (MCHEmul::Computer* c)
{
	c -> _status = MCHEmul::Computer::_STATUSSTOPPED;

	return (false);
}

// ---
bool MCHEmul::Computer::ContinueAction::execute (MCHEmul::Computer* c)
{
	c -> _status = MCHEmul::Computer::_STATUSRUNNING;

	return (true);
}

// ---
bool MCHEmul::Computer::NextCommandAction::execute (MCHEmul::Computer* c)
{
	c -> _status = MCHEmul::Computer::_STATUSRUNNING;

	// When the execution is done step by step...
	// ...flusing the memory is better to trace what happens (if configured)
	// Otherwise misunderstandings could happen...
	c -> memory () -> configuration ().executeMemorySetCommandsBuffered ();

	return (true);
}

// ----
bool MCHEmul::Computer::NextStackCommandAction::execute (MCHEmul::Computer* c)
{
	// Meaning not initialized...
	if (_initialPosition == -1) 
	{
		_initialPosition = c -> memory () -> stack () -> position ();

		c -> _status = MCHEmul::Computer::_STATUSRUNNING;
	}
	// But if it was, the status will depend on whether 
	// the stack position is or not what it was at the beginning...
	else
		c -> _status = 
			(_initialPosition == c -> memory () -> stack () -> position ())
				? MCHEmul::Computer::_STATUSSTOPPED
				: MCHEmul::Computer::_STATUSRUNNING;

	c -> memory () -> configuration ().executeMemorySetCommandsBuffered ();

	return (c -> _status == MCHEmul::Computer::_STATUSRUNNING);
}

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
	  _templateActions (), _actionsAt (), 
	  _status (MCHEmul::Computer::_STATUSRUNNING), _actionForNextCycle (MCHEmul::Computer::_ACTIONNOTHING),
	  _deepDebug (), // No active at all...
	  _exit (false), _restartAfterExit (false), _restartLevel (0), // Meaning full!
	  _debugLevel (MCHEmul::_DEBUGNOTHING),
	  _stabilizationLoops (sL),
	  _startAtSpecificAddress (false), _startingAddress (), // By default the one defined by the CPU...
	  _startStopped (false),
	  _error (MCHEmul::_NOERROR),
	  _screen (nullptr), _sound (nullptr), _inputOSSystem (nullptr), _graphicalChip (nullptr),
	  _plainChips (nullptr), _plainDevices (nullptr),
	  _clock (cs), 
	  _lastAction (MCHEmul::Computer::_ACTIONNOTHING),
	  _stabilized (false), _currentStabilizationLoops (0),
	  _templateListActions ()
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
			{ MCHEmul::Computer::_ACTIONNOTHING,	new MCHEmul::Computer::NoAction }, 
			{ MCHEmul::Computer::_ACTIONCONTINUE,	new MCHEmul::Computer::ContinueAction }, 
			{ MCHEmul::Computer::_ACTIONSTOP,		new MCHEmul::Computer::StopAction }, 
			{ MCHEmul::Computer::_ACTIONNEXT,		new MCHEmul::Computer::NextCommandAction },
			{ MCHEmul::Computer::_ACTIONNEXTSTACK,	new MCHEmul::Computer::NextStackCommandAction }
		};

	// Generate the plain vision of both chips and devices
	// just to speed up the executio of the code...
	// The plain version is used within the methods runXXX
	_plainChips = new MCHEmul::Chip* [_chips.size ()];
	size_t nC = 0; for (const auto& i : _chips) _plainChips [nC++] = i.second;
	_plainDevices = new MCHEmul::IODevice* [_devices.size ()];
	size_t nD = 0; for (const auto& i : _devices) _plainDevices [nD++] = i.second;
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

	// See that [] is not used in the delete
	// as the members will haven been already destroyed at this point...
	// We want just to delete the array!
	delete _plainChips;
	delete _plainDevices;
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
	_actionForNextCycle = MCHEmul::Computer::_ACTIONNOTHING;
	_lastAction = MCHEmul::Computer::_ACTIONNOTHING;

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
	static_cast <MCHEmul::Computer::NextStackCommandAction*> 
		(_templateListActions [MCHEmul::Computer::_ACTIONNEXTSTACK]) -> init (); // To be sure...

	// Does the computer have to start stopped?
	if (_startStopped)
		_actionForNextCycle = MCHEmul::Computer::_ACTIONSTOP;

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
		_actionForNextCycle = MCHEmul::Computer::_ACTIONNOTHING;
		_lastAction = MCHEmul::Computer::_ACTIONNOTHING;

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
	// NOTE: The variable _actionForNextCycle is set to _ACTIONNOTHING by default...
	// ...but some specific action when executed could set another value.
	if (!executeActionAtPC (a))
	{
		MCHEmul::Memory::configuration ().executeMemorySetCommandsBuffered ();

		specificComputerCycle (); // Just in case...

		return (true); // It has decided not to execute the cycle, 
					   // however the memory actions are unbufefred if any! (to trace the program properly)...
	}

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
				_LOG ("Error executing CPU Cycle (Intruction or Interrupt)");

			return (false); // Error...
		}

		specificComputerCycle ();
	}
	else
	{
		// While the computer start the configuration of the memory could even change
		// As the simulation is not a pure realtime emulation, the stabilization could take time
		// let's say a couple of cyles. Once the stabiliation finishes, the better is to reload the program counter
		// that will take into account the new configuration of the memory if changes happened!
		if (_stabilized = (++_currentStabilizationLoops >= _stabilizationLoops))
		{
			if (_startAtSpecificAddress)
				_cpu -> programCounter ().setAddress (_startingAddress);
			else
				_cpu -> restartPC (); 
		}
	}

	if (_debugLevel >= MCHEmul::_DEBUGALL)
	{
		MCHEmul::InfoStructure rst;
		MCHEmul::CPUSimpleStatusCommand stCmd; stCmd.execute (nullptr, this, rst);
		_LOG ("\n+\n" + // Every instruction executed is debugged...
			MCHEmul::FormatterBuilder::instance () -> formatter ("DEBUGCPUINFO") -> format (rst));
	}

	for (size_t i = 0; i < _chips.size (); i++)
	{
		if (!_plainChips [i] -> simulate (_cpu))
		{
			_exit = true;

			_error = MCHEmul::_CHIP_ERROR;

			if (_debugLevel >= MCHEmul::_DEBUGERRORS)
				_LOG ("Error simulating chip:\n" + 
						_plainChips [i] -> getInfoStructure ().asString ());

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

	// Every cycle is separated with a +....
	if (deepDebug ())
		_deepDebug.writeSimpleLine ("+");

	return (true);
}

// ---
bool MCHEmul::Computer::runIOCycle ()
{
	for (size_t i = 0; i < _devices.size (); i++)
	{
		if (!_plainDevices [i] -> simulate (_cpu))
		{
			_exit = true;

			_error = MCHEmul::_DEVICE_ERROR;

			if (_debugLevel >= MCHEmul::_DEBUGERRORS)
				_LOG ("Error in IO device:\n" + 
					_plainDevices [i] -> getInfoStructure ().asString ());

			return (false); // Error...
		}
	}

	_exit |= _inputOSSystem -> quitRequested ();

	return (true);
}

// ---
bool MCHEmul::Computer::activateDeepDebug 
	(const std::string& fN, 
		const MCHEmul::Address& d1, const MCHEmul::Address& d2,
		bool cpud, 
		const std::vector <int>& cId,
		const std::vector <int>& iId, const std::vector <int>& mId, bool a)
{ 
	bool result = _deepDebug.activate (fN, a);

	if (result)
	{ 
		if (cpud)
		{
			cpu () -> setDeepDebugFile (&_deepDebug); /** Minimum the CPU is activated. */
			cpu () -> setDeepDebugLimits (d1, d2);
		}

		// For the chips...
		// if there is only one element in the list and it is a -1, means all!
		if (cId.size () == 1 && cId [0] == -1)
		{
			for (auto& i : _chips)
				i.second -> setDeepDebugFile (&_deepDebug);
		}
		else
		{
			for (auto i : cId)
				if (existsChip (i))
					chip (i) -> setDeepDebugFile (&_deepDebug);
		}

		// For the peripherals...
		// if there is only one element in the list and it is a -1, means all!
		if (iId.size () == 1 && iId [0] == -1)
		{
			for (auto& i : _devices)
				i.second -> setDeepDebugFile (&_deepDebug);
		}
		else
		{
			for (auto i : iId)
				if (existsDevice (i))
					device (i) -> setDeepDebugFile (&_deepDebug);
		}

		// For the memory...
		// if there is only one element in the list and it is a -1, means all!
		_memory -> setDeepDebugFile (&_deepDebug, mId);
	}

	return (result); 
}

// ---
bool MCHEmul::Computer::desactivateDeepDebug ()
{ 
	bool result = _deepDebug.desactivate ();

	if (result)
	{ 
		cpu () -> setDeepDebugFile (nullptr);
			
		for (auto& i : _chips)
			i.second -> setDeepDebugFile (nullptr);
			
		for (auto& i : _devices)
			i.second -> setDeepDebugFile (nullptr);

		_memory -> setDeepDebugFile (nullptr, { });
	}

	return (result); 
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

	result.add ("ATTRS",			_attributes);
	result.add ("CPU",				std::move (_cpu -> getInfoStructure ()));
	result.add ("CLK",				std::move (_clock.asString ()));
	result.add ("MemoryStructure",	std::move (_cpu -> memoryRef () -> dumpStructure ().getInfoStructure ()));
	result.add ("Stack",			std::move (_cpu -> memoryRef () -> stack () -> getInfoStructure ()));

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
	unsigned int na = (_actionForNextCycle != MCHEmul::Computer::_ACTIONNOTHING) ? _actionForNextCycle : a;
	// ..and even over the ones defined at one specific point...
	unsigned int act = 
		(na != MCHEmul::Computer::_ACTIONNOTHING)
			? na
			: (at == _actionsAt.end ()) 
				? MCHEmul::Computer::_ACTIONNOTHING
				: (*at).second;

	// By defaultm the next action is set to nothing...
	_actionForNextCycle = MCHEmul::Computer::_ACTIONNOTHING;
	// ...but the method executed next could set another value...

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
