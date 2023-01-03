#include <C64/OSIO.hpp>
#include <C64/CIA1.hpp>

// ---
const std::map <SDL_Scancode, const std::vector <C64::InputOSSystem::KeyMPos>> C64::InputOSSystem::_C64KEYS
	(
		{
			/** letters */
			{ SDL_SCANCODE_A,			{ std::make_pair (1, 2) } },
			{ SDL_SCANCODE_B,			{ std::make_pair (3, 4) } },
			{ SDL_SCANCODE_C,			{ std::make_pair (2, 4) } },
			{ SDL_SCANCODE_D,			{ std::make_pair (2, 2) } },
			{ SDL_SCANCODE_E,			{ std::make_pair (1, 6) } },
			{ SDL_SCANCODE_F,			{ std::make_pair (2, 5) } },
			{ SDL_SCANCODE_G,			{ std::make_pair (3, 2) } },
			{ SDL_SCANCODE_H,			{ std::make_pair (3, 5) } },
			{ SDL_SCANCODE_I,			{ std::make_pair (4, 1) } },
			{ SDL_SCANCODE_J,			{ std::make_pair (4, 2) } },
			{ SDL_SCANCODE_K,			{ std::make_pair (4, 5) } },
			{ SDL_SCANCODE_L,			{ std::make_pair (5, 2) } },
			{ SDL_SCANCODE_M,			{ std::make_pair (4, 4) } },
			{ SDL_SCANCODE_N,			{ std::make_pair (4, 7) } },
			{ SDL_SCANCODE_O,			{ std::make_pair (4, 6) } },
			{ SDL_SCANCODE_P,			{ std::make_pair (5, 1) } },
			{ SDL_SCANCODE_Q,			{ std::make_pair (7, 6) } },
			{ SDL_SCANCODE_R,			{ std::make_pair (2, 1) } },
			{ SDL_SCANCODE_S,			{ std::make_pair (1, 5) } },
			{ SDL_SCANCODE_T,			{ std::make_pair (2, 6) } },
			{ SDL_SCANCODE_U,			{ std::make_pair (3, 6) } },
			{ SDL_SCANCODE_V,			{ std::make_pair (3, 7) } },
			{ SDL_SCANCODE_W,			{ std::make_pair (1, 1) } },
			{ SDL_SCANCODE_X,			{ std::make_pair (2, 7) } },
			{ SDL_SCANCODE_Y,			{ std::make_pair (3, 1) } },
			{ SDL_SCANCODE_Z,			{ std::make_pair (1, 4) } },
			/* keymap numbers */
			{ SDL_SCANCODE_1,			{ std::make_pair (7, 0) } },
			{ SDL_SCANCODE_2,			{ std::make_pair (7, 3) } },
			{ SDL_SCANCODE_3,			{ std::make_pair (1, 0) } },
			{ SDL_SCANCODE_4,			{ std::make_pair (1, 3) } },
			{ SDL_SCANCODE_5,			{ std::make_pair (2, 0) } },
			{ SDL_SCANCODE_6,			{ std::make_pair (2, 3) } },
			{ SDL_SCANCODE_7,			{ std::make_pair (3, 0) } },
			{ SDL_SCANCODE_8,			{ std::make_pair (3, 3) } },
			{ SDL_SCANCODE_9,			{ std::make_pair (4, 0) } },
			{ SDL_SCANCODE_0,			{ std::make_pair (4, 3) } },
			/* keymap function keys */
			{ SDL_SCANCODE_F1,			{ std::make_pair (0, 4) } },
			{ SDL_SCANCODE_F3,			{ std::make_pair (0, 5) } },
			{ SDL_SCANCODE_F5,			{ std::make_pair (0, 6) } },
			{ SDL_SCANCODE_F7,			{ std::make_pair (0, 3) } },
			/* keymap: other */
			{ SDL_SCANCODE_RETURN,		{ std::make_pair (0, 1) } },
			{ SDL_SCANCODE_SPACE,		{ std::make_pair (7, 4) } },
			{ SDL_SCANCODE_LSHIFT,		{ std::make_pair (1, 7) } },
			{ SDL_SCANCODE_RSHIFT,		{ std::make_pair (6, 4) } },
			{ SDL_SCANCODE_COMMA,		{ std::make_pair (5, 7) } },
			{ SDL_SCANCODE_PERIOD,		{ std::make_pair (5, 4) } },
			{ SDL_SCANCODE_SLASH,		{ std::make_pair (6, 7) } },
			{ SDL_SCANCODE_SEMICOLON,	{ std::make_pair (6, 2) } },
			{ SDL_SCANCODE_EQUALS,		{ std::make_pair (6, 5) } },
			{ SDL_SCANCODE_BACKSPACE,	{ std::make_pair (0, 0) } },
			{ SDL_SCANCODE_MINUS,		{ std::make_pair (5, 3) } },
			/* keymap: these are mapped to other keys */
			{ SDL_SCANCODE_BACKSLASH,	{ std::make_pair (5, 5) } }, // : 
			{ SDL_SCANCODE_LEFTBRACKET, { std::make_pair (5, 0) } }, // +
			{ SDL_SCANCODE_RIGHTBRACKET,{ std::make_pair (6, 1) } }, // *
			{ SDL_SCANCODE_APOSTROPHE,	{ std::make_pair (5, 6) } }, // @
			{ SDL_SCANCODE_LGUI,		{ std::make_pair (7, 5) } }, // Commodore Key
			{ SDL_SCANCODE_LCTRL,		{ std::make_pair (7, 2) } }, // Ctrl
			{ SDL_SCANCODE_RCTRL,		{ std::make_pair (7, 2) } }, // Ctrl (the same assigned)
			/** The cursor keys */
			{ SDL_SCANCODE_LEFT,		{ std::make_pair (1, 7) /** Shift */, std::make_pair (0, 2) /** Right */ } },
			{ SDL_SCANCODE_RIGHT,		{ std::make_pair (0, 2) } },
			{ SDL_SCANCODE_UP,			{ std::make_pair (1, 7), std::make_pair (0, 7) } },
			{ SDL_SCANCODE_DOWN,		{ std::make_pair (0, 7) } },
			/** Other function keys */
			{ SDL_SCANCODE_F2,			{ std::make_pair (1, 7), std::make_pair (0, 4) } },
			{ SDL_SCANCODE_F4,			{ std::make_pair (1, 7), std::make_pair (0, 5) } },
			{ SDL_SCANCODE_F6,			{ std::make_pair (1, 7), std::make_pair (0, 6) } },
			{ SDL_SCANCODE_F8,			{ std::make_pair (1, 7), std::make_pair (0, 3) } }
		}
	);

// ---
void C64::InputOSSystem::linkToChips (const MCHEmul::Chips& c)
{
	for (const auto& i : c)
		if ((_cia1 = dynamic_cast <C64::CIA1*> (i.second)) != nullptr)
			break;

	// Can't be null after this method...
	assert (_cia1 != nullptr);
}

// ---
void C64::InputOSSystem::whenKeyPressed (SDL_Scancode k)
{
	std::map <SDL_Scancode, const std::vector <C64::InputOSSystem::KeyMPos>>::const_iterator i = _C64KEYS.find (k);
	if (i != _C64KEYS.end ())
		for (const auto& j : (*i).second)
			_cia1 -> setKeyboardStatusMatrix (j.first, j.second, false /** Meaning pressed. */);
}

// ---
void C64::InputOSSystem::whenKeyReleased (SDL_Scancode k)
{
	std::map <SDL_Scancode, const std::vector <C64::InputOSSystem::KeyMPos>>::const_iterator i = _C64KEYS.find (k);
	if (i != _C64KEYS.end ())
		for (const auto& j : (*i).second)
			_cia1 -> setKeyboardStatusMatrix (j.first, j.second, true /** Meaning released. */);
}

// ---
void C64::InputOSSystem::whenJoystickMoved (const MCHEmul::InputOSSystem::JoystickMovementMap& jm)
{
	// It necessary to translate the position of the joysticks into the C64 records.
	// In the C64 there are only 2 possible joysticks with 2 axis (x and y) each.
	if (jm.size () > 2)
		return; // No possible to manage...

	bool e = false;
	for (const auto& i : jm)
		e = (i.first != 0 && i.first != 1) || (i.second.size () > 2);
	if (e)
		return; // Either, the joysticks id are not recognized or the number of axis received is wrong...

	// Time to translate into C64 register values...
	int dr [2] = { 0, 0 };
	for (const auto& i : jm)
	{
		size_t ct = 0;
		for (auto j : i.second)
		{ 
			dr [i.first] |= (ct == 0) 
				? ((j > 0) ? 8 /** right. */ : ((j < 0) ? 4 /** left. */ : 0))
				: ((j > 0) ? 2 /** down. */ : ((j < 0) ? 1 /** up. */ : 0));

			ct++;
		}
	}

	// The events on the joystick 1 are set on the same place than the keyboard...
	_cia1 -> setJoystick1InputPending (dr [0] != 0x00);
	for (size_t i = 0; i < 8; i++)
		_cia1 -> setKeyboardStatusMatrix (i, 
			(dr [0] == 0) ? 0xff : _cia1 -> keyboardStatusMatrix (i) & (0xff - dr [0] /** 0 means swicth on. */));

	// The movement of the joystick 2...
	// it is simplier!
	_cia1 -> setJoystick2Status ((dr [1] == 0) 
		? 0xff : _cia1 -> joystick2Status () & (0xff - dr [1] /** 0 means switch on. */));
}

// ---
void C64::InputOSSystem::whenJoystickButtonPressed (SDL_JoyButtonEvent jb)
{
	// Only two joysticks are allowed...
	if (jb.which == 0)
		// The events on the joystick 1 are set on the same place than the keyboard...
		for (size_t i = 0; i < 8; i++)
			_cia1 -> setKeyboardStatusMatrix (i, _cia1 -> keyboardStatusMatrix (i) & 
				(0xff - 0x10 /** bit 4 clear when on. */));
	else
	if (jb.which == 1)
		_cia1 -> setJoystick2Status (_cia1 -> joystick2Status () & (0xff /** 0 means switch on. */ - 0x10));
}

// ---
void C64::InputOSSystem::whenJoystickButtonReleased (SDL_JoyButtonEvent jb)
{
	// Only two joysticks are allowed...
	if (jb.which == 0)
		// The events on the joystick 1 are set on the same place than the keyboard...
		for (size_t i = 0; i < 8; i++)
			_cia1 -> setKeyboardStatusMatrix (i, _cia1 -> keyboardStatusMatrix (i) | 0x010);
	else
	if (jb.which == 1)
		_cia1 -> setJoystick2Status (_cia1 -> joystick2Status () | 0x10);
}
