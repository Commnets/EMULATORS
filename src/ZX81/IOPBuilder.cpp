#include <ZX81/IOPBuilder.hpp>
#include <ZX81/Cartridge.hpp>

// ---
MCHEmul::IOPeripheral* ZX81::IOPeripheralBuilder::createPeripheral 
	(int id, MCHEmul::Computer* c, const MCHEmul::Attributes& prms) const
{
	MCHEmul::IOPeripheral* result = nullptr;

	if (id == ZX81::Cartridge::_ID)
		result = new ZX81::Cartridge;
	else
		result = SINCLAIR::IOPeripheralBuilder::createPeripheral (id, c, prms);

	// Take care, it could be null...
	return (result);
}
