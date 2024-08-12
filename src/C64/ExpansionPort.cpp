#include <C64/ExpansionPort.hpp>
#include <C64/PLA.hpp>
#include <C64/IOExpansionMemory.hpp>
#include <C64/Cartridge.hpp>

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

// ---
void C64::ExpansionIOPort::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* ntfier)
{
	// The expansion port deals only with events comming from the IO expansion memory....
	// ...and makes sense only when there is something connected there...
	if (_expansionElement == nullptr ||
		dynamic_cast <C64::IOExpansionMemory*> (ntfier) == nullptr)
		return;

	// Gets a reference to the cartridge connected...
	// ...and to the event received...
	C64::IOExpansionMemory::EventData* ioEvnt = nullptr;
	C64::Cartridge* cT = dynamic_cast <C64::Cartridge*> (_expansionElement);
	// ...but if any one of them is nullptr the program makes no sense...
	if (cT == nullptr ||
		(ioEvnt = dynamic_cast <C64::IOExpansionMemory::EventData*> (evnt.data ().get ())) == nullptr)
		return;

	// Finally it deals with the event...
	switch (evnt.id ())
	{
		case C64::IOExpansionMemory::_SET:
			cT -> expansionMemorySet (static_cast <C64::IOExpansionMemory*> (ntfier),
				ioEvnt -> _position, ioEvnt -> _value);
			break;

		case C64::IOExpansionMemory::_READ:
			cT -> expansionMemoryRead (static_cast <C64::IOExpansionMemory*> (ntfier),
				ioEvnt -> _position, ioEvnt -> _value);
			break;
	}
}
