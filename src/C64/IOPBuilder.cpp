#include <C64/IOPBuilder.hpp>
#include <C64/UserPeripherals.hpp>

// ---
MCHEmul::IOPeripheral* C64::IOPeripheralBuilder::createPeripheral (int id, const MCHEmul::Attributes& prms) const
{
	MCHEmul::IOPeripheral* result = nullptr;

	if (id == C64::UserIONoPeripheral::_ID)
		result = new C64::UserIONoPeripheral;

	// Take care, it could be null...
	return (result);
}
