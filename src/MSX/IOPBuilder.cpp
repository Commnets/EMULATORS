#include <MSX/IOPBuilder.hpp>
#include <MSX/Datasette.hpp>
#include <MSX/MSX.hpp>

// ---
MCHEmul::IOPeripheral::Infos MSX::IOPeripheralBuilder::possiblePeripherals () const
{
	MCHEmul::IOPeripheral::Infos result = 
		std::move (MCHEmul::IOPeripheralBuilder::possiblePeripherals ());

	result.emplace_back (MCHEmul::IOPeripheral::Info 
		{ MCHEmul::Typewriter::_ID, MCHEmul::Typewriter::_ATTRIBUTES });
	result.emplace_back (MCHEmul::IOPeripheral::Info 
		{ MSX::DatasetteInjection::_ID, MSX::DatasetteInjection::_ATTRIBUTES });

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
	else if (id == MSX::DatasetteInjection::_ID)
		result = new MSX::DatasetteInjection ();

	// Take care, it could be null...
	return (result);
}
