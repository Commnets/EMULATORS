#include <VIC20/ExpansionPort.hpp>

// ---
bool VIC20::ExpansionIOPort::simulate (MCHEmul::CPU* cpu)
{
	bool result = COMMODORE::ExpansionIOPort::simulate (cpu);
	if (!result)
		result;

	if (expansionElement () != nullptr &&
		dynamic_cast <COMMODORE::ExpansionPeripheral*> (expansionElement ()) -> pinStatusChanged ())
		notify (MCHEmul::Event (_VIC20EXPANSIONPORTMEMLINESACTUALIZED));

	return (true);
}
