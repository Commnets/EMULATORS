#include <COMMODORE/SerialPort.hpp>
#include <COMMODORE/SerialIOPeripherals.hpp>

// ---
COMMODORE::SerialIOPort::SerialIOPort ()
	: MCHEmul::IODevice (MCHEmul::IODevice::Type::_INPUTOUTPUT, _ID, 
		{ { "Name", "Serial Port" },
		  { "Type", "Input/Output" },
		  { "Manufacturer", "Commodore Business Machines CBM" } })
{ 
	// Nothing else to do...
}

// ---
bool COMMODORE::SerialIOPort::connectPeripheral (MCHEmul::IOPeripheral* p)
{
	// If it is not of the right type, it could be connected at all!
	if (dynamic_cast <COMMODORE::SerialIOPeripheral*> (p) == nullptr)
	{ 
		_error = MCHEmul::_PERIPHERAL_ERROR;

		return (false);
	}

	// There can be only one peripheral connected at the same time...
	if (!peripherals ().empty ())
		MCHEmul::IODevice::disconnectAllPeripherals ();
	return (MCHEmul::IODevice::connectPeripheral (p));
}
