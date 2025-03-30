#include <MSX/IOPBuilder.hpp>
#include <MSX/MSX.hpp>

// ---
MCHEmul::IOPeripheral* MSX::IOPeripheralBuilder::createPeripheral 
	(int id, MCHEmul::Computer* c, const MCHEmul::Attributes& prms) const
{
	assert (dynamic_cast <MSX::MSXComputer*> (c) != nullptr);

	MCHEmul::IOPeripheral* result = nullptr;

	// TODO

	// Take care, it could be null...
	return (result);
}
