#include <core/Computer.hpp>

// ---
MCHEmul::Computer::~Computer ()
{ 
	delete _cpu; 

	delete _memory;

	for (auto i : _chips)
		delete i.second;
}

// ---
bool MCHEmul::Computer::initialize ()
{
	bool result = true;

	_cpu -> initialize ();

	for (auto i : chips ())
		result &= i.second -> initialize ();
	if (!result)
		_lastError = MCHEmul::_INIT_ERROR;

	_memory -> initialize ();

	_stack -> initialize ();

	return (result);
}

// ---
bool MCHEmul::Computer::run ()
{
	if (!initialize ())
		return (false);

	_exit = false;
	_lastError = MCHEmul::_NOERROR;
	while (!_exit)
	{
		if (!_cpu -> executeNextTransaction (this))
		{
			_exit = true;

			_lastError = MCHEmul::_CPU_ERROR;
		}
		else
		{
			for (auto i : _chips)
			{
				if (i.second -> simulate (this))
				{
					_exit = true;

					_lastError = MCHEmul::_CHIP_ERROR;
				}
			}
		}
	}

	return (_lastError != MCHEmul::_NOERROR);
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
	o << *c.stack () << std::endl;
	o << c.attributes ();

	return (o);
}
