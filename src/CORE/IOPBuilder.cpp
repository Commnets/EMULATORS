#include <CORE/IOPBuilder.hpp>

// ---
MCHEmul::IOPeripheral* MCHEmul::IOPeripheralBuilder::peripheral 
	(int id, MCHEmul::Computer* c, const MCHEmul::Attributes& prms) const
{
	MCHEmul::IOPeripherals::const_iterator i = _peripherals.find (id);
	if (i != _peripherals.end ())
		return ((*i).second);

	MCHEmul::IOPeripheral* result = createPeripheral (id, c, prms);
	if (result != nullptr) // only the valid ones are inserted...
		_peripherals.insert (MCHEmul::IOPeripherals::value_type (id, result));

	return (result);
}
