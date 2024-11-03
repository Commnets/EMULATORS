#include <COMMODORE/TED/TED.hpp>
#include <COMMODORE/TED/TEDSoundWrapper.hpp>
#include <F6500/IRQInterrupt.hpp>

// ---
const MCHEmul::RasterData COMMODORE::TED_PAL::_VRASTERDATA (260, 274, 3, 202, 245, 259, 312, 4, 4);
const MCHEmul::RasterData COMMODORE::TED_PAL::_HRASTERDATA 
	(376, 384, 432, 295, 343, 375, 456 /** For everyting to run, it has to be divisible by 8. */, 7, 9);
const MCHEmul::RasterData COMMODORE::TED_NTSC::_VRASTERDATA (235, 249, 3, 202, 220, 234, 262, 4, 4);
const MCHEmul::RasterData COMMODORE::TED_NTSC::_HRASTERDATA 
	(376, 384, 432, 295, 343, 375, 456 /** For everyting to run, it has to be divisible by 8. */, 7, 9);
// This two positions are fized...
const MCHEmul::Address COMMODORE::TED::_MEMORYPOSIDLE1 = MCHEmul::Address ({ 0xff, 0x39 }, false);
const MCHEmul::Address COMMODORE::TED::_MEMORYPOSIDLE2 = MCHEmul::Address ({ 0xff, 0x3f }, false);

// ---
COMMODORE::TED::SoundFunction::SoundFunction (MCHEmul::SoundLibWrapper* sW)
	: MCHEmul::SoundChip (_ID, 
		{ { "Name", "SoundFunction" },
		  { "Code", "Inside TED" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Year", "1984" } },
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
COMMODORE::TED::TED (int intId, const MCHEmul::RasterData& vd, const MCHEmul::RasterData& hd,
		int vV, MCHEmul::SoundLibWrapper* sW, const MCHEmul::Attributes& attrs)
	: MCHEmul::GraphicalChip (_ID, attrs),
	  _interruptId (intId),
	  _T1 (1, COMMODORE::TEDTimer::RunMode::_FROMINITIALVALUE),
	  _T2 (2, COMMODORE::TEDTimer::RunMode::_CONTINUOUS),
	  _T3 (3, COMMODORE::TEDTimer::RunMode::_CONTINUOUS),
	  _soundFunction (new COMMODORE::TED::SoundFunction (sW)),
	  _TEDRegisters (nullptr), 
	  _TEDView (vV),
	  _cyclesPerRasterLine (57),
	  _raster (vd, hd, 8 /** step. */),
	  _lastCPUCycles (0),
	  _format (nullptr),
	  _cycleInRasterLine (1),
	  _videoActive (true),
	  _lastVBlankEntered (false),
	  _lastBadLineScrollY (-1), _newBadLineCondition (false), _badLineStopCyclesAdded (false),
	  _tedGraphicInfo ()
{
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

	_T1.initialize ();
	_T2.initialize ();
	_T3.initialize ();

	// The sound function is initialized independly...

	_TEDRegisters -> lookAtTimers (&_T1, &_T2, &_T3);
	_TEDRegisters -> lookAtSoundLibWrapper (_soundFunction -> soundWrapper ());
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

	// If the "video is frozen" is actived nothing is done...
	if (_TEDRegisters -> freezeActive ())
		return (true);

	// Adapt the size of the display zone to the parameters specificied in the register...
	// The zone where sprites and texts are finally visible is call the "screen zone"
	_raster.reduceDisplayZone
		(!_TEDRegisters -> textDisplay25RowsActive (), !_TEDRegisters -> textDisplay40ColumnsActive ());

	// The simulation has to be repeated as many time as cycles have spent since the last invocation...
	bool r = true;
	for (unsigned int i = (cpu -> clockCycles  () - _lastCPUCycles); i > 0 && r; i--)
	{
		_IFDEBUG debugTEDCycle (cpu, i);

		// Whether the video is active or not is only checked at the very first bad line...
		_videoActive = (_raster.currentLine () == _FIRSTBADLINE) 
			? !_TEDRegisters -> blankEntireScreen () : _videoActive; // The value can change at any cycle of this first bad line...

		// Is there a bad line situation?
		// A bad line can happen at any cycle within the raster line.
		// When happened, the graphics are read and the situation is latched because maybe additional stop cycles could be needed.
		if (isNewBadLine ())
		{
			if (deepDebugActive ())
				*_deepDebugFile << "\t\t\t\t\tBad line situation\n";

			_newBadLineCondition = true;		// latched...
			_badLineStopCyclesAdded = false;	// ...the cycles have to be added...

			_tedGraphicInfo._idleState = false; // No longer in "idle" state but in the "screen" one!
		}

		// When TED is about to read graphics info (bad line),
		// the CPU has to stop 3 cycles in advance (just for READ activities) for those activities,
		// unless it was stopped previously and that stop situation were still valid...
		// In the case of graphics that stop only happens when the situation arise in the "screen cycles" (40)
		if (!cpu -> stopped () && 
				(_newBadLineCondition && (_cycleInRasterLine >= 4 && _cycleInRasterLine < 44)))
			cpu -> setStop (true, MCHEmul::InstructionDefined::_CYCLEREAD /** only read in not allowed. */, 
				cpu -> clockCycles () - i, 3);

		// Treat the right cycle...
		// ...and as a consequence the CPU can be also stopped...
		unsigned int cS = 0;
		if ((cS = treatRasterCycle ()) > 0)
			cpu -> setStop (true, MCHEmul::InstructionDefined::_CYCLEALL /** fully stopped. */, cpu -> clockCycles () - i, (int) cS);

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

		// Simulate the timers!
		r &= _T1.simulate (cpu);
		r &= _T2.simulate (cpu);
		r &= _T3.simulate (cpu);

		// Per cycle, the IRQ condition is checked! 
		// (many reasons during the cycle can unchain the IRQ interrupt)
		int cI = -1;
		if ((cI = (int) _TEDRegisters -> reasonIRQCode ()) != 0)
			cpu -> requestInterrupt (
				F6500::IRQInterrupt::_ID, 
				cpu -> clockCycles  () - i, 
				this,
				cI);
	}

	// When the raster enters the non visible part of the screen,
	// a notification is sent (to the Screen class usually) 
	// just to draw the screen...
	if (_raster.isInLastVBlankZone ())
	{
		if (!_lastVBlankEntered)
		{
			_lastVBlankEntered = true;

			notify (MCHEmul::Event (_GRAPHICSREADY)); 
		}
	}
	else
		_lastVBlankEntered = false;

	_TEDRegisters -> setGraphicalInfo (COMMODORE::TEDRegisters::GraphicalInfo 
		(_raster.currentLine (), _raster.currentColumn (), 0, 0, 0));

	_lastCPUCycles = cpu -> clockCycles ();

	return (true);

	// Note that the _soundFunction is not simulated here...
	// It is added like another different chip on regards simulation
	// The framework need a sound chip always!
}

// ---
MCHEmul::InfoStructure COMMODORE::TED::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::GraphicalChip::getInfoStructure ());

	result.remove ("Memory"); // This info is not neccesary...
	result.add ("TEDRegisters",		std::move (_TEDRegisters -> getInfoStructure ()));
	result.add ("Raster",			std::move (_raster.getInfoStructure ()));
	result.add ("Sound",			std::move (_soundFunction -> getInfoStructure ()));
	result.add ("TEDTimer1",		std::move (_T1.getInfoStructure ()));
	result.add ("TEDTimer2",		std::move (_T2.getInfoStructure ()));
	result.add ("TEDTimer3",		std::move (_T3.getInfoStructure ()));

	return (result);
}

// ---
MCHEmul::ScreenMemory* COMMODORE::TED::createScreenMemory ()
{
	static struct RGBStr { unsigned int R,G,B; } _COLORS [128] =
		{ 
			// Black
			{ 0x00,0x00,0x00 },{ 0x00,0x00,0x00 },{ 0x00,0x00,0x00 },{ 0x00,0x00,0x00 },
			{ 0x00,0x00,0x00 },{ 0x00,0x00,0x00 },{ 0x00,0x00,0x00 },{ 0x00,0x00,0x00 },
			// White
			{ 0x20,0x20,0x20 },{ 0x40,0x40,0x40 },{ 0x60,0x60,0x60 },{ 0x80,0x80,0x80 },
			{ 0x9f,0x9f,0x9f },{ 0xbf,0xbf,0xbf },{ 0xdf,0xdf,0xdf },{ 0xff,0xff,0xff },
			// Red
			{ 0x58,0x09,0x02 },{ 0x78,0x29,0x22 },{ 0x98,0x49,0x42 },{ 0xb8,0x69,0x62 },
			{ 0xd8,0x88,0x82 },{ 0xf7,0xa8,0xa2 },{ 0xff,0xc8,0xc2 },{ 0xff,0xe8,0xe2 },
			// Cyan
			{ 0x00,0x37,0x3d },{ 0x08,0x57,0x5d },{ 0x27,0x77,0x7d },{ 0x47,0x96,0x9d },
			{ 0x67,0xb6,0xbd },{ 0x87,0xd6,0xdd },{ 0xa7,0xf6,0xfd },{ 0xc7,0xff,0xff },
			// Purple
			{ 0x4b,0x00,0x56 },{ 0x6b,0x1f,0x76 },{ 0x8b,0x3f,0x96 },{ 0xaa,0x5f,0xb6 },
			{ 0xca,0x7f,0xb6 },{ 0xea,0x9f,0xf6 },{ 0xff,0xbf,0xff },{ 0xff,0xdf,0xff },
			// Green
			{ 0x00,0x40,0x00 },{ 0x15,0x60,0x09 },{ 0x35,0x80,0x29 },{ 0x55,0xa0,0x49 },
			{ 0x74,0xc0,0x69 },{ 0x94,0xe0,0x89 },{ 0xb4,0xff,0xa9 },{ 0xd4,0xff,0xc9 },
			// Blue
			{ 0x20,0x11,0x6d },{ 0x40,0x31,0x8d },{ 0x60,0x51,0xac },{ 0x80,0x71,0xcc },
			{ 0x9f,0x90,0xec },{ 0xbf,0xb0,0xff },{ 0xdf,0xd0,0xff },{ 0xff,0xf0,0xff },
			// Yellow
			{ 0x20,0x2f,0x00 },{ 0x40,0x4f,0x00 },{ 0x60,0x6f,0x13 },{ 0x80,0x8e,0x33 },
			{ 0x9f,0xae,0x53 },{ 0xbf,0xce,0x72 },{ 0xdf,0xee,0x92 },{ 0xff,0xff,0xb2 },
			// Orange
			{ 0x4b,0x15,0x00 },{ 0x6b,0x34,0x09 },{ 0x8b,0x54,0x29 },{ 0xaa,0x74,0x49 },
			{ 0xca,0x94,0x69 },{ 0xea,0xb4,0x89 },{ 0xff,0xd4,0xa9 },{ 0xff,0xf4,0xc9 },
			// Brown
			{ 0x37,0x22,0x00 },{ 0x57,0x42,0x00 },{ 0x77,0x62,0x19 },{ 0x97,0x81,0x39 },
			{ 0xb7,0xa1,0x58 },{ 0xd7,0xc1,0x78 },{ 0xf6,0xe1,0x98 },{ 0xff,0xff,0xb8 },
			// Yellow - Green
			{ 0x09,0x3a,0x00 },{ 0x28,0x59,0x00 },{ 0x48,0x79,0x19 },{ 0x68,0x99,0x39 },
			{ 0x88,0xb9,0x58 },{ 0xa8,0xd9,0x78 },{ 0xc8,0xf9,0x98 },{ 0xe8,0xff,0xb8 },
			// Pink
			{ 0x5d,0x01,0x20 },{ 0x7d,0x21,0x40 },{ 0x9c,0x41,0x60 },{ 0xbc,0x61,0x80 },
			{ 0xdc,0x80,0x9f },{ 0xfc,0xa0,0xbf },{ 0xff,0xc0,0xdf },{ 0xff,0xe0,0xff },
			// Blue - Green
			{ 0x00,0x3f,0x20 },{ 0x03,0x5f,0x40 },{ 0x23,0x7f,0x60 },{ 0x43,0x9e,0x80 },
			{ 0x63,0xbe,0x9f },{ 0x82,0xde,0xbf },{ 0xa2,0xfe,0xdf },{ 0xc2,0xff,0xff },
			// Light Blue
			{ 0x00,0x2b,0x56 },{ 0x15,0x4b,0x76 },{ 0x35,0x6b,0x96 },{ 0x55,0x8b,0xb6 },
			{ 0x74,0xab,0xd6 },{ 0x94,0xcb,0xf6 },{ 0xb4,0xea,0xff },{ 0xd4,0xff,0xff },
			// Dark Blue
			{ 0x37,0x06,0x67 },{ 0x57,0x26,0x87 },{ 0x77,0x46,0xa7 },{ 0x97,0x66,0xc6 },
			{ 0xb7,0x86,0xe6 },{ 0xd7,0xa6,0xff },{ 0xf6,0xc5,0xff },{ 0xff,0xe5,0xff },
			// Light Green
			{ 0x00,0x42,0x02 },{ 0x08,0x62,0x22 },{ 0x27,0x82,0x42 },{ 0x47,0xa2,0x62 },
			{ 0x67,0xc2,0x82 },{ 0x87,0xe2,0xa2 },{ 0xa7,0xff,0xc2 },{ 0xc7,0xff,0xe2 }
		};

	// From a structure based on positions, to a structure based on luminance...
	unsigned int* cP = new unsigned int [128];
	for (size_t i = 0; i < 16; i++)
		for (size_t j = 0; j < 8; j++)
			cP [i + (j << 4)] = SDL_MapRGBA (_format, 
				_COLORS [(i << 3) + j].R, _COLORS [(i << 3) + j].G, _COLORS [(i << 3) + j].B, 0xe0);
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
		// In raster cycle 5 (=396) the graphics information moves...
		case 5:
			{
				_tedGraphicInfo._VC = _tedGraphicInfo._VCBASE;
				_tedGraphicInfo._VLMI = 0;

				if (_newBadLineCondition)
					_tedGraphicInfo._RC = 0;
			}

			break;

		// Just read the graphics...
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
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
			{
				rG = true;
			}

			break;

		// In cycle 57 again the graphical info is updated...
		case 57:
			{
				if (_tedGraphicInfo._RC == 7)
				{
					_tedGraphicInfo._VCBASE = _tedGraphicInfo._VC;
					if (!_newBadLineCondition) // When RC gets 7 and there is no bad condition...
											   // ...then the idle state is set back...
						_tedGraphicInfo._idleState = true;
				}

				if (++_tedGraphicInfo._RC == 8)
					_tedGraphicInfo._RC = 0; // Only from 0 to 7...
			}

			break;

		// By default it does nothing...
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
				result = 48 - _cycleInRasterLine;
			}

			if (deepDebugActive ())
				*_deepDebugFile
					<< "\t\t\t\t\tReading Video Matrix & Color RAM" << "\n";

			readVideoMatrixAndColorRAM ();
		}

		if (deepDebugActive ())
			*_deepDebugFile
				<< "\t\t\t\t\tReading Graphics" << "\n";

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
				_TEDRegisters -> borderColor ().asChar ());

		return;
	}

	// In other case...
	// Everyting is the color of the background initially...
	// ..and it will be covered with the foreground (border) later if needed..
	// This is how the TED works...
	screenMemory () -> setHorizontalLine ((size_t) cav, (size_t) rv,
		(cav + 8) > _raster.visibleColumns () ? (_raster.visibleColumns () - cav) : 8, 
			_TEDRegisters -> backgroundColor ().asChar ());

	// Now the information is drawn,...
	// ...and also the collisions detected at the same time
	drawGraphicsAndMoveToScreen (
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
			_TEDRegisters -> borderColor ().asChar ());
	}
}

// ---
void COMMODORE::TED::drawGraphicsAndMoveToScreen (const COMMODORE::TED::DrawContext& dC)
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
			result._foregroundColorData [i] = 
				(unsigned int) (_tedGraphicInfo._colorData [iBy].value ());

		// When 0, it is background...
		// Not necessary to the color of the pixels as it will be always the basic background color,
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
		// But only 8 colors are allowed (+ luminance)
		if ((_tedGraphicInfo._colorData [iBy] & 0x08) == 0x00) 
		{
			unsigned int fc = 
				inv 
					? 0x00 // When invalid all pixels are black...
					: _tedGraphicInfo._colorData [iBy].value () & ~0x08 
						/** Only the first 8 colors are allowed (the bit 3 is removed). */;

			// ...and remember we are dealing with pairs of pixels...

			switch (cs)
			{
				case 0x01:
					{
						result._foregroundColorData [i + 1] = fc;
					}

					break;

				case 0x02:
					{
						result._foregroundColorData [i] = fc;
					}

					break;

				case 0x03:
					{
						result._foregroundColorData [i] = fc;
						result._foregroundColorData [i + 1] = fc;
					}

					break;

				// Not possible to be here, just in case...
				default:
					break;
			}
		}
		// But if it is 1, 
		// then it will be draw as in the multicolor version...
		// But only 8 colors are allowed (+ luminance)
		else
		{
			unsigned int fc = 
				inv 
					? 0x00 
					: (unsigned int) ((cs == 0x03) 
						? (_tedGraphicInfo._colorData [iBy].value () & ~0x08
							/** Only the first 7 colors are allowed (the bit 3 is removed). */)
						: _TEDRegisters -> backgroundColor (cs).asChar ());

			result._foregroundColorData [i] = fc;
			result._foregroundColorData [i + 1] = fc;
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
				? (_tedGraphicInfo._colorData [iBy].value ()) 
				: _TEDRegisters -> backgroundColor (cs).asChar ();

		if (bS)
		{
			result._foregroundColorData [i] = fc;
		}
		else
		// ...all of them are treated as background...
		// ...but with the possibility to have different colors!
		// The value 0x00 has been already treated in the main loop...
		if (cs != 0x00)
			result._foregroundColorData [i] = fc;
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
					? (((_tedGraphicInfo._screenCodeData [iBy].value () & 0x70 /** bits 4, 5 & 6. */) >> 4 /** moved to color info. */)
						| ((_tedGraphicInfo._colorData [iBy].value () & 0x07 /** bits 0, 1 & 2. */) << 4 /** moved to luminance info. */))
					: ((_tedGraphicInfo._screenCodeData [iBy].value () & 0x07 /** bits 0, 1 & 2. */)
						| (_tedGraphicInfo._colorData [iBy].value () & 0x70 /** bits 4, 5 & 6. */));

		result._foregroundColorData [i] = fc;
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
					: (cs == 0x01) // The color is the defined in the video matrix (color) & color data (luminance)...
						? (((_tedGraphicInfo._screenCodeData [iBy].value () & 0xf0 /** bits 4, 5, 6 & 7. */) >> 4) /** to color position. */
							| (_tedGraphicInfo._colorData [iBy].value () & 0x70 /** bits 4, 5 & 6. */))
						: ((cs == 0x02) // The color is defined in the video matrix (color) & color data (luminance)...
							? ((_tedGraphicInfo._screenCodeData [iBy].value () & 0x0f /** bits 0, 1, 2 & 3 */)
								| ((_tedGraphicInfo._colorData [iBy].value () & 0x07 /** bits 0, 1 & 2. */) << 4) /** to luminance position. */)
							: (_TEDRegisters -> backgroundColor (0x01).asChar ()));

		result._foregroundColorData [i] = fc;
		result._foregroundColorData [i + 1] = fc;
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

		// ...and the foreground ones finally...
		if (cT._foregroundColorData [i] != ~0)
			screenMemory () -> setPixel (pos, (size_t) dC._RR, cT._foregroundColorData [i]);
	}
}

// ---
void COMMODORE::TED::debugTEDCycle (MCHEmul::CPU* cpu, unsigned int i)
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeCompleteLine (className (), i , "Info Cycle", 
		{ { "Raster",
				std::to_string (_raster.currentColumnAtBase0 ()) + "," +
				std::to_string (_raster.currentLineAtBase0 ()) + "," +
				std::to_string (_cycleInRasterLine) },
		  { "Graphics mode",
				std::to_string ((int) _TEDRegisters -> graphicModeActive ()) },
		  { "Memory",
				"Screen=$" + MCHEmul::removeAll0 (_TEDRegisters -> screenMemory ().asString
					(MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) + "," +
				"Characters=$" + MCHEmul::removeAll0 (_TEDRegisters -> charDataMemory ().asString
					(MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) + "," +
				"Bitmap=$" + MCHEmul::removeAll0 (_TEDRegisters -> bitmapMemory ().asString
				(MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) } });

	// Draws lines where there is a IRQ interruption...
	unsigned short lrt = 
		_raster.lineInVisibleZone (_TEDRegisters -> IRQRasterLineAt ());
	if (lrt <= _raster.vData ().lastVisiblePosition ())
		screenMemory () -> setHorizontalLine (0, lrt, _raster.visibleColumns (), 
			_TEDRegisters -> borderColor ().nextLuminance ().asChar ());
}

// ---
COMMODORE::TED_PAL::TED_PAL (int intId, int vV, MCHEmul::SoundLibWrapper* wS)
	: COMMODORE::TED (intId,
		 _VRASTERDATA, _HRASTERDATA, vV, wS,
		{ { "Name", "TED" },
		  { "Code", "7360/8360 for PAL" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Year", "1984" } })
{
	// Nothing else to do...
}

// ---
COMMODORE::TED_NTSC::TED_NTSC (int intId, int vV, MCHEmul::SoundLibWrapper* wS)
	: COMMODORE::TED (intId,
		 _VRASTERDATA, _HRASTERDATA, vV, wS,
		{ { "Name", "TED" },
		  { "Code", "7360/8360 for NTSC" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Year", "1984" } })
{
	// Nothing else to do...
}
