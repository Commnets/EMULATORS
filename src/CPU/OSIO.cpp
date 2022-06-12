#include <CPU/OSIO.hpp>

// ---
void MCHEmul::InputOSSystem::refresh ()
{
	SDL_Event event;
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

			case SDL_QUIT:
				_quitRequested = true;
				break;

			default:
				break;
		}
	}
}
