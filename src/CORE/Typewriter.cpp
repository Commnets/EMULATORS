#include <CORE/Typewriter.hpp>
#include <CORE/CPU.hpp>
#include <CORE/LogChannel.hpp>

// ---
MCHEmul::Typewriter::Typewriter (unsigned int nC, 
		MCHEmul::Computer* c, const MCHEmul::Attributes& attrs)
	: MCHEmul::InputOSSystemPeripheral (_ID, nC, c, attrs),
	  _keystrokes { }
{
	setClassName ("Typewriter");
}

// ---
bool MCHEmul::Typewriter::connectData (MCHEmul::FileData* dt)
{ 
	if (dynamic_cast <MCHEmul::KeystrokeData*> (dt) == nullptr)
		return (false); // That type of info is not valid for the typewriter...

	_keystrokes = static_cast <MCHEmul::KeystrokeData*> (dt) -> _keystrokes;

	// The connection of the keystrokes has to be done using the specifc InputOSSystem of the emulation...
	std::vector <SDL_Event> events;
	auto addKeyPress = [&](SDL_Scancode sc) -> void
		{
			SDL_Event evnt; 
			evnt.type = SDL_KEYDOWN; evnt.key.keysym.scancode = sc;
			events.emplace_back (evnt);
		};

	auto addKeyRelease = [&](SDL_Scancode sc) -> void
		{
			SDL_Event evnt;
			evnt.type = SDL_KEYUP; evnt.key.keysym.scancode = sc;
			events.emplace_back (evnt);
		};

	auto addKeyFullEvent = [&](SDL_Scancode sc) -> void
		{
			addKeyPress (sc);
			addKeyRelease (sc);
			// Two events are need to identify what happens in the system...
			// ...and bot has to be injected into the parent...
		};

	for (const auto& i : _keystrokes)
	{
		if (i.length () == 1)
			addKeyFullEvent (
				(i == "\n")
					? SDL_SCANCODE_RETURN
					: SDL_GetScancodeFromKey (static_cast <SDL_Keycode> (std::tolower (i [0]))));
		else
		{
			// Gets all the scan codes...
			MCHEmul::Strings strs = MCHEmul::getElementsFrom (i, '+');
			std::vector <SDL_Scancode> scs;
			for (auto& j : strs)
			{
				if (j.length () == 1)
					scs.emplace_back 
						(SDL_GetScancodeFromKey (static_cast <SDL_Keycode> (std::tolower (j [0]))));
				else
				{
					j = MCHEmul::upper (j);
					if (j == "LCTRL")
						scs.emplace_back (SDL_SCANCODE_LCTRL);
					else if (j == "RCTRL")
						scs.emplace_back (SDL_SCANCODE_RCTRL);
					else if (j == "LSHIFT")
						scs.emplace_back (SDL_SCANCODE_LSHIFT);
					else if (j == "RSHIFT")
						scs.emplace_back (SDL_SCANCODE_RSHIFT);
					else if (j == "LALT")
						scs.emplace_back (SDL_SCANCODE_LALT);
					else if (j == "RALT")
						scs.emplace_back (SDL_SCANCODE_RALT);
					// Any other thing not listed here is ignored, 
					// but some member might be inserted eventually, so the outcome could be uncontrolable...
				}
			}

			// BNow the scan codes and inserted twice...
			// One pressing them...
			for (const auto& j : scs)
				addKeyPress (j);
			// ...and the other ones releasing them in the reverse order...
			for (std::vector <SDL_Scancode>::const_reverse_iterator j = scs.rbegin (); 
				j != scs.rend (); j++)
				addKeyRelease ((*j));
		}

	}

	loadEvents (events);
	
	return (true); 
}

// ---
MCHEmul::FileData* MCHEmul::Typewriter::retrieveData () const
{
	// The format understood by this retrive method is just KeystrokeData...
	MCHEmul::FileData* result = new MCHEmul::KeystrokeData; 
	MCHEmul::KeystrokeData* key = dynamic_cast <MCHEmul::KeystrokeData*> (result);
	// If is not needed to ckeck it because it has been the one loaded at the beginning...

	key -> _keystrokes = _keystrokes;

	return (result);
}
