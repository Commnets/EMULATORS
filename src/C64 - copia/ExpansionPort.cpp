#include <C64/ExpansionPort.hpp>
#include <C64/PLA.hpp>

// ---
void C64::ExpansionIOPort::linkToChips (const MCHEmul::Chips& c)
{
	for (MCHEmul::Chips::const_iterator i = c.begin (); 
		i != c.end () && _pla == nullptr; i++)
		_pla = dynamic_cast <C64::PLA*> ((*i).second);

	// It can't be null after this method...
	assert (_pla != nullptr);

	// Connections...
	// Observing when the lines _GAME and _EXROM changes...
	_pla -> observe (this);
}

// ---
bool C64::ExpansionIOPort::simulate (MCHEmul::CPU* cpu)
{
	bool result = COMMODORE::ExpansionIOPort::simulate (cpu);
	if (!result)
		result;

	if (expansionElement () != nullptr &&
		dynamic_cast <COMMODORE::ExpansionPeripheral*> (expansionElement ()) -> pinStatusChanged ())
		notify (MCHEmul::Event (_C64EXPANSIONPORTMEMLINESACTUALIZED, 
			(expansionElement () -> PIN_UP (_GAME) ? 0 : 1) + // When it is active the value goes to 0...
			(expansionElement () -> PIN_UP (_EXROM) ? 0 : 2))); // When active the value goes to 0...

	return (true);
}
