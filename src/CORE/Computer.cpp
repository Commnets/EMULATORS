#include <CORE/Computer.hpp>
#include <CORE/StdCommands.hpp>
#include <thread>
#include <sstream>

// ---
MCHEmul::Computer::Computer (MCHEmul::CPU* cpu, const MCHEmul::Chips& c, 
		MCHEmul::Memory* m, const MCHEmul::IODevices& d, unsigned int cs, const MCHEmul::Attributes& attrs)
	: _cpu (cpu), _chips (c), _memory (m), _devices (d), _attributes (attrs), _actionsAt (),
	  _exit (false), _debugLevel (MCHEmul::_DEBUGNOTHING),
	  _lastError (MCHEmul::_NOERROR),
	  _screen (nullptr), _inputOSSystem (nullptr), _graphicalChip (nullptr),
	  _clock (cs), 
	  _lastAction (0) // Meaning no action...
{ 
	assert (_cpu != nullptr);
	assert (_memory != nullptr && _memory -> stack () != nullptr);

	_cpu -> setMemoryRef (_memory);

	for (auto i : _chips)
	{
		if (_graphicalChip == nullptr)
			_graphicalChip = dynamic_cast <MCHEmul::GraphicalChip*> (i.second);

		i.second -> setMemoryRef (_memory);
	}

	// To define a graphical chip is mandatory...
	assert (_graphicalChip != nullptr);

	for (auto i : _devices)
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

	for (auto i : _chips)
		delete (i.second);

	for (auto i : _devices)
		delete (i.second);
}

// ---
bool MCHEmul::Computer::initialize ()
{
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
	for (auto i : chips ())
		resultChips &= i.second -> initialize ();
	if (!resultChips)
	{
		_lastError = MCHEmul::_INIT_ERROR;

		return (false);
	}

	bool resultIO = true;
	for (auto i : devices ())
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
	if (!executeAction (_lastAction /* can be modified within the method. */, (at == _actionsAt.end ()) ? 0 : (*at).second, a))
		return (true); // It has decided not to execute the cycle...

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
		std::cout << "->" << *_cpu -> lastInstruction () << std::endl;
		MCHEmul::CPUStatusCommand stCmd; MCHEmul::Attributes rst; stCmd.execute (this, rst);
		std::cout << (*rst.begin ()).second << std::endl;
	}

	for (auto i : _chips)
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
	for (auto i : _devices)
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
std::ostream& MCHEmul::operator << (std::ostream& o, const MCHEmul::Computer& c)
{
	o << *c.cpu () << std::endl;
	for (auto i : c.chips ())
		o << *i.second << std::endl;
	o << *c.memory () << std::endl;
	for (auto i : c.devices ())
		o << *i.second << std::endl;
	o << c.attributes ();
	return (o);
}

// ---
bool MCHEmul::Computer::executeAction (unsigned int& lA, unsigned int at, unsigned int a)
{
	// The action defined at the program counter point has priority, if any...
	// Otherwise the external action point, if any is taking into consideracion!
	unsigned int act = (at == 0) ? a : at;

	// Meaning nothing...
	if (act == 0)
		return (true); // The last action doesn't change at all...

	// Meaning to stop, when no previously stopped...
	if (act == 1 && lA != 1)
	{
		cpu () -> setStop (true);

		lA = 1;
	}

	// Meaning to run, when previously stopped...
	if (act == 2 && lA == 1)
	{
		cpu () -> setStop (false);

		lA = 0;
	}

	return (true);
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
