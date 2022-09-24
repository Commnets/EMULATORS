#include <C64/UserPort.hpp>
#include <C64/UserPeripherals.hpp>

// ---
C64::UserIOPort::UserIOPort ()
	: MCHEmul::IODevice (MCHEmul::IODevice::Type::_INPUTOUTPUT, _ID,
		{ 
		})
{ 
	// Nothing else to do
}

// ---
void C64::UserIOPort::addPeripheral (MCHEmul::IOPeripheral* p)
{
	if (dynamic_cast <C64::UserIOPeripheral*> (p) == nullptr)
		return;

	MCHEmul::IODevice::addPeripheral (p);
}
