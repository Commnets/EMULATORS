#include <CORE/OSIO.hpp>
#include <CORE/OSIOPeripheral.hpp>
#include <CORE/SDL.hpp>

// ---
MCHEmul::InputOSSystem::InputOSSystem (int id, const MCHEmul::Attributes& attrs)
	: IODevice (Type::_OUTPUT, id, attrs),
	  _quitRequested (false),
	  _joysticks (),
	  _conversionJoystickMap (), // Nothing by default...
	  _clock (25), // The events of the system will be read 25 times per second...
	  _joyMovementMap (),
	  _joystickIndices (),
	  _lastJoystickInstanceId (0),
	  _lastJoystickIndex (-1)
{ 
	setClassName ("IOSystem"); 
}

// ---
MCHEmul::InputOSSystem::~InputOSSystem ()
{
	closeJoysticks ();
}

// ---
std::string MCHEmul::InputOSSystem::currentAssignementJoysticksAsString () const
{ 
	bool f = true;
	std::string result;
	for (const auto& i : _conversionJoystickMap) 
	{
		result += (f ? "" : ",") + 
			std::to_string (i.first) + "->" + std::to_string (i.second);

		f = false;
	}

	return (result); 
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

	// Release previous references and invalidate their ID translations.
	closeJoysticks ();

#if MCHEMUL_SDL_VERSION == 2
	bool result = openJoysticksSDL2 ();
#elif MCHEMUL_SDL_VERSION == 3
	bool result = openJoysticksSDL3 ();
#endif
	if (!result)
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	_error = MCHEmul::_NOERROR;

	_clock.start ();

	return (true);
}

// ---
bool MCHEmul::InputOSSystem::simulate (MCHEmul::CPU* cpu)
{
	if (_clock.tooQuick ())
	{
		_clock.countCycles (0);

		return (true);
	}

	// Peripherals run at the same pace as the input device.
	if (!MCHEmul::IODevice::simulate (cpu))
		return (false);

	bool mainEvent = false;
	SDL_Event event;
	MCHEmul::InputOSSystem::SDL_JoyAxisEvents js;

	// Native events take priority over peripheral-injected events.
	while (SDL_PollEvent (&event))
	{
#if MCHEMUL_SDL_VERSION == 2
		// Dispatch first so short-circuit evaluation never skips a queued event.
		mainEvent = manageEventSDL2 (event, js) || mainEvent;
#elif MCHEMUL_SDL_VERSION == 3
		if (!convertMouseEventSDL3 (event))
		{
			_error = MCHEmul::_DEVICE_ERROR;

			return (false);
		}

		// Dispatch first so short-circuit evaluation never skips a queued event.
		mainEvent = manageEventSDL3 (event, js) || mainEvent;
#endif
	}

	// Injected events already use framework coordinates and bypass conversion.
	// At most one recognized injected event is handled per input cycle.
	if (!mainEvent)
	{
		for (MCHEmul::IOPeripherals::const_iterator i = _peripherals.begin ();
				i != _peripherals.end () && !mainEvent; i++)
		{
			MCHEmul::InputOSSystemPeripheral* peripheral =
				static_cast <MCHEmul::InputOSSystemPeripheral*> ((*i).second);

			if (peripheral -> eventReady ())
			{
#if MCHEMUL_SDL_VERSION == 2
				mainEvent =
					manageEventSDL2 (peripheral -> eventToInject (), js);
#elif MCHEMUL_SDL_VERSION == 3
				mainEvent =
					manageEventSDL3 (peripheral -> eventToInject (), js);
#endif
			}
		}
	}

	_clock.countCycles (1);

	if (!js.empty ())
		treatJoystickMovementEvents (std::move (js));

	return (true);
}

// ---
MCHEmul::InfoStructure MCHEmul::InputOSSystem::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::IODevice::getInfoStructure ());

	std::string jI;
	for (const auto& i : _joystickIndices)
	{
		std::map <int, int>::const_iterator j = _conversionJoystickMap.find (i.second);
		jI += (jI.empty () ? "" : ",") + std::to_string (i.second) + "(" +
			((j == _conversionJoystickMap.end ()) ? "none" : std::to_string ((*j).second)) + ")";
	}

	result.add ("JOYSTICKS", std::move (jI));

	return (result);
}

#if MCHEMUL_SDL_VERSION == 2
// ---
bool MCHEmul::InputOSSystem::manageEventSDL2 (
	const SDL_Event& event, MCHEmul::InputOSSystem::SDL_JoyAxisEvents& js)
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
			return (false);
	}

	return (true);
}
#elif MCHEMUL_SDL_VERSION == 3
// ---
bool MCHEmul::InputOSSystem::manageEventSDL3 (
	const SDL_Event& event, MCHEmul::InputOSSystem::SDL_JoyAxisEvents& js)
{
	switch (event.type)
	{
		case SDL_EVENT_KEY_DOWN:
			whenKeyPressed (event.key.scancode);
			break;

		case SDL_EVENT_KEY_UP:
			whenKeyReleased (event.key.scancode);
			break;

		case SDL_EVENT_JOYSTICK_AXIS_MOTION:
			js.push_back (event.jaxis);
			break;

		case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
			whenJoystickButtonPressed (event.jbutton);
			break;

		case SDL_EVENT_JOYSTICK_BUTTON_UP:
			whenJoystickButtonReleased (event.jbutton);
			break;

		case SDL_EVENT_MOUSE_MOTION:
			whenMouseMoved (event.motion);
			break;

		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			whenMouseButtonPressed (event.button);
			break;

		case SDL_EVENT_MOUSE_BUTTON_UP:
			whenMouseButtonReleased (event.button);
			break;

		case SDL_EVENT_QUIT:
			_quitRequested = true;
			break;

		default:
			return (false);
	}

	return (true);
}

// ---
bool MCHEmul::InputOSSystem::convertMouseEventSDL3 (SDL_Event& event) const
{
	SDL_WindowID windowId = 0;

	switch (event.type)
	{
		case SDL_EVENT_MOUSE_MOTION:
			windowId = event.motion.windowID;
			break;

		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		case SDL_EVENT_MOUSE_BUTTON_UP:
			windowId = event.button.windowID;
			break;

		default:
			return (true);
	}

	// Events without an associated window retain their original coordinates.
	if (windowId == 0)
		return (true);

	SDL_Window* window = SDL_GetWindowFromID (windowId);
	if (window == nullptr)
		return (false);

	SDL_Renderer* renderer = SDL_GetRenderer (window);
	if (renderer == nullptr)
		return (false);

	return (SDL_ConvertEventToRenderCoordinates (renderer, &event));
}
#endif

#if MCHEMUL_SDL_VERSION == 2
// ---
bool MCHEmul::InputOSSystem::openJoysticksSDL2 ()
{
	int count = SDL_NumJoysticks ();
	if (count < 0)
		return (false);

	_joysticks.reserve ((size_t) count);
	_joystickIndices.reserve ((size_t) count);
	for (int i = 0; i < count; i++)
	{
		SDL_Joystick* joystick = SDL_JoystickOpen (i);
		if (joystick == nullptr)
			continue;

		SDL_JoystickID instanceId = SDL_JoystickInstanceID (joystick);
		if (instanceId < 0)
		{
			SDL_JoystickClose (joystick);

			continue;
		}

		_joysticks.push_back (joystick);
		_joystickIndices.emplace_back (instanceId, i);
	}

	return (true);
}
#elif MCHEMUL_SDL_VERSION == 3
// ---
bool MCHEmul::InputOSSystem::openJoysticksSDL3 ()
{
	int count = 0;
	SDL_JoystickID* instanceIds = SDL_GetJoysticks (&count);
	if (instanceIds == nullptr)
		return (false);

	_joysticks.reserve ((size_t) count);
	_joystickIndices.reserve ((size_t) count);
	for (int i = 0; i < count; i++)
	{
		SDL_Joystick* joystick = SDL_OpenJoystick (instanceIds [i]);
		if (joystick == nullptr)
			continue;

		_joysticks.push_back (joystick);
		_joystickIndices.emplace_back (instanceIds [i], i);
	}

	SDL_free (instanceIds);

	return (true);
}
#endif

// ---
void MCHEmul::InputOSSystem::closeJoysticks ()
{
	// No cached translation may survive closing or rebuilding the device list.
	_lastJoystickIndex = -1;
	_lastJoystickInstanceId = 0;

	for (SDL_Joystick* joystick : _joysticks)
#if MCHEMUL_SDL_VERSION == 2
		SDL_JoystickClose (joystick);
#elif MCHEMUL_SDL_VERSION == 3
		SDL_CloseJoystick (joystick);
#endif

	_joysticks.clear ();
	_joystickIndices.clear ();
	_joyMovementMap.clear ();
}

// ---
void MCHEmul::InputOSSystem::treatJoystickMovementEvents (MCHEmul::InputOSSystem::SDL_JoyAxisEvents&& js)
{
	bool updated = false;
	for (const auto& i : js)
	{
		int index = joystickIndex (i.which);
		if (index < 0)
			continue;

		MCHEmul::InputOSSystem::JoystickMovementMap::iterator j = _joyMovementMap.find (index);
		if (j == _joyMovementMap.end ())
			j = _joyMovementMap.emplace (index, std::vector <int> ((size_t) i.axis + 1, 0)).first;
		else if ((*j).second.size () < (size_t) i.axis + 1)
			(*j).second.resize ((size_t) i.axis + 1, 0);

		(*j).second [i.axis] = i.value;
		updated = true;
	}

	if (updated)
		whenJoystickMoved (_joyMovementMap);
}
