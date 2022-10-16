#include <CORE/IO.hpp>
#include <CORE/global.hpp>
#include <CORE/FmterBuilder.hpp>
#include <CORE/Formatter.hpp>

// ---
MCHEmul::IODevice::IODevice (MCHEmul::IODevice::Type t, int id, const MCHEmul::Attributes& attrs)
	: MCHEmul::InfoClass ("IODevice"),
	  _type (t), 
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
	for (const auto& i : _peripherals)
		result &= i.second -> initialize ();

	if (!result)
		_lastError = MCHEmul::_INIT_ERROR;

	return (result);
}

// ---
bool MCHEmul::IODevice::simulate ()
{
	bool result = true;
	for (const auto& i : _peripherals)
		result &= i.second -> simulate ();

	if (!result)
		_lastError = MCHEmul::_DEVICE_ERROR;

	return (result);
}

// ---
MCHEmul::InfoStructure MCHEmul::IODevice::getInfoStructure () const
{
	MCHEmul::InfoStructure result;

	result.add ("ID", _id);
	result.add ("ATTRS", _attributes);

	MCHEmul::InfoStructure pers;
	for (const auto& i : _peripherals)
		pers.add (std::to_string (i.first), i.second -> getInfoStructure ());
	result.add ("IOPERIPHERALS", pers);

	return (result);
}
