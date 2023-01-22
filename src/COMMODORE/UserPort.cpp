#include <COMMODORE/UserPort.hpp>
#include <COMMODORE/UserIOPeripherals.hpp>

// ---
COMMODORE::UserIOPort::UserIOPort ()
	: MCHEmul::IODevice (MCHEmul::IODevice::Type::_INPUTOUTPUT, _ID, { })
{ 
	// Nothing else to do...
}

// ---
void COMMODORE::UserIOPort::connectPeripheral (MCHEmul::IOPeripheral* p)
{
	// If it is not of the right type, it could be connected at all!
	if (dynamic_cast <COMMODORE::UserIOPeripheral*> (p) == nullptr)
		return;

	// There can be only one peripheral connected at the same time...
	if (!peripherals ().empty ())
		MCHEmul::IODevice::disconnectAllPeripherals ();

	MCHEmul::IODevice::connectPeripheral (p);
}
