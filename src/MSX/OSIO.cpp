#include <MSX/OSIO.hpp>
#include <MSX/PPI8255.hpp>
#include <MSX/AY38910.hpp>

const MSX::InputOSSystem::Keystrokes MSX::InputOSSystem::_NOKEYSTROKES = { };

// ---
MSX::InputOSSystem::InputOSSystem (const MSX::InputOSSystem::KeystrockesMap& ks)
	: MCHEmul::InputOSSystem (_ID, 
		{ { "Name", "IOSystem" },
		  { "Type", "Input" },
		  { "Frequency", "50.0Hz" } }),
	_MSXKEYS (ks),
	_ppi8255 (nullptr),
	_AY38910 (nullptr)
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
		if (dynamic_cast <MSX::AY38910*> (i.second) != nullptr)
			_AY38910 = static_cast <MSX::AY38910*> (i.second);
	}

	// Neither chip can be null after linking a standard MSX input system.
	assert (_ppi8255 != nullptr && _AY38910 != nullptr);

	// The PPI chip has to receive the events from the system
	// becaise among other thigs it manages the keyboard!
	_ppi8255 -> observe (this);
	// The PSG receives movement and button events for its two general-purpose ports.
	_AY38910 -> observe (this);
}
