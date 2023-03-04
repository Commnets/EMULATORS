#include <C64/IOPBuilder.hpp>
#include <C64/Cartridge.hpp>

// ---
MCHEmul::IOPeripheral* C64::IOPeripheralBuilder::createPeripheral (int id, const MCHEmul::Attributes& prms) const
{
	MCHEmul::IOPeripheral* result = nullptr;

	if (id == C64::Cartridge::_ID)
		result = new C64::Cartridge;
	else
		result = COMMODORE::IOPeripheralBuilder::createPeripheral (id, prms);

	// Take care, it could be null...
	return (result);
}
