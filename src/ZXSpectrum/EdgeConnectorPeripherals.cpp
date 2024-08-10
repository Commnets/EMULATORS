#include <ZXSpectrum/EdgeConnectorPeripherals.hpp>

// ---
MCHEmul::InfoStructure ZXSPECTRUM::EdgeConnectorPeripheral::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::IOPeripheral::getInfoStructure ());

	result.add ("DATA", _data._data.empty () ? std::string ("no data") : _data._name);

	return (result);
}
