#include <CPU/Computer.hpp>

// ---
MCHEmul::Computer::Computer (MCHEmul::CPU* cpu, const MCHEmul::Chips& c, 
		MCHEmul::Memory* m, const MCHEmul::IODevices& d, const MCHEmul::Attributes& attrs)
	: _cpu (cpu), _chips (c), _memory (m), _devices (d), _attributes (attrs)
{ 
	assert (_cpu != nullptr && _memory != nullptr && _memory -> stack () != nullptr);

	_cpu -> setMemoryRef (_memory);

	for (auto i : _chips)
		i.second -> setMemoryRef (_memory);

	for (auto i : _devices)
		i.second -> linkToChips (_chips);
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

	if (_memory -> initialize ())
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
	if (!initialize ())
		return (false);

	return (runImpl ());
}

// ---
bool MCHEmul::Computer::runFrom (const MCHEmul::Address& a)
{
	// It must be initialized before!

	// Sets the initial address to start from...
	cpu () -> programCounter ().setAddress (a);

	return (runImpl ());
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
bool MCHEmul::Computer::runImpl ()
{
	_exit = false;
	_lastError = MCHEmul::_NOERROR;
	while (!_exit)
	{
		if (!_cpu -> executeNextTransaction ())
		{
			_exit = true;

			_lastError = MCHEmul::_CPU_ERROR;
		}
		else
		{
			for (auto i : _chips)
			{
				if (!i.second -> simulate (_cpu))
				{
					_exit = true;

					_lastError = MCHEmul::_CHIP_ERROR;
				}
			}
		}
	}

	return (_lastError != MCHEmul::_NOERROR);
}
