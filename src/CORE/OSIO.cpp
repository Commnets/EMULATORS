#include <CORE/OSIO.hpp>
#include <SDL.h>

// ---
MCHEmul::InputOSSystem::InputOSSystem (int id, const MCHEmul::Attributes& attrs)
	: IODevice (Type::_OUTPUT, id, attrs),
	  _quitRequested (false),
	  _joysticks (),
	  _conversionJoystickMap (), // Nothing by default...
	  _clock (50), // The events of the system will be read 50 times per second...
	  _movementMap ()
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

	_movementMap = MCHEmul::InputOSSystem::JoystickMovementMap ();

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

			case SDL_QUIT:
				_quitRequested = true;
				break;

			default:
				break;
		}
	}

	_clock.countCycles (1);

	if (!js.empty ())
		treatJoystickEvents (std::move (js)); // No longer needed...

	return (true);
}

// ---
MCHEmul::InfoStructure MCHEmul::InputOSSystem::getInfoStructure () const
{
	MCHEmul::InfoStructure result = MCHEmul::IODevice::getInfoStructure ();

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

	result.add ("JOYSTICKS", jI);

	return (result);
}

// ---
void MCHEmul::InputOSSystem::treatJoystickEvents (MCHEmul::InputOSSystem::SDL_JoyAxisEvents&& js)
{
	MCHEmul::InputOSSystem::JoystickMovementMap mMC (_movementMap);

	for (const auto& i : js)
	{ 
		std::map <int, int>::const_iterator cM;
		int nJ = ((cM = _conversionJoystickMap.find (i.which)) == _conversionJoystickMap.end ()) ? i.which : (*cM).second;
		MCHEmul::InputOSSystem::JoystickMovementMap::const_iterator j = _movementMap.find (nJ);
		if (j == _movementMap.end ())
			_movementMap [nJ] = std::vector <int> (i.axis + 1, 0);
		else
		if ((*j).second.size () < (size_t) (i.axis + 1))
			_movementMap [nJ].resize (i.axis + 1, 0);

		_movementMap [nJ][i.axis] = i.value;
	}

	// ...and only when something changes it is communicated as a movement in the joystick...
	// so it means that when they become 0 back, the joystick will stopped...
	if (_movementMap != mMC)
		whenJoystickMoved (_movementMap); // No longer needed...
}
