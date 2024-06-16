#include <ZX81/IOPBuilder.hpp>
#include <ZX81/Cartridge.hpp>
#include <ZX81/Datasette.hpp>

// ---
MCHEmul::IOPeripheral* ZX81::IOPeripheralBuilder::createPeripheral 
	(int id, MCHEmul::Computer* c, const MCHEmul::Attributes& prms) const
{
	MCHEmul::IOPeripheral* result = nullptr;

	if (id == ZX81::Cartridge::_ID)
		result = new ZX81::Cartridge;
	else if (id == ZX81::Datasette::_ID)
		result = new ZX81::Datasette (3100);
	else if (id == ZX81::DatasetteP::_ID)
		result = new ZX81::DatasetteP (100);
	else
		result = SINCLAIR::IOPeripheralBuilder::createPeripheral (id, c, prms);

	// Take care, it could be null...
	return (result);
}
