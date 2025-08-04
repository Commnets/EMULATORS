#include <CORE/OSIO.hpp>
#include <CORE/OSIOPeripheral.hpp>
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
bool MCHEmul::InputOSSystem::connectPeripheral (MCHEmul::IOPeripheral* p)
{
	if (dynamic_cast <MCHEmul::InputOSSystemPeripheral*> (p) == nullptr)
		return (false); // Not supported...

	return (IODevice::connectPeripheral (p)); // No peripheral is supported, so it will return false...
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
	bool mainEvent = false;
	SDL_Event event;
	MCHEmul::InputOSSystem::SDL_JoyAxisEvents js;
	auto manageEvent = [&](const SDL_Event& event) -> void
		{
			mainEvent = true;

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
					mainEvent = false;
					break;
			}
		};

	if (_clock.tooQuick ())
	{
		_clock.countCycles (0);

		return (true); // Nothing read, but everything ok...
	}

	// The execution of the peripherals has to have 
	// the same execution pace than the main device...
	if (!MCHEmul::IODevice::simulate (cpu))
		return (false);

	// Manage the internal events...
	// The priority is for the systemn events...
	while (SDL_PollEvent (&event))
		manageEvent (event);

	// If there was no events generated from the system, 
	// there might be an opportunity to manage the ones comming from the outside!
	// Manage the events injected from the peripherals connected to this device...
	// Bear in mind that a static_cast conversion id done because it is supossed that all 
	// peripherals connected to this one are of the type InputOSSystemPeripheral: @see connectPeripheral method
	// If there were be more than 1 peripherals connected generating events simultaneously, only one
	// would treated per IODevice cycle.
	if (!mainEvent)
	{
		for (MCHEmul::IOPeripherals::const_iterator i = _peripherals.begin (); 
				i != _peripherals.end () && !mainEvent; i++)
			if (static_cast <MCHEmul::InputOSSystemPeripheral*> ((*i).second) -> eventReady ())
				manageEvent (static_cast <MCHEmul::InputOSSystemPeripheral*> ((*i).second) -> eventToInject ());
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
