#include <ZXSpectrum/ULA.hpp>
#include <ZXSpectrum/ULARegisters.hpp>
#include <ZXSpectrum/ZXSpectrum.hpp>
#include <ZXSpectrum/OSIO.hpp>
#include <FZ80/INTInterrupt.hpp>

// ---
const MCHEmul::RasterData ZXSPECTRUM::ULA_PAL::_VRASTERDATA
	(248, 256 /** 8 VSYNC. */, 0 /** 56 top border. */, 191 /** 192 draw. */, 247 /** 56 bottom. */, 247, 247, 312, 0, 0);
const MCHEmul::RasterData ZXSPECTRUM::ULA_PAL::_HRASTERDATA
	(320, 416 /** 96 = Blanking Period + HSYNC (different in 5C, happens before, than in 6C) */, 
	 0 /** 32 left border. */, 255 /** 256 (32 chars * 8 ULA cycles or 4 CPU cycles) draw. */, 
	 287 /** 32 right border. */, 319 /** Other 32 non visible. */, 319, 448, 0, 0);
// NTSC is explained in similar terms than PAL...
const MCHEmul::RasterData ZXSPECTRUM::ULA_NTSC::_VRASTERDATA (216, 224, 0, 191, 215, 215, 215, 264, 0, 0);
const MCHEmul::RasterData ZXSPECTRUM::ULA_NTSC::_HRASTERDATA (320, 416, 0, 255, 287, 319, 319, 448, 0, 0);

// ---
ZXSPECTRUM::ULA::SoundFunction::SoundFunction (unsigned int cF, unsigned int sF)
	: MCHEmul::SoundChip (_ID, 
		{ { "Name", "SoundFunction" },
		  { "Code", "Inside ULA ZXSpectrum" },
		  { "Manufacturer", "ferranti" },
		  { "Year", "1982 - 1985" } },
		nullptr), // There is no need of any wrapper...
	  _chipFrequency (cF),
	  _ULARegisters (nullptr), // Assigned later when ULA ins set up!
	  _lastCPUCycles (0),
	  _clocksPerSample ((unsigned int) (((double) cF * 1.0f) / (double) (sF))),
	  _counterClocksPerSample (0)
{
	setClassName ("SoundFunction");
}

// ---
bool ZXSPECTRUM::ULA::SoundFunction::initialize ()
{
	MCHEmul::SoundChip::initialize ();

	_counterClocksPerSample = 0;

	_lastCPUCycles = 0;

	return (true);
}

// ---
bool ZXSPECTRUM::ULA::SoundFunction::simulate (MCHEmul::CPU* cpu)
{
	assert (_ULARegisters != nullptr); // Just in case...

	// First time?
	if (_lastCPUCycles == 0)
	{ 
		_lastCPUCycles = cpu -> clockCycles (); // Nothing to do...

		return (true);
	}

	// The sound function has to be simulated in every clock cycle...
	// But as it is a function of the ULA, the speed hpuld be then double!
	for (unsigned int i = (cpu -> clockCycles  () - _lastCPUCycles); i > 0; i--)
	{
		_IFDEBUG debugULASoundCycle (cpu, i);

		if (++_counterClocksPerSample >= _clocksPerSample)
		{
			// Just check whether the EAR is or not activated...
			// ...to determine whether the sound function is on or off!
			_counterClocksPerSample = 0;
			char dt = (_ULARegisters -> buzzerSignal ()) ? (char) 100 /** The max volumen. */ : (char) 0; // Full sound or not...
			if (soundMemory () -> addSampleData (&dt, sizeof (char)))
				MCHEmul::SoundChip::notify (MCHEmul::Event (_SOUNDREADY)); // When buffer is full, notify!
		}
	}

	_lastCPUCycles = cpu -> clockCycles ();

	return (true);
}

// ---
MCHEmul::InfoStructure ZXSPECTRUM::ULA::SoundFunction::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::SoundChip::getInfoStructure ());

	result.add ("SAMPLINGFREQ", std::to_string (samplingFrecuency ()));
	result.add ("CLOCKSSAMPLING", std::to_string (_clocksPerSample));

	return (result);
}

// ---
void ZXSPECTRUM::ULA::SoundFunction::debugULASoundCycle (MCHEmul::CPU* cpu, unsigned int i)
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeCompleteLine (className (), cpu -> clockCycles () - i, "Info Cyle", 
		MCHEmul::Attributes ({ 
			{ "Sound", std::string (_ULARegisters -> EARSignal () ? "UP" : "DOWN") } }));
}

// ---
ZXSPECTRUM::ULA::ULA (const MCHEmul::RasterData& vd, const MCHEmul::RasterData& hd,
		unsigned int cF,
		unsigned char f,
		int vV, const MCHEmul::Attributes& attrs)
	: MCHEmul::GraphicalChip (_ID, 
		{ { "Name", "ULA" },
		  // https://www.spectrumforeveryone.com/technical/zx-spectrum-ula-types/
		  // There were many types of ULA during Spectrum life period....
		  // The differences are not, in principle, interesting to be applicable to this simulation...
		  // However they are highlighted in futher descriptions...
		  { "Code", "5C102E, 5C112E, 5C112E-2, 5C112E-3, 6C001E-5, 6C001E-6, 6C001E-7, 7K010E-5 ULA / Amstrad 40056, +2A/+3 Gate Array" },
		  { "Manufacturer", "Ferranti" },
		  { "Year", "1982-1985" } }),
	  _soundFunction (new ZXSPECTRUM::ULA::SoundFunction (cF, _SOUNDSAMPLINGCLOCK)),
	  _ULARegisters (new ZXSPECTRUM::ULARegisters),
	  _ULAView (vV),
	  _raster (vd, hd, 1 /** The step is 1 pixel. */),
	  _showEvents (false),
	  _videoSignalData (),
	  _blinkingFrequency (f),
	  _lastCPUCycles (0),
	  _format (nullptr),
	  _screenMemoryAccessedFromCPUCycles { },
	  _cyclesStopped (0),
	  _eventStatus ()
{
	setClassName ("ULA");

	_format = SDL_AllocFormat (SDL_PIXELFORMAT_ARGB8888);

	// Asign the ULA registers also to the ULA function...
	_soundFunction -> setULARegisters (_ULARegisters);
}

// ---
ZXSPECTRUM::ULA::~ULA ()
{
	SDL_FreeFormat (_format);

	// The sound function is not deleted here because it belongs already to the computer!
}

// ---
bool ZXSPECTRUM::ULA::initialize ()
{
	assert (MCHEmul::GraphicalChip::memoryRef () != nullptr);

	if (!MCHEmul::GraphicalChip::initialize ())
		return (false);

	_raster.initialize ();

	// Notice that all attributes related with drawing signal are not inialized
	// to avoid that when restart a new ulaevents instruction must be commanded!

	_ULARegisters -> initialize ();

	_videoSignalData = ZXSPECTRUM::ULA::VideoSignalData ();

	_lastCPUCycles = 0;

	_cyclesStopped = 0;

	_eventStatus = ZXSPECTRUM::ULA::EventsStatus ();

	_screenMemoryAccessedFromCPUCycles = { };

	return (true);
}

// ---
bool ZXSPECTRUM::ULA::simulate (MCHEmul::CPU* cpu)
{
	// First time?
	if (_lastCPUCycles == 0)
	{ 
		_lastCPUCycles = cpu -> clockCycles (); // Nothing to do...

		return (true);
	}

	// Moments (CPU cycles) when the graphics are read!
	// Bear in mind that the instruction already executed
	// The ULA has to do the same, detect whether the instruction had had taken longer because contention and how much..
	// and then stop the CPU after the main cycle!...
	std::vector <unsigned int> ulaVRAM = { };
	// Simulate the visulization...
	for (unsigned int i = 
			((cpu -> clockCycles  () - _lastCPUCycles) << 1 /** ULA cycles = 2 * CPU Cycles. */); 
			i > 0; i--)
	{
		_IFDEBUG debugULACycle (cpu, i);

		if (_cyclesStopped > 0)
			--_cyclesStopped;

		// Decide whether it is needed to draw or not the events...
		if (_showEvents)
		{
			if (!_eventStatus._INTActive.peekValue () && 
				cpu -> programCounter ().internalRepresentation () == 0x038)
				_eventStatus._INTActive = true;
			// When a NMI interrupt is in execution...
			if (!_eventStatus._NMIActive.peekValue () &&
				cpu -> programCounter ().internalRepresentation () == 0x066)
				_eventStatus._NMIActive = true;
			// When the HALT situation is active...
			if (cpu -> lastInstruction () -> code () == 0x076)
			{
				if (!_eventStatus._HALTActivated.peekValue ())
					_eventStatus._HALTActivated = true;
			}
			else
			{
				_eventStatus._HALTActivated = false;
				_eventStatus._HALTCounter = 0;
			}
		}

		// Read the graphics and draw the visible zone, if it is the case...
		// ...Mark also whethe rthe event have to be drawn or not!
		if (_raster.isInVisibleZone ())
		{
			// IMPORTANT NOTE about contention:
			// This piece of code is to detect the contention and
			// to introduce the rigth delay in the execution of a instruction already executed!
			// The method "specificComputerCycle" of the class ZXSPECTRUM::SinclairZXSpectrum 
			// Set the clock cycles where the contention happened...
			// Take a loot to it

			// If a graphic info (video/color) has been read...
			if (readGraphicInfoAndDrawVisibleZone (cpu) &&
				// ...and it is not already stopped....
				!cpu -> stopped () &&
				// ...and the CPU accessed (it did it already!) this space
				!_screenMemoryAccessedFromCPUCycles.empty ())
				// Keep when, to introduce later the right delay...
				// This is the moment where the ULA started to read the VRAM
				// ...and it takes 4 CPU cycles (8 ULA's)
				// So as this "simulate" method executes as many times as cycles the last instruction took
				// this situation can happen several times...
				// Notice that 3 cycles are substracted, because the ULA detection cycle starts (CLWAIT) T1 never finishes either!
				ulaVRAM.emplace_back (cpu -> clockCycles () - (i >> 1) - 3);

			if (_showEvents)
				drawEvents ();
			// IMPORTANT NOTE:
			// The content situation is calculated later, out of the loop
			// so if existed it would be drawn in the next execution of this method
		}
	
		// First, moves the raster...
		_raster.moveCycles (1);
		// But, if is starting the screen...
		if (_raster.vData ().currentPositionAtBase0 () == 0 &&
			_raster.hData ().currentPositionAtBase0 () == 0)
		{
			// An interrupt is generated...
			cpu -> requestInterrupt 
				(FZ80::INTInterrupt::_ID, i, this, 0 /** The reason is that the screen is complete. */);

			// ...a notification to draw the screen is also generated...
			MCHEmul::GraphicalChip::notify (MCHEmul::Event (_GRAPHICSREADY));

			// ...and additionally, the blinker is updated...
			// that depends on the type of visualization!
			if (++_videoSignalData._flashCounter == _blinkingFrequency)
			{
				_videoSignalData._flashCounter = 0;
				_videoSignalData._flash = !_videoSignalData._flash;
			}
		}
	}

	// Now after the execution of the ULA finishes...
	// ...it is time to calculate any delay...
	// if the a potential contention could have happened...
	if (!ulaVRAM.empty ()) 
	{
		unsigned int ws = 0;
		std::tie (ws, _cyclesStopped) = calculateContention (ulaVRAM);
		if (_cyclesStopped != 0)
			cpu -> setStop (true, MCHEmul::InstructionDefined::_CYCLEALL, ws, _cyclesStopped);

		// The contention will exist if stop the CPU is needed...
		_eventStatus._contentedSituation = (_cyclesStopped != 0);
	}

	// If the MIC signal changed, a signal to the datasette is send...
	// ...the datasette would manage that change as a modification in the information written if any...
	if (_ULARegisters -> MICSignalChanged ()) // After ckecked it, the value returns to false...
		notify (MCHEmul::Event (MCHEmul::DatasetteIOPort::_WRITE, _ULARegisters -> MICSignal () ? 1 : 0));

	_lastCPUCycles = cpu -> clockCycles ();

	return (true);
}

// ---
MCHEmul::InfoStructure ZXSPECTRUM::ULA::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::GraphicalChip::getInfoStructure ());

	result.remove ("Memory"); // This info is not neccesary...
	result.add ("ULARegisters",		std::move (_ULARegisters -> getInfoStructure ()));
	result.add ("Raster",			std::move (_raster.getInfoStructure ()));
	result.add ("ULASoundFunction",	std::move (_soundFunction -> getInfoStructure ()));

	return (result);
}

// ---
void ZXSPECTRUM::ULA::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	auto pressOrReleaseKey = [=](SDL_Scancode sc, bool pressed) -> void
		{
			const ZXSPECTRUM::InputOSSystem::Keystrokes& ks = 
				((ZXSPECTRUM::InputOSSystem*) n) -> keystrokesFor (sc);
			if (!ks.empty ()) // The key has to be defined...
				for (const auto& j : ks)
					_ULARegisters -> setKeyboardStatus (j.first, j.second, pressed);
		};

	switch (evnt.id ())
	{
		case MCHEmul::InputOSSystem::_KEYBOARDKEYPRESSED:
			{
				pressOrReleaseKey (std::static_pointer_cast <MCHEmul::InputOSSystem::KeyboardEvent> 
					(evnt.data ()) -> _key, true);
			}

			break;

		case MCHEmul::InputOSSystem::_KEYBOARDKEYRELEASED:
			{
				pressOrReleaseKey (std::static_pointer_cast <MCHEmul::InputOSSystem::KeyboardEvent> 
					(evnt.data ()) -> _key, false);
			}

			break;

		case MCHEmul::InputOSSystem::_JOYSTICKMOVED:
			{
				std::shared_ptr <MCHEmul::InputOSSystem::JoystickMovementEvent> jm = 
					std::static_pointer_cast <MCHEmul::InputOSSystem::JoystickMovementEvent> (evnt.data ());
				if ((jm -> _joystickId != 0 && jm -> _joystickId != 1) || jm -> _axisValues.size () > 2)
					break; // Only joysticks 0 y 1 are allowed and never more than 2 axis each!

				size_t ct = 0;
				for (size_t i = 0; i < 4; // Clear the situation of all axis...
					_ULARegisters -> setJoystickStatus ((ZXSPECTRUM::ULARegisters::JoystickElement) (i++), false));
				for (auto i : jm -> _axisValues)
				{
					if (ct == 0)
					{
						if (i > 0) 
						{
							_ULARegisters -> setJoystickStatus 
								(ZXSPECTRUM::ULARegisters::JoystickElement::_RIGHT, true);

							// And simulates the key 8 pressed for the emulation of the joystick cursor type...
							pressOrReleaseKey (SDL_SCANCODE_8, true);
							// The opposite direction, which is the left and simulated with the key 5, 
							// is not pressed but released...
							pressOrReleaseKey (SDL_SCANCODE_5, false);
						}
						else 
						if (i < 0) 
						{
							_ULARegisters -> setJoystickStatus 
								(ZXSPECTRUM::ULARegisters::JoystickElement::_LEFT, true);

							// And simulates the key 5 pressed for the emulation of the joystick cursor type...
							pressOrReleaseKey (SDL_SCANCODE_5, true);
							// The opposite direction, which is the right and simulated with the key 8, 
							// is not pressed but released...
							pressOrReleaseKey (SDL_SCANCODE_8, false);
						}
						else
						{
							// Stop!
							pressOrReleaseKey (SDL_SCANCODE_8, false);
							pressOrReleaseKey (SDL_SCANCODE_5, false);
						}
					}
					else
					{
						if (i > 0)
						{
							_ULARegisters -> setJoystickStatus 
								(ZXSPECTRUM::ULARegisters::JoystickElement::_DOWN, true);

							// And simulates the key 6 pressed for the emulation of the joystick cursor type...
							pressOrReleaseKey (SDL_SCANCODE_6, true);
							// The opposite direction, which is the up and simulated with the key 7, 
							// is not pressed but released...
							pressOrReleaseKey (SDL_SCANCODE_7, false);
						}
						else 
						if (i < 0)
						{
							_ULARegisters -> setJoystickStatus 
								(ZXSPECTRUM::ULARegisters::JoystickElement::_UP, true);

							// And simulates the key 7 pressed for the emulation of the joystick cursor type...
							pressOrReleaseKey (SDL_SCANCODE_7, true);
							// The opposite direction, which is the up and simulated with the key 6, 
							// is not pressed but released...
							pressOrReleaseKey (SDL_SCANCODE_6, false);
						}
						else
						{
							// Stop!
							pressOrReleaseKey (SDL_SCANCODE_6, false);
							pressOrReleaseKey (SDL_SCANCODE_7, false);
						}
					}

					ct++;
				}
			}

			break;

		case MCHEmul::InputOSSystem::_JOYSTICKBUTTONPRESSED:
			{
				std::shared_ptr <MCHEmul::InputOSSystem::JoystickButtonEvent> jb = 
					std::static_pointer_cast <MCHEmul::InputOSSystem::JoystickButtonEvent> (evnt.data ());
				if (jb -> _joystickId != 0 && jb -> _joystickId != 1)
					break; // Only joysticks 0 y 1 are allowed!

				// The button is pressed...
				_ULARegisters -> setJoystickStatus 
					(ZXSPECTRUM::ULARegisters::JoystickElement::_FIRE, true);

				// And simulates the key 0 pressed for the emulation of the joystick cursor type...
				pressOrReleaseKey (SDL_SCANCODE_0, true);
			}

			break;

		case MCHEmul::InputOSSystem::_JOYSTICKBUTTONRELEASED:
			{
				std::shared_ptr <MCHEmul::InputOSSystem::JoystickButtonEvent> jb = 
					std::static_pointer_cast <MCHEmul::InputOSSystem::JoystickButtonEvent> (evnt.data ());
				if (jb -> _joystickId != 0 && jb -> _joystickId != 1)
					break; // Only joysticks 0 y 1 are allowed!

				// The button is no longer pressed...
				_ULARegisters -> setJoystickStatus 
					(ZXSPECTRUM::ULARegisters::JoystickElement::_FIRE, false);

				// And simulates the key 0 released for the emulation of the joystick cursor type...
				pressOrReleaseKey (SDL_SCANCODE_0, false);
			}

			break;

		// The rest of the events are not taken here into account!

		// When a signal in the datasette has chanegd, 
		// it means than something has benn read...
		// Put in the EAR signal for the simulation to read it!
		case MCHEmul::DatasetteIOPort::_READ:
			{
				// Activates the EAR signal to be properly read later...
				// Remember that is the Issue 3 the one being emulated...
				_ULARegisters -> setEARSignal (evnt.value () == 1 ? true : false);
				// And the buzzer is also activated...
				_ULARegisters -> alignBuzzerSignal ();
			}

			break;

		default:
			break;
	}
}

// ---
MCHEmul::ScreenMemory* ZXSPECTRUM::ULA::createScreenMemory ()
{
	unsigned int* cP = new unsigned int [25];
	// The colors are partially transparents to allow the blending...
	cP [0]  = SDL_MapRGBA (_format, 0x00, 0x00, 0x00, 0xe0); // Black
	cP [1]  = SDL_MapRGBA (_format, 0x01, 0x00, 0xce, 0xe0); // Blue
	cP [2]  = SDL_MapRGBA (_format, 0xcf, 0x01, 0x00, 0xe0); // Red
	cP [3]  = SDL_MapRGBA (_format, 0xcf, 0x01, 0xce, 0xe0); // Magenta
	cP [4]  = SDL_MapRGBA (_format, 0x00, 0xcf, 0x15, 0xe0); // Green
	cP [5]  = SDL_MapRGBA (_format, 0x01, 0xcf, 0xcf, 0xe0); // Cyan
	cP [6]  = SDL_MapRGBA (_format, 0xcf, 0xcf, 0x15, 0xe0); // Yellow
	cP [7]  = SDL_MapRGBA (_format, 0xcf, 0xcf, 0xcf, 0xe0); // White
	cP [8]  = SDL_MapRGBA (_format, 0x00, 0x00, 0x00, 0xe0); // Bright Black
	cP [9]  = SDL_MapRGBA (_format, 0x02, 0x00, 0xfd, 0xe0); // Bright Blue
	cP [10] = SDL_MapRGBA (_format, 0xff, 0x02, 0x01, 0xe0); // Bright Red
	cP [11] = SDL_MapRGBA (_format, 0xff, 0x02, 0xfd, 0xe0); // Bright Magenta
	cP [12] = SDL_MapRGBA (_format, 0x00, 0xff, 0x1c, 0xe0); // Bright Green
	cP [13] = SDL_MapRGBA (_format, 0x02, 0xff, 0xff, 0xe0); // Bright Cyan
	cP [14] = SDL_MapRGBA (_format, 0xff, 0xff, 0x1d, 0xe0); // Bright Yellow
	cP [15] = SDL_MapRGBA (_format, 0xff, 0xff, 0xff, 0xe0); // Bright White

	// Colors used for events!
	cP [16] = SDL_MapRGBA (_format, 0X00, 0Xf5, 0xff, 0xff); // light Cyan
	cP [17] = SDL_MapRGBA (_format, 0Xfc, 0Xe7, 0x00, 0xff); // light Yellow
	cP [18] = SDL_MapRGBA (_format, 0Xff, 0X6d, 0x28, 0xff); // light Orange
	cP [19] = SDL_MapRGBA (_format, 0Xff, 0X66, 0xff, 0xff); // light Violet
	cP [20] = SDL_MapRGBA (_format, 0Xb2, 0X66, 0xff, 0xff); // light Purple
	cP [21] = SDL_MapRGBA (_format, 0X00, 0X80, 0xff, 0xff); // light Blue
	cP [22] = SDL_MapRGBA (_format, 0X00, 0Xff, 0x00, 0xff); // light Green
	cP [23] = SDL_MapRGBA (_format, 0X50, 0X50, 0x50, 0xff); // Dark Grey
	cP [24] = SDL_MapRGBA (_format, 0X99, 0X4c, 0x00, 0xff); // Brown

	return (new MCHEmul::ScreenMemory (numberColumns (), numberRows (), cP));
}

// --
bool ZXSPECTRUM::ULA::readGraphicInfoAndDrawVisibleZone (MCHEmul::CPU* cpu)
{
	// Here it is sure that the raster is in the visible zone...

	// Gets the position with in the visible zone...
	// That position will take into account the birder (top and left)...
	unsigned short x = 0, y = 0;
	_raster.currentVisiblePosition (x, y);

	// Draws a point with the background first...
	// Unless the raster were in a display zone, it will be the only thing to be drawn...
	_screenMemory -> setPixel 
		(x, y, (unsigned int) (_ULARegisters -> borderColor () & 0x07)); // The bright has no effect in the border...

	// When the raster zone is not in the pure screen zone....
	// There is nothing else to do, but the signal _vidEn  becomes false...
	_videoSignalData._vidEN.set (
			_raster.vData ().isInScreenZone () &&
			_raster.hData ().isInScreenZone ());
	if (!_videoSignalData._vidEN.value ())
		return (false); // The screen part happens when raster is between 0 and 191 (as per definition at the top) and the 
						// ...the hotizontal raster is between 0 and 255...

	// At this point the signal _vidEN gets active...

	// From here onwards, the raster is really in the visible part...

	// But, when the raster is at the beginning any internal registers is again started...
	if (_raster.vData ().currentPosition () == 0 && 
		_raster.hData ().currentPosition () == 0)
	{
		_videoSignalData.initializeDisplayZone (); 
		// ..everything except the flash situation and _vidEn situation...

		_eventStatus._screenPart = true;
	}

	// The _vidEN generates a positive pulse that indicates that the data has to be read...
	// Calculates the location within the screen zone...
	// But 0,0 will be the left up corner in the screen zone
	unsigned short xS = x - _raster.hData ().firstScreenPosition ();
	unsigned short yS = y - _raster.vData ().firstScreenPosition ();
	
	// First time that the _vidEN signal is activated, the data has to be read...
	// ...and in the rest it is read just when the internal clocks says that!
	bool rG = false; // It will be true when the graphics were read...
	if (_videoSignalData._vidEN.positiveEdge ())
	{ 
		readGraphicInfo (xS, yS); 

		rG = _eventStatus._graphicRead = true; 
	}

	// Everytime the clock works a bit is extrated from the shift register 
	// and stored inthe _lastBitShifted...
	// The method returns true when the last bit is shifted and a new one has to be read...
	if (_videoSignalData.clock ())
	{ 
		// The position where to read the next byte is the next position in the screen...
		// That can also be at the beginning of the memory RAM...
		if (++xS >= _raster.hData ().screenPositions ()) 
			{ xS = 0; if (++yS >= _raster.vData ().screenPositions ()) yS = 0; }
		readGraphicInfo (xS, yS);

		rG = _eventStatus._graphicRead = true;
	}

	// But always draw the content of the poixels shifted...
	// but taking into account the flash attribute (1 every 32 frames)....
	// In this case for both PAPER and INK the bright attribute is taking into account...
	unsigned char brightVal = (_videoSignalData._attribute.value () & 0x40) ? 0x08 : 0x00;
	// But also whether the pixels has or not to flash...
	bool fl = _videoSignalData._attribute.value () & 0x80 /** Flash? */ 
		&& _videoSignalData._flash /** Invert? */;
	// to finally draw the pixel...
	_screenMemory -> setPixel (x, y, unsigned int // To be adapted to the right estructure...
		(_videoSignalData._lastBitShifted ^ fl
			? (_videoSignalData._attribute.value () & 0x07 + brightVal)
			: ((_videoSignalData._attribute.value () & 0x38) >> 3) + brightVal));

	return (rG);
}

// ---
void ZXSPECTRUM::ULA::drawEvents ()
{
	// Draw the events...
	unsigned int cEvent = std::numeric_limits <unsigned int>::max ();
	if (_eventStatus._graphicRead)
		cEvent = 23; // It is the basic access... (very light grey)
	if (_eventStatus._screenPart)
		cEvent = 16; // The second most important event...
	if (_eventStatus._contentedSituation)
		cEvent = 18; // But this one is critical to be controlled...
	if (_eventStatus._HALTActivated && _eventStatus._HALTCounter++ < 5)
		cEvent = 19; // A HALT is launched...
	if (_eventStatus._INTActive) 
		cEvent = 20; // When the INT interruption is active
	if (_eventStatus._NMIActive)
		cEvent = 21; // When the NMI interruption is active
	if (cEvent != std::numeric_limits <unsigned int>::max ())
		screenMemory () -> setHorizontalLine // Draw two pixels when the events has happpened...
			(_raster.hData ().currentVisiblePosition (), _raster.vData ().currentVisiblePosition (), 
				((_raster.hData ().currentVisiblePosition () + 2) < _raster.hData ().lastVisiblePosition ()) ? 2 : 1, cEvent);
}

// ---
std::tuple <unsigned int, int> ZXSPECTRUM::ULA::calculateContention 
	(const std::vector <unsigned int>& uC) const
{
	static const unsigned int ULAWAIT [8] = { 6, 5, 4, 3, 2, 1, 0, 0 };

	int cS = 0;
	size_t cCP = 0;

	// If the instruction accessed the VRAM before ULA, 
	// there was no contention possibility....
	while (cCP < _screenMemoryAccessedFromCPUCycles.size () && // The size first...
		   _screenMemoryAccessedFromCPUCycles [cCP] < uC [0]) cCP++;
	// Now, for every access to the ULA that have taken place,
	// looks for an access to the CPU near (less than 8 clock cycles)
	// If found:
	// stop there must be stop cycles added.
	// As a consequence, the following CPU accesses, if any, 
	// shouldn't have happened then at the clock cycles they took place.
	// but the additional stop cycles added after instead...
	// So, the process is repited with the next CPU access, until all ULA access moments are treated.
	// If not found go for the next ULA access.
	unsigned int e = 0;
	unsigned int lB = 0;
	for (size_t uCP = 0; 
			uCP != uC.size () && cCP < _screenMemoryAccessedFromCPUCycles.size (); uCP++)
		if ((e = ((lB = (_screenMemoryAccessedFromCPUCycles [cCP] + cS)) - uC [uCP])) < 4) 
			{ cS += ULAWAIT [e]; cCP++; }

	// But if after reviewing the accesses to the ULA, 
	// there would be still pending CPU accesses to treat...
	// it would be mean, that those CPU accesses could happen in other ULA access moments
	// still pending to be tratead maybe later...
	if (cCP < _screenMemoryAccessedFromCPUCycles.size ())
	{
		if (cCP != 0 || cS != 0)
		{
			std::vector <unsigned int> cP;
			for (; cCP < _screenMemoryAccessedFromCPUCycles.size (); 
				cP.emplace_back (_screenMemoryAccessedFromCPUCycles [cCP++] + cS));
			_screenMemoryAccessedFromCPUCycles = std::move (cP);
		}
	}
	else
;		_screenMemoryAccessedFromCPUCycles = { };

	return (std::tuple <unsigned int, int> (lB, cS));
}

// ---
void ZXSPECTRUM::ULA::debugULACycle (MCHEmul::CPU* cpu, unsigned int i)
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeCompleteLine (className (), cpu -> clockCycles () - (i >> 1), "Info Cyle", 
		MCHEmul::Attributes ({ 
			{ "Raster",
				std::to_string (_raster.currentColumnAtBase0 ()) + "," +
				std::to_string (_raster.currentLineAtBase0 ()) } }));
}

// ---
ZXSPECTRUM::ULA_PAL::ULA_PAL (int vV, unsigned int cF /** Clock frequency. */)
	: ZXSPECTRUM::ULA (_VRASTERDATA, _HRASTERDATA, cF, 12 /** Blinking frequency */, vV,
		 { { "Name", "ULA" },
		  { "Code", "5C102E, 5C112E, 5C112E-2, 5C112E-3, 6C001E-5, 6C001E-6, 6C001E-7, 7K010E-5 ULA / Amstrad 40056, +2A/+3 Gate Array" },
		  { "Manufacturer", "Ferranti" },
		  { "Year", "1982-1985" } })
{
	// Nothing else...
}

// ---
ZXSPECTRUM::ULA_NTSC::ULA_NTSC (int vV, unsigned int cF /** Clock frequency. */)
	: ZXSPECTRUM::ULA (_VRASTERDATA, _HRASTERDATA, cF, 15 /** Blinking frequency. */, vV,
		 { { "Name", "ULA" },
		  { "Code", "5C102E, 5C112E, 5C112E-2, 5C112E-3, 6C001E-5, 6C001E-6, 6C001E-7, 7K010E-5 ULA / Amstrad 40056, +2A/+3 Gate Array" },
		  { "Manufacturer", "Ferranti" },
		  { "Year", "1982-1985" } })
{
	// Nothing else...
}
