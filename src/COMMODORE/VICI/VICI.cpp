#include <COMMODORE/VICI/VICI.hpp>

// ---
/** This values have been clculated from the info at: http://tinyvga.com/6561. */
const MCHEmul::RasterData COMMODORE::VICI_PAL::_VRASTERDATA	(0, 28, 28, 311, 311, 311, 312, 0, 0);
/** Really the movement is half the values show, but they have been multiplied by two to maitain the proportion inthe screen. */
const MCHEmul::RasterData COMMODORE::VICI_PAL::_HRASTERDATA	
	(0, 40, 40, 504, 504, 567, 568 /** For everything to run well, it has to be a multiple of 8. */, 0, 0);
const MCHEmul::RasterData COMMODORE::VICI_NTSC::_VRASTERDATA (0, 28, 28, 260, 260, 260, 261, 0, 0);
/** Really the movement is half the vakues show, but they have been multiplied by two to maitain the proportion inthe screen. */
const MCHEmul::RasterData COMMODORE::VICI_NTSC::_HRASTERDATA 
	(0, 0, 0, 415, 415, 519, 520 /** For everything to run well, it has to be a multiple of 8. */, 0, 0);
// Memory where to read when there is no text window...
const MCHEmul::Address COMMODORE::VICI::_MEMORYPOSOFF = MCHEmul::Address ({ 0x00, 0x1c }, true /* already in little - endian. */);

// ---
COMMODORE::VICI::SoundFunction::SoundFunction (MCHEmul::SoundLibWrapper* sW)
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
	assert (dynamic_cast <COMMODORE::VICISoundLibWrapper*> (soundWrapper ()) != nullptr);

	setClassName ("SoundFunction");
}

// ---
bool COMMODORE::VICI::SoundFunction::initialize ()
{
	if (!MCHEmul::SoundChip::initialize ())
		return (false);

	_lastCPUCycles = 0;

	return (true);
}

// ---
bool COMMODORE::VICI::SoundFunction::simulate (MCHEmul::CPU* cpu)
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
MCHEmul::InfoStructure COMMODORE::VICI::SoundFunction::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::SoundChip::getInfoStructure ());

	result.remove ("Memory"); // This info is not neccesary...
	result.add ("SoundLibWrapper",	std::move (soundWrapper () -> getInfoStructure ())); // To know which library is behing...

	return (result);
}

// ---
COMMODORE::VICI::VICI (const MCHEmul::RasterData& vd, const MCHEmul::RasterData& hd, 
		int vV, unsigned short cRL, MCHEmul::SoundLibWrapper* sW, const MCHEmul::Attributes& attrs)
	: MCHEmul::GraphicalChip (_ID, 
		{ { "Name", "VICI" },
		  { "Code", "6560 (NTSC), 6561/6561E/6561-101 (PAL)" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Year", "1977" } }),
	  _soundFunction (new COMMODORE::VICI::SoundFunction (sW)),
	  _VICIRegisters (nullptr),
	  _VICIView (vV),
	  _cyclesPerRasterLine (cRL),
	  _raster (vd, hd, 8 /** The step is here 8 pixels */),
	  _lastCPUCycles (0),
	  _format (nullptr),
	  _cycleInRasterLine (0),
	  _lastVBlankEntered (false),
	  _scrX1 (0), _scrY1 (0), _scrX2 (0), _scrY2 (0)
{
	assert (_cyclesPerRasterLine >= 65);

	MCHEmul::GraphicalChip::setClassName ("VICI");

	_format = SDL_AllocFormat (SDL_PIXELFORMAT_ARGB8888);
}

// ---
COMMODORE::VICI::~VICI ()
{
	/**
		delete (_soundFunction);
		It is not invoked, because the sound function is
		managed as a different chip from the computer on regards simulation.
		That chip is owned by the computer and then destroyed by it. */

	SDL_FreeFormat (_format);
}

// ---
bool COMMODORE::VICI::initialize ()
{
	assert (MCHEmul::GraphicalChip::memoryRef () != nullptr);

	// The sound function is not initialized, because at Computer level
	// it is treated as an independent chip, that will be also initialized!
	if (!MCHEmul::GraphicalChip::initialize ())
		return (false);

	// Gets the memory block dedicated to the VICI
	if (!(_VICIRegisters = 
		dynamic_cast <COMMODORE::VICIRegisters*> 
			(MCHEmul::GraphicalChip::memoryRef () -> subset (COMMODORE::VICIRegisters::_VICREGS_SUBSET))))
	{
		MCHEmul::GraphicalChip::_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	_raster.initialize ();

	_VICIRegisters -> setSoundLibWrapper (_soundFunction -> soundWrapper ());
	_VICIRegisters -> initialize ();

	_lastCPUCycles = 0;
	
	_cycleInRasterLine = 1;

	_lastVBlankEntered = false;

	_scrX1 = _scrY1 = _scrX2 = _scrY2 = 0;

	_vicGraphicInfo.initialize (_VICIRegisters);

	return (true);
}

// ---
bool COMMODORE::VICI::simulate (MCHEmul::CPU* cpu)
{
	// First time?
	if (_lastCPUCycles == 0)
	{ 
		_lastCPUCycles = cpu -> clockCycles (); // Nothing to do...

		return (true);
	}

	// Simulate the visulization...
	for (unsigned int i = (cpu -> clockCycles  () - _lastCPUCycles); i > 0; i--)
	{
		if (MCHEmul::GraphicalChip::deepDebugActive ())
		{
			*MCHEmul::GraphicalChip::_deepDebugFile
				// Where
				<< "VICI\t" 
				// When
				<< std::to_string (cpu -> clockCycles () - i) << "\t" // clock cycles at that point
				// What
				<< "Info cycle\t\t"
				// Data
				<< "Raster:["
				<< std::to_string (_raster.currentColumnAtBase0 ()) << "," 
				<< std::to_string (_raster.currentLineAtBase0 ()) << ","
				<< std::to_string (_cycleInRasterLine)
				<< "], Memory:["
				<< "SM=$" << MCHEmul::removeAll0 (_VICIRegisters -> screenMemory ().asString
					(MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) << ","
				<< "CM=$" << MCHEmul::removeAll0 (_VICIRegisters -> charDataMemory ().asString
					(MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) << ","
				<< "BM=$" << MCHEmul::removeAll0 (_VICIRegisters -> colourMemory ().asString
					(MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) << "]\n";
		}

		// Read the graphics and draw the visible zone, 
		// if it is the case...
		readGraphicsAndDrawVisibleZone ();
		// ...and then treat the current cycle...
		treatRasterCycle ();
		// ...and also moves 8 pixels right in the raster line and jump to other line is possible...
		// Notice that the variable _isNewRasterLine becomes true when a new line comes...
		if (_raster.moveCycles (1))
			_cycleInRasterLine = 1;

		// When the raster enters the non visible part of the screen,
		// a notification is sent (to the Screen class usually) 
		// just to draw the screen...
		if (_raster.isInFirstVBlankZone ())
		{
			if (!_lastVBlankEntered)
			{
				_lastVBlankEntered = true;

				MCHEmul::GraphicalChip::notify (MCHEmul::Event (_GRAPHICSREADY));

				_vicGraphicInfo.initialize (_VICIRegisters);
			}
		}
		else
			_lastVBlankEntered = false;
	}

	_VICIRegisters -> setCurrentRasterLine (_raster.currentLine ()); 

	_lastCPUCycles = cpu -> clockCycles ();

	return (true);

	// Note that the _soundFunction is not simulated here...
	// It is added like another different chip on regards simulation
	// The framework need a sound chip always!
}

// ---
MCHEmul::InfoStructure COMMODORE::VICI::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::GraphicalChip::getInfoStructure ());

	result.remove ("Memory"); // This info is not neccesary...
	result.add ("VICIRegisters",	std::move (_VICIRegisters -> getInfoStructure ()));
	result.add ("Raster",			std::move (_raster.getInfoStructure ()));
	result.add ("Sound",			std::move (_soundFunction -> getInfoStructure ()));

	return (result);
}

// ---
void COMMODORE::VICI::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
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
MCHEmul::ScreenMemory* COMMODORE::VICI::createScreenMemory ()
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
void COMMODORE::VICI::readGraphicsAndDrawVisibleZone ()
{
	// The graphical info is always read!
	readVideoMatrixAndColorRAM ();
	readGraphicalInfo ();

	// Calculate the screen position, if it is needed...
	// If the screen position has not been reached yet, or it is already out of it, the previous value is maintained...
	calculateScreenPositions ();

	// ..and draws the graphics & border if it has to do so...
	if (_raster.isInVisibleZone ())
		drawVisibleZone ();
}

// ---
void COMMODORE::VICI::treatRasterCycle ()
{
	// At the very last cycle, the row is moved...
	if (_cycleInRasterLine == _cyclesPerRasterLine)
		_vicGraphicInfo.nextVertical (_VICIRegisters);
	else
	// In any raster cycles, expect the last one, the column is then moved...
	if (_cycleInRasterLine < _cyclesPerRasterLine)
		_vicGraphicInfo.nextHorizontal (_VICIRegisters);

	// ...then finally moves to the next cycle...
	_cycleInRasterLine++;
}

// ---
void COMMODORE::VICI::drawVisibleZone ()
{
	// These two variables are very key.
	// They hold the position of the raster within the VISIBLE ZONE.
	// It is the left up corner of the "computer screen" will be cv = 0 & rv = 0...
	unsigned short cv, rv; 
	_raster.currentVisiblePosition (cv, rv);

	// In other case...
	// Everyting is the color of the border initially...
	// ..and it will be covered with the screen later if needed..
	// This is how the VICI works...I guess!
	screenMemory () -> setHorizontalLine ((size_t) cv, (size_t) rv,
		(cv + 8) > _raster.visibleColumns () ? (_raster.visibleColumns () - cv) : 8, 
			_VICIRegisters -> borderColor ());

	// Now the information is drawn...
	if (_vicGraphicInfo.draw ())
		drawGraphics (
			{
				/** _ICD */ _raster.hData ().firstDisplayPosition (),
				/** _LCD */ _raster.hData ().lastDisplayPosition (),
				/** _RC	 */ cv,		// Where the horizontal raster is (not adjusted to 8) inside the window
				/** _IRD */ _raster.vData ().firstDisplayPosition (),
				/** _LRD */ _raster.vData ().lastDisplayPosition (),
				/** _RR	 */ rv		// Where the vertical raster is inside the window (it is not the chip raster line)
			});

	// The position related with the lightpen is calculated...
	unsigned short dx, dy;
	_raster.currentDisplayPosition (dx, dy);
	unsigned short lpx, lpy;
	_VICIRegisters -> currentLightPenPosition (lpx, lpy);
}

// ---
void COMMODORE::VICI::drawGraphics (const COMMODORE::VICI::DrawContext& dC)
{
	// First of all, the screen is drawn!...
	screenMemory () -> setHorizontalLine ((size_t) dC._RC, (size_t) dC._RR,
		(dC._RC + 8) > _raster.visibleColumns () ? (_raster.visibleColumns () - dC._RC) : 8, 
			_VICIRegisters -> backgroundColor ());

	// Multicolor mode?
	// where the color of the pixels are determined by combination 
	// of the pair of bits in the char data definition memory...
	// or just high resolution one?
	// where the color of the pixels is determined by any individual bit in
	// in the char definition memory...
	int cb = (int) dC._RC - (int) dC._ICD;
	drawResultToScreen (
		(((_vicGraphicInfo._colorData.value () & 0x08) == 0x00) // MSB low nibble on? = multicolorMode...
			? drawHighResolutionMode (cb)
			: drawMulticolorMode (cb)), dC);
}

// ---
COMMODORE::VICI::DrawResult COMMODORE::VICI::drawHighResolutionMode (int cb)
{
	COMMODORE::VICI::DrawResult result;

	for (unsigned short i = 0 ; 
			i < 8 /** To paint always 8 pixels but in blocks of 2. */; i += 2)
	{
		if (_vicGraphicInfo._graphicData [3 - (i >> 1)])
		{
			// If the pixel is se and the inverse mode is not active...
			// ...the color will be in the foreground...
			if (!_VICIRegisters -> inverseMode ())
			{
				result._foregroundColorData [i] = 
				result._foregroundColorData [i + 1] = _vicGraphicInfo._colorData.value () & 0x07;
			}
		}
		else
		{
			// But if the pixels is off and the mode inverse is active...
			// ...the color will be in the background...
			if (_VICIRegisters -> inverseMode ())
			{
				result._backgroundColorData [i] = 
				result._backgroundColorData [i + 1] = _vicGraphicInfo._colorData.value () & 0x07;
			}
		}
	}

	return (result);
}

// ---
COMMODORE::VICI::DrawResult COMMODORE::VICI::drawMulticolorMode (int cb)
{
	COMMODORE::VICI::DrawResult result;

	for (unsigned short i = 0 ; 
			i < 8 /** To paint always 8 pixels but in blocks of 2. */; i += 2)
	{
		unsigned char cs = 
			(_vicGraphicInfo._graphicData.value () >> (2 - (i >> 2))) & 0x03; // 0, 1, 2, 3?
		// The value 0 means background, and it has already been drawn!
		if (cs == 0x00)
			continue;

		// Depending on the value of the par of bits, the rule will be one or another...
		unsigned int fc = 
			(cs == 0x01)
				? _VICIRegisters -> borderColor ()
				: ((cs == 0x10)
					? _vicGraphicInfo._colorData.value () & 0x07
					: _VICIRegisters -> auxiliarColor ());

		result._foregroundColorData [i] = 
		result._foregroundColorData [i + 1] = fc;
	}

	return (result);
}

// ---
void COMMODORE::VICI::drawResultToScreen (const COMMODORE::VICI::DrawResult& cT, const COMMODORE::VICI::DrawContext& dC)
{
	// The four pixels to draw...
	for (size_t i = 0; i < 8; i++)
	{
		size_t pos = (size_t) dC._RC + i;

		// And then the background pixels...
		if (cT._backgroundColorData [i] != ~0)
			screenMemory () -> setPixel (pos, (size_t) dC._RR, cT._backgroundColorData [i]);
		// and the foreground ones finally...
		if (cT._foregroundColorData [i] != ~0)
			screenMemory () -> setPixel (pos, (size_t) dC._RR, cT._foregroundColorData [i]);
	}
}

// ---
COMMODORE::VICI_PAL::VICI_PAL (int vV, MCHEmul::SoundLibWrapper* wS)
	: COMMODORE::VICI (
		 _VRASTERDATA, _HRASTERDATA, vV, COMMODORE::VICI_PAL::_CYCLESPERRASTERLINE, wS,
		 { { "Name", "VIC-I (PAL) Video Chip Interface" },
		   { "Code", "6561/6561E/6561-101" },
		   { "Manufacturer", "MOS Technology INC/Commodore Semiconductor Group (CBM)"},
		   { "Year", "1977" } })
{
	// Nothing else to do...
}

// ---
COMMODORE::VICI_NTSC::VICI_NTSC (int vV, MCHEmul::SoundLibWrapper* wS)
	: COMMODORE::VICI (
		 _VRASTERDATA, _HRASTERDATA, vV, _CYCLESPERRASTERLINE, wS,
			{ { "Name", "VICI" },
			  { "Code", "6560 (NTSC)" },
			  { "Manufacturer", "Commodore Business Machines CBM" },
			  { "Year", "1977" } })
{
	// Nothing else to do...
}
