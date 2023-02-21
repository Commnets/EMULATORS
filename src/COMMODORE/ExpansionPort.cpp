#include <COMMODORE/ExpansionPort.hpp>
#include <COMMODORE/ExpansionPeripherals.hpp>

// ---
COMMODORE::ExpansionIOPort::ExpansionIOPort ()
	: MCHEmul::IODevice (MCHEmul::IODevice::Type::_INPUT, _ID, 
		{ { "Name", "Expansion Port" },
		  { "Type", "Input/Output" },
		  { "Manufacturer", "Commodore Business Machines CBM" } }),
	  _expansionElement (nullptr),
	  _firstExecution (true)
{
	// Nothing else to do...
}

// ---
bool COMMODORE::ExpansionIOPort::connectPeripheral (MCHEmul::IOPeripheral* p)
{
	// If it is not of the right type, it could be connected at all!
	if (dynamic_cast <COMMODORE::ExpansionPeripheral*> (p) == nullptr)
	{
		_error = MCHEmul::_PERIPHERAL_ERROR;

		return (false);
	}

	// There can be only one peripheral connected at the same time...
	if (!peripherals ().empty ())
		MCHEmul::IODevice::disconnectAllPeripherals ();
	_expansionElement = static_cast <COMMODORE::ExpansionPeripheral*> (p);
	return (MCHEmul::IODevice::connectPeripheral (p));
}

// ---
bool COMMODORE::ExpansionIOPort::simulate (MCHEmul::CPU* cpu)
{
	// If it is the first execution and there is cartridge connected...
	// ...then a notification is done!
	if (_firstExecution && _expansionElement != nullptr)
		notify (MCHEmul::Event (COMMODORE::ExpansionIOPort::_CARTRIDGEIN));

	_firstExecution = false;

	return (true);
}
