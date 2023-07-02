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
		int vV, const MCHEmul::Attributes& attrs)
	: MCHEmul::GraphicalChip (_ID, 
		{ { "Name", "VICII" },
		  { "Code", "6567/8562/8564 (NTSC), 6569/8565/8566 (PAL)" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Year", "1983" } }),
	  _VICIIRegisters (nullptr), 
	  _VICIIView (vV),
	  _raster (vd, hd),
	  _lastCPUCycles (0),
	  _format (nullptr),
	  _graphicsScreenCodeData (MCHEmul::UBytes::_E), 
	  _graphicsGraphicData (MCHEmul::UBytes::_E),
	  _graphicsColorData (MCHEmul::UBytes::_E),
	  _graphicsSprites (8, MCHEmul::UBytes::_E),
	  _graphicsLineSprites (8, MCHEmul::UBytes::_E),
	  _isNewRasterLine (false),
	  _videoActive (true),
	  _lastVBlankEntered (false)
{
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
	_videoActive = true;

	_lastVBlankEntered = false;

	return (true);
}

// ---
bool COMMODORE::VICII::simulate (MCHEmul::CPU* cpu)
{
	return (simulate_II (cpu));
}

// ---
bool COMMODORE::VICII::simulate_I (MCHEmul::CPU* cpu)
{
	// If the video reset is active nothing is done...
	if (_VICIIRegisters -> videoResetActive ())
		return (true);

	// Notice that the bad line detection routine takes into account 
	// the value of the YSCROLL register as the graphics information to be shown 
	// is loaded at the beginning of every bad line...
	auto isBadRasterLine = [=]() -> bool
		{ return (_videoActive && 
				  _raster.vData ().currentPosition () >= _FIRSTBADLINE &&
				  _raster.vData ().currentPosition () <= _LASTBADLINE && 
				  (_raster.currentLine () & 0x07 /** The three last bits. */) == _VICIIRegisters -> verticalScrollPosition ()); };

	// Reduce the visible zone if any... The info is passed to the raster!
	_raster.reduceDisplayZone
		(!_VICIIRegisters -> textDisplay25RowsActive (), !_VICIIRegisters -> textDisplay40ColumnsActive ());

	for (unsigned int i = (cpu -> clockCycles  () - _lastCPUCycles); i > 0 ; i--)
	{
		_videoActive = (_raster.currentLine () == _FIRSTBADLINE) 
			? !_VICIIRegisters -> blankEntireScreen () : _videoActive; // Only at first bad line it can change its value...

		if (_isNewRasterLine)
		{
			memoryRef () -> setActiveView (_VICIIView);

			// The sprites info at that line (if any) has to be read...
			size_t nS = 0;
			readSpriteDataAt (_raster.currentLine (), nS);

			// and if is it also a bad line?...
			if (isBadRasterLine ())
				// ..the graphics / text / color info has to be read too
				readGraphicsInfoAt (_raster.currentLine () - 
					_FIRSTBADLINE - _VICIIRegisters -> verticalScrollPosition ());

			// The VICII doesn't work actually like this
			// The information about the sprites 0 to 3 is read during the last cycles of the previous raster line
			// but this is a good proxy!

			// If the current line is one where has been declared to issue an IRQ...
			// ...the situation is flagged into the register
			// Whether finally a IRQ is or not issued is something that is calculated booton in this method...
			if (_raster.currentLine () == _VICIIRegisters -> IRQRasterLineAt ())
				_VICIIRegisters -> activateRasterIRQ ();

			_isNewRasterLine = false;

			memoryRef () -> setCPUView ();
		}

		if (_raster.isInVisibleZone ())
		{
			// READ: Important variables
			// Where is the raster in the visible part of the screen? (starting from 0)
			unsigned short cv, rv; _raster.currentVisiblePosition (cv, rv);
			// Which is the horizontal closest block of 8 pixels (from left to the right)?
			// Take into account that the step - size of the raster is always 8 pixels per cycle..
			// It always starts at 0...
			unsigned short cav = (cv >> 3) << 3;

			// Draws the border...
			screenMemory () -> setHorizontalLine ((size_t) cav, (size_t) rv,
				(cav + 8) > _raster.visibleColumns () ? (_raster.visibleColumns () - cav) : 8, 
					_VICIIRegisters -> foregroundColor ());

			// When the raster is in the display zone but in the screen vertical zone too
			// and for sure the vide is active, then everything has to happen!
			if (_raster.isInDisplayZone () && 
				_raster.vData ().isInScreenZone () &&
				_videoActive)
			{
				if (_raster.isInDisplayZone () && 
					(_raster.vData ().currentPosition () - 
						_VICIIRegisters -> verticalScrollPosition ()) > _LASTBADLINE)
					emptyGraphicsInfo (); // Just in case to avoid paint something innecesary...

				// Draws the background,
				// taking into account that the screen can be reduced in the X axis...
				if (cav < _raster.hData ().firstScreenPosition () &&
					(cav + 8) > _raster.hData ().firstScreenPosition ())
					screenMemory () -> setHorizontalLine ((size_t) _raster.hData ().firstScreenPosition (), (size_t) rv,
						cav + 8 - _raster.hData ().firstScreenPosition (), _VICIIRegisters -> backgroundColor ());
				else 
				if (cav < _raster.hData ().lastScreenPosition ())
				{
					unsigned short lbk = 8;  // Number of pixels to be drawn...
					if ((cav + 8) > _raster.hData ().lastScreenPosition ())
						lbk = _raster.hData ().lastScreenPosition ()  - cav + 1;
					screenMemory () -> setHorizontalLine ((size_t) cav, (size_t) rv, lbk, _VICIIRegisters -> backgroundColor ());
				}

				drawGraphicsSpritesAndDetectCollisions (cv, cav, rv);
			}
		}

		// 1 cycle = 8 horizontal columns = 8 pixels...
		_isNewRasterLine = _raster.moveCycles (1); 

		if (_raster.isInLastVBlank ())
		{
			if (!_lastVBlankEntered)
			{
				_lastVBlankEntered = true;

				// The limit of the visible screen has been reached!
				// so it is time to actualize the graphics...
				notify (MCHEmul::Event (_GRAPHICSREADY)); 
			}
		}
		else
			_lastVBlankEntered = false;
	}

	// It might have been incremented after reading graphics...
	_lastCPUCycles = cpu -> clockCycles (); 

	// To store back the info in the VIC Registers...
	_VICIIRegisters -> setCurrentRasterLine (_raster.currentLine ()); 

	// Is it needed to generate any IRQ?
	// It is here after the full simulation of the VICII (raster, collisions and lightpen)
	if (_VICIIRegisters -> launchIRQ ())
		cpu -> interrupt (F6500::IRQInterrupt::_ID) -> setActive (true);

	return (true);
}

// ---
bool COMMODORE::VICII::simulate_II (MCHEmul::CPU* cpu)
{
	// If the "video reset flag" is actived nothing is done...
	if (_VICIIRegisters -> videoResetActive ())
		return (true);

	// This is a function used inside to determine whether 
	// a rater line is or not a bad one!
	auto isBadRasterLine = [=]() -> bool
		{ return (_videoActive && 
				  _raster.currentLine () >= _FIRSTBADLINE &&
				  _raster.currentLine () <= _LASTBADLINE && 
				  (_raster.currentLine () & 0x07 /** The three last bits. */) == _VICIIRegisters -> verticalScrollPosition ()); };

	// Adapt the size of the display zone to the parameters specificied in the register...
	// The zone where sprites and texts are finally visible is call the "screen zone"
	_raster.reduceDisplayZone
		(!_VICIIRegisters -> textDisplay25RowsActive (), !_VICIIRegisters -> textDisplay40ColumnsActive ());

	for (unsigned int i = (cpu -> clockCycles  () - _lastCPUCycles); i > 0 ; i--)
	{
		_videoActive = (_raster.currentLine () == _FIRSTBADLINE) 
			? !_VICIIRegisters -> blankEntireScreen () : _videoActive; // Only at first bad line it can change its value...

		// Any time a new line comes..
		// ...and when the video is not active, the graphical information is read...
		if (_isNewRasterLine)
		{
			memoryRef () -> setActiveView (_VICIIView);

			// The sprites info at that line (if any) has to be read...
			size_t nS = 0;
			readSpriteDataAt (_raster.currentLine (), nS);

			// and if is it also a bad line?...
			if (isBadRasterLine ())
				// ..the graphics / text / color info has to be read too
				readGraphicsInfoAt (_raster.currentLine () - 
					_FIRSTBADLINE - _VICIIRegisters -> verticalScrollPosition ());

			// The VICII doesn't work actually like this
			// The information about the sprites 0 to 3 is read during the last cycles of the previous raster line
			// but this is a good proxy!

			// If the current line is one where has been declared to issue an IRQ...
			// ...the situation is flagged into the register
			// Whether finally a IRQ is or not issued is something that is calculated booton in this method...
			if (_raster.currentLine () == _VICIIRegisters -> IRQRasterLineAt ())
				_VICIIRegisters -> activateRasterIRQ ();

			_isNewRasterLine = false;

			memoryRef () -> setCPUView ();
		}

		// Nothing else is done
		// if the raster is not in a visible zone...
		// But if it is...
		if (_raster.isInVisibleZone ())
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

		_isNewRasterLine = _raster.moveCycles (1);

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
	}

	_lastCPUCycles = cpu -> clockCycles (); 

	_VICIIRegisters -> setCurrentRasterLine (_raster.currentLine ()); 

	if (_VICIIRegisters -> launchIRQ ())
		cpu -> interrupt (F6500::IRQInterrupt::_ID) -> setActive (true);

	return (true);
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

	// This variable will hold the bytes drawn per sprite
	// it will be used later to determine the collision among sprites....
	std::vector <MCHEmul::UByte> colSprites (8, MCHEmul::UByte::_0);
	// This other one will hold the char text info...
	MCHEmul::UByte colGraphics = MCHEmul::UByte::_0;

	// The interim function to draw the sprites...
	auto drawSprites = [&](bool f /** false if they don't have priorit against the background. */) -> void
	{
		for (int i = 7; i >= 0; i--)
			if (_VICIIRegisters -> spriteEnable ((size_t) i) &&
				((f && !_VICIIRegisters -> spriteToForegroundPriority ((size_t) i)) ||
				(!f && _VICIIRegisters -> spriteToForegroundPriority ((size_t) i))))
				colSprites [(size_t) i] = drawSprite ((size_t) i, dC);
	};

	drawSprites (false);

	// The method could be invoked directly, 
	// ...but is better to avoid no useful invocations...
	if (_raster.isInDisplayZone ())
		colGraphics = drawGraphics (dC);

	drawSprites (true);

	detectCollisions (colGraphics, colSprites);
}

// ---
MCHEmul::ScreenMemory* COMMODORE::VICII::createScreenMemory ()
{
	unsigned int* cP = new unsigned int [16];
	cP [0]  = SDL_MapRGB (_format, 0x00, 0x00, 0x00); // Black
	cP [1]  = SDL_MapRGB (_format, 0xff, 0xff, 0xff); // White
	cP [2]  = SDL_MapRGB (_format, 0x92, 0x4a, 0x40); // Red
	cP [3]  = SDL_MapRGB (_format, 0x84, 0xc5, 0xcc); // Cyan
	cP [4]  = SDL_MapRGB (_format, 0x93, 0x51, 0xb6); // Violet
	cP [5]  = SDL_MapRGB (_format, 0x72, 0xb1, 0x4b); // Green
	cP [6]  = SDL_MapRGB (_format, 0x48, 0x3a, 0xaa); // Blue
	cP [7]  = SDL_MapRGB (_format, 0xd5, 0xdf, 0x7c); // Yellow
	cP [8]  = SDL_MapRGB (_format, 0x99, 0x69, 0x2d); // Brown
	cP [9]  = SDL_MapRGB (_format, 0x67, 0x52, 0x00); // Light Red
	cP [10] = SDL_MapRGB (_format, 0xc1, 0x81, 0x78); // Orange
	cP [11] = SDL_MapRGB (_format, 0x60, 0x60, 0x60); // Dark Grey
	cP [12] = SDL_MapRGB (_format, 0x8a, 0x8a, 0x8a); // Medium Grey
	cP [13] = SDL_MapRGB (_format, 0xb3, 0xec, 0x91); // Light Green
	cP [14] = SDL_MapRGB (_format, 0x86, 0x7a, 0xde); // Light Blue
	cP [15] = SDL_MapRGB (_format, 0xb3, 0xb3, 0xb3); // Light Grey

	return (new MCHEmul::ScreenMemory (numberColumns (), numberRows (), cP));
}

// ---
MCHEmul::UByte COMMODORE::VICII::drawGraphics (const COMMODORE::VICII::DrawContext& dC)
{
	// If no graphic has been loaded, it is not needed to continue...
	if (_graphicsGraphicData.size () == 0)
		return (MCHEmul::UByte::_0); // It could happen at the first lines of the screen when the vertical SCROLL is active...

	// The "display" line being involved...
	// rc is the "display" line affected by the SROLLY
	// it could be also negative moving from -7,
	// When e.g. the raster is at the very first "display" line,
	// there are no reductions in the screen (display == screen) and SCROLLY = 0x03,
	// 0x03 is the difference between the FIRTBADLINE = 0x30 and the first visible line = 0x33 (with no reduction)
	int rc = (int) dC._RR - (int) dC._IRD - (int) dC._SR + 0x03;
	if (rc < 0)
		return (MCHEmul::UByte::_0);

	// The "display" column being involved...
	// In cb, the SCROLLX is involved, so it could be negative! starting from -7, 
	// When e.g. the raster is at the very first "screen "dislay" column, 
	// there are no reductions is the screen (display == screen) and SCROLLX = 0x00
	int cb = (int) dC._RCA - (int) dC._ICD - (int) dC._SC;

	// At this poitn rc positive for sure, and cb could be negative...
	// Never invoke the methods within the swith case statements direcly
	// a crash might be generated...

	MCHEmul::UByte result = MCHEmul::UByte::_0;
	switch (_VICIIRegisters -> graphicModeActive ())
	{
		case COMMODORE::VICIIRegisters::GraphicMode::_CHARMODE:
			result = drawMonoColorChar (cb, rc, _graphicsGraphicData, _graphicsColorData, dC);
			break;

		case COMMODORE::VICIIRegisters::GraphicMode::_MULTICOLORCHARMODE:
			result = drawMultiColorChar (cb, rc, _graphicsGraphicData, _graphicsColorData, dC);
			break;

		case COMMODORE::VICIIRegisters::GraphicMode::_EXTENDEDBACKGROUNDMODE:
			result = drawMultiColorExtendedChar (cb, rc, _graphicsScreenCodeData, _graphicsGraphicData, _graphicsColorData, dC);
			break;

		case COMMODORE::VICIIRegisters::GraphicMode::_BITMAPMODE:
			result = drawMonoColorBitMap (cb, rc, _graphicsScreenCodeData, _graphicsGraphicData, dC);
			break;

		case COMMODORE::VICIIRegisters::GraphicMode::_MULTICOLORBITMAPMODE:
			result = drawMultiColorBitMap (cb, rc, _graphicsScreenCodeData, _graphicsGraphicData, _graphicsColorData, dC);
			break;

		case COMMODORE::VICIIRegisters::GraphicMode::_INVALIDTEXMODE:
			result = drawMultiColorChar (cb, rc, _graphicsGraphicData, _graphicsColorData, dC, true /** everything black. */);
			break;

		case COMMODORE::VICIIRegisters::GraphicMode::_INVALIDBITMAPMODE1:
			result = drawMonoColorBitMap (cb, rc, _graphicsScreenCodeData, _graphicsGraphicData, dC, true /** everything black. */);
			break;

		case COMMODORE::VICIIRegisters::GraphicMode::_INVALIDBITMAPMODE2:
			result = drawMultiColorBitMap 
				(cb, rc, _graphicsScreenCodeData, _graphicsGraphicData, _graphicsColorData, dC, true /* everything black. */);
			break;

		default:
			assert (0); // Not possible...
			break;
	}

	return (result);
}

// ---
MCHEmul::UByte COMMODORE::VICII::drawMonoColorChar (int cb, int rc,
	const MCHEmul::UBytes& bt, const MCHEmul::UBytes& clr, const COMMODORE::VICII::DrawContext& dC)
{
	MCHEmul::UByte result = MCHEmul::UByte::_0;

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
		unsigned short pos = dC._RCA + i;
		if (bt [(iBy << 3) + nrc].bit (iBt))
		{
			result.setBit (7 - i, true);
			if (pos >= dC._ICS && pos <= dC._LCS)
				screenMemory () -> setPixel ((size_t) pos, (size_t) dC._RR, 
					(unsigned int) (clr [iBy].value () & 0x0f /** Useful nibble. */));
		}
	}

	return (result);
}

// ---
MCHEmul::UByte COMMODORE::VICII::drawMultiColorChar (int cb, int rc,
	const MCHEmul::UBytes& bt, const MCHEmul::UBytes& clr, const COMMODORE::VICII::DrawContext& dC, bool blk)
{
	MCHEmul::UByte result = MCHEmul::UByte::_0;

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

		// If the cs is 0, it would have to be drawn in the background color and it is already!
		if (cs == 0x00)
			continue;

		// The combinations 0x00 (binary 00) and 0x01 (binary 01) are considered as background!
		// So they can not be used in collisions...
		if (cs == 0x02 || cs == 0x03)
		{
			result.setBit (7 - i, true); 
			result.setBit (6 - i, true);
		}

		// When blank nothing is drawn.
		// Everything is kept in the background color!
		if (blk)
			continue;

		// The way the pixels are going to be drawn will depend on the information in the color memory
		// If the most significant bit of the low significant nibble is set to 1
		// the data will be managed in a monocolor way...
		unsigned short pos = dC._RCA + i;
		if ((clr [iBy] & 0x08) == 0x00) 
		{
			unsigned int fc = clr [iBy].value () & 0x07;
			if ((cs & 0x02) == 0x02 /** if set. */ && (pos >= dC._ICS && pos <= dC._LCS)) 
				screenMemory () -> setPixel ((size_t) pos, (size_t) dC._RR, fc);
			if ((cs & 0x01) == 0x01 /** if set. */ && ((pos + 1) >= dC._ICS && (pos + 1) <= dC._LCS)) 
				screenMemory () -> setPixel ((size_t) pos + 1, (size_t) dC._RR, fc);
		}
		// If it is 1, then it will be draw as in the multicolor version...
		else
		{
			unsigned int fc = 
				(unsigned int) ((cs == 0x03) 
					? (clr [iBy].value () & 0x07) 
					: (_VICIIRegisters -> backgroundColor (cs)) & 0x0f) /** Useful nibble. */;
			if (pos >= dC._ICS && pos <= dC._LCS)
				screenMemory () -> setPixel ((size_t) pos, (size_t) dC._RR, fc);
			if ((pos + 1) >= dC._ICS && (pos + 1) <= dC._LCS)
				screenMemory () -> setPixel (((size_t) pos + 1), (size_t) dC._RR, fc);
		}
	}

	return (result);
}

// ---
MCHEmul::UByte COMMODORE::VICII::drawMultiColorExtendedChar (int cb, int rc,
	const MCHEmul::UBytes& sc, const MCHEmul::UBytes& bt, const MCHEmul::UBytes& clr, const COMMODORE::VICII::DrawContext& dC)
{
	MCHEmul::UByte result = MCHEmul::UByte::_0;

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
		result.setBit (7 - i, bS);
		unsigned int cs = ((sc [iBy].value () & 0xc0) >> 6) & 0x03; // 0, 1, 2, or 3
		unsigned short pos = dC._RCA + i;
		unsigned int fc = (bS ? clr [iBy].value () : _VICIIRegisters -> backgroundColor (cs)) & 0x0f /** useful nibble. */;
		if (pos >= dC._ICS && pos <= dC._LCS)
			screenMemory () -> setPixel ((size_t) pos, (size_t) dC._RR, fc);
	}

	return (result);
}

// ---
MCHEmul::UByte COMMODORE::VICII::drawMonoColorBitMap (int cb, int rc,
	const MCHEmul::UBytes& sc, const MCHEmul::UBytes& bt, const COMMODORE::VICII::DrawContext& dC, bool blk)
{
	MCHEmul::UByte result = MCHEmul::UByte::_0;

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
		result.setBit (7 - i, bS);
	
		if (blk)
			continue;

		unsigned int fc = bS 
				? (sc [iBy].value () & 0xf0) >> 4	// If the bit is 1, the color is determined by the MSNibble
				: (sc [iBy].value () & 0x0f);		// ...and for LSNibble if it is 0...
		unsigned short pos = dC._RCA + i;
		if (pos >= dC._ICS && pos <= dC._LCS)
			screenMemory () -> setPixel ((size_t) pos, (size_t) dC._RR, fc); 
	}

	return (result);
}

// ---
MCHEmul::UByte COMMODORE::VICII::drawMultiColorBitMap (int cb, int rc,
	const MCHEmul::UBytes& sc, const MCHEmul::UBytes& bt, const MCHEmul::UBytes& clr, const COMMODORE::VICII::DrawContext& dC, bool blk)
{
	MCHEmul::UByte result = MCHEmul::UByte::_0;

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

		// If 0, the pixel should be drawn in the background color and it is already...
		if (cs == 0x00)
			continue;

		// The combinations 0x00 (binary 00) and 0x01 (binary 01) are considered as background!
		// So they cannot be used in collisions...
		if (cs == 0x02 || cs == 0x03)
		{
			result.setBit (7 - i, true); 
			result.setBit (6 - i, true);
		}

		if (blk)
			continue;

		unsigned short pos = dC._RCA + i;
		unsigned fc = // The value 0x00 is not tested....
				(cs == 0x01) // The color is the defined in the video matrix, high nibble...
						? (sc [iBy].value () & 0xf0) >> 4
						: ((cs == 0x02) // The color is defined in the video matrix, low nibble...
							? sc [iBy].value () & 0x0f
							: clr [iBy].value () & 0x0f); // The color is defined in color matrix...
		if (pos >= dC._ICS && pos <= dC._LCS) 
			screenMemory () -> setPixel ((size_t) pos, (size_t) dC._RR, fc);
		if ((pos + 1) >= dC._ICS && (pos + 1) <= dC._LCS) 
			screenMemory () -> setPixel (((size_t) pos + 1), (size_t) dC._RR, fc);
	}

	return (result);
}

// ---
MCHEmul::UByte COMMODORE::VICII::drawSprite (size_t spr, const COMMODORE::VICII::DrawContext& dC)
{
	if (_graphicsLineSprites [spr].size () == 0)
		return (MCHEmul::UByte::_0);

	// The "display" line being involved...
	int r = (int) dC._RR - (int) dC._IRD;
	// The "display" column being involved, adjusted to blocks of 8!
	int c = (int) dC._RCA /** raster column adjusted to blocks of 8. */ - (int) dC._ICD;

	// at this point r and c are positive always...
	// Never invoke the methods within the ? : structure direcly
	// crash might be generated...

	return (_VICIIRegisters -> spriteMulticolorMode (spr)
		? drawMultiColorSprite (c, r, spr, dC)
		: drawMonoColorSprite (c, r, spr, dC));
}

// ---
MCHEmul::UByte COMMODORE::VICII::drawMonoColorSprite (int c, int r, size_t spr, const COMMODORE::VICII::DrawContext& dC)
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

		unsigned short pos = dC._RCA + i;
		for (size_t j = 0; j < (size_t) dW; j++)
		{
			result.setBit (7 - (i + j), true);
			if (((pos + j) >= dC._ICS && (pos + j) <= dC._LCS) &&
				(dC._RR >= dC._IRS && dC._RR <= dC._LRS))
				screenMemory () -> setPixel ((size_t) (pos + j), (size_t) dC._RR, 
					_VICIIRegisters -> spriteColor (spr) & 0x0f /** useful nibble. */);
		}
	}

	return (result);
}

// ---
MCHEmul::UByte COMMODORE::VICII::drawMultiColorSprite (int c, int r, size_t spr, const COMMODORE::VICII::DrawContext& dC)
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
		unsigned short pos = dC._RCA + i;
		for (size_t j = 0; j < (size_t) (2 * dW); j++)
		{
			result.setBit (7 - (i + j), true); 
			result.setBit (6 - (i + j), true);
			if (((pos + j) >= dC._ICS && (pos + j) <= dC._LCS) &&
				(dC._RR >= dC._IRS && dC._RR <= dC._LRS))
				screenMemory () -> setPixel ((size_t) (pos + j), (size_t) dC._RR, fc);
		}
	}

	return (result);
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
		 _VRASTERDATA, _HRASTERDATA, vV,
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
		 _VRASTERDATA, _HRASTERDATA, vV,
		 { { "Name", "VIC-II (PAL) Video Chip Interface II" },
		   { "Code", "6569/8565/8566" },
		   { "Manufacturer", "MOS Technology INC/Commodore Semiconductor Group (CBM)"},
		   { "Year", "1980" } })
{
	// Nothing else to do...
}
