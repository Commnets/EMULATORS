#include <CORE/OSIO.hpp>
#include <SDL.h>

// ---
bool MCHEmul::InputOSSystem::initialize ()
{ 
	_quitRequested = false; 
	
	return (true); 
}

// ---
bool MCHEmul::InputOSSystem::simulate ()
{
	if (!MCHEmul::IODevice::simulate ())
		return (false);

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
		whenJoystickMoved (js);

	return (true);
}
