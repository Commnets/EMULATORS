#include <COMMODORE/TED/TED.hpp>
#include <COMMODORE/TED/TEDSoundWrapper.hpp>
#include <F6500/IRQInterrupt.hpp>

// ---
const MCHEmul::Address COMMODORE::TED::_COLORMEMORY ({ 0x00, 0xd8 }, false);
const MCHEmul::RasterData COMMODORE::TED::_PALVRASTERDATA (0, 16, 51, 250, 299, 311, 312, 4, 4);
const MCHEmul::RasterData COMMODORE::TED::_PALHRASTERDATA 
	(404, 480, 24, 343, 380, 403, 504 /** For everyting to run, it has to be divisible by 8. */, 7, 9);
const MCHEmul::RasterData COMMODORE::TED::_NTSCVRASTERDATA (27, 41, 51, 250, 12, 26, 262, 4, 4);
const MCHEmul::RasterData COMMODORE::TED::_NTSCHRASTERDATA 
	(412, 488, 24, 343, 388, 411, 512 /** For everything to run, it has to be divisible by 8. */, 7, 9);
// This two positions are fized...
const MCHEmul::Address COMMODORE::TED::_MEMORYPOSIDLE1 = MCHEmul::Address ({ 0xff, 0x39 }, false);
const MCHEmul::Address COMMODORE::TED::_MEMORYPOSIDLE2 = MCHEmul::Address ({ 0xff, 0x3f }, false);

// ---
COMMODORE::TED::SoundFunction::SoundFunction (MCHEmul::SoundLibWrapper* sW)
	: MCHEmul::SoundChip (_ID, 
		{ { "Name", "SoundFunction" },
		  { "Code", "Inside 6560 (NTSC), 6561/6561E/6561-101 (PAL)" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Year", "1977" } },
		sW),
	  _lastCPUCycles (0)
{ 
	// Take care that the sound emulation library was not null at all...
	// ...and also belonging to the right type..
	assert (dynamic_cast <COMMODORE::TEDSoundLibWrapper*> (soundWrapper ()) != nullptr);

	setClassName ("SoundFunction");
}

// ---
bool COMMODORE::TED::SoundFunction::initialize ()
{
	if (!MCHEmul::SoundChip::initialize ())
		return (false);

	_lastCPUCycles = 0;

	return (true);
}

// ---
bool COMMODORE::TED::SoundFunction::simulate (MCHEmul::CPU* cpu)
{
	// First time?
	if (_lastCPUCycles == 0)
	{ 
		_lastCPUCycles = cpu -> clockCycles (); // Nothing to do...

		return (true);
	}

	if (soundWrapper () != nullptr)
	{
		for (unsigned int i = (cpu -> clockCycles () - _lastCPUCycles); i > 0 ; i--)
		{
			// The number of bytes that can come from the wrapper might not be fixed...
			MCHEmul::UBytes data;
			if (soundWrapper () -> getData (cpu, data))
			{
				for (size_t j = 0; j < data.size (); j++)
				{
					char dt = data [j].value ();
					if (soundMemory () -> addSampleData (&dt, sizeof (char)))
						MCHEmul::SoundChip::notify (MCHEmul::Event (_SOUNDREADY));
				}
			}
		}
	}

	_lastCPUCycles = cpu -> clockCycles ();

	return (true);
}

// ---
MCHEmul::InfoStructure COMMODORE::TED::SoundFunction::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::SoundChip::getInfoStructure ());

	result.remove ("Memory"); // This info is not neccesary...
	result.add ("SoundLibWrapper",	std::move (soundWrapper () -> getInfoStructure ())); // To know which library is behing...

	return (result);
}

// ---
COMMODORE::TED::TED (int vV, MCHEmul::SoundLibWrapper* sW, const MCHEmul::Attributes& attrs)
	: MCHEmul::GraphicalChip (_ID, 
		{ { "Name", "TED" },
		  { "Code", "7360/8360 Both for PAL & NTSC" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Year", "1984" } }),
	  _soundFunction (new COMMODORE::TED::SoundFunction (sW)),
	  _TEDRegisters (nullptr), 
	  _TEDView (vV),
	  _cyclesPerRasterLine (63),
	  _incCyclesPerRasterLine (0),
	  _raster (_PALVRASTERDATA, _PALHRASTERDATA, 8 /** step. */),
	  _lastCPUCycles (0),
	  _format (nullptr),
	  _cycleInRasterLine (1),
	  _videoActive (true),
	  _lastVBlankEntered (false),
	  _lastBadLineScrollY (-1), _newBadLineCondition (false), _badLineStopCyclesAdded (false),
	  _tedGraphicInfo ()
{
	assert (_cyclesPerRasterLine >= 63);

	setClassName ("TED");

	_format = SDL_AllocFormat (SDL_PIXELFORMAT_ARGB8888);
}

// ---
COMMODORE::TED::~TED ()
{
	/**
		delete (_soundFunction);
		It is not invoked, because the sound function is
		managed as a different chip from the computer on regards simulation.
		That chip is owned by the computer and then destroyed by it. */

	SDL_FreeFormat (_format);
}

// ---
bool COMMODORE::TED::initialize ()
{
	assert (memoryRef () != nullptr);

	if (!MCHEmul::GraphicalChip::initialize ())
		return (false);

	// Gets the memory block dedicated to the TED
	if (!(_TEDRegisters = 
		dynamic_cast <COMMODORE::TEDRegisters*> (memoryRef () -> subset (COMMODORE::TEDRegisters::_TEDREGS_SUBSET))))
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	_raster.initialize ();

	_TEDRegisters -> initialize ();

	_lastCPUCycles = 0;
	
	_cycleInRasterLine = 1;

	_videoActive = true;

	_lastVBlankEntered = false;

	_lastBadLineScrollY = -1;
	_newBadLineCondition = false;
	_badLineStopCyclesAdded = false;

	return (true);
}

// ---
bool COMMODORE::TED::simulate (MCHEmul::CPU* cpu)
{
	// First time?
	if (_lastCPUCycles == 0)
	{ 
		_lastCPUCycles = cpu -> clockCycles (); // Nothing to do...

		return (true);
	}

	auto isNewBadLine = [&]() -> bool
	{
		bool result = 
			_videoActive && // Bad lines only possible when the video is active...
			_raster.currentLine () >= _FIRSTBADLINE && 
			_raster.currentLine () <= _LASTBADLINE &&
			(unsigned char) (_raster.currentLine () & 0x07) /** The three last bits. */ == 
				_TEDRegisters -> verticalScrollPosition () && // aligned with the scrollY
			_lastBadLineScrollY != (int) _TEDRegisters -> verticalScrollPosition (); //..and obvious if that situation in the scroll changed
		
		if (result)
			_lastBadLineScrollY = _TEDRegisters -> verticalScrollPosition ();

		return (result);
	};

	// If the "video reset flag" is actived nothing is done...
	if (_TEDRegisters -> videoResetActive ())
		return (true);

	// Adapt the size of the display zone to the parameters specificied in the register...
	// The zone where sprites and texts are finally visible is call the "screen zone"
	_raster.reduceDisplayZone
		(!_TEDRegisters -> textDisplay25RowsActive (), !_TEDRegisters -> textDisplay40ColumnsActive ());

	// The simulation has to be repeated as many time as cycles have spent since the last invocation...
	for (unsigned int i = (cpu -> clockCycles  () - _lastCPUCycles); i > 0; i--)
	{
		if (deepDebugActive ())
		{
			*_deepDebugFile
				// Where
				<< "TED\t" 
				// When
				<< std::to_string (cpu -> clockCycles () - i) << "\t" // clock cycles at that point
				// What
				<< "Info cycle\t\t"
				// Data
				<< "Raster:["
				<< std::to_string (_raster.currentColumnAtBase0 ()) << "," 
				<< std::to_string (_raster.currentLineAtBase0 ()) << ","
				<< std::to_string (_cycleInRasterLine)
				<< "], Graphics:"
				<< std::to_string ((int) _TEDRegisters -> graphicModeActive ())
				<< ", Memory:["
				<< "Bk=" << std::to_string ((int) 1) << ","
				<< "SM=$" << MCHEmul::removeAll0 (_TEDRegisters -> screenMemory ().asString
					(MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) << ","
				<< "CM=$" << MCHEmul::removeAll0 (_TEDRegisters -> charDataMemory ().asString
					(MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) << ","
				<< "BM=$" << MCHEmul::removeAll0 (_TEDRegisters -> bitmapMemory ().asString
					(MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) << "]\n";

			// Draws lines where there is a IRQ interruption...
			unsigned short lrt = 
				_raster.lineInVisibleZone (_TEDRegisters -> IRQRasterLineAt ());
			if (lrt <= _raster.vData ().lastVisiblePosition ())
			{
				unsigned char clrt = _TEDRegisters -> backgroundColor () == 15 
					? 0 : _TEDRegisters -> backgroundColor () + 1; /** to be visible. */
				screenMemory () -> setHorizontalLine (0, lrt, _raster.visibleColumns (), clrt);
			}
		}

		// Whether the video is active or not is only checked at the very first bad line...
		_videoActive = (_raster.currentLine () == _FIRSTBADLINE) 
			? !_TEDRegisters -> blankEntireScreen () : _videoActive; // The value can change at any cycle of this first bad line...

		// Is there a bad line situation?
		// A bad line can happen at any cycle within the raster line.
		// When happened, the graphics are read and the situation is latched because maybe additional stop cycles could be needed.
		if (isNewBadLine ())
		{
			if (deepDebugActive ())
				*_deepDebugFile << "\t\t\t\tBad line situation\n";

			_newBadLineCondition = true;		// latched...
			_badLineStopCyclesAdded = false;	// ...the cycles have to be added...

			_tedGraphicInfo._idleState = false; // No longer in "idle" state but in the "screen" one!
		}

		// When TED is about to read sprites or graphics info (bad line),
		// the CPU has to stop 3 cycles in advance (just for READ activities) for those activities,
		// unless it was stopped previously and that stop situation were still valid...
		// In the case of graphics that stop only happens when the situation arise in the "screen cycles" (40)
		if (!cpu -> stopped () && 
				(_newBadLineCondition && (_cycleInRasterLine >= 12 && _cycleInRasterLine < 52)))
			cpu -> setStop (true, MCHEmul::Instruction::_CYCLEREAD /** only read in not allowed. */, 
				cpu -> clockCycles () - i, 3);

		// Treat the right cycle...
		// ...and as a consequence the CPU can be also stopped...
		unsigned int cS = 0;
		if ((cS = treatRasterCycle ()) > 0)
			cpu -> setStop (true, MCHEmul::Instruction::_CYCLEALL /** fully stopped. */, cpu -> clockCycles () - i, (int) cS);

		// Draws the graphics & border if it has to do so...
		if (_raster.isInVisibleZone ())
			drawVisibleZone (cpu);

		// Move to the next cycle...
		_cycleInRasterLine++;

		// Move 8 pixels right in the raster line and jump to other line is possible...
		// Notice that the variable _isNewRasterLine becomes true when a new line comes...
		// Always when there is a new line the Raster IRQ has to be checked, 
		// and the situation is flagged into the register if true...
		// Whether finally a IRQ is or not actually launched is something that is determined later per cycle
		// just to take into account other issuing possibilities like two sprites collision analized later.
		if (_raster.moveCycles (1))
		{
			_cycleInRasterLine = 1;

			_lastBadLineScrollY = -1;
			_newBadLineCondition = false;
			_badLineStopCyclesAdded = false;

			// The graphics counters are set back to initial values at line 0!
			if (_raster.currentLine () == 0)
			{ 
				_tedGraphicInfo._VCBASE = _tedGraphicInfo._VC = 0;
				_tedGraphicInfo._RC = 0;
			}
			// In any other line number, VC start back to count from the value in VCBASE.
			// VCBASE is actualized only then RC reaches 8. @see rasterCycle 58 treatment.
			else 
				_tedGraphicInfo._VC = _tedGraphicInfo._VCBASE;

			// If the current line is where a IR has been set...
			if (_raster.currentLine () == _TEDRegisters -> IRQRasterLineAt ())
				_TEDRegisters -> activateRasterIRQ (); // ...the interrupt is activated (but not necessary launched!)
		}

		// Per cycle, the IRQ condition is checked! 
		// (many reasons during the cycle can unchain the IRQ interrupt)
		int cI = -1;
		if ((cI = (int)_TEDRegisters -> reasonIRQCode ()) != 0)
			cpu -> requestInterrupt (
				F6500::IRQInterrupt::_ID, 
				cpu -> clockCycles  () - i, 
				this,
				cI);
	}

	// When the raster enters the non visible part of the screen,
	// a notification is sent (to the Screen class usually) 
	// just to draw the screen...
	if (_raster.isInLastVBlank ())
	{
		if (!_lastVBlankEntered)
		{
			_lastVBlankEntered = true;

			notify (MCHEmul::Event (_GRAPHICSREADY)); 
		}
	}
	else
		_lastVBlankEntered = false;

	_TEDRegisters -> setCurrentRasterLine (_raster.currentLine ()); 

	_lastCPUCycles = cpu -> clockCycles ();

	return (true);
}

// ---
MCHEmul::InfoStructure COMMODORE::TED::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::GraphicalChip::getInfoStructure ());

	result.remove ("Memory"); // This info is not neccesary...
	result.add ("TEDRegisters",	std::move (_TEDRegisters -> getInfoStructure ()));
	result.add ("Raster",			std::move (_raster.getInfoStructure ()));

	return (result);
}

// ---
void COMMODORE::TED::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	// To set the bank...
	switch (evnt.id ())
	{
		case MCHEmul::InputOSSystem::_MOUSEMOVED:
			{
				unsigned short x = (unsigned short) 
					std::dynamic_pointer_cast <MCHEmul::InputOSSystem::MouseMovementEvent> (evnt.data ()) -> _x;
				unsigned short y = (unsigned short) 
					std::dynamic_pointer_cast <MCHEmul::InputOSSystem::MouseMovementEvent> (evnt.data ()) -> _y;
				setLightPenPosition ((x >= _raster.hData ().firstDisplayPosition () && 
									  y <= _raster.hData ().lastDisplayPosition ()) 
										? (x - _raster.hData ().firstDisplayPosition ()) : 0, 
									 (y >= _raster.vData ().firstDisplayPosition () && 
									  y <= _raster.vData ().lastDisplayPosition ()) 
										? (y - _raster.vData ().firstDisplayPosition ()) : 0);
			}

			break;

		// The lightpen actives when the right button of the mouse is pressed...
		case MCHEmul::InputOSSystem::_MOUSEBUTTONPRESSED:
			{
				if (std::dynamic_pointer_cast <MCHEmul::InputOSSystem::MouseButtonEvent> 
					(evnt.data ()) -> _buttonId == 0 /** Left. */)
					setLightPenActive (true);
			}
			
			break;

		// ...and stop being active when released...
		case MCHEmul::InputOSSystem::_MOUSEBUTTONRELEASED:
			{
				if (std::dynamic_pointer_cast <MCHEmul::InputOSSystem::MouseButtonEvent> 
					(evnt.data ()) -> _buttonId == 0 /** Left. */)
					setLightPenActive (false);
			}

			break;

		default:
			break;

	}
}

// ---
MCHEmul::ScreenMemory* COMMODORE::TED::createScreenMemory ()
{
	unsigned int* cP = new unsigned int [16];
	// The colors are partially transparents to allow the blending...
	cP [0]  = SDL_MapRGBA (_format, 0x00, 0x00, 0x00, 0xe0); // Black
	cP [1]  = SDL_MapRGBA (_format, 0xff, 0xff, 0xff, 0xe0); // White
	cP [2]  = SDL_MapRGBA (_format, 0x92, 0x4a, 0x40, 0xe0); // Red
	cP [3]  = SDL_MapRGBA (_format, 0x84, 0xc5, 0xcc, 0xe0); // Cyan
	cP [4]  = SDL_MapRGBA (_format, 0x93, 0x51, 0xb6, 0xe0); // Violet
	cP [5]  = SDL_MapRGBA (_format, 0x72, 0xb1, 0x4b, 0xe0); // Green
	cP [6]  = SDL_MapRGBA (_format, 0x48, 0x3a, 0xaa, 0xe0); // Blue
	cP [7]  = SDL_MapRGBA (_format, 0xd5, 0xdf, 0x7c, 0xe0); // Yellow
	cP [8]  = SDL_MapRGBA (_format, 0x99, 0x69, 0x2d, 0xe0); // Brown
	cP [9]  = SDL_MapRGBA (_format, 0x67, 0x52, 0x00, 0xe0); // Light Red
	cP [10] = SDL_MapRGBA (_format, 0xc1, 0x81, 0x78, 0xe0); // Orange
	cP [11] = SDL_MapRGBA (_format, 0x60, 0x60, 0x60, 0xe0); // Dark Grey
	cP [12] = SDL_MapRGBA (_format, 0x8a, 0x8a, 0x8a, 0xe0); // Medium Grey
	cP [13] = SDL_MapRGBA (_format, 0xb3, 0xec, 0x91, 0xe0); // Light Green
	cP [14] = SDL_MapRGBA (_format, 0x86, 0x7a, 0xde, 0xe0); // Light Blue
	cP [15] = SDL_MapRGBA (_format, 0xb3, 0xb3, 0xb3, 0xe0); // Light Grey

	return (new MCHEmul::ScreenMemory (numberColumns (), numberRows (), cP));
}

// ---
unsigned int COMMODORE::TED::treatRasterCycle ()
{
	unsigned int result = 0;

	// Read graphics zone?
	bool rG = false;
	switch (_cycleInRasterLine)
	{
		// Read the sprite 3 to 7 data?
		case 1:
		case 3:
		case 5:
		case 7:
		case 9:
			{
			}

			break;

		// In raster cycle 14 the graphics information moves...
		case 14:
			{
				_tedGraphicInfo._VC = _tedGraphicInfo._VCBASE;
				_tedGraphicInfo._VLMI = 0;

				if (_newBadLineCondition)
					_tedGraphicInfo._RC = 0;
			}

			break;

		// Between the raster cycle 15 and 55 the graphics are read...
		// ...But in the cycle 15 also...
		// After reading info of the sprites, cycles 58 - 62 (previos raster line) + 1 - 9 (this line),
		// the information about the active sprites is actualized...
		case 15:
			{
			}

			break;

		// Just read the graphics...
		case 16:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
		case 23:
		case 24:
		case 25:
		case 26:
		case 27:
		case 28:
		case 29:
		case 30:
		case 31:
		case 32:
		case 33:
		case 34:
		case 35:
		case 36:
		case 37:
		case 38:
		case 39:
		case 40:
		case 41:
		case 42:
		case 43:
		case 44:
		case 45:
		case 46:
		case 47:
		case 48:
		case 49:
		case 50:
		case 51:
		// 52 is a little bit special...
		case 53:
		case 54:
		case 55:
			{
				rG = true;
			}

			break;

		// The raster cycle 55/56 (we have choosen 55) is very important for sprite behaviour.
		// At this point the active sprites are defined.
		// Notice that this identification is done before two events:
		// The TED to decide whether it has or not to stop 3 cycles because sprite info is going to be read (first sprite at cycle 55),
		// and the sprite info is actually read (first sprite at cycle 58)
		// That the reason to choose 52, when in the real TED this happen at cycles 55/56.
		case 52:
			{
			}

			break;

		// In cycle 58 again the graphical info is updated...
		case 58:
			{
				if (_tedGraphicInfo._RC == 7)
				{
					_tedGraphicInfo._VCBASE = _tedGraphicInfo._VC;
					if (!_newBadLineCondition) // When RC gets 7 and there is no bad condition, then the idle state is set back...
						_tedGraphicInfo._idleState = true;
				}

				if (++_tedGraphicInfo._RC == 8)
					_tedGraphicInfo._RC = 0; // Only from 0 to 7...
			}

			break;

		default:
			break;
	}

	// Have graphics to be updated?
	if (rG)
	{
		if (_newBadLineCondition)
		{
			if (!_badLineStopCyclesAdded)
			{
				_badLineStopCyclesAdded = true;

				// 40 cycles more (maximum) just for reading the chars...
				result = 56 - _cycleInRasterLine;
			}

			if (deepDebugActive ())
				*_deepDebugFile
					<< "\t\t\t\tReading Video Matrix & Color RAM" << "\n";

			readVideoMatrixAndColorRAM ();
		}

		if (deepDebugActive ())
			*_deepDebugFile
				<< "\t\t\t\tReading Graphics" << "\n";

		readGraphicalInfo ();

		if (!_tedGraphicInfo._idleState)
			_tedGraphicInfo._VC++;	
		_tedGraphicInfo._VLMI++;
	}

	return (result);
}

// ---
void COMMODORE::TED::drawVisibleZone (MCHEmul::CPU* cpu)
{
	// These two variables are very key.
	// They hold the position of the raster within the VISIBLE ZONE.
	// It is the left up corner of the "computer screen" will be cv = 0 & rv = 0...
	unsigned short cv, rv; 
	_raster.currentVisiblePosition (cv, rv);
	// The same value than cv, but adjusted to a multiple of 8.
	unsigned short cav = (cv >> 3) << 3;

	// If the video is not active, 
	// then everything will have the border color...
	if (!_videoActive)
	{
		screenMemory () -> setHorizontalLine ((size_t) cav, (size_t) rv,
			(cav + 8) > _raster.visibleColumns () ? (_raster.visibleColumns () - cav) : 8, 
				_TEDRegisters -> foregroundColor ());

		return;
	}

	// In other case...
	// Everyting is the color of the background initially...
	// ..and it will be covered with the foreground (border) later if needed..
	// This is how the TED works...
	screenMemory () -> setHorizontalLine ((size_t) cav, (size_t) rv,
		(cav + 8) > _raster.visibleColumns () ? (_raster.visibleColumns () - cav) : 8, 
			_TEDRegisters -> backgroundColor ());

	// Now the information is drawn,...
	// ...and also the collisions detected at the same time
	drawGraphicsAndDetectCollisions (
		{
			/** _ICD */ _raster.hData ().firstDisplayPosition (),		// DISLAY: The original...
			/** _ICS */ _raster.hData ().firstScreenPosition (),		// SCREEN: And the real one (after reduction size)
			/** _LCD */ _raster.hData ().lastDisplayPosition (),		// DISPLAY: The original...
			/** _LCS */ _raster.hData ().lastScreenPosition (),			// SCREEN: And the real one (after reduction size)
			/** _SC	 */ _TEDRegisters -> horizontalScrollPosition (),	// From 0 - 7 
			/** _RC	 */ cv,												// Where the horizontal raster is (not adjusted to 8) inside the window
			/** _RCA */ cav,											// Where the horizontal raster is (adjusted to 8) inside the window
			/** _IRD */ _raster.vData ().firstDisplayPosition (),		// DISPLAY: The original... 
			/** _IRS */ _raster.vData ().firstScreenPosition (),		// SCREEN:  And the real one (after reduction size)
			/** _LRD */ _raster.vData ().lastDisplayPosition (),		// DISPLAY: The original...
			/** _LRS */ _raster.vData ().lastScreenPosition (),			// SCREEN: And the real one (after reduction size)
			/** _SR	 */ _TEDRegisters -> verticalScrollPosition (),	// From 0 - 7 (taken into account in bad lines)
			/** _RR	 */ rv												// Where the vertical raster is inside the window (it is not the chip raster line)
		});

	// If the raster is not in the very visible zone...
	// it is time to cover with the border...
	if (!_raster.isInScreenZone () ||
		(_raster.isInScreenZone () && (cav + 8) > _raster.hData ().lastScreenPosition ()))
	{
		// This is the starting pixel to start to draw...
		unsigned short stp = cav;
		// ...and the number to draw by default...
		unsigned short lbk = (cav + 8) > _raster.visibleColumns () 
			? (_raster.visibleColumns () - cav) : 8;

		// But when the raster line is in the "screen" part of the window,
		// any of these two previous variables could change...
		if (rv >= _raster.vData ().firstScreenPosition () &&
			rv <= _raster.vData ().lastScreenPosition ())
		{
			// If the initial horizontal position is before the "screen" part...
			// ...but not the final position, the number of pixels to draw have to be reduced 
			if (cav < _raster.hData ().firstScreenPosition () &&
				(cav + 8) >= _raster.hData ().firstScreenPosition ())
				lbk = _raster.hData ().firstScreenPosition () - cav;

			// If the initial horizontal position is still in the "screen" part...
			// ...but not the final position, 
			// ...both the starting pixel and the number of pixels to draw have to be reduced
			if (cav < _raster.hData ().lastScreenPosition () && 
				(cav + 8) > _raster.hData ().lastScreenPosition ())
			{
				stp = _raster.hData ().lastScreenPosition () + 1;
				lbk = (cav + 8) - stp;
			}
		}

		screenMemory () -> setHorizontalLine ((size_t) stp, (size_t) rv, lbk, 
			_TEDRegisters -> foregroundColor ());
	}

	// The IRQ related with the lightpen is calculated...
	unsigned short dx, dy;
	_raster.currentDisplayPosition (dx, dy);
	unsigned short lpx, lpy;
	_TEDRegisters -> currentLightPenPosition (lpx, lpy);
	if (dy == lpy && (lpx >= dx && lpx < (dx + 8))) // The beam moves every 8 pixels...
		_TEDRegisters -> activateLightPenOnScreenIRQ ();
}

// ---
void COMMODORE::TED::drawGraphicsAndDetectCollisions (const COMMODORE::TED::DrawContext& dC)
{
	// This varible keeps info about the text/graphics:
	// Whether the 8 pixels to draw are foreground or background...
	// ...and the color of the ones that are not finally background!
	// ...and also info to control later the collision with sprites!
	COMMODORE::TED::DrawResult colGraphics = drawGraphics (dC);

	// The the graphical info is moved to the screen...
	drawResultToScreen (colGraphics, dC);
}

// ---
COMMODORE::TED::DrawResult COMMODORE::TED::drawGraphics (const COMMODORE::TED::DrawContext& dC)
{
	// To draw is only valid in the graphical state...
	if (_tedGraphicInfo._idleState)
		return (COMMODORE::TED::DrawResult ());

	// The "display" column being involved...
	// In cb, the SCROLLX is involved, so it could be negative! starting from -7, 
	// When e.g. the raster is at the very first "screen "dislay" column, 
	// there are no reductions is the screen (display == screen) and SCROLLX = 0x00
	int cb = (int) dC._RCA - (int) dC._ICD - (int) dC._SC;
	// To draw just only when there is a possibility of at least draw a pixel!
	if (cb <= -8)
		return (COMMODORE::TED::DrawResult ());

	// At this point rc positive for sure, and cb could be negative...
	// Never invoke the methods within the swith case statements direcly
	// a crash might be generated...

	COMMODORE::TED::DrawResult result;
	switch (_TEDRegisters -> graphicModeActive ())
	{
		case COMMODORE::TEDRegisters::GraphicMode::_CHARMODE:
			{
				result = std::move (drawMonoColorChar (cb));
			}

			break;

		case COMMODORE::TEDRegisters::GraphicMode::_MULTICOLORCHARMODE:
			{
				result = std::move (drawMultiColorChar (cb));
			}

			break;

		case COMMODORE::TEDRegisters::GraphicMode::_EXTENDEDBACKGROUNDMODE:
			{
				result = std::move (drawMultiColorExtendedChar (cb));
			}

			break;

		case COMMODORE::TEDRegisters::GraphicMode::_BITMAPMODE:
			{
				result = std::move (drawMonoColorBitMap (cb));
			}

			break;

		case COMMODORE::TEDRegisters::GraphicMode::_MULTICOLORBITMAPMODE:
			{
				result = std::move (drawMultiColorBitMap (cb));
			}

			break;

		case COMMODORE::TEDRegisters::GraphicMode::_INVALIDTEXMODE:
			{
				// Like multicolor char mode, but invalid...
				result = std::move (drawMultiColorChar (cb, result._invalid = true));
			}

			break;

		case COMMODORE::TEDRegisters::GraphicMode::_INVALIDBITMAPMODE1:
			{
				// Like moncocolor bitmap mode, but invalid...
				result = std::move (drawMonoColorBitMap (cb, result._invalid = true));
			}
			break;

		case COMMODORE::TEDRegisters::GraphicMode::_INVALIDBITMAPMODE2:
			{
				// Like multicolor bitmap mode, but invalid...
				result = std::move (drawMultiColorBitMap (cb, result._invalid = true));
			}

			break;

		default:
			assert (0); // Not possible...the code shouldn't pass over this point!
			break;
	}

	return (result);
}

// ---
COMMODORE::TED::DrawResult COMMODORE::TED::drawMonoColorChar (int cb)
{
	COMMODORE::TED::DrawResult result;

	for (int i = 0; i < 8 /** To paint always 8 pixels */; i++)
	{
		int pp = cb + i;
		if (pp < 0)
			continue;
		if (pp >= 320)
			break;

		size_t iBy = ((size_t) pp) >> 3; // To determine the byte...
		size_t iBt = 7 - (((size_t) pp) % 8); // From MSB to LSB...

		if (_tedGraphicInfo._graphicData [iBy].bit (iBt))
		{
			result._collisionData.setBit (7 - i, true);

			result._foregroundColorData [i] = 
				(unsigned int) (_tedGraphicInfo._colorData [iBy].value () & 0x0f /** Useful nibble. */);
		}
		// When 0, it is background...
		// Not necessary to specify neither collision information
		// nor the color of the pixels as it will be always the basic background color,
		// that has already been set to the value of $d021 in the main loop...
	}

	return (result);
}

// ---
COMMODORE::TED::DrawResult COMMODORE::TED::drawMultiColorChar (int cb, bool inv)
{
	COMMODORE::TED::DrawResult result;

	for (unsigned short i = 0 ; i < 8 /** To paint always 8 pixels but in blocks of 2. */; i += 2)
	{
		int pp = cb + i;
		if (pp < -1)
			continue;
		if (pp >= 320)
			break;

		// After this pp can be -1...

		size_t iBy = 0;
		unsigned char cs = 0;
		if (pp >= 0)
		{
			iBy = ((size_t) pp) >> 3; 
			size_t iBt = 3 - ((((size_t) pp) % 8) >> 1);
			cs = (_tedGraphicInfo._graphicData [iBy].value () >> (iBt << 1)) & 0x03; // 0, 1, 2 or 3
		}
		// This is the case when pp == -1...
		else
			cs = (_tedGraphicInfo._graphicData [0].value () >> 6) & 0x03; // 0, 1, 2 or 3

		// If 0, the pixel should be drawn (and considered) as background 
		// and it is already the default status tha comes from the parent method...
		if (cs == 0x00)
			continue;

		// The way the pixels are going to be drawn will depend on the information in the color memory
		// If the most significant bit of the low significant nibble of the color memory is set to 1
		// the data will be managed in a monocolor way...
		if ((_tedGraphicInfo._colorData [iBy] & 0x08) == 0x00) 
		{
			unsigned int fc = 
				inv 
					? 0x00 // When invalid all pixels are black...
					: _tedGraphicInfo._colorData [iBy].value () & 0x07;

			// ...and remember we are dealing with pairs of pixels...

			switch (cs)
			{
				case 0x01:
					{
						result._backgroundColorData [i + 1] = fc;
					}

					break;

				case 0x02:
					{
						result._collisionData.setBit (7 - i, true);

						result._foregroundColorData [i] = fc;
					}

					break;

				case 0x03:
					{
						result._collisionData.setBit (7 - i, true);
						result._collisionData.setBit (6 - i, true);

						result._foregroundColorData [i] = fc;
						result._foregroundColorData [i + 1] = fc;
					}

					break;

				default:
					break;
			}
		}
		// But if it is 1, 
		// then it will be draw as in the multicolor version...
		else
		{
			unsigned int fc = 
				inv 
					? 0x00 
					: (unsigned int) ((cs == 0x03) 
						? (_tedGraphicInfo._colorData [iBy].value () & 0x07)
						: _TEDRegisters -> backgroundColor (cs));

			// The combination "01" is also considered as part of the background...
			// ...and are not taken into account to detect collision...
			if (cs == 0x01)
			{
				result._backgroundColorData [i] = fc;
				result._backgroundColorData [i + 1] = fc;
			}
			// ..while the other two are part of the foreground...
			// ..and also included in the collision info!
			else
			{
				result._collisionData.setBit (7 - i, true);
				result._collisionData.setBit (6 - i, true);

				result._foregroundColorData [i] = fc;
				result._foregroundColorData [i + 1] = fc;
			}
		}
	}

	return (result);
}

// ---
COMMODORE::TED::DrawResult COMMODORE::TED::drawMultiColorExtendedChar (int cb)
{
	// The mode is quite similar to the standard text mode, 
	// with the difference the 0 pixels (background) can have different background colors...

	COMMODORE::TED::DrawResult result;

	for (int i = 0; i < 8 /** To paint always 8 pixels */; i++)
	{
		int pp = cb + i;
		if (pp < 0)
			continue;
		if (pp >= 320)
			break;

		size_t iBy = ((size_t) pp) >> 3 /** To determine the byte. */;
		size_t iBt = 7 - (((size_t) pp) % 8); /** From MSB to LSB. */
		// The color of the pixel 0 is determined by the 2 MSBites of the char code...
		bool bS = _tedGraphicInfo._graphicData [iBy].bit (iBt); // To know whether the bit is 1 or 0...
		unsigned int cs = ((_tedGraphicInfo._screenCodeData [iBy].value () & 0xc0) >> 6) & 0x03; // 0, 1, 2, or 3
		unsigned int fc = 
			bS 
				? (_tedGraphicInfo._colorData [iBy].value () & 0x0f) 
				: _TEDRegisters -> backgroundColor (cs);

		if (bS)
		{
			result._collisionData.setBit (7 - i, true);

			result._foregroundColorData [i] = fc;
		}
		else
		// ...all of them are treated as background...
		// ...but with the possibility to have different colors!
		// The value 0x00 has been already treated in the main loop...(drawn as $d021)
		if (cs != 0x00)
			result._backgroundColorData [i] = fc;
	}

	return (result);
}

// ---
COMMODORE::TED::DrawResult COMMODORE::TED::drawMonoColorBitMap (int cb, bool inv)
{
	COMMODORE::TED::DrawResult result;

	for (int i = 0; i < 8 /** To paint always 8 pixels. */; i++)
	{
		int pp = cb + i;
		if (pp < 0)
			continue;
		if (pp >= 320)
			break;

		size_t iBy = ((size_t) pp) >> 3; // To determine the byte...
		size_t iBt = 7 - (((size_t) pp) % 8); // From MSB to LSB...
		bool bS = _tedGraphicInfo._graphicData [iBy].bit (iBt);
		unsigned int fc = 
			inv 
				? 0x00 // When invalid, all pixels are black...
				: bS 
					? (_tedGraphicInfo._screenCodeData [iBy].value () & 0xf0) >> 4	// If the bit is 1, the color is determined by the MSNibble
					: (_tedGraphicInfo._screenCodeData [iBy].value () & 0x0f);		// ...and for LSNibble if it is 0...

		if (bS)
		{
			result._collisionData.setBit (7 - i, true);

			result._foregroundColorData [i] = fc;
		}
		// The pixels 0 are treated as background...
		// but they will have different color that the one defined in $d021 (and treated in the main loop)..
		// but the one defined in the graphics data (2 nibbles)...
		else
			result._backgroundColorData [i] = fc;
	}

	return (result);
}

// ---
COMMODORE::TED::DrawResult COMMODORE::TED::drawMultiColorBitMap (int cb, bool inv)
{
	COMMODORE::TED::DrawResult result;

	for (unsigned short i = 0 ; i < 8 /** To paint always 8 pixels but in blocks of 2. */; i += 2)
	{
		int pp = cb + i;
		if (pp < -1)
			continue;
		if (pp >= 320)
			break;

		// After this point pp can be -1

		size_t iBy = 0;
		unsigned char cs = 0;
		if (pp >= 0)
		{
			iBy = ((size_t) pp) >> 3; 
			size_t iBt = 3 - ((((size_t) pp) % 8) >> 1);
			cs = (_tedGraphicInfo._graphicData [iBy].value () >> (iBt << 1)) & 0x03; // 0, 1, 2 or 3
		}
		// This is the case when pp == -1...
		else
			cs = (_tedGraphicInfo._graphicData [0].value () >> 6) & 0x03; // 0, 1, 2 or 3

		// If 0, the pixel should be drawn (and considered) as background 
		// and it is already the default status tha comes from the parent method...
		if (cs == 0x00)
			continue;

		unsigned fc = // The value 0x00 is not tested....
				inv
					? 0x00 // When invalid all pixels are black...
					: (cs == 0x01) // The color is the defined in the video matrix, high nibble...
						? (_tedGraphicInfo._screenCodeData [iBy].value () & 0xf0) >> 4
						: ((cs == 0x02) // The color is defined in the video matrix, low nibble...
							? (_tedGraphicInfo._screenCodeData [iBy].value () & 0x0f)
							: (_tedGraphicInfo._colorData [iBy].value () & 0x0f)); // The color is defined in color matrix...

		// The combination "01" is managed as background also...
		// ...the 0x00 has already been jumped an then treated as background!
		if (cs == 0x01)
		{
			result._backgroundColorData [i] = fc;
			result._backgroundColorData [i + 1] = fc;
		}
		// ...while the rest as managed as foreground...
		else
		{
			result._collisionData.setBit (7 -i, true);
			result._collisionData.setBit (6 -i, true);

			result._foregroundColorData [i] = fc;
			result._foregroundColorData [i + 1] = fc;
		}
	}

	return (result);
}

// ---
void COMMODORE::TED::drawResultToScreen (const COMMODORE::TED::DrawResult& cT, const COMMODORE::TED::DrawContext& dC)
{
	// The eight pixels to draw...
	for (size_t i = 0; i < 8; i++)
	{
		size_t pos = (size_t) dC._RCA + i;

		// If the graphic mode was invalid...
		if (cT._invalid)
			// the pixel will be always black by default...
			screenMemory () -> setPixel (pos, (size_t) dC._RR, 0x00 /** black. */);

		// And then the background pixels...
		if (cT._backgroundColorData [i] != ~0)
			screenMemory () -> setPixel (pos, (size_t) dC._RR, cT._backgroundColorData [i]);
		// and the foreground ones finally...
		if (cT._foregroundColorData [i] != ~0)
			screenMemory () -> setPixel (pos, (size_t) dC._RR, cT._foregroundColorData [i]);
	}
}
