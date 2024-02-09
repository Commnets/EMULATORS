#include <C264/IOPBuilder.hpp>
#include <C264/Cartridge.hpp>

// ---
MCHEmul::IOPeripheral* C264::IOPeripheralBuilder::createPeripheral 
	(int id, MCHEmul::Computer* c, const MCHEmul::Attributes& prms) const
{
	MCHEmul::IOPeripheral* result = nullptr;

	if (id == C264::Cartridge::_ID)
		result = new C264::Cartridge;
	else
		result = COMMODORE::IOPeripheralBuilder::createPeripheral (id, c, prms);

	// Take care, it could be null...
	return (result);
}
