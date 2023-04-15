#include <C64/OSIO.hpp>
#include <C64/CIA1.hpp>

// ---
const C64::InputOSSystem::KeystrockesMap C64::InputOSSystem::_C64KEYS
	(
		{
			/** letters */
			{ SDL_SCANCODE_A,				{ std::make_pair (1, 2) } },
			{ SDL_SCANCODE_B,				{ std::make_pair (3, 4) } },
			{ SDL_SCANCODE_C,				{ std::make_pair (2, 4) } },
			{ SDL_SCANCODE_D,				{ std::make_pair (2, 2) } },
			{ SDL_SCANCODE_E,				{ std::make_pair (1, 6) } },
			{ SDL_SCANCODE_F,				{ std::make_pair (2, 5) } },
			{ SDL_SCANCODE_G,				{ std::make_pair (3, 2) } },
			{ SDL_SCANCODE_H,				{ std::make_pair (3, 5) } },
			{ SDL_SCANCODE_I,				{ std::make_pair (4, 1) } },
			{ SDL_SCANCODE_J,				{ std::make_pair (4, 2) } },
			{ SDL_SCANCODE_K,				{ std::make_pair (4, 5) } },
			{ SDL_SCANCODE_L,				{ std::make_pair (5, 2) } },
			{ SDL_SCANCODE_M,				{ std::make_pair (4, 4) } },
			{ SDL_SCANCODE_N,				{ std::make_pair (4, 7) } },
			{ SDL_SCANCODE_O,				{ std::make_pair (4, 6) } },
			{ SDL_SCANCODE_P,				{ std::make_pair (5, 1) } },
			{ SDL_SCANCODE_Q,				{ std::make_pair (7, 6) } },
			{ SDL_SCANCODE_R,				{ std::make_pair (2, 1) } },
			{ SDL_SCANCODE_S,				{ std::make_pair (1, 5) } },
			{ SDL_SCANCODE_T,				{ std::make_pair (2, 6) } },
			{ SDL_SCANCODE_U,				{ std::make_pair (3, 6) } },
			{ SDL_SCANCODE_V,				{ std::make_pair (3, 7) } },
			{ SDL_SCANCODE_W,				{ std::make_pair (1, 1) } },
			{ SDL_SCANCODE_X,				{ std::make_pair (2, 7) } },
			{ SDL_SCANCODE_Y,				{ std::make_pair (3, 1) } },
			{ SDL_SCANCODE_Z,				{ std::make_pair (1, 4) } },
			/* keymap numbers */
			{ SDL_SCANCODE_1,				{ std::make_pair (7, 0) } },
			{ SDL_SCANCODE_2,				{ std::make_pair (7, 3) } },
			{ SDL_SCANCODE_3,				{ std::make_pair (1, 0) } },
			{ SDL_SCANCODE_4,				{ std::make_pair (1, 3) } },
			{ SDL_SCANCODE_5,				{ std::make_pair (2, 0) } },
			{ SDL_SCANCODE_6,				{ std::make_pair (2, 3) } },
			{ SDL_SCANCODE_7,				{ std::make_pair (3, 0) } },
			{ SDL_SCANCODE_8,				{ std::make_pair (3, 3) } },
			{ SDL_SCANCODE_9,				{ std::make_pair (4, 0) } },
			{ SDL_SCANCODE_0,				{ std::make_pair (4, 3) } },
			/* keymap function keys */
			{ SDL_SCANCODE_F1,				{ std::make_pair (0, 4) } },
			{ SDL_SCANCODE_F3,				{ std::make_pair (0, 5) } },
			{ SDL_SCANCODE_F5,				{ std::make_pair (0, 6) } },
			{ SDL_SCANCODE_F7,				{ std::make_pair (0, 3) } },
			/** the cursor/control keys */
			{ SDL_SCANCODE_RIGHT,			{ std::make_pair (0, 2) } },
			{ SDL_SCANCODE_DOWN,			{ std::make_pair (0, 7) } },
			{ SDL_SCANCODE_LCTRL,			{ std::make_pair (7, 2) } }, // Ctrl instead
			{ SDL_SCANCODE_RCTRL,			{ std::make_pair (7, 2) } }, // Ctrl (the same assigned) instead
			{ SDL_SCANCODE_LALT,			{ std::make_pair (7, 7) } }, // Stop instead (+ shift = Run)
			{ SDL_SCANCODE_TAB,				{ std::make_pair (7, 5) } }, // Commodore key instead
			/* keymap: others */
			{ SDL_SCANCODE_RETURN,			{ std::make_pair (0, 1) } },
			{ SDL_SCANCODE_SPACE,			{ std::make_pair (7, 4) } },
			{ SDL_SCANCODE_LSHIFT,			{ std::make_pair (1, 7) } },
			{ SDL_SCANCODE_RSHIFT,			{ std::make_pair (6, 4) } },
			{ SDL_SCANCODE_COMMA,			{ std::make_pair (5, 7) } },
			{ SDL_SCANCODE_PERIOD,			{ std::make_pair (5, 4) } },
			{ SDL_SCANCODE_LEFTBRACKET,		{ std::make_pair (5, 0) } }, // + instead
			{ SDL_SCANCODE_RIGHTBRACKET,	{ std::make_pair (6, 1) } }, // * instead
			{ SDL_SCANCODE_APOSTROPHE,		{ std::make_pair (5, 5) } }, // : instead 
			{ SDL_SCANCODE_BACKSLASH,		{ std::make_pair (5, 6) } }, // @ instead
			{ SDL_SCANCODE_MINUS,			{ std::make_pair (5, 3) } }, 
			{ SDL_SCANCODE_SLASH,			{ std::make_pair (6, 7) } },
			{ SDL_SCANCODE_SEMICOLON,		{ std::make_pair (6, 2) } },
			{ SDL_SCANCODE_EQUALS,			{ std::make_pair (6, 5) } },
			{ SDL_SCANCODE_BACKSPACE,		{ std::make_pair (0, 0) } },
			{ SDL_SCANCODE_ESCAPE,			{ std::make_pair (7, 1) } }, // The arrow back instead
			{ SDL_SCANCODE_INSERT,			{ std::make_pair (1, 7), std::make_pair (0, 0) } },
			{ SDL_SCANCODE_HOME,			{ std::make_pair (6, 3) } },
			{ SDL_SCANCODE_GRAVE,			{ std::make_pair (6, 0) } }, // The pound key instead
			/** Derivated. */
			{ SDL_SCANCODE_F2,				{ std::make_pair (1, 7), std::make_pair (0, 4) } },
			{ SDL_SCANCODE_F4,				{ std::make_pair (1, 7), std::make_pair (0, 5) } },
			{ SDL_SCANCODE_F6,				{ std::make_pair (1, 7), std::make_pair (0, 6) } },
			{ SDL_SCANCODE_F8,				{ std::make_pair (1, 7), std::make_pair (0, 3) } },
			{ SDL_SCANCODE_LEFT,			{ std::make_pair (1, 7), std::make_pair (0, 2) } },
			{ SDL_SCANCODE_UP,				{ std::make_pair (1, 7), std::make_pair (0, 7) } }
		}
	);

const C64::InputOSSystem::Keystrokes C64::InputOSSystem::_NOKEYSTROKES = { };

// ---
C64::InputOSSystem::InputOSSystem ()
	: MCHEmul::InputOSSystem (_ID, 
		{ { "Name", "IOSystem" },
		  { "Type", "Input" },
		  { "Frequency", "50.0Hz" } }),
	  _cia1 (nullptr)
{ 
	// Nothing else to do...
}

// ---
void C64::InputOSSystem::linkToChips (const MCHEmul::Chips& c)
{
	for (MCHEmul::Chips::const_iterator i = c.begin (); i != c.end () && _cia1 == nullptr;
		_cia1 = dynamic_cast <C64::CIA1*> ((*i++).second));
	// Can't be null after this method...
	assert (_cia1 != nullptr);

	// The CIA 1 will receive the event related with the io system...
	_cia1 -> observe (this);
}
