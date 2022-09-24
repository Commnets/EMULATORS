#include <CORE/Computer.hpp>
#include <chrono>
#include <thread>

// ---
MCHEmul::Computer::Computer (MCHEmul::CPU* cpu, const MCHEmul::Chips& c, 
		MCHEmul::Memory* m, const MCHEmul::IODevices& d, unsigned int cs, const MCHEmul::Attributes& attrs)
	: _cpu (cpu), _chips (c), _memory (m), _devices (d), _cyclesPerSecond (cs), _attributes (attrs),
	  _exit (false), _debugLevel (MCHEmul::_DEBUGNOTHING),
	  _lastError (MCHEmul::_NOERROR),
	  _screen (nullptr), _inputOSSystem (nullptr)
{ 
	assert (_cpu != nullptr);
	assert (_memory != nullptr && _memory -> stack () != nullptr);
	assert (_cyclesPerSecond > 0);

	_cpu -> setMemoryRef (_memory);

	for (auto i : _chips)
		i.second -> setMemoryRef (_memory);

	for (auto i : _devices)
	{
		if (_screen == nullptr) 
			_screen = dynamic_cast <MCHEmul::Screen*> (i.second);
		if (_inputOSSystem == nullptr) 
			_inputOSSystem = dynamic_cast <MCHEmul::InputOSSystem*> (i.second);

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

	if (!_memory -> initialize ())
	{
		_lastError = MCHEmul::_INIT_ERROR;

		return (false);
	}

	if (_screen == nullptr || _inputOSSystem == nullptr)
	{
		_lastError = MCHEmul::_INIT_ERROR;

		return (false);
	}

	bool resultIO = true;
	for (auto i : devices ())
		resultChips &= i.second -> initialize ();
	if (!resultChips)
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
		ok &= runComputerCycle ();
		ok &= runIOCycle ();
	}

	return (_lastError != MCHEmul::_NOERROR);
}

// ---
bool MCHEmul::Computer::runComputerCycle ()
{
	static const long long nanosc = (long long) 1.0e9;
	
	unsigned int iC = _cpu -> clockCycles ();
	std::chrono::time_point <std::chrono::steady_clock> iT = std::chrono::steady_clock ().now ();

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
		std::cout << "->" 
			<< *_cpu -> lastInstruction () << "\t" 
			<< _cpu -> programCounter () << "\t" 
			<< _cpu -> statusRegister ();
		for (auto i : _cpu -> internalRegisters ())
			std::cout << "\t" << i;
		std::cout << std::endl;
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

	long long el =
		std::chrono::duration_cast <std::chrono::nanoseconds> (std::chrono::steady_clock::now () - iT).count ();
	long long mel = (long long) ((double) (_cpu -> clockCycles () - iC) / (double) _cyclesPerSecond * (double) nanosc);
	if ((mel * 4500 /** i7 over 6510 */) > el)
		std::this_thread::sleep_for (std::chrono::nanoseconds ((mel * 4500) - el));

	return (true);
}

// ---
bool MCHEmul::Computer::runIOCycle ()
{
	for (auto i : _devices)
	{
		if (!i.second -> refresh ())
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
