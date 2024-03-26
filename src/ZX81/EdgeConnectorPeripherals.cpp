#include <ZX81/EdgeConnectorPeripherals.hpp>

// ---
MCHEmul::InfoStructure ZX81::EdgeConnectorPeripheral::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::IOPeripheral::getInfoStructure ());

	result.add ("DATA", _data._data.empty () ? std::string ("no data") : _data._name);

	return (result);
}
