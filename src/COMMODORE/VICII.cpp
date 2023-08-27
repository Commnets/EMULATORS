#include <COMMODORE/VICII.hpp>
#include <F6500/IRQInterrupt.hpp>

// ---
const MCHEmul::Address COMMODORE::VICII::_COLORMEMORY ({ 0x00, 0xd8 }, false);
const MCHEmul::RasterData COMMODORE::VICII_NTSC::_VRASTERDATA (27, 41, 51, 250, 12, 26, 262, 4, 4);
const MCHEmul::RasterData COMMODORE::VICII_NTSC::_HRASTERDATA (412, 488, 24, 343, 388, 411, 512, 7, 9);
const MCHEmul::RasterData COMMODORE::VICII_PAL::_VRASTERDATA (0, 16, 51, 250, 299, 311, 312, 4, 4);
const MCHEmul::RasterData COMMODORE::VICII_PAL::_HRASTERDATA (404, 480, 24, 343, 380, 403, 504, 7, 9);

// ---
COMMODORE::VICII::VICII (const MCHEmul::RasterData& vd, const MCHEmul::RasterData& hd, 
		int vV, unsigned short cRL, const MCHEmul::Attributes& attrs)
	: MCHEmul::GraphicalChip (_ID, 
		{ { "Name", "VICII" },
		  { "Code", "6567/8562/8564 (NTSC), 6569/8565/8566 (PAL)" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Year", "1983" } }),
	  _VICIIRegisters (nullptr), 
	  _VICIIView (vV),
	  _cyclesPerRasterLine (cRL),
	  _raster (vd, hd),
	  _lastCPUCycles (0),
	  _format (nullptr),
	  _graphicsScreenCodeData (MCHEmul::UBytes::_E), 
	  _graphicsGraphicData (MCHEmul::UBytes::_E),
	  _graphicsColorData (MCHEmul::UBytes::_E),
	  _graphicsSprites (8, MCHEmul::UBytes::_E),
	  _graphicsLineSprites (8, MCHEmul::UBytes::_E),
	  _isNewRasterLine (true), // The first
	  _cycleInRasterLine (1),
	  _videoActive (true),
	  _lastVBlankEntered (false)
{
	assert (_cyclesPerRasterLine >= 63);

	setClassName ("VICII");

	_format = SDL_AllocFormat (SDL_PIXELFORMAT_ARGB8888);
}

// ---
COMMODORE::VICII::~VICII ()
{
	SDL_FreeFormat (_format);
}

// ---
bool COMMODORE::VICII::initialize ()
{
	assert (memoryRef () != nullptr);

	if (!MCHEmul::GraphicalChip::initialize ())
		return (false);

	// Gets the memory block dedicated to the VICII
	if (!(_VICIIRegisters = 
		dynamic_cast <COMMODORE::VICIIRegisters*> (memoryRef () -> subset (COMMODORE::VICIIRegisters::_VICREGS_SUBSET))))
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	_raster.initialize ();

	_VICIIRegisters -> initialize ();

	_lastCPUCycles = 0;

	_graphicsScreenCodeData = MCHEmul::UBytes::_E; 
	_graphicsGraphicData = MCHEmul::UBytes::_E;
	_graphicsColorData = MCHEmul::UBytes::_E;
	_graphicsSprites = std::vector <MCHEmul::UBytes> (8, MCHEmul::UBytes::_E);

	_isNewRasterLine = true; // The first...
	
	_cycleInRasterLine = 1;

	_videoActive = true;

	_lastVBlankEntered = false;

	return (true);
}

// ---
bool COMMODORE::VICII::simulate (MCHEmul::CPU* cpu)
{
	// If the "video reset flag" is actived nothing is done...
	if (_VICIIRegisters -> videoResetActive ())
		return (true);

	// Adapt the size of the display zone to the parameters specificied in the register...
	// The zone where sprites and texts are finally visible is call the "screen zone"
	_raster.reduceDisplayZone
		(!_VICIIRegisters -> textDisplay25RowsActive (), !_VICIIRegisters -> textDisplay40ColumnsActive ());

	// The simulation has to be repeated as many timeas as cycles have spent since the last invocation...
	for (unsigned int i = (cpu -> clockCycles  () - _lastCPUCycles); i > 0; i--)
	{
		if (deepDebugActive ())
			*_deepDebugFile
				// Where
				<< "VICII\t" 
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
				<< std::to_string ((int) _VICIIRegisters -> graphicModeActive ()) << "\n";

		_videoActive = (_raster.currentLine () == _FIRSTBADLINE) 
			? !_VICIIRegisters -> blankEntireScreen () : _videoActive; // Only at first bad line it can change its value...

		// Always when there is a new line the Raster IRQ has to be checked, 
		// and the situation flagged into the register if true...
		// Whether finally a IRQ is or not issued is something that is calculated bottom in this method...
		if (_isNewRasterLine &&
			_raster.currentLine () == _VICIIRegisters -> IRQRasterLineAt ())
				_VICIIRegisters -> activateRasterIRQ ();

		// When VICII is about to read graphics/sprite info, 
		// the CPU has to be stopped three cycles in advance for reading activities,
		// unless it was stopped previously and that stop situation were still valid...
		if (!cpu -> stopped () && isAboutToReadGraphicalInfo ())
			cpu -> setStop (true, MCHEmul::Instruction::_CYCLEREAD, cpu -> clockCycles  () - i, 3);

		// Depending on the cycle the VICII does different things...
		unsigned int cS = 0;
		if (_cycleInRasterLine >= 1 && _cycleInRasterLine < 10) simulate_BEFORESCREENCYCLES (cpu, cS);
		else if (_cycleInRasterLine >= 10 && _cycleInRasterLine < 55) simulate_SCREENCYCLES (cpu, cS);
		// The non scrren zone can vary depending on the type of VICII chip...
		else simulate_AFTERSCREENCYCLES (cpu, cS);
		// Stops the CPU when it has been decided in the internal methods...
		// ...and for the number of cycles also decided...
		if (cS > 0)
			cpu -> setStop (true, MCHEmul::Instruction::_CYCLEREAD, cpu -> clockCycles  () - i, (int) cS);

		// Is this zone there would be a remaining visible zone...
		if (_raster.isInVisibleZone ())
			drawInVisibleZone (cpu);

		// ...When the deep debug mode is active a line where the interruption is defined is also drawn...
		if (deepDebugActive ())
		{
			unsigned short lrt = 
				_raster.lineInVisibleZone (_VICIIRegisters -> IRQRasterLineAt ());
			if (lrt <= _raster.vData ().lastVisiblePosition ())
			{
				unsigned char clrt = _VICIIRegisters -> backgroundColor () == 15 
					? 0 : _VICIIRegisters -> backgroundColor () + 1; /** to be visible. */
				screenMemory () -> setHorizontalLine (0, lrt, _raster.visibleColumns (), clrt);
			}
		}

		// Move to the next cycle...
		_cycleInRasterLine++;
		// Move 8 pixels right in the raster line and jump to line is possible...
		// Notice that the variable _isNewRasterLine becomes true when a new line comes...
		if (_isNewRasterLine = _raster.moveCycles (1))
			_cycleInRasterLine = 1;

		// When the raster enters the non visible part of the screen,
		// a notification is sent just to draw the screen...
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

		// Per cycle, the IRQ condition is checked! (many reasons during the cycle can unchain the IRQ interrupt)
		if (_VICIIRegisters -> launchIRQ ())
			cpu -> requestInterrupt (F6500::IRQInterrupt::_ID, cpu -> clockCycles  () - i, this);
	}

	_VICIIRegisters -> setCurrentRasterLine (_raster.currentLine ()); 

	_lastCPUCycles = cpu -> clockCycles ();

	return (true);
}

// ---
void COMMODORE::VICII::simulate_BEFORESCREENCYCLES (MCHEmul::CPU* cpu, unsigned int& cS)
{
	if (_cycleInRasterLine == 1 ||
		_cycleInRasterLine == 3 ||
		_cycleInRasterLine == 5 ||
		_cycleInRasterLine == 7 ||
		_cycleInRasterLine == 9)
	{
		memoryRef () -> setActiveView (_VICIIView);

		// Is there sprite info avaliable?
		if (readSpriteDataAt (_raster.currentLine (), 
			((_cycleInRasterLine - 1) >> 1) + 3 /** 3, 4, 5, 6 or 7. */))
		{ 
			cS = 2;	// VICII has to read three bytes, Meaning 2 clock cycles stopped more... 

			if (deepDebugActive ())
				*_deepDebugFile
					<< "\t\t\t\tReading info sprite:" << std::to_string ((_cycleInRasterLine >> 1) + 3) << "\n";
		}

		memoryRef () -> setCPUView ();
	}
}

// ---
void COMMODORE::VICII::simulate_SCREENCYCLES (MCHEmul::CPU* cpu, unsigned int& cS)
{
	// This is not exactly what VICII does
	// as it is reading cycle by cyle the bytes in the memory
	// But it is a good aproximation!
	if (_cycleInRasterLine == 15)
	{
		if (isBadLine ())
		{
			memoryRef () -> setActiveView (_VICIIView);

			readGraphicsInfoAt (_raster.currentLine () - 
				_FIRSTBADLINE - _VICIIRegisters -> verticalScrollPosition ());

			// 40 cycles more just for reading the chars...
			cS = 40;

			if (deepDebugActive ())
				*_deepDebugFile
					<< "\t\t\t\tReading graphical info" << "\n";

			memoryRef () -> setCPUView ();
		}
	}
}

// ---
void COMMODORE::VICII::simulate_AFTERSCREENCYCLES (MCHEmul::CPU* cpu, unsigned int & cS)
{
	// This is not totally true as it will depend on the type of VIC, 
	// ...but it a s good aproximation...
	if (_cycleInRasterLine == 58 + (_cyclesPerRasterLine - COMMODORE::VICII_PAL::_CYCLESPERRASTERLINE) ||
		_cycleInRasterLine == 60 + (_cyclesPerRasterLine - COMMODORE::VICII_PAL::_CYCLESPERRASTERLINE) ||
		_cycleInRasterLine == 62 + (_cyclesPerRasterLine - COMMODORE::VICII_PAL::_CYCLESPERRASTERLINE)) 
	{
		memoryRef () -> setActiveView (_VICIIView);

		// Is there sprite info available?
		if (readSpriteDataAt (_raster.nextLine (), // The sprite info read is for the next line...
			(_cycleInRasterLine - (58 + (_cyclesPerRasterLine - COMMODORE::VICII_PAL::_CYCLESPERRASTERLINE))) >> 1 /** 0, 1 or 2. */))
		{
			cS = 2;	// VICII has to read three bytes, Meaning 2 clock cycles stopped more... 

			if (deepDebugActive ())
				*_deepDebugFile
					<< "\t\t\t\tReading info sprite:" << std::to_string ((_cycleInRasterLine - 58) >> 1) << "\n";
		}

		memoryRef () -> setCPUView ();
	}
}

// ---
void COMMODORE::VICII::drawInVisibleZone (MCHEmul::CPU* cpu)
{
	// These two variables are very key
	// They hold the position of the raster within the visible zone.
	// It is the left up corner of the "computer screen" will be cv = 0 & rv = 0...
	unsigned short cv, rv; 
	_raster.currentVisiblePosition (cv, rv);
	unsigned short cav = (cv >> 3) << 3;

	// If the video is not active, 
	// then evrything will have the border color...
	if (!_videoActive)
		screenMemory () -> setHorizontalLine ((size_t) cav, (size_t) rv,
			(cav + 8) > _raster.visibleColumns () ? (_raster.visibleColumns () - cav) : 8, 
				_VICIIRegisters -> foregroundColor ());
	// Otherwise the elements are drawn...
	else
	{
		if (_raster.isInDisplayZone () && 
			(_raster.vData ().currentPosition () - 
				_VICIIRegisters -> verticalScrollPosition ()) > _LASTBADLINE)
			emptyGraphicsInfo (); // Just in case to avoid paint something innecesary...

		// Everyting is the color of the background initially...
		// ..and it will be covered with the foreground (border) later if needed..
		// This is how the VICII works...
		screenMemory () -> setHorizontalLine ((size_t) cav, (size_t) rv,
			(cav + 8) > _raster.visibleColumns () ? (_raster.visibleColumns () - cav) : 8, 
				_VICIIRegisters -> backgroundColor ());

		// Now the information is drawn...
		drawGraphicsSpritesAndDetectCollisions (cv, cav, rv);

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
				_VICIIRegisters -> foregroundColor ());
		}

		// The IRQ related with the lightpen...
		unsigned short dx, dy;
		_raster.currentDisplayPosition (dx, dy);
		unsigned short lpx, lpy;
		_VICIIRegisters -> currentLightPenPosition (lpx, lpy);
		if (dy == lpy && (lpx >= dx && lpx < (dx + 8))) // The beam moves every 8 pixels...
			_VICIIRegisters -> activateLightPenOnScreenIRQ ();
	}
}

// ---
MCHEmul::InfoStructure COMMODORE::VICII::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::GraphicalChip::getInfoStructure ());

	result.remove ("Memory"); // This info is not neccesary...
	result.add ("VICIIRegisters",	std::move (_VICIIRegisters -> getInfoStructure ()));
	result.add ("Raster",			std::move (_raster.getInfoStructure ()));

	return (result);
}

// ---
void COMMODORE::VICII::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	// To set the bank...
	switch (evnt.id ())
	{
		case _BANK0SET:
		case _BANK1SET:
		case _BANK2SET:
		case _BANK3SET:
			{
				setBank (evnt.id () - _BANK0SET);
			}

			break;

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
void COMMODORE::VICII::drawGraphicsSpritesAndDetectCollisions (unsigned short cv, unsigned short cav, unsigned short rv)
{
	COMMODORE::VICII::DrawContext dC = 
		{
			/** _ICD */ _raster.hData ().firstDisplayPosition (),		// DISLAY: The original...
			/** _ICS */ _raster.hData ().firstScreenPosition (),		// SCREEN: And the real one (after reduction size)
			/** _LCD */ _raster.hData ().lastDisplayPosition (),		// DISPLAY: The original...
			/** _LCS */ _raster.hData ().lastScreenPosition (),			// SCREEN: And the real one (after reduction size)
			/** _SC	 */ _VICIIRegisters -> horizontalScrollPosition (),	// From 0 - 7 
			/** _RC	 */ cv,												// Where the horizontal raster is (not adjusted to 8) inside the window
			/** _RCA */ cav,											// Where the horizontal raster is (adjusted to 8) inside the window
			/** _IRD */ _raster.vData ().firstDisplayPosition (),		// DISPLAY: The original... 
			/** _IRS */ _raster.vData ().firstScreenPosition (),		// SCREEN:  And the real one (after reduction size)
			/** _LRD */ _raster.vData ().lastDisplayPosition (),		// DISPLAY: The original...
			/** _LRS */ _raster.vData ().lastScreenPosition (),			// SCREEN: And the real one (after reduction size)
			/** _SR	 */ _VICIIRegisters -> verticalScrollPosition (),	// From 0 - 7 (taken into account in bad lines)
			/** _RR	 */ rv												// Where the vertical raster is inside the window (it is not the chip raster line)
		};

	// This varible keeps info about the text/graphics:
	// Whether the 8 pixels to draw are foreground or background...
	// ...and the color of the ones that are not finally background!
	// ...and also info to control later the collision with sprites!
	COMMODORE::VICII::DrawResult colGraphics;
	if (_raster.isInDisplayZone ()) // Makes only sense when the raster in in visible zone...
		colGraphics = drawGraphics (dC);

	// The sprites are draw over the _background or the _foreground data
	// ...attending on how they are configured...
	// The info has also to be calculated wven when the raste is not in the visible zone yet!
	std::vector <MCHEmul::UByte> sprCollData (8, MCHEmul::UByte::_0);
	for (int i = 7; i >= 0; i--)
	{
		if (_VICIIRegisters -> spriteEnable ((size_t) i))
			sprCollData [(size_t) i] =
				_VICIIRegisters -> spriteToForegroundPriority ((size_t) i)
					? drawSpriteOver (i, colGraphics._backgroundColorData, dC)
					: drawSpriteOver (i, colGraphics._foregroundColorData, dC);
	}

	// The the graphical info is moved to the screen...
	drawResultToScreen (colGraphics, dC);
	// ...and the collisions are also detected...
	detectCollisions (colGraphics._collisionData, sprCollData);
}

// ---
MCHEmul::ScreenMemory* COMMODORE::VICII::createScreenMemory ()
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
COMMODORE::VICII::DrawResult COMMODORE::VICII::drawGraphics (const COMMODORE::VICII::DrawContext& dC)
{
	// If no graphic has been loaded, it is not needed to continue...
	if (_graphicsGraphicData.size () == 0)
		return (COMMODORE::VICII::DrawResult ()); // It could happen at the first lines of the screen when the vertical SCROLL is active...

	// The "display" line being involved...
	// rc is the "display" line affected by the SROLLY
	// it could be also negative moving from -7,
	// When e.g. the raster is at the very first "display" line,
	// there are no reductions in the screen (display == screen) and SCROLLY = 0x03,
	// 0x03 is the difference between the FIRTBADLINE = 0x30 and the first visible line = 0x33 (with no reduction)
	int rc = (int) dC._RR - (int) dC._IRD - (int) dC._SR + 0x03;
	if (rc < 0)
		return (COMMODORE::VICII::DrawResult ());

	// The "display" column being involved...
	// In cb, the SCROLLX is involved, so it could be negative! starting from -7, 
	// When e.g. the raster is at the very first "screen "dislay" column, 
	// there are no reductions is the screen (display == screen) and SCROLLX = 0x00
	int cb = (int) dC._RCA - (int) dC._ICD - (int) dC._SC;

	// At this poitn rc positive for sure, and cb could be negative...
	// Never invoke the methods within the swith case statements direcly
	// a crash might be generated...

	COMMODORE::VICII::DrawResult result;
	switch (_VICIIRegisters -> graphicModeActive ())
	{
		case COMMODORE::VICIIRegisters::GraphicMode::_CHARMODE:
			{
				result = std::move (drawMonoColorChar (cb, rc, _graphicsGraphicData, _graphicsColorData));
			}

			break;

		case COMMODORE::VICIIRegisters::GraphicMode::_MULTICOLORCHARMODE:
			{
				result = std::move (drawMultiColorChar (cb, rc, _graphicsGraphicData, _graphicsColorData));
			}

			break;

		case COMMODORE::VICIIRegisters::GraphicMode::_EXTENDEDBACKGROUNDMODE:
			{
				result = std::move (drawMultiColorExtendedChar (cb, rc, _graphicsScreenCodeData, _graphicsGraphicData, _graphicsColorData));
			}

			break;

		case COMMODORE::VICIIRegisters::GraphicMode::_BITMAPMODE:
			{
				result = std::move (drawMonoColorBitMap (cb, rc, _graphicsScreenCodeData, _graphicsGraphicData));
			}

			break;

		case COMMODORE::VICIIRegisters::GraphicMode::_MULTICOLORBITMAPMODE:
			{
				result = std::move (drawMultiColorBitMap (cb, rc, _graphicsScreenCodeData, _graphicsGraphicData, _graphicsColorData));
			}

			break;

		case COMMODORE::VICIIRegisters::GraphicMode::_INVALIDTEXMODE:
			{
				// Like multicolor char mode...
				result = std::move (drawMultiColorChar (cb, rc, _graphicsGraphicData, _graphicsColorData));
				// ...but invalid.
				result._invalid = true;
			}

			break;

		case COMMODORE::VICIIRegisters::GraphicMode::_INVALIDBITMAPMODE1:
			{
				// linke moncocolor bitmap mode...
				result = std::move (drawMonoColorBitMap (cb, rc, _graphicsScreenCodeData, _graphicsGraphicData));
				// ...but invalid.
				result._invalid = true;
			}
			break;

		case COMMODORE::VICIIRegisters::GraphicMode::_INVALIDBITMAPMODE2:
			{
				// Like multicolor bitmap mode...
				result = std::move (
					drawMultiColorBitMap 
						(cb, rc, _graphicsScreenCodeData, _graphicsGraphicData, _graphicsColorData));
				// ...but invalid.
				result._invalid = true;
			}

			break;

		default:
			assert (0); // Not possible...the code shouldn't pass over this point!
			break;
	}

	return (result);
}

// ---
COMMODORE::VICII::DrawResult COMMODORE::VICII::drawMonoColorChar (int cb, int rc,
	const MCHEmul::UBytes& bt, const MCHEmul::UBytes& clr)
{
	COMMODORE::VICII::DrawResult result;

	// The graphics are described in blocks of 8 bytes...
	// // and a row is made up of 8 * 40 = 320 bytes.
	// ...so this is to know wich "bytes "byte line" of every char is involved in the graphic line!
	size_t nrc = ((size_t) rc) % 8;

	for (int i = 0; i < 8 /** To paint always 8 pixels */; i++)
	{
		int pp = cb + i;
		if (pp < 0)
			continue;

		size_t iBy = ((size_t) pp) >> 3 /** To determine the byte. */;
		size_t iBt = 7 - (((size_t) pp) % 8); /** From MSB to LSB. */

		if (bt [(iBy << 3) + nrc].bit (iBt))
		{
			result._collisionData.setBit (7 - i, true);

			result._foregroundColorData [i] = 
				(unsigned int) (clr [iBy].value () & 0x0f /** Useful nibble. */);
		}
		// When 0, it is background...
		// Not necessary to specify neither collision information
		// nor the color of the pixels as it will be always the basic background color,
		// that has already been set to the value of $d021 in the main loop...
	}

	return (result);
}

// ---
COMMODORE::VICII::DrawResult COMMODORE::VICII::drawMultiColorChar (int cb, int rc,
	const MCHEmul::UBytes& bt, const MCHEmul::UBytes& clr)
{
	COMMODORE::VICII::DrawResult result;

	// The graphics are described in blocks of 8 bytes...
	// // and a row is made up of 8 * 40 = 320 bytes.
	// ...so this is to know wich "bytes "byte line" of every char is involved in the graphic line!
	size_t nrc = ((size_t) rc) % 8;

	for (unsigned short i = 0 ; i < 8 /** To paint always 8 pixels but in blocks of 2. */; i += 2)
	{
		int pp = cb + i;
		if (pp < -1)
			continue;

		// After this pp can be -1...

		size_t iBy = 0;
		unsigned char cs = 0;
		if (pp >= 0)
		{
			iBy = ((size_t) pp) >> 3; 
			size_t iBt = 3 - ((((size_t) pp) % 8) >> 1);
			cs = (bt [(iBy << 3) + nrc].value () >> (iBt << 1)) & 0x03; // 0, 1, 2 or 3
		}
		// This is the case when pp == -1...
		else
			cs = (bt [nrc].value () >> 6) & 0x03; // 0, 1, 2 or 3

		// If 0, the pixel should be drawn (and considered) as background 
		// and it is already the default status tha comes from the parent method...
		if (cs == 0x00)
			continue;

		// The way the pixels are going to be drawn will depend on the information in the color memory
		// If the most significant bit of the low significant nibble of the color memory is set to 1
		// the data will be managed in a monocolor way...
		if ((clr [iBy] & 0x08) == 0x00) 
		{
			unsigned int fc = clr [iBy].value () & 0x07;

			// ...and remember we are dealing with pairs of pixels...

			if ((cs & 0x02) == 0x02 /** if set. */) 
			{
				result._collisionData.setBit (7 - i, true);

				result._foregroundColorData [i] = fc;
			}
			
			if ((cs & 0x01) == 0x01 /** if set. */)
			{
				result._collisionData.setBit (6 - i, true);

				result._foregroundColorData [i + 1] = fc;
			}

			// if boths "bits" are set, boths pixels will be drawn...
		}
		// But if it is 1, 
		// then it will be draw as in the multicolor version...
		else
		{
			unsigned int fc = 
				(unsigned int) ((cs == 0x03) 
					? (clr [iBy].value () & 0x07)
					: (_VICIIRegisters -> backgroundColor (cs)) & 0x0f) /** Useful nibble. */;

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
COMMODORE::VICII::DrawResult COMMODORE::VICII::drawMultiColorExtendedChar (int cb, int rc,
	const MCHEmul::UBytes& sc, const MCHEmul::UBytes& bt, const MCHEmul::UBytes& clr)
{
	// The mode is quite similar to the standard text mode, 
	// with the difference the 0 pixels (background) can have different background colors...

	COMMODORE::VICII::DrawResult result;

	// The graphics are described in blocks of 8 bytes...
	// // and a row is made up of 8 * 40 = 320 bytes.
	// ...so this is to know wich "bytes "byte line" of every char is involved in the graphic line!
	size_t nrc = ((size_t) rc) % 8;

	for (int i = 0; i < 8 /** To paint always 8 pixels */; i++)
	{
		int pp = cb + i;
		if (pp < 0)
			continue;

		size_t iBy = ((size_t) pp) >> 3 /** To determine the byte. */;
		size_t iBt = 7 - (((size_t) pp) % 8); /** From MSB to LSB. */
		// The color of the pixel 0 is determined by the 2 MSBites of the char code...
		bool bS = bt [(iBy << 3) + nrc].bit (iBt); // To know whether the bit is 1 or 0...
		unsigned int cs = ((sc [iBy].value () & 0xc0) >> 6) & 0x03; // 0, 1, 2, or 3
		unsigned int fc = (bS ? clr [iBy].value () : _VICIIRegisters -> backgroundColor (cs)) & 0x0f /** useful nibble. */;

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
COMMODORE::VICII::DrawResult COMMODORE::VICII::drawMonoColorBitMap (int cb, int rc,
	const MCHEmul::UBytes& sc, const MCHEmul::UBytes& bt)
{
	COMMODORE::VICII::DrawResult result;

	// The graphics are described in blocks of 8 bytes...
	// // and a row is made up of 8 * 40 = 320 bytes.
	// ...so this is to know wich "bytes "byte line" of every char is involved in the graphic line!
	size_t nrc = ((size_t) rc) % 8;

	for (int i = 0; i < 8 /** To paint always 8 pixels. */; i++)
	{
		int pp = cb + i;
		if (pp < 0)
			continue;

		size_t iBy = ((size_t) pp) >> 3 /** To determine the byte. */;
		size_t iBt = 7 - (((size_t) pp) % 8); /** From MSB to LSB. */
		bool bS = bt [(iBy << 3) + nrc].bit (iBt);
		unsigned int fc = bS 
				? (sc [iBy].value () & 0xf0) >> 4	// If the bit is 1, the color is determined by the MSNibble
				: (sc [iBy].value () & 0x0f);		// ...and for LSNibble if it is 0...

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
COMMODORE::VICII::DrawResult COMMODORE::VICII::drawMultiColorBitMap (int cb, int rc,
	const MCHEmul::UBytes& sc, const MCHEmul::UBytes& bt, const MCHEmul::UBytes& clr)
{
	COMMODORE::VICII::DrawResult result;

	// The graphics are described in blocks of 8 bytes...
	// // and a row is made up of 8 * 40 = 320 bytes.
	// ...so this is to know wich "bytes "byte line" of every char is involved in the graphic line!
	size_t nrc = ((size_t) rc) % 8;

	for (unsigned short i = 0 ; i < 8 /** To paint always 8 pixels but in blocks of 2. */; i += 2)
	{
		int pp = cb + i;
		if (pp < -1)
			continue;

		// After this point pp can be -1

		size_t iBy = 0;
		unsigned char cs = 0;
		if (pp >= 0)
		{
			iBy = ((size_t) pp) >> 3; 
			size_t iBt = 3 - ((((size_t) pp) % 8) >> 1);
			cs = (bt [(iBy << 3) + nrc].value () >> (iBt << 1)) & 0x03; // 0, 1, 2 or 3
		}
		// This is the case when pp == -1...
		else
			cs = (bt [nrc].value () >> 6) & 0x03; // 0, 1, 2 or 3

		// If 0, the pixel should be drawn (and considered) as background 
		// and it is already the default status tha comes from the parent method...
		if (cs == 0x00)
			continue;

		unsigned fc = // The value 0x00 is not tested....
				(cs == 0x01) // The color is the defined in the video matrix, high nibble...
					? (sc [iBy].value () & 0xf0) >> 4
					: ((cs == 0x02) // The color is defined in the video matrix, low nibble...
						? sc [iBy].value () & 0x0f
						: clr [iBy].value () & 0x0f); // The color is defined in color matrix...

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
MCHEmul::UByte COMMODORE::VICII::drawSpriteOver (size_t spr, unsigned int* d, const COMMODORE::VICII::DrawContext& dC)
{
	MCHEmul::UByte result = MCHEmul::UByte::_0;

	if (_graphicsLineSprites [spr].size () == 0)
		return (result);

	// The "display" line being involved...
	int r = (int) dC._RR - (int) dC._IRD;
	// The "display" column being involved, adjusted to blocks of 8!
	int c = (int) dC._RCA /** raster column adjusted to blocks of 8. */ - (int) dC._ICD;

	// at this point r and c are positive always...
	// Never invoke the methods within the ? : structure direcly
	// crash might be generated...

	return (
		_VICIIRegisters -> spriteMulticolorMode (spr)
			? drawMultiColorSpriteOver (c, r, spr, d)
			: drawMonoColorSpriteOver (c, r, spr, d));
}

// ---
MCHEmul::UByte COMMODORE::VICII::drawMonoColorSpriteOver (int c, int r, size_t spr, unsigned int* d)
{
	MCHEmul::UByte result = MCHEmul::UByte::_0;

	// Horizontal info about the sprite
	unsigned short dW	= _VICIIRegisters -> spriteDoubleWidth (spr) ? 2 : 1;
	unsigned short x	= _VICIIRegisters -> spriteXCoord (spr);
	// Fix raster position in the raster line of the left up corner of sprite
	// It can be negative if it is partially hidden...
	int cX				= (int) x - 24; 
	unsigned short wX	= 24 /** normal width in pixels. */ * dW;
	unsigned short dW8	= 8 * dW; // 8 or 16
	// Vertical info about the sprite
	unsigned short dH	= _VICIIRegisters -> spriteDoubleHeight (spr) ? 2 : 1;
	unsigned short y	= _VICIIRegisters -> spriteYCoord (spr);
	// Fix raster line of the left up corner of sprite
	// It can be negative if it is partially hidden...
	int rY				= (int) y - 50;
	unsigned short wY	= 21 /** normal height in pixels. */ * dH;

	if (r < rY || r >= (rY + (int) wY))
		return (result); // Not visible in the vertical zone...
	if ((c + 8 /** pixels */) < cX || c >= (cX + (int) wX))
		return (result); // Not visible in the horizontal zone...

	for (unsigned int i = 0; 
			i < 8 /** always to draw 8 pixels */; i += dW /** the size of the pixels block. */)
	{
		int pp = (c + (int) i);
		if (pp < cX)
			continue; // Not visible...
		if (pp >= (cX + (int) wX))
			break; // No more draws...

		// To determine the initial byte (iBy) and bit (iBt) with the info about the sprite...
		// The bit moves from 7 to 0, and the byte increases...
		size_t iBy = (size_t) ((pp - cX) / (int) dW8);
		size_t iBt = (size_t) (7 - (((pp - cX) % (int) dW8) / (int) dW));
		bool dP = _graphicsLineSprites [spr][iBy].bit (iBt);
		if (!dP)
			continue; // The point is not visible...

		for (size_t j = 0; j < (size_t) dW; j++)
		{
			result.setBit (7 - (i + j), true);

			d [i + j] = _VICIIRegisters -> spriteColor (spr) & 0x0f /** useful nibble. */;
		}
	}

	return (result);
}

// ---
MCHEmul::UByte COMMODORE::VICII::drawMultiColorSpriteOver (int c, int r, size_t spr, unsigned int* d)
{
	MCHEmul::UByte result = MCHEmul::UByte::_0;

	// Horizontal info about the sprite
	unsigned short dW	= _VICIIRegisters -> spriteDoubleWidth (spr) ? 2 : 1;
	unsigned short x	= _VICIIRegisters -> spriteXCoord (spr);
	// Fix raster position in the raster line of the left up corner of sprite
	// It can be negative if it is partially hidden...
	int cX				= (int) x - 24; 
	unsigned short wX	= 24 /** normal width in pixels. */ * dW;
	unsigned short dW8	= 8 * dW; // 8 or 16
	// Vertical info about the sprite
	unsigned short dH	= _VICIIRegisters -> spriteDoubleHeight (spr) ? 2 : 1;
	unsigned short y	= _VICIIRegisters -> spriteYCoord (spr);
	// Fix raster line of the left up corner of sprite
	// It can be negative if it is partially hidden...
	int rY				= (int) y - 50;
	unsigned short wY	= 21 /** normal height in pixels. */ * dH;

	if (r < rY || r >= (rY + (int) wY))
		return (result); // Not visible in the vertical zone...
	if ((c + 8 /** pixels */) < cX || c >= (cX + (int) wX))
		return (result); // Not visible in the horizontal zone...

	for (unsigned int i = 0; i < 8 /** pixels. */; i += (2 * dW))
	{
		int pp = (c + (int) i);
		if (pp < cX)
			continue; // Not visible...
		if (pp >= (cX + (int) wX))
			break; // No more draws...

		// To determine the initial byte (iBy) and bit (iBt) with the info about the sprite...
		// The bit to select moves from 0 to 3, represeting the pair of bits (0,1), (2,3), (4,5), (6,7)
		size_t iBy = (size_t) ((pp - cX) / (int) dW8);
		size_t iBt = (size_t) (3 - (((pp - cX) % (int) dW8) / (2 * (int) dW)));
		unsigned char cs = (_graphicsLineSprites [spr][iBy].value () >> (iBt << 1)) & 0x03;
		if (cs == 0)
			continue; // The point has no color...

		unsigned int fc = (unsigned int) ((cs == 0x01) 
			? _VICIIRegisters -> spriteSharedColor (0) 
			: ((cs == 0x02) ? _VICIIRegisters -> spriteColor (spr) : _VICIIRegisters -> spriteSharedColor (1)));
		for (size_t j = 0; j < (size_t) (2 * dW); j++)
		{ 
			result.setBit (7 - (i + j), true);

			d [i + j] = fc;
		}
	}

	return (result);
}

// ---
void COMMODORE::VICII::drawResultToScreen (const COMMODORE::VICII::DrawResult& cT, const COMMODORE::VICII::DrawContext& dC)
{
	// The line is not visible yet...
	if (dC._RR < dC._IRS || dC._RR > dC._LRS)
		return;

	// The eight pixels to draw...
	for (size_t i = 0; i < 8; i++)
	{
		size_t pos = (size_t) dC._RCA + i;

		// If the graphic mode was invalid...
		if (cT._invalid)
		{
			// the pixel will be always black...
			screenMemory () -> setPixel (pos, (size_t) dC._RR, 0 /** black. */);
			// ...and only the sprites over the foreground will be visible.
			if (cT._foregroundColorData [i] != ~0)
				screenMemory () -> setPixel (pos, (size_t) dC._RR, cT._foregroundColorData [i]);
		}
		// ...but if not...
		else
		{
			// First the pure background...
			// Where only the bits in multicolor modes than are defined as background ("00" & "01") will be draw,
			// ...and also the sprites below the foregound!
			if (cT._backgroundColorData [i] != ~0)
				screenMemory () -> setPixel (pos, (size_t) dC._RR, cT._backgroundColorData [i]);
			// Now the foreground info, inclusing the sprites over the foreground.
			if (cT._foregroundColorData [i] != ~0)
				screenMemory () -> setPixel (pos, (size_t) dC._RR, cT._foregroundColorData [i]);
		}
	}
}

// ---
void COMMODORE::VICII::detectCollisions (const MCHEmul::UByte& g, const std::vector <MCHEmul::UByte>& s)
{
	// Now it is time to detect collisions...
	// First among the graphics and the sprites
	bool cGS = false;
	for (size_t i = 0; i < s.size () && !cGS; i++)
		if ((cGS = (s [i].value () & g.value ()) != 0x00)) // ...at the first collision detected, the check stops...
			_VICIIRegisters -> setSpriteCollisionWithDataHappened (i);
	if (cGS) 
		_VICIIRegisters -> activateSpriteCollisionWithDataIRQ ();
	
	// ...and among sprites...
	bool cSS = false;
	for (size_t i = 0; i < s.size (); i++)
	{
		for (size_t j = i + 1; j < s.size (); j++)
		{
			if ((cSS = 
					((s [i].value () & s [j].value ()) != 0x00)))
			{ 
				_VICIIRegisters -> setSpriteCollision (i);
				_VICIIRegisters -> setSpriteCollision (j);
			}
		}
	}

	if (cSS) 
		_VICIIRegisters -> activateSpriteCollisionIRQ ();
}

// ---
COMMODORE::VICII_NTSC::VICII_NTSC (int vV)
	: COMMODORE::VICII (
		 _VRASTERDATA, _HRASTERDATA, vV, 64,
		 { { "Name", "VIC-II (NTSC) Video Chip Interface II" },
		   { "Code", "6567/8562/8564" },
		   { "Manufacturer", "MOS Technology INC/Commodore Semiconductor Group (CBM)"},
		   { "Year", "1980" } })
{
	// Nothing else to do...
}

// ---
COMMODORE::VICII_PAL::VICII_PAL (int vV)
	: COMMODORE::VICII (
		 _VRASTERDATA, _HRASTERDATA, vV, 63,
		 { { "Name", "VIC-II (PAL) Video Chip Interface II" },
		   { "Code", "6569/8565/8566" },
		   { "Manufacturer", "MOS Technology INC/Commodore Semiconductor Group (CBM)"},
		   { "Year", "1980" } })
{
	// Nothing else to do...
}
