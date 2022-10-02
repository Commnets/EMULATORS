#include <CORE/IO.hpp>
#include <CORE/global.hpp>

// ---
MCHEmul::IODevice::IODevice (MCHEmul::IODevice::Type t, int id, const MCHEmul::Attributes& attrs)
	: _type (t), 
	  _id (id), 
	  _chips (), 
	  _attributes (),
	  _peripherals (),
	  _lastError (MCHEmul::_NOERROR)
{
	// Nothing else to do
}

// ---		
void MCHEmul::IODevice::addPeripheral (MCHEmul::IOPeripheral* p)
{
	if (p == nullptr)
		return;

	MCHEmul::IOPeripherals::const_iterator i = _peripherals.find (p -> id ());
	if (i == _peripherals.end ())
	{
		_peripherals.insert (MCHEmul::IOPeripherals::value_type (p -> id (), p));

		p -> _device = this; // link it...
	}
}

// ---
void MCHEmul::IODevice::removePeripheral (int id)
{
	MCHEmul::IOPeripherals::const_iterator i = _peripherals.find (id);
	if (i != _peripherals.end ())
	{
		(*i).second -> _device = nullptr;

		_peripherals.erase (i); // unlink it...
	}
}

// ---
bool MCHEmul::IODevice::initialize ()
{
	bool result = true;
	for (auto i : _peripherals)
		result &= i.second -> initialize ();

	if (!result)
		_lastError = MCHEmul::_INIT_ERROR;

	return (result);
}

// ---
bool MCHEmul::IODevice::simulate ()
{
	bool result = true;
	for (auto i : _peripherals)
		result &= i.second -> simulate ();

	if (!result)
		_lastError = MCHEmul::_DEVICE_ERROR;

	return (result);
}

// ---
std::ostream& MCHEmul::operator << (std::ostream& o, const MCHEmul::IODevice& d)
{
	o << d._id << std::endl;
	o << d._attributes;

	bool f = true;
	for (auto i : d._peripherals)
	{
		if (f) o << std::endl;
		o << i.second;
		f = false;
	}

	return (o);
}
