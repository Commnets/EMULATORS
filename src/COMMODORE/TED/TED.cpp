#include <COMMODORE/TED/TED.hpp>
#include <COMMODORE/TED/TEDSoundWrapper.hpp>
#include <F6500/IRQInterrupt.hpp>

// ---
const MCHEmul::RasterData COMMODORE::TED_PAL::_VRASTERDATA (260, 274, 3, 202, 245, 259, 259, 312, 4, 4);
const MCHEmul::RasterData COMMODORE::TED_PAL::_HRASTERDATA 
	(376, 384, 432, 295, 343, 375, 375, 456 /** For everyting to run, it has to be divisible by 8. */, 7, 9);
const MCHEmul::RasterData COMMODORE::TED_NTSC::_VRASTERDATA (235, 249, 3, 202, 220, 234, 234, 262, 4, 4);
const MCHEmul::RasterData COMMODORE::TED_NTSC::_HRASTERDATA 
	(376, 384, 432, 295, 343, 375, 375, 456 /** For everyting to run, it has to be divisible by 8. */, 7, 9);
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
COMMODORE::TED::TED (int intId, unsigned short clkcpum, unsigned short sfq, 
		const MCHEmul::RasterData& vd, const MCHEmul::RasterData& hd,
		int vV, MCHEmul::SoundLibWrapper* sW, const MCHEmul::Attributes& attrs)
	: MCHEmul::GraphicalChip (_ID, attrs),
	  _interruptId (intId),
	  _timesFasterThanCPU (clkcpum),
	  _screenfrequency (sfq),
	  _T1 (1, COMMODORE::TEDTimer::RunMode::_FROMINITIALVALUE),
	  _T2 (2, COMMODORE::TEDTimer::RunMode::_CONTINUOUS),
	  _T3 (3, COMMODORE::TEDTimer::RunMode::_CONTINUOUS),
	  _soundFunction (new COMMODORE::TED::SoundFunction (sW)),
	  _TEDRegisters (nullptr), 
	  _TEDView (vV),
	  _cyclesPerRasterLine (57),
	  _raster (vd, hd, 8 /** step. */),
	  _drawRasterInterruptPositions (false),
	  _drawOtherEvents (false),
	  _timesFrameDrawn (0),
	  _lastCPUCycles (0),
	  _format (nullptr),
	  _cycleInRasterLine (1),
	  _videoActive (true),
	  _lastVBlankEntered (false),
	  _lastBadLineScrollY (-1), _newBadLineCondition (false), _badLineStopCyclesAdded (false),
	  _tedGraphicInfo (),
	  _eventStatus { 0 }
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

	_timesFrameDrawn = 0;

	_lastCPUCycles = 0;
	
	_cycleInRasterLine = 1;

	_videoActive = true;

	_lastVBlankEntered = false;

	_lastBadLineScrollY = -1;
	_newBadLineCondition = false;
	_badLineStopCyclesAdded = false;

	_tedGraphicInfo = { };
	_eventStatus = { 0 };

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

	// The simulation has to be repeated as many time 
	// as cycles have been spent since the last invocation...
	for (unsigned int i = (cpu -> clockCycles  () - _lastCPUCycles); i > 0; i--)
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
			_IFDEBUG debugBadLine ();

			_newBadLineCondition = true;		// latched...
			_badLineStopCyclesAdded = false;	// ...the cycles have to be added...

			_tedGraphicInfo._idleState = false; // No longer in "idle" state but in the "screen" one!

			// To keep track of the event later, if needed...
			_eventStatus._badLine = 
				_raster.vData ().currentVisiblePosition ();
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
				
				// Flash counter is incremented every frame...
				_TEDRegisters -> incrementFlashCounter (); 
				
				// Every half the frequency of the TED....
				// ...the cursor hardware status (used in standard char mode changes)
				if (++_timesFrameDrawn >= (_screenfrequency >> 2))
				{
					_timesFrameDrawn = 0;
					_tedGraphicInfo.changeCursorHardwareStatus ();
				}
			}
			// In any other line number, VC start back to count from the value in VCBASE.
			// VCBASE is actualized only then RC reaches 8. @see rasterCycle 58 treatment.
			else 
				_tedGraphicInfo._VC = _tedGraphicInfo._VCBASE;

			// If the current line is where a IR has been set...
			if (_raster.currentLine () == _TEDRegisters -> IRQRasterLineAt ())
				_TEDRegisters -> activateRasterIRQ (); // ...the interrupt is activated (but not necessary launched!)
		}

		// Update the position of the raster in the TED Registers...
		_TEDRegisters -> setCurrentRasterPosition 
			(_raster.currentLine (), _raster.currentColumn ());

		// Simulate the timers!
		_T1.simulate (cpu);
		_T2.simulate (cpu);
		_T3.simulate (cpu);

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
MCHEmul::UBytes COMMODORE::TED::screenMemorySnapShot (MCHEmul::CPU* cpu) const
{ 	
	// Usually in this point the active view should be the CPU one, 
	// But the one needed is the TED, so it is changed if neded...
	int aVID = cpu -> memoryRef () -> activeView () -> id ();
	if (aVID != _TEDView)
		cpu -> memoryRef () -> setActiveView (_TEDView);

	MCHEmul::UBytes result = cpu -> memoryRef () -> values 
		(_TEDRegisters -> screenMemory (), 0x03e8 /** 1000 positions = 40 x 25. */);

	cpu -> memoryRef () -> setActiveView (aVID);

	return (result);
}

// ---
MCHEmul::UBytes COMMODORE::TED::attributeMemorySnapShot (MCHEmul::CPU* cpu) const
{ 
	// The TED doesn't manage colors only but attributes instead...
	// ...the meaning of each will depend on the mode used...
	int aVID = cpu -> memoryRef () -> activeView () -> id ();
	if (aVID == _TEDView)
		cpu -> memoryRef () -> setCPUView (); // Change to the main one...

	// Gets the data from the memory directly...
	MCHEmul::UBytes result = cpu -> memoryRef () -> values 
		(_TEDRegisters -> attributeMemory (), 0x03e8 /** 1000 positions = 40 x 25. */);
	for (size_t i = 0; i < result.size (); i++)
		result [i] = result [i]; // All bits are usefull...

	cpu -> memoryRef () -> setActiveView (aVID);

	return (result);
}

// ---
MCHEmul::UBytes COMMODORE::TED::bitmapMemorySnapShot (MCHEmul::CPU* cpu) const
{ 
	int aVID = cpu -> memoryRef () -> activeView () -> id ();
	if (aVID != _TEDView)
		cpu -> memoryRef () -> setActiveView (_TEDView);

	MCHEmul::UBytes result = cpu -> memoryRef () -> values 
		(_TEDRegisters -> bitmapMemory (), 0x1f40 /** 8000 positions = 40 x 25 x 8. */); 

	cpu -> memoryRef () -> setActiveView (aVID);

	return (result);
}

// ---
MCHEmul::Strings COMMODORE::TED::charsDrawSnapshot (MCHEmul::CPU* cpu,
	const std::vector <size_t>& chrs) const
{
	// Lambda function to draw the content of a byte in a "monocolor way".
	// That is: one bit, one pixel, one color...
	auto textByteMonocolor = [](const MCHEmul::UByte& u) -> std::string 
		{ 
			std::string result;
			for (size_t l = 0; l < 8; l++)
				result += ((u.value () & (1 << (7 - l))) != 0x00) ? "#" : " ";
			return (result);
		};

	// Lambda function to draw the content of a byte in "multicolor way".
	// That is: a par of bit, two pixels, one color...
	auto textByteMultiColor = [](const MCHEmul::UByte& u) -> std::string
		{
			std::string result;

			for (size_t l = 0; l < 8; l += 2)
			{ 
				switch ((u.value () & (0x03 << (6 - l))) >> (6 - l))
				{
					case 0x00: result += "  "; break;
					case 0x01: result += "OO"; break;
					case 0x02: result += "XX"; break;
					case 0x03: result += "##"; break;
					// This situation doesn't happen...
					default: break;
				}
			}

			return (result);
		};

	int aVID = cpu -> memoryRef () -> activeView () -> id ();
	if (aVID != _TEDView)
		cpu -> memoryRef () -> setActiveView (_TEDView);

	MCHEmul::Strings result;
	for (size_t i = 0; i < 256; i++)
	{
		if (!chrs.empty () && 
			std::find (chrs.begin (), chrs.end (), i) == chrs.end ())
			continue;

		MCHEmul::Address chrAdd = _TEDRegisters -> charDataMemory () + (i << 3);
		std::string dt = std::to_string (i) + "---\n$" +
			MCHEmul::removeAll0 (chrAdd.asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) + "\n";
		MCHEmul::UBytes chrDt = cpu -> memoryRef () -> values (chrAdd, 0x08);
		for (size_t j = 0; j < 8; j++) // 8 lines per character...
		{
			if (j != 0)
				dt += "\n";

			// In multicolor, depending on the bit 3 of the attribute byte
			// in the color matrix, the character can be draw as monocolor or as multicolor...
			// The rest of the modes are draw as monocolor, including the extended multicolor char...
			dt += textByteMonocolor (chrDt [j]);
			if (_TEDRegisters -> graphicMulticolorTextModeActive ())
				dt += " | " /** Separated by a symbol. */ + textByteMultiColor (chrDt [j]);
			dt += "|$" + chrDt [j].asString (MCHEmul::UByte::OutputFormat::_HEXA, 2);
		}

		result.emplace_back (std::move (dt));
	}

	result.emplace_back ("---");

	cpu -> memoryRef () -> setActiveView (aVID);

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
	unsigned int* cP = new unsigned int [128 + 5];
	for (size_t i = 0; i < 16; i++)
		for (size_t j = 0; j < 8; j++)
			cP [i + (j << 4)] = SDL_MapRGBA (_format, 
				_COLORS [(i << 3) + j].R, _COLORS [(i << 3) + j].G, _COLORS [(i << 3) + j].B, 0xe0);

	// More colors just to reflect events...
	cP [128] = SDL_MapRGBA (_format, 0X00, 0Xf5, 0xff, 0xff); // light Cyan
	cP [129] = SDL_MapRGBA (_format, 0Xfc, 0Xe7, 0x00, 0xff); // light Yellow
	cP [130] = SDL_MapRGBA (_format, 0Xff, 0X6d, 0x28, 0xff); // light Orange
	cP [131] = SDL_MapRGBA (_format, 0Xea, 0X04, 0x7e, 0xff); // light Purple
	cP [132] = SDL_MapRGBA (_format, 0X3e, 0Xc7, 0x0b, 0xff); // light Green

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

			readVideoMatrixAndColorRAM ();

			_IFDEBUG debugReadingVideoMatrix ();
		}

		readGraphicalInfo ();

		_IFDEBUG debugReadingGraphics ();

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
			/** _SC	 */ _TEDRegisters -> horizontalScrollPosition (),	// From 0 - 7 
			/** _RC	 */ cv,												// Where the horizontal raster is (not adjusted to 8) inside the window
			/** _RCA */ cav,											// Where the horizontal raster is (adjusted to 8) inside the window
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

	// If there were requested to draw the position where the Raster Interrupt is generated...
	// All draw are replace by a line with a color different of the background!
	// The line is only drawn if that position where within the visible zone!
	if (_drawRasterInterruptPositions)
	{
		unsigned short lrt = 
			_raster.lineInVisibleZone (_TEDRegisters -> IRQRasterLineAt ());
		if (lrt <= _raster.vData ().lastVisiblePosition ())
		{
			screenMemory () -> setHorizontalLine ((size_t) cav, (size_t) lrt,
				(cav + 8) > _raster.visibleColumns () ? (_raster.visibleColumns () - cav) : 8, 130);
			if (cav >= 8 && cav < 40 && lrt > 5)
				screenMemory () -> setString ((size_t) 0, (size_t) (lrt - 5), 
					std::to_string ((unsigned int) _TEDRegisters -> IRQRasterLineAt ()) + " " +
					std::to_string ((unsigned int) lrt), 131);
		}
	}

	// If it activated to draw other events that happen during the interation of the VICII...
	if (_drawOtherEvents)
		drawOtherEvents ();
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
void COMMODORE::TED::drawOtherEvents ()
{
	// Draw the bad line event...
	// From the moment the condition is identified and a full line!
	if (_eventStatus._badLine != std::numeric_limits <unsigned short>::max ())
	{ 
		if (_raster.vData ().currentVisiblePosition () == _eventStatus._badLine)
			screenMemory () -> setHorizontalLine 
				(_raster.hData ().currentVisiblePosition (), _eventStatus._badLine, 2, 128); // in points and draw in auxiliar color...
		else
			_eventStatus._badLine = std::numeric_limits <unsigned short>::max ();
	}
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
				// Calculates whether the position of the cursor is or not visible...
				// ...as this is the only mode where the cursor can be drawn...
				unsigned short cp = _TEDRegisters -> cursorPosition (); // This is number between 0 and 1000...
				int cpy = ((((int) cp / 0x28)) << 3) + (int) _raster.vData ().firstScreenPosition () 
							+ ((int) _TEDRegisters -> verticalScrollPosition () - 3);
				int cpx = (((cp % 0x28)) << 3) - dC._SC; // Not needed to add the border, but it can be negative because the scroll...
				bool dCA = cpx >= cb && cpx <= (cb + 7); // Is the first part of the cursor visible?
				bool dCB = (cpx + 7 + dC._SC) >= cb && (cpx + 7 + dC._SC) <= (cb + 7); // or the last one?
				int dCI = 0, dCN = 8; // From what position to draw, and how many pixles should be drawn...
				if (dCA) { dCI = dC._SC; dCN = 8 - dCI; }
				else if (dCB) { dCI = 0; dCN = dC._SC; }

				result = std::move (drawMonoColorChar (cb, 
					(dC._RR >= cpy && dC._RR < (cpy + 8)) && (dCA || dCB), dCI, dCN));
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
COMMODORE::TED::DrawResult COMMODORE::TED::drawMonoColorChar (int cb, bool c, int dCI, int dCN)
{
	COMMODORE::TED::DrawResult result;

	for (int i = 0; i < 8 /** To paint always 8 pixels */; i++)
	{
		int pp = cb + i;
		if (pp < 0)
			continue;
		if (pp >= 320)
			break;

		// Calculate the byte and bit where the pixel info is...
		size_t iBy = ((size_t) pp) >> 3; // To determine the byte...
		size_t iBt = 7 - (((size_t) pp) % 8); // From MSB to LSB...

		// Pixel must be on or off?
		bool dP = calcPixelHiResMode (iBy, iBt);
		// ...if the cursor is over the pixels to draw and the cursor hardware is on, changes the status of the pixel...
		// Hardware status only happen in the MonoColorChar mode...
		if (c && _tedGraphicInfo.cursorHardwareStatus () && 
			i >= dCI && i < (dCI + dCN)) dP = !dP;
		// ...and finally draws it...
		// In the case of dP == false, it is redundant at it is already background...
		result._foregroundColorData [i] = dP 
			? (unsigned int) (_tedGraphicInfo._colorData [iBy].value () & 0x7f /** Without the bit 7 (tha define blinking mode or not) */)
			: (unsigned int) (_TEDRegisters -> backgroundColor ().asChar ());

		// When 0, it is background...
		// Not necessary to the color of the pixels as it will be always the basic background color,
	}

	return (result);
}

// ---
COMMODORE::TED::DrawResult COMMODORE::TED::drawMultiColorChar (int cb, bool inv)
{
	COMMODORE::TED::DrawResult result;

	for (unsigned short i = 0 ; i < 8; i++)
	{
		int pp = cb + i;
		if (pp < -1)
			continue;
		if (pp >= 320)
			break;

		// After this pp can be -1...

		// Calculate the byte where the pixel info is...
		size_t iBy = ((size_t) pp) >> 3; 
		// If the bit 3 of the attribute byte is on, 
		// the character will be drawn in real multicolor mode...
		if (_tedGraphicInfo._colorData [iBy].bit (3))
		{
			unsigned char cs = 0;
			if (pp >= 0)
			{
				// Calculate the bit where the pixel info is...
				// in this case there a pair of bit with the color info...
				size_t iBt = 3 - ((((size_t) pp) % 8) >> 1);
				cs = (_tedGraphicInfo._graphicData [iBy].value () >> (iBt << 1)) & 0x03; // 0, 1, 2 or 3
			}
			// This is the case when pp == -1...
			else
				cs = (_tedGraphicInfo._graphicData [0].value () >> 6) & 0x03; // 0, 1, 2 or 3

			// If the pixel 7 of the character data is set to 1, 
			// the video inverse is then calculated...
			if (_tedGraphicInfo._screenCodeData [iBy].bit (7) &&
				_TEDRegisters -> reverseVideoActive ()) cs = 3 - cs;
			// The blinking is defined in the bit 7 of the color attribute...
			if (_tedGraphicInfo._colorData [iBy].bit (7) &&
				_TEDRegisters -> flashCounterOn ()) cs = 3 - cs;

			// The color of the pixels will depend on the value of cs....
			// In the case of cs == 0x00 it will be redudant as it is already background, but just in case...
			switch (cs)
			{
				case 0x00:
				case 0x01:
				case 0x02:
					{
						// i + 1 is always < 8...
						result._foregroundColorData [i] =
						result._foregroundColorData [i + 1] = 
							_TEDRegisters -> backgroundColor ((size_t) cs).asChar ();
					}

					break;

				case 0x03:
					{
						// i + 1 is always < 8...
						result._foregroundColorData [i]		= 
						result._foregroundColorData [i + 1] = 
							_tedGraphicInfo._colorData [iBy].value () & 0b01110111; // Without bits 3 & 7
					}

					break;

				default:
					// Other value is not possible...
					break;
			}

			// As two pixels have been drawn...
			// This is the way to guranttee that i is always even...
			i++; 
		}
		// ...but if not, it will be drawn in hi - res mode (like above)
		else
		{
			if (pp < 0)
				continue; 

			// Calculate the bit where the pixel info is...
			size_t iBt = 7 - (((size_t) pp) % 8); 
			// Calc the status of the pixel in hi - res mode...
			// Notice that here the is no hardware cursor possible!
			bool dP = calcPixelHiResMode (iBy, iBt);
			// ...and finally draws it...
			// In the case of dP == false, it is redundant at it is already background...
			result._foregroundColorData [i] = dP 
				? (unsigned int) (_tedGraphicInfo._colorData [iBy].value () & 0x7f /** Without the bit 7 (tha define blinking mode or not) */)
				: (unsigned int) (_TEDRegisters -> backgroundColor ().asChar ());
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

		// Calculate the byte and bit where the pixel info is...
		size_t iBy = ((size_t) pp) >> 3 /** To determine the byte. */;
		size_t iBt = 7 - (((size_t) pp) % 8); /** From MSB to LSB. */

		// Pixel must be on or off?
		// In this mode there is no reverse mode or blinking...
		bool dP = _tedGraphicInfo._graphicData [iBy].bit (iBt);
		// The color of the pixel will depend on whether it is on or off...
		// ...and also on the 2 MSB bites of the screen code byte...
		// In the case of cs == 0x00 it will be redudant as it is already background, but just in case...
		unsigned int cs = ((_tedGraphicInfo._screenCodeData [iBy].value () & 0xc0) >> 6) & 0x03; // 0, 1, 2, or 3
		unsigned int fc = 
			dP 
				? (_tedGraphicInfo._colorData [iBy].value ()) 
				: _TEDRegisters -> backgroundColor (cs).asChar ();
		// ...and draws it!!
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

		// Determines the color as a function of the screen code and the attribute byte...
		unsigned int fc = 
			inv 
				? 0x00 // When invalid, all pixels are black...
				: bS 
					? (((_tedGraphicInfo._screenCodeData [iBy].value () & 0x70 /** bits 4, 5 & 6. */) >> 4 /** moved to color info. */)
						| ((_tedGraphicInfo._colorData [iBy].value () & 0x07 /** bits 0, 1 & 2. */) << 4 /** moved to luminance info. */))
					: ((_tedGraphicInfo._screenCodeData [iBy].value () & 0x07 /** bits 0, 1 & 2. */)
						| (_tedGraphicInfo._colorData [iBy].value () & 0x70 /** bits 4, 5 & 6. */));

		// ...and draws it!!
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

		// Determines the color...
		unsigned fc = // The value 0x00 is not tested....
				inv
					? 0x00 // When invalid all pixels are black...
					: (cs == 0x00) // In the case of cs == 0x00 it will be redudant as it is already background, but just in case...
						? _TEDRegisters -> backgroundColor ().asChar ()
						: ((cs == 0x01) // The color is the defined in the video matrix (color) & color data (luminance)...
							? (((_tedGraphicInfo._screenCodeData [iBy].value () & 0xf0 /** bits 4, 5, 6 & 7. */) >> 4) /** to color position. */
								// There is a mistake in the TED documentation, it shows bits 4, 5 & 6 instead of bits 0, 1 & 2...
								| ((_tedGraphicInfo._colorData [iBy].value () & 0x07 /** bits 0, 1 & 2. */) << 4))
							: ((cs == 0x02) // The color is defined in the video matrix (color) & color data (luminance)...
								? ((_tedGraphicInfo._screenCodeData [iBy].value () & 0x0f /** bits 0, 1, 2 & 3 */)
									| (_tedGraphicInfo._colorData [iBy].value () & 0x70 /** bits 4, 5 & 6. */) /** to luminance position. */)
								: (_TEDRegisters -> backgroundColor (0x01).asChar ())));

		// ...and draws it!!
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
				"Attributes=$" + MCHEmul::removeAll0 (_TEDRegisters -> attributeMemory ().asString
					(MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) + "," +
				"Characters=$" + MCHEmul::removeAll0 (_TEDRegisters -> charDataMemory ().asString
					(MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) + 
					"(ROM=" + (_TEDRegisters -> ROMActiveToFetchCharAndBitmap () ? "TRUE" : "FALSE") + ")," +
				"Bitmap=$" + MCHEmul::removeAll0 (_TEDRegisters -> bitmapMemory ().asString
				(MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) } });
}

// ---
void COMMODORE::TED::debugBadLine ()
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeLineData ("Bad line situation");
}

// ---
void COMMODORE::TED::debugReadingVideoMatrix ()
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeLineData ("Reading Video Matrix & Attribute RAM [" +
					_tedGraphicInfo._lastScreenCodeDataRead.asString (MCHEmul::UByte::OutputFormat::_HEXA) + ", " +
					_tedGraphicInfo._lastColorDataRead.asString (MCHEmul::UByte::OutputFormat::_HEXA) + "]");
}

// ---
void COMMODORE::TED::debugReadingGraphics ()
{
	assert (_deepDebugFile != nullptr);
	
	_deepDebugFile -> writeLineData ("Reading Graphics [" + 
		_tedGraphicInfo._lastGraphicDataRead.asString (MCHEmul::UByte::OutputFormat::_HEXA) + "]");
}

// ---
COMMODORE::TED_PAL::TED_PAL (int intId, unsigned short clkcpum, int vV, MCHEmul::SoundLibWrapper* wS)
	: COMMODORE::TED (intId, clkcpum, _SCREENFREQUENCY,
		 _VRASTERDATA, _HRASTERDATA, vV, wS,
		{ { "Name", "TED" },
		  { "Code", "7360/8360 for PAL" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Year", "1984" } })
{
	// Nothing else to do...
}

// ---
COMMODORE::TED_NTSC::TED_NTSC (int intId, unsigned short clkcpum, int vV, MCHEmul::SoundLibWrapper* wS)
	: COMMODORE::TED (intId, clkcpum, _SCREENFREQUENCY,
		 _VRASTERDATA, _HRASTERDATA, vV, wS,
		{ { "Name", "TED" },
		  { "Code", "7360/8360 for NTSC" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Year", "1984" } })
{
	// Nothing else to do...
}
