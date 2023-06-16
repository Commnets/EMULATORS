#include <CORE/OSIO.hpp>
#include <SDL.h>

// ---
MCHEmul::InputOSSystem::InputOSSystem (int id, const MCHEmul::Attributes& attrs)
	: IODevice (Type::_OUTPUT, id, attrs),
	  _quitRequested (false),
	  _joysticks (),
	  _conversionJoystickMap (), // Nothing by default...
	  _clock (25), // The events of the system will be read 25 times per second...
	  _joyMovementMap ()
{ 
	setClassName ("IOSystem"); 
}

// ---
bool MCHEmul::InputOSSystem::initialize ()
{ 
	_quitRequested = false; 
	
	_joysticks.clear ();

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

	_joyMovementMap = { };

	_clock.start ();

	return ((e == 0) ? true : false); 
}

// ---
bool MCHEmul::InputOSSystem::simulate (MCHEmul::CPU* cpu)
{
	if (!MCHEmul::IODevice::simulate (cpu))
		return (false);

	if (_clock.tooQuick ())
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

			case SDL_MOUSEMOTION:
				whenMouseMoved (event.motion);
				break;

			case SDL_MOUSEBUTTONDOWN:
				whenMouseButtonPressed (event.button);
					break;

			case SDL_MOUSEBUTTONUP:
				whenMouseButtonReleased (event.button);
					break;

			case SDL_QUIT:
				_quitRequested = true;
				break;

			default:
				break;
		}
	}

	_clock.countCycles (1);

	if (!js.empty ())
		treatJoystickMovementEvents (std::move (js)); // No longer needed...

	return (true);
}

// ---
MCHEmul::InfoStructure MCHEmul::InputOSSystem::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::IODevice::getInfoStructure ());

	// Just to add the information about the joysticks...
	int ct = 0;
	std::map <int, int>::const_iterator j;
	std::string jI = "";
	for (const auto& i : _joysticks) 
	{ 
		jI += ((ct != 0) ? "," : "") + std::to_string (ct) + "(" + 
			(((j = _conversionJoystickMap.find (ct)) == _conversionJoystickMap.end ()) ? "none" : std::to_string ((*j).second)) + ")";
		ct++;
	}

	result.add ("JOYSTICKS", std::move (jI));

	return (result);
}

// ---
void MCHEmul::InputOSSystem::treatJoystickMovementEvents (MCHEmul::InputOSSystem::SDL_JoyAxisEvents&& js)
{
	// All event received in a cycle are consolidated...
	for (const auto& i : js)
	{ 
		MCHEmul::InputOSSystem::JoystickMovementMap::iterator j = 
			_joyMovementMap.find (i.which);
		if (j == _joyMovementMap.end ())
			_joyMovementMap [i.which] = std::vector <int> (i.axis + 1, 0);
		else
		if ((*j).second.size () < (size_t) (i.axis + 1))
			(*j).second.resize (i.axis + 1, 0);

		// Adds the value of the axis...
		_joyMovementMap [i.which][i.axis] = i.value;
	}

	// ...and the consolidated status is notified...
	whenJoystickMoved (_joyMovementMap); 
}
