#include <MSX/OSIO.hpp>
#include <MSX/PPI8255.hpp>

const MSX::InputOSSystem::Keystrokes MSX::InputOSSystem::_NOKEYSTROKES = { };

// ---
MSX::InputOSSystem::InputOSSystem (const MSX::InputOSSystem::KeystrockesMap& ks)
	: MCHEmul::InputOSSystem (_ID, 
		{ { "Name", "IOSystem" },
		  { "Type", "Input" },
		  { "Frequency", "50.0Hz" } }),
	_MSXKEYS (ks),
	_ppi8255 (nullptr)
{ 
	// Nothing else to do...
}

// ---
void MSX::InputOSSystem::linkToChips (const MCHEmul::Chips& c)
{
	for (const auto& i : c)
	{
		if (dynamic_cast <MSX::PPI8255*> (i.second) != nullptr)
			_ppi8255 = static_cast <MSX::PPI8255*> (i.second);
	}

	// It can not be nullptr...
	assert (_ppi8255 != nullptr);

	// The PPI chip has to receive the events from the system
	// becaise among other thigs it manages the keyboard!
	_ppi8255 -> observe (this);
}
