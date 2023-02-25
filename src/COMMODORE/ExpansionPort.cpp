#include <COMMODORE/ExpansionPort.hpp>
#include <COMMODORE/ExpansionPeripherals.hpp>

// ---
COMMODORE::ExpansionIOPort::ExpansionIOPort ()
	: MCHEmul::IODevice (MCHEmul::IODevice::Type::_INPUT, _ID, 
		{ { "Name", "Expansion Port" },
		  { "Type", "Input/Output" },
		  { "Manufacturer", "Commodore Business Machines CBM" } }),
	  _expansionElement (nullptr),
	  _connectionNotified (false)
{
	// Nothing else to do...
}

// ---
bool COMMODORE::ExpansionIOPort::connectPeripheral (MCHEmul::IOPeripheral* p)
{
	// If it is not of the right type, it could be connected at all!
	if (dynamic_cast <COMMODORE::ExpansionPeripheral*> (p) == nullptr && p != nullptr)
	{
		_error = MCHEmul::_PERIPHERAL_ERROR;

		return (false);
	}

	// There can be only one peripheral connected at the same time...
	if (!peripherals ().empty ())
		MCHEmul::IODevice::disconnectAllPeripherals ();
	if (_expansionElement != nullptr)
		notify (MCHEmul::Event (COMMODORE::ExpansionIOPort::_EXPANSIONELEMENTOUT));
	_expansionElement = static_cast <COMMODORE::ExpansionPeripheral*> (p);
	_connectionNotified = false; // New element, connection not notified still...
	return (MCHEmul::IODevice::connectPeripheral (p));
}

// ---
bool COMMODORE::ExpansionIOPort::simulate (MCHEmul::CPU* cpu)
{
	// The expansion element is considerer connected just if it has onfo inside...
	// Otherwise it will be still uncomplete!
	if (_expansionElement != nullptr && 
		_expansionElement -> hasDataLoaded () && !_connectionNotified)
	{ 
		_connectionNotified = true; 

		notify (MCHEmul::Event (COMMODORE::ExpansionIOPort::_EXPANSIONELEMENTIN));
	}

	return (true);
}
