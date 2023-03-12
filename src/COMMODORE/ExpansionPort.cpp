#include <COMMODORE/ExpansionPort.hpp>
#include <COMMODORE/ExpansionPeripherals.hpp>

// ---
COMMODORE::ExpansionIOPort::ExpansionIOPort ()
	: MCHEmul::IODevice (MCHEmul::IODevice::Type::_INPUT, _ID, 
		{ { "Name", "Expansion Port" },
		  { "Type", "Input/Output" },
		  { "Manufacturer", "Commodore Business Machines CBM" } }),
	  _expansionElement (nullptr),
	  _expansionElementOut (false) // Only true when the expansion element is extracted...
{
	// Nothing else to do...
}

// ---
bool COMMODORE::ExpansionIOPort::connectPeripheral (MCHEmul::IOPeripheral* p)
{
	// If it is not of the right type, it could be connected at all!
	if (p != nullptr && dynamic_cast <COMMODORE::ExpansionPeripheral*> (p) == nullptr)
	{
		_error = MCHEmul::_PERIPHERAL_ERROR;

		return (false);
	}

	// There can be only one peripheral connected at the same time...
	if (!peripherals ().empty ())
	{
		notify (MCHEmul::Event (COMMODORE::ExpansionIOPort::_EXPANSIONELEMENTOUT));

		MCHEmul::IODevice::disconnectAllPeripherals ();
	}

	_expansionElement = static_cast <COMMODORE::ExpansionPeripheral*> (p);
	return (MCHEmul::IODevice::connectPeripheral (p));
}

// ---
bool COMMODORE::ExpansionIOPort::disconnectPeripheral (int id)
{
	MCHEmul::IOPeripherals::const_iterator i = peripherals ().find (id);
	if (i != peripherals ().end ())
	{ 
		_expansionElement = nullptr;

		_expansionElementOut = true;

		notify (MCHEmul::Event (COMMODORE::ExpansionIOPort::_EXPANSIONELEMENTOUT));
	}

	return (MCHEmul::IODevice::disconnectPeripheral (id));
}

// ---
bool COMMODORE::ExpansionIOPort::simulate (MCHEmul::CPU* cpu)
{
	// The expansion element is considerer connected just if it has info inside...
	// Otherwise it will be still uncomplete!
	if (_expansionElement != nullptr && 
		_expansionElement -> dataJustLoaded ())
		notify (MCHEmul::Event (COMMODORE::ExpansionIOPort::_EXPANSIONELEMENTIN));

	// if the expansion element has been extracted...
	if (_expansionElement == nullptr && _expansionElementOut)
	{
		_expansionElementOut = false;

		notify (MCHEmul::Event (COMMODORE::ExpansionIOPort::_EXPANSIONELEMENTOUT));
	}

	// The standard simulation is invoked to
	// involke the simulation methdos for all devices connected...
	return (MCHEmul::IODevice::simulate (cpu));
}
