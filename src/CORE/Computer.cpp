#include <CORE/Computer.hpp>
#include <CORE/StdCommands.hpp>
#include <thread>
#include <sstream>

// ---
MCHEmul::Computer::Computer (MCHEmul::CPU* cpu, const MCHEmul::Chips& c, 
		MCHEmul::Memory* m, const MCHEmul::IODevices& d, unsigned int cs, const MCHEmul::Attributes& attrs)
	: _cpu (cpu), _chips (c), _memory (m), _devices (d), _clock (cs), _attributes (attrs),
	  _exit (false), _debugLevel (MCHEmul::_DEBUGNOTHING),
	  _lastError (MCHEmul::_NOERROR),
	  _screen (nullptr), _inputOSSystem (nullptr), _graphicalChip (nullptr)
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

		ok &= runComputerCycle ();
		ok &= runIOCycle ();
		
		finishCycle ();
	}

	return (_lastError != MCHEmul::_NOERROR);
}

// ---
bool MCHEmul::Computer::runComputerCycle ()
{
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
std::ostream& MCHEmul::operator << (std::ostream& o, const MCHEmul::Computer& c)
{
	o << "---" << std::endl;
	o << "Computer Info" << std::endl;
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
