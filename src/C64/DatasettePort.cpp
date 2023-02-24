#include <C64/DatasettePort.hpp>
#include <C64/CIA1.hpp>
#include <C64/SFChip.hpp>

// ---
void C64::DatasetteIOPort::linkToChips (const MCHEmul::Chips& c)
{
	for (MCHEmul::Chips::const_iterator i = c.begin (); i != c.end () && 
		(_cia1 == nullptr || _specialChip == nullptr); i++)
	{ 
		if (_cia1 == nullptr) 
			_cia1 = dynamic_cast <C64::CIA1*> ((*i).second);
		if (_specialChip == nullptr) 
			_specialChip = dynamic_cast <C64::SpecialFunctionsChip*> ((*i).second);
	}

	// None can't be null after this method...
	assert (_cia1 != nullptr);
	assert (_specialChip != nullptr);

	// Connections...
	_cia1 -> observe (this); // Observing when bits are sent from the device...
	_specialChip -> observe (this); // Observing when keys are pressed...
	observe (_specialChip); // Observing when bits are sent to the device...
}
