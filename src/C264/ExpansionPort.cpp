#include <C264/ExpansionPort.hpp>

// ---
bool C264::ExpansionIOPort::simulate (MCHEmul::CPU* cpu)
{
	bool result = COMMODORE::ExpansionIOPort::simulate (cpu);
	if (!result)
		result;

	if (expansionElement () != nullptr &&
		dynamic_cast <COMMODORE::ExpansionPeripheral*> (expansionElement ()) -> pinStatusChanged ())
		notify (MCHEmul::Event (_C264EXPANSIONPORTMEMLINESACTUALIZED));

	return (true);
}
