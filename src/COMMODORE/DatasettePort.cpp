#include <COMMODORE/DatasettePort.hpp>
#include <COMMODORE/DatasettePeripherals.hpp>

// ---
COMMODORE::DatasetteIOPort::DatasetteIOPort ()
	: MCHEmul::IODevice (MCHEmul::IODevice::Type::_INPUTOUTPUT, _ID, 
		{ { "Name", "Datasette Port" },
		  { "Type", "Input/Output" },
		  { "Pins", "6 pairs = 12" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Year", "Unknow" } }),
	  _datasette (nullptr)
{
	// Nothing else to do...
}

// ---
bool COMMODORE::DatasetteIOPort::pinD4 () const
{
	return (_datasette == nullptr) ? false : _datasette -> pinD4 ();
}

// ---
void COMMODORE::DatasetteIOPort::pinE5 (bool d)
{
	if (_datasette != nullptr) 
		_datasette -> pintE5 (d);
}

// ---
bool COMMODORE::DatasetteIOPort::pintF6 () const
{ 
	return (_datasette == nullptr) ? false : _datasette -> pinF6 (); 
}

// ---
bool COMMODORE::DatasetteIOPort::connectPeripheral (MCHEmul::IOPeripheral* p)
{
	// If it is not of the right type, it could be connected at all!
	if (dynamic_cast <COMMODORE::DatasettePeripheral*> (p) == nullptr)
	{ 
		_error = MCHEmul::_PERIPHERAL_ERROR;

		return (false);
	}

	// There can be only one peripheral connected at the same time...
	if (!peripherals ().empty ())
		MCHEmul::IODevice::disconnectAllPeripherals ();
	_datasette = static_cast <COMMODORE::DatasettePeripheral*> (p);
	return (MCHEmul::IODevice::connectPeripheral (p));
}
