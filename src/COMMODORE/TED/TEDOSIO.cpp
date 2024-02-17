#include <COMMODORE/TED/TEDOSIO.hpp>
#include <COMMODORE/TED/TED.hpp>

// ---
COMMODORE::TEDInputOSSystem::TEDInputOSSystem (int id)
	: MCHEmul::InputOSSystem (id, 
		{ { "Name", "TEDIOSystem" },
		  { "Type", "Input" },
		  { "Frequency", "50.0Hz" } })
{ 
	// Nothing else to do...
}

// ---
void COMMODORE::TEDInputOSSystem::linkToChips (const MCHEmul::Chips& c)
{
	for (MCHEmul::Chips::const_iterator i = c.begin (); i != c.end (); i++)
	{
		if (dynamic_cast <COMMODORE::TED*> ((*i).second) != nullptr)
			_ted = dynamic_cast <COMMODORE::TED*> ((*i).second);
	}

	// Can't be null after this method...
	assert (_ted != nullptr);

	// The TED will receive the event related with the io system...
	_ted -> observe (this);
}
