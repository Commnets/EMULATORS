#include <ZX81/EdgeConnector.hpp>
#include <ZX81/Printer.hpp>

// ---
ZX81::EdgeConnector::EdgeConnector (ZX81::Type t)
	: MCHEmul::IODevice (MCHEmul::IODevice::Type::_INPUT, _ID, 
		{ { "Name", "Expansion Port" },
		  { "Type", "Input/Output" },
		  { "Manufacturer", "Sinclair Research/Timex Coporation" } }),
	  _computerType (t),
	  _expansionElement (nullptr),
	  _expansionElementOut (false) // Only true when the expansion element is extracted...
{
	// Nothing else to do...
}

// ---
bool ZX81::EdgeConnector::connectPeripheral (MCHEmul::IOPeripheral* p)
{
	// If it is not of the right type, it could be connected at all!
	if (p != nullptr && 
		dynamic_cast <ZX81::EdgeConnectorPeripheral*> (p) == nullptr)
	{
		_error = MCHEmul::_PERIPHERAL_ERROR;

		return (false);
	}

	// It is not possible to connect the Thermal Printer to the ZX80...
	// because it is valid just for the ZX81 (any version)...
	if (_computerType == ZX81::Type::_ZX80 &&
		dynamic_cast <ZX81::ThermalPrinterSimulation*> (p) != nullptr)
	{
		_error = MCHEmul::_PERIPHERAL_ERROR;

		return (false);
	}

	// There can be only one peripheral connected at the same time...
	if (!peripherals ().empty ())
	{
		notify (MCHEmul::Event (_EXPANSIONELEMENTOUT));

		MCHEmul::IODevice::disconnectAllPeripherals ();
	}

	_expansionElement = static_cast <ZX81::EdgeConnectorPeripheral*> (p);
	return (MCHEmul::IODevice::connectPeripheral (p));
}

// ---
bool ZX81::EdgeConnector::disconnectPeripheral (int id)
{
	MCHEmul::IOPeripherals::const_iterator i = peripherals ().find (id);
	if (i != peripherals ().end ())
	{ 
		_expansionElement = nullptr;

		_expansionElementOut = true;

		notify (MCHEmul::Event (_EXPANSIONELEMENTOUT));
	}

	return (MCHEmul::IODevice::disconnectPeripheral (id));
}

// ---
bool ZX81::EdgeConnector::simulate (MCHEmul::CPU* cpu)
{
	// The expansion element is considerer connected just if it has info inside...
	// Otherwise it will be still uncomplete!
	if (_expansionElement != nullptr && 
		_expansionElement -> dataJustLoaded ())
		notify (MCHEmul::Event (_EXPANSIONELEMENTIN));

	// if the expansion element has been extracted...
	if (_expansionElement == nullptr && _expansionElementOut)
	{
		_expansionElementOut = false;

		notify (MCHEmul::Event (_EXPANSIONELEMENTOUT));
	}

	// The standard simulation is invoked to
	// involke the simulation methdos for all devices connected...
	return (MCHEmul::IODevice::simulate (cpu));
}
