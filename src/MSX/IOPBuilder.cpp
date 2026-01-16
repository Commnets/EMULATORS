#include <MSX/IOPBuilder.hpp>
#include <MSX/MSX.hpp>

// ---
MCHEmul::IOPeripheral* MSX::IOPeripheralBuilder::createPeripheral 
	(int id, MCHEmul::Computer* c, const MCHEmul::Attributes& prms) const
{
	assert (dynamic_cast <MSX::MSXComputer*> (c) != nullptr);

	MCHEmul::IOPeripheral* result = nullptr;

	if (id == MCHEmul::Typewriter::_ID)
		result = new MCHEmul::Typewriter (3 /** 1/25 * 3 = 1 every 120 miliseconds. */, c);

	// Take care, it could be null...
	return (result);
}
