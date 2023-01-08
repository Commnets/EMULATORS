#include <CORE/OSIO.hpp>
#include <SDL.h>

// ---
bool MCHEmul::InputOSSystem::initialize ()
{ 
	_quitRequested = false; 
	
	int e = SDL_InitSubSystem (SDL_INIT_JOYSTICK);
	if (SDL_NumJoysticks () > 0)
	{
		for (int i = 0; i < SDL_NumJoysticks (); i++)
		{
			SDL_Joystick* joy = SDL_JoystickOpen (i);
			if (joy)
				_joysticks.push_back (joy);
		}
	}

	SDL_JoystickEventState (SDL_ENABLE);

	_movementMap = MCHEmul::InputOSSystem::JoystickMovementMap ();

	_clock.start ();

	return ((e == 0) ? true : false); 
}

// ---
bool MCHEmul::InputOSSystem::simulate ()
{
	if (!MCHEmul::IODevice::simulate ())
		return (false);

	if (_clock.realCyclesPerSecond () > _clock.cyclesPerSecond ())
	{
		_clock.countCycles (0);

		return (true); // Nothing read, but everything ok...
	}

	SDL_Event event;

	MCHEmul::InputOSSystem::SDL_JoyAxisEvents js;

	while (SDL_PollEvent (&event))
	{
		switch (event.type)
		{
			case SDL_KEYDOWN:
				whenKeyPressed (event.key.keysym.scancode);
				break;

			case SDL_KEYUP:
				whenKeyReleased (event.key.keysym.scancode);
				break;

			case SDL_JOYAXISMOTION:
				js.push_back (event.jaxis);
				break;

			case SDL_JOYBUTTONDOWN:
				whenJoystickButtonPressed (event.jbutton);
				break;
			
			case SDL_JOYBUTTONUP:
				whenJoystickButtonReleased (event.jbutton);
				break;

			case SDL_QUIT:
				_quitRequested = true;
				break;

			default:
				break;
		}
	}

	if (!js.empty ())
		treatJoystickEvents (js);

	return (true);
}

// ---
void MCHEmul::InputOSSystem::treatJoystickEvents (const MCHEmul::InputOSSystem::SDL_JoyAxisEvents& js)
{
	MCHEmul::InputOSSystem::JoystickMovementMap mMC (_movementMap);

	for (const auto& i : js)
	{ 
		MCHEmul::InputOSSystem::JoystickMovementMap::const_iterator j = _movementMap.find (i.which);
		if (j == _movementMap.end ())
			_movementMap [i.which] = std::vector <int> (i.axis + 1, 0);
		else
		if ((*j).second.size () < (size_t) (i.axis + 1))
			_movementMap [i.which].resize (i.axis + 1, 0);

		_movementMap [i.which][i.axis] = i.value;
	}

	// ...and only when something changes it is communicated as a movement in the joystick...
	// so it means that when they become 0 back, the joystick will stopped...
	if (_movementMap != mMC)
		whenJoystickMoved (_movementMap);
}
