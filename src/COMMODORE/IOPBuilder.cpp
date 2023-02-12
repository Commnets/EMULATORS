#include <COMMODORE/IOPBuilder.hpp>
#include <COMMODORE/UserIOPeripherals.hpp>
#include <COMMODORE/DatasettePeripherals.hpp>
#include <COMMODORE/ExpansionPeripherals.hpp>
#include <COMMODORE/Cartridge.hpp>
#include <COMMODORE/1530Datasette.hpp>

// ---
MCHEmul::IOPeripheral* COMMODORE::IOPeripheralBuilder::createPeripheral (int id, const MCHEmul::Attributes& prms) const
{
	MCHEmul::IOPeripheral* result = nullptr;

	if (id == COMMODORE::NoUserIOPeripheral::_ID)
		result = new COMMODORE::NoUserIOPeripheral;
	else if (id == COMMODORE::NoDatasettePeripheral::_ID)
		result = new COMMODORE::NoDatasettePeripheral;
	else if (id == COMMODORE::NoExpansionPeripheral::_ID)
		result = new COMMODORE::NoExpansionPeripheral;
	else if (id == COMMODORE::Datasette1530::_ID)
		result = new COMMODORE::Datasette1530;
	else if (id == COMMODORE::Cartridge::_ID)
		result = new COMMODORE::Cartridge;

	// Take care, it could be null...
	return (result);
}
