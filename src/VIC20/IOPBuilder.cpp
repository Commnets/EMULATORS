#include <VIC20/IOPBuilder.hpp>
#include <VIC20/Cartridge.hpp>

// ---
MCHEmul::IOPeripheral* VIC20::IOPeripheralBuilder::createPeripheral 
	(int id, MCHEmul::Computer* c, const MCHEmul::Attributes& prms) const
{
	MCHEmul::IOPeripheral* result = nullptr;

	if (id == VIC20::Cartridge::_ID)
		result = new VIC20::Cartridge;
	else
		result = COMMODORE::IOPeripheralBuilder::createPeripheral (id, c, prms);

	// Take care, it could be null...
	return (result);
}
