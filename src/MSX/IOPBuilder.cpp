#include <MSX/IOPBuilder.hpp>
#include <MSX/MSX.hpp>

// ---
MCHEmul::IOPeripheral::Infos MSX::IOPeripheralBuilder::possiblePeripherals () const
{
	MCHEmul::IOPeripheral::Infos result = 
		std::move (MCHEmul::IOPeripheralBuilder::possiblePeripherals ());

	result.emplace_back (MCHEmul::IOPeripheral::Info 
		{ MCHEmul::Typewriter::_ID, MCHEmul::Typewriter::_ATTRIBUTES });

	return (result);
}

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
