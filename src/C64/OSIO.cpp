#include <C64/OSIO.hpp>
#include <C64/CIA1.hpp>

// ---
const std::map <SDL_Scancode, C64::InputOSSystem::KeyMPos> C64::InputOSSystem::_C64KEYS
	(
		{
			/** letters */
			{ SDL_SCANCODE_A, std::make_pair (1,2) },
			{ SDL_SCANCODE_B, std::make_pair (3,4) },
			{ SDL_SCANCODE_C, std::make_pair (2,4) },
			{ SDL_SCANCODE_D, std::make_pair (2,2) },
			{ SDL_SCANCODE_E, std::make_pair (1,6) },
			{ SDL_SCANCODE_F, std::make_pair (2,5) },
			{ SDL_SCANCODE_G, std::make_pair (3,2) },
			{ SDL_SCANCODE_H, std::make_pair (3,5) },
			{ SDL_SCANCODE_I, std::make_pair (4,1) },
			{ SDL_SCANCODE_J, std::make_pair (4,2) },
			{ SDL_SCANCODE_K, std::make_pair (4,5) },
			{ SDL_SCANCODE_L, std::make_pair (5,2) },
			{ SDL_SCANCODE_M, std::make_pair (4,4) },
			{ SDL_SCANCODE_N, std::make_pair (4,7) },
			{ SDL_SCANCODE_O, std::make_pair (4,6) },
			{ SDL_SCANCODE_P, std::make_pair (5,1) },
			{ SDL_SCANCODE_Q, std::make_pair (7,6) },
			{ SDL_SCANCODE_R, std::make_pair (2,1) },
			{ SDL_SCANCODE_S, std::make_pair (1,5) },
			{ SDL_SCANCODE_T, std::make_pair (2,6) },
			{ SDL_SCANCODE_U, std::make_pair (3,6) },
			{ SDL_SCANCODE_V, std::make_pair (3,7) },
			{ SDL_SCANCODE_W, std::make_pair (1,1) },
			{ SDL_SCANCODE_X, std::make_pair (2,7) },
			{ SDL_SCANCODE_Y, std::make_pair (3,1) },
			{ SDL_SCANCODE_Z, std::make_pair (1,4) },
			/* keymap numbers */
			{ SDL_SCANCODE_1, std::make_pair (7,0) },
			{ SDL_SCANCODE_2, std::make_pair (7,3) },
			{ SDL_SCANCODE_3, std::make_pair (1,0) },
			{ SDL_SCANCODE_4, std::make_pair (1,3) },
			{ SDL_SCANCODE_5, std::make_pair (2,0) },
			{ SDL_SCANCODE_6, std::make_pair (2,3) },
			{ SDL_SCANCODE_7, std::make_pair (3,0) },
			{ SDL_SCANCODE_8, std::make_pair (3,3) },
			{ SDL_SCANCODE_9, std::make_pair (4,0) },
			{ SDL_SCANCODE_0, std::make_pair (4,3) },
			/* keymap function keys */
			{ SDL_SCANCODE_F1, std::make_pair (0,4) },
			{ SDL_SCANCODE_F3, std::make_pair (0,4) },
			{ SDL_SCANCODE_F5, std::make_pair (0,4) },
			{ SDL_SCANCODE_F7, std::make_pair (0,4) },
			/* keymap: other */
			{ SDL_SCANCODE_RETURN, std::make_pair (0,1) },
			{ SDL_SCANCODE_SPACE, std::make_pair (7,4) },
			{ SDL_SCANCODE_LSHIFT, std::make_pair (1,7) },
			{ SDL_SCANCODE_RSHIFT, std::make_pair (6,4) },
			{ SDL_SCANCODE_COMMA, std::make_pair (5,7) },
			{ SDL_SCANCODE_PERIOD, std::make_pair (5,4) },
			{ SDL_SCANCODE_SLASH, std::make_pair (6,7) },
			{ SDL_SCANCODE_SEMICOLON, std::make_pair (6,2) },
			{ SDL_SCANCODE_EQUALS, std::make_pair (6,5) },
			{ SDL_SCANCODE_BACKSPACE, std::make_pair (0,0) },
			{ SDL_SCANCODE_MINUS, std::make_pair (5,3) },
			/* keymap: these are mapped to other keys */
			{ SDL_SCANCODE_BACKSLASH, std::make_pair (5,5) }, // : 
			{ SDL_SCANCODE_LEFTBRACKET, std::make_pair (5,0) }, // +
			{ SDL_SCANCODE_RIGHTBRACKET, std::make_pair (6,1) }, // *
			{ SDL_SCANCODE_APOSTROPHE, std::make_pair (5,6) }, // @
			{ SDL_SCANCODE_LGUI, std::make_pair (7,5) } // commodore key
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
	std::map <SDL_Scancode, C64::InputOSSystem::KeyMPos>::const_iterator i = _C64KEYS.find (k);
	if (i != _C64KEYS.end ())
		_cia1 -> setKeyboardStatusMatrix ((*i).second.first, (*i).second.second, false /** Meaning pressed. */);
}

// ---
void C64::InputOSSystem::whenKeyReleased (SDL_Scancode k)
{
	std::map <SDL_Scancode, C64::InputOSSystem::KeyMPos>::const_iterator i = _C64KEYS.find (k);
	if (i != _C64KEYS.end ())
		_cia1 -> setKeyboardStatusMatrix ((*i).second.first, (*i).second.second, true /** meaning released. */);
}

// ---
void C64::InputOSSystem::whenJoystickMoved (const MCHEmul::InputOSSystem::SDL_JoyAxisEvents& js)
{
	unsigned char dr [2] = { 0, 0 }; 
	for (const auto& i : js)
	{
		// Only events for two joysticks (1 & 2) are allowed...
		if (i.which != 0 && i.which != 1)
			break;

		dr [i.which] |= (i.axis == 0 /** x axis. */) 
			? ((i.value > 0) ? 8 /** right. */ : ((i.value < 0) ? 4 /** left. */ : 0))
			: ((i.axis == 1 /** y axis. */) 
				? ((i.value > 0) ? 2 /** down. */ : ((i.value < 0) ? 1 /** up. */ : 0))
				: 0 /** axis not supported. */); 
	}

	// The events on the joystick 1 are set on the same place than the keyboard...
	for (size_t i = 0; i < 8; i++)
		_cia1 -> setKeyboardStatusMatrix (i, _cia1 -> keyboardStatusMatrix (i) | (0xff /** 0 means switch on. */ - dr [0]));

	// The movement of the joystick 2...
	// it is simplier!
	_cia1 -> setJoystick2Status (_cia1 -> joystick2Status () | (0xff /** 0 means switch on. */ - dr [1]));
}

// ---
void C64::InputOSSystem::whenJoystickButtonPressed (SDL_JoyButtonEvent jb)
{
	// Only two joysticks are allowed...
	if (jb.which == 0)
		// The events on the joystick 1 are set on the same place than the keyboard...
		for (size_t i = 0; i < 8; i++)
			_cia1 -> setKeyboardStatusMatrix (i, _cia1 -> keyboardStatusMatrix (i) | 
				(0xff /** 0 means switch on. */ - 0x10 /** bit 4 on. */));
	else
	if (jb.which == 1)
		_cia1 -> setJoystick2Status (_cia1 -> joystick2Status () | (0xff /** 0 means switch on. */ - 0x10 /** bit 4 on */));
}

// ---
void C64::InputOSSystem::whenJoystickButtonReleased (SDL_JoyButtonEvent jb)
{
	// Only two joysticks are allowed...
	if (jb.which == 0)
		// The events on the joystick 1 are set on the same place than the keyboard...
		for (size_t i = 0; i < 8; i++)
			_cia1 -> setKeyboardStatusMatrix (i, _cia1 -> keyboardStatusMatrix (i) & 
				(0xff /** 0 means switch on. */ - 0x10 /** bit 4 on. */));
	else
	if (jb.which == 1)
		_cia1 -> setJoystick2Status (_cia1 -> joystick2Status () & (0xff /** 0 means switch on. */ - 0x10 /** bit 4 on */));
}
