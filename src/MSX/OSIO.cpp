#include <MSX/OSIO.hpp>

// ---
const MSX::InputOSSystem::KeystrockesMap MSX::InputOSSystem::_MSXKEYS
	(
		{
			// TODO: Define the map of keystrokes
		}
	);

const MSX::InputOSSystem::Keystrokes MSX::InputOSSystem::_NOKEYSTROKES = { };

// ---
MSX::InputOSSystem::InputOSSystem ()
	: MCHEmul::InputOSSystem (_ID, 
		{ { "Name", "IOSystem" },
		  { "Type", "Input" },
		  { "Frequency", "50.0Hz" } })
{ 
	// Nothing else to do...
}

// ---
void MSX::InputOSSystem::linkToChips (const MCHEmul::Chips& c)
{
	// TODO: Link to the graphical chip...
}
