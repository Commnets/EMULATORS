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
	// To make the translation from the key code to the scan code...
	// All these membes are the ones to be used in the different KeystrokeTypeIO...
	static const std::map <std::string, SDL_Scancode> _SYMBOLSTRANSLATE = {
		/** letters */
		{ "A",				SDL_SCANCODE_A },
		{ "B",				SDL_SCANCODE_B },
		{ "C",				SDL_SCANCODE_C },
		{ "D",				SDL_SCANCODE_D },
		{ "E",				SDL_SCANCODE_E },
		{ "F",				SDL_SCANCODE_F },
		{ "G",				SDL_SCANCODE_G },
		{ "H",				SDL_SCANCODE_H },
		{ "I",				SDL_SCANCODE_I },
		{ "J",				SDL_SCANCODE_J },
		{ "K",				SDL_SCANCODE_K },
		{ "L",				SDL_SCANCODE_L },
		{ "M",				SDL_SCANCODE_M },
		{ "N",				SDL_SCANCODE_N },
		{ "O",				SDL_SCANCODE_O },
		{ "P",				SDL_SCANCODE_P },
		{ "Q",				SDL_SCANCODE_Q },
		{ "R",				SDL_SCANCODE_R },
		{ "S",				SDL_SCANCODE_S },
		{ "T",				SDL_SCANCODE_T },
		{ "U",				SDL_SCANCODE_U },
		{ "V",				SDL_SCANCODE_V },
		{ "W",				SDL_SCANCODE_W },
		{ "X",				SDL_SCANCODE_X },
		{ "Y",				SDL_SCANCODE_Y },
		{ "Z",				SDL_SCANCODE_Z },
		/* keymap numbers */
		{ "1",				SDL_SCANCODE_1 },
		{ "2",				SDL_SCANCODE_2 },
		{ "3",				SDL_SCANCODE_3 },
		{ "4",				SDL_SCANCODE_4 },
		{ "5",				SDL_SCANCODE_5 },
		{ "6",				SDL_SCANCODE_6 },
		{ "7",				SDL_SCANCODE_7 },
		{ "8",				SDL_SCANCODE_8 },
		{ "9",				SDL_SCANCODE_9 },
		{ "0",				SDL_SCANCODE_0 },
		/* keymap: special keys */
		{ "RETURN",			SDL_SCANCODE_RETURN },
		{ "SPACE",			SDL_SCANCODE_SPACE },
		{ "LSHIFT",			SDL_SCANCODE_LSHIFT },
		{ "RSHIFT",			SDL_SCANCODE_RSHIFT },
		{ "LCTRL",			SDL_SCANCODE_LCTRL },
		{ "RCTRL",			SDL_SCANCODE_RCTRL },
		{ "LEFT",			SDL_SCANCODE_LEFT },
		{ "RIGHT",			SDL_SCANCODE_RIGHT },
		{ "UP",				SDL_SCANCODE_UP },
		{ "DOWN",			SDL_SCANCODE_DOWN },
		{ "HOME",			SDL_SCANCODE_HOME },
		{ ",",				SDL_SCANCODE_COMMA },
		{ ".",				SDL_SCANCODE_PERIOD },
		{ "[",				SDL_SCANCODE_LEFTBRACKET },
		{ "]",				SDL_SCANCODE_RIGHTBRACKET },
		{ "´",				SDL_SCANCODE_APOSTROPHE },
		{ "\\",				SDL_SCANCODE_BACKSLASH },
		{ "-",				SDL_SCANCODE_MINUS },
		{ "/",				SDL_SCANCODE_SLASH },
		{ ";",				SDL_SCANCODE_SEMICOLON },
		{ "=",				SDL_SCANCODE_EQUALS },
		{ "`",				SDL_SCANCODE_GRAVE },
		/** Derivated. */
		{ "F1",				SDL_SCANCODE_F1 },
		{ "F2",				SDL_SCANCODE_F2 },
		{ "F3",				SDL_SCANCODE_F3 },
		{ "F4",				SDL_SCANCODE_F4 },
		{ "F5",				SDL_SCANCODE_F5 },
		{ "F6",				SDL_SCANCODE_F6 },
		{ "F7",				SDL_SCANCODE_F7 },
		{ "F8",				SDL_SCANCODE_F8 },
		{ "F9",				SDL_SCANCODE_F9 },
		{ "F10",			SDL_SCANCODE_F10 },
		{ "F11",			SDL_SCANCODE_F11 },
		{ "F12",			SDL_SCANCODE_F12 }
	};

	if (dynamic_cast <MCHEmul::KeystrokeData*> (dt) == nullptr)
		return (false); // That type of info is not valid for the typewriter...

	_keystrokes = static_cast <MCHEmul::KeystrokeData*> (dt) -> _keystrokes;

	_eventNumber = 0; // Just in case...

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

	// Manage all keystrokes...
	for (const auto& i : _keystrokes)
	{
		// Gets all the scan codes...
		MCHEmul::Strings strs = MCHEmul::getElementsFrom (i, '+');
		std::vector <SDL_Scancode> scs;
		std::map <std::string, SDL_Scancode>::const_iterator k;
		for (auto& j : strs)
		{
			// Any other key not listed is ignored, 
			// but some member might be inserted eventually, so the outcome could be uncontrolable...
			k = _SYMBOLSTRANSLATE.find (j);
			if (k != _SYMBOLSTRANSLATE.end ())
				scs.emplace_back ((*k).second);
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
