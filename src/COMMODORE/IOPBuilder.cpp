#include <COMMODORE/IOPBuilder.hpp>
#include <COMMODORE/UserIOPeripherals.hpp>
#include <COMMODORE/DatasettePeripherals.hpp>

// ---
MCHEmul::IOPeripheral* COMMODORE::IOPeripheralBuilder::createPeripheral (int id, const MCHEmul::Attributes& prms) const
{
	MCHEmul::IOPeripheral* result = nullptr;

	if (id == COMMODORE::NoUserIOPeripheral::_ID)
		result = new COMMODORE::NoUserIOPeripheral;
	else if (id == COMMODORE::NoDatasettePeripheral::_ID)
		result = new COMMODORE::NoDatasettePeripheral;

	// Take care, it could be null...
	return (result);
}
