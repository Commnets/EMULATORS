#include <COMMODORE/UserPort.hpp>
#include <COMMODORE/UserIOPeripherals.hpp>

// ---
COMMODORE::UserIOPort::UserIOPort ()
	: MCHEmul::IODevice (MCHEmul::IODevice::Type::_INPUTOUTPUT, _ID, 
		{ { "Name", "User Port" },
		  { "Type", "Input/Output" },
		  { "Pins", "24" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Year", "Unknow" } })
{ 
	// Nothing else to do...
}

// ---
bool COMMODORE::UserIOPort::connectPeripheral (MCHEmul::IOPeripheral* p)
{
	// If it is not of the right type, it could be connected at all!
	if (dynamic_cast <COMMODORE::UserIOPeripheral*> (p) == nullptr)
	{ 
		_error = MCHEmul::_PERIPHERAL_ERROR;

		return (false);
	}

	// There can be only one peripheral connected at the same time...
	if (!peripherals ().empty ())
		MCHEmul::IODevice::disconnectAllPeripherals ();
	return (MCHEmul::IODevice::connectPeripheral (p));
}
