#include <CORE/IOPBuilder.hpp>

// ---
MCHEmul::IOPeripheral* MCHEmul::IOPeripheralBuilder::peripheral (int id, const MCHEmul::Attributes& prms) const
{
	MCHEmul::IOPeripherals::const_iterator i = _peripherals.find (id);
	if (i != _peripherals.end ())
		return ((*i).second);

	MCHEmul::IOPeripheral* result = createPeripheral (id, prms);
	assert (result != nullptr);

	_peripherals.insert (MCHEmul::IOPeripherals::value_type (id, result));

	return (result);
}
