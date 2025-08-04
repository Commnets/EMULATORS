#include <VIC20/DatasettePort.hpp>
#include <VIC20/VIA1.hpp>
#include <VIC20/VIA2.hpp>

// ---
void VIC20::DatasetteIOPort::linkToChips (const MCHEmul::Chips& c)
{
	for (MCHEmul::Chips::const_iterator i = c.begin (); 
		i != c.end () && (_via1 == nullptr || _via2 == nullptr); i++)
	{
		if (_via1 == nullptr) 
			_via1 = dynamic_cast <VIC20::VIA1*> ((*i).second);
		if (_via2 == nullptr) 
			_via2 = dynamic_cast <VIC20::VIA2*> ((*i).second);
	}

	// None can't be null after this method...
	assert (_via1 != nullptr && 
			_via2 != nullptr);

	// Connections...
	_via1 -> observe (this); // Observing when bits are sent from the device...
	_via2 -> observe (this); // Observing when bits are sent from the device...
}
