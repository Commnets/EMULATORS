#include <TEXASINSTRUMENTS/TMS99xxFamily/TMS99xxFamily.hpp>
#include <TEXASINSTRUMENTS/TMS99xxFamily/TMS99xxFamilyRegisters.hpp>

// ---
// PAL Systems...
const MCHEmul::RasterData TEXASINSTRUMENTS::TMS9929A::_VRASTERDATA
	(248, 256 /** 8 VSYNC. */, 0 /** 56 top border. */, 191 /** 192 draw. */, 247 /** 56 bottom. */, 247, 247, 312, 0, 0);
const MCHEmul::RasterData TEXASINSTRUMENTS::TMS9929A::_HRASTERDATA
	(320, 416 /** 96 = Blanking Period + HSYNC (different in 5C, happens before, than in 6C) */, 
	 0 /** 32 left border. */, 255 /** 256 (32 chars * 8 TMS9918 cycles or 4 CPU cycles) draw. */, 
	 287 /** 32 right border. */, 319 /** Other 32 non visible. */, 319, 448, 0, 0);
// NTSC is explained in similar terms than PAL...
const MCHEmul::RasterData TEXASINSTRUMENTS::TMS9918A::_VRASTERDATA (216, 224, 0, 191, 215, 215, 215, 264, 0, 0);
const MCHEmul::RasterData TEXASINSTRUMENTS::TMS9918A::_HRASTERDATA (320, 416, 0, 255, 287, 319, 319, 448, 0, 0);

// ---
TEXASINSTRUMENTS::TMS99xxFamily::TMS99xxFamily (const MCHEmul::RasterData& vd, const MCHEmul::RasterData& hd, 
		TEXASINSTRUMENTS::TMS99xxFamilyRegisters* reg,
		unsigned int cF, 
		int iId,
		const MCHEmul::Attributes& attrs)
	: MCHEmul::GraphicalChip (_ID, attrs),
	  _TMS99xxFamilyRegisters (reg),
	  _raster (vd, hd, 1 /** The step is 1 pixel. */),
	  _clockFactor (cF),
	  _interruptId (iId),
	  _showEvents (false),
	  _lastCPUCycles (0),
	  _format (nullptr),
	  _eventStatus ({ }), // No event status so far!
	  _internalRegisters (nullptr)
{
	// Can not be 0!...
	assert (_clockFactor != 0);

	setClassName ("TMS99xxFamily");

	// If nullptr a temporal one is created that it will be deleted when the object is destroyed...
	if (_TMS99xxFamilyRegisters == nullptr)
		_internalRegisters = _TMS99xxFamilyRegisters = new TEXASINSTRUMENTS::TMS99xxFamilyRegisters;

	_format = SDL_AllocFormat (SDL_PIXELFORMAT_ARGB8888);
}

// ---
TEXASINSTRUMENTS::TMS99xxFamily::~TMS99xxFamily ()
{
	SDL_FreeFormat (_format);

	// That could be nullptr, it they would have been created externally!
	delete (_internalRegisters); // ...and in that case, nothing will happen...
}

// ---
bool TEXASINSTRUMENTS::TMS99xxFamily::initialize ()
{
	assert (MCHEmul::GraphicalChip::memoryRef () != nullptr);

	if (!MCHEmul::GraphicalChip::initialize ())
		return (false);

	_raster.initialize ();

	_TMS99xxFamilyRegisters -> initialize ();

	_lastCPUCycles = 0;

	_eventStatus = { };

	return (true);
}

// ---
bool TEXASINSTRUMENTS::TMS99xxFamily::simulate (MCHEmul::CPU* cpu)
{
	// First time?
	if (_lastCPUCycles == 0)
	{ 
		_lastCPUCycles = cpu -> clockCycles (); // Nothing to do...

		return (true);
	}

	// Simulate the visulization...
	for (unsigned int i = 
			((cpu -> clockCycles  () - _lastCPUCycles) * _clockFactor); 
		i > 0; i--)
	{
		_IFDEBUG debugTMS9918ACycle (cpu, i);

		// Read the graphics and draw the visible zone, if it is the case...
		// ...Mark also whethe rthe event have to be drawn or not!
		if (_raster.isInVisibleZone ())
		{
			readGraphicInfoAndDrawVisibleZone (cpu);

			if (_showEvents)
				drawEvents ();
		}
	
		// First, moves the raster...
		_raster.moveCycles (1);
		// But, if is starting the screen...
		if (_raster.vData ().currentPositionAtBase0 () == 0 &&
			_raster.hData ().currentPositionAtBase0 () == 0)
		{
			// Set the bit 7 in the status register (INT)
			_TMS99xxFamilyRegisters -> setSreenUpdateHappen ();
	
			// ...a notification to draw the screen is also generated...
			MCHEmul::GraphicalChip::notify (MCHEmul::Event (_GRAPHICSREADY));
		}

		// An interrupt is launched if the system was configured to execute that action...
		if (_TMS99xxFamilyRegisters -> screenUpdateHappen () &&
			_TMS99xxFamilyRegisters -> launchScreenUpdateInterrupt ())
			cpu -> requestInterrupt
				(_interruptId /** Received at construction time. */, i, this, 0 /** The reason is that the screen is complete. */);
		// The interrupt routine must clear that bit 7 or otherwise...
		// ...the interrupts will be called over and over again!
	}

	_lastCPUCycles = cpu -> clockCycles ();

	return (true);
}

// ---
MCHEmul::InfoStructure TEXASINSTRUMENTS::TMS99xxFamily::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::GraphicalChip::getInfoStructure ());

	result.remove ("Memory"); // This info is not neccesary...
	result.add ("TMS99xxFamilyRegisters",	std::move (_TMS99xxFamilyRegisters -> getInfoStructure ()));
	result.add ("Raster",				std::move (_raster.getInfoStructure ()));

	return (result);
}

// ---
void TEXASINSTRUMENTS::TMS99xxFamily::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	switch (evnt.id ())
	{
		// TODO
		
		default:
			break;
	}
}

// ---
MCHEmul::ScreenMemory* TEXASINSTRUMENTS::TMS99xxFamily::createScreenMemory ()
{
	unsigned int* cP = new unsigned int [20];
	// The colors are partially transparents to allow the blending...
	cP [0]  = SDL_MapRGBA (_format, 0x00, 0x00, 0x00, 0x00); // Transparent
	cP [1]  = SDL_MapRGBA (_format, 0x00, 0x00, 0x00, 0xe0); // Black
	cP [2]  = SDL_MapRGBA (_format, 0x3e, 0xb8, 0x49, 0xe0); // Medium Green
	cP [3]  = SDL_MapRGBA (_format, 0x74, 0xd0, 0x7d, 0xe0); // Light Green
	cP [4]  = SDL_MapRGBA (_format, 0x59, 0x55, 0xe0, 0xe0); // Dark Blue
	cP [5]  = SDL_MapRGBA (_format, 0x80, 0x76, 0xf1, 0xe0); // Light Blue
	cP [6]  = SDL_MapRGBA (_format, 0xb9, 0x5e, 0x51, 0xe0); // Dark Red
	cP [7]  = SDL_MapRGBA (_format, 0x65, 0xdb, 0xef, 0xe0); // Cyan
	cP [8]  = SDL_MapRGBA (_format, 0xdb, 0x65, 0x69, 0xe0); // Medium Red
	cP [9]  = SDL_MapRGBA (_format, 0xff, 0x89, 0x7d, 0xe0); // Light Red
	cP [10] = SDL_MapRGBA (_format, 0xcc, 0xc3, 0x5e, 0xe0); // Dark Yellow
	cP [11] = SDL_MapRGBA (_format, 0xde, 0xd0, 0x87, 0xe0); // Light Yellow
	cP [12] = SDL_MapRGBA (_format, 0x3a, 0xa2, 0x42, 0xe0); // Dark Green
	cP [13] = SDL_MapRGBA (_format, 0xb7, 0x66, 0xb5, 0xe0); // Magenta
	cP [14] = SDL_MapRGBA (_format, 0xcc, 0xcc, 0xcc, 0xe0); // Gray
	cP [15] = SDL_MapRGBA (_format, 0xff, 0xff, 0xff, 0xe0); // White

	// Colors used for events!
	cP [16] = SDL_MapRGBA (_format, 0X00, 0Xf5, 0xff, 0xff); // light Cyan
	cP [17] = SDL_MapRGBA (_format, 0Xfc, 0Xe7, 0x00, 0xff); // light Yellow
	cP [18] = SDL_MapRGBA (_format, 0Xff, 0X6d, 0x28, 0xff); // light Orange
	cP [19] = SDL_MapRGBA (_format, 0Xea, 0X04, 0x7e, 0xff); // light Purple

	return (new MCHEmul::ScreenMemory (numberColumns (), numberRows (), cP));
}

// --
void TEXASINSTRUMENTS::TMS99xxFamily::readGraphicInfoAndDrawVisibleZone (MCHEmul::CPU* cpu)
{
	// Here it is sure that the raster in the visible zone...

	// Gets the position with in the visible zone...
	// That position will take into account the birder (top and left)...
	unsigned short x = 0, y = 0;
	_raster.currentVisiblePosition (x, y);
		
	// Draws a point with the background first...
	// Unless the raster were in a display zone, it will be the only thing to be drawn...
	_screenMemory -> setPixel 
		(x, y, (unsigned int) _TMS99xxFamilyRegisters -> backDropColor () & 0x07);

	// If it is not still in the screen position, there is anything else to do...
	if (!_raster.isInScreenZone ())
		return;

	// If the screen is in blank, 
	// ...the system doesn't continue either...
	if (_TMS99xxFamilyRegisters -> blankScreen ())
	{
		_IFDEBUG debugVideoNoActive ();

		return;
	}

	// Draw the graphics and sprites and detect the collisions 
	// All that done depending on the graphical mode used too...
	drawGraphicsSpritesAndDetectCollisions (x, y,
		_TMS99xxFamilyRegisters -> readGraphicInfo 
			(x - _raster.hData ().firstScreenPosition (), // Positions within the screen part of the memory...
			 y - _raster.vData ().firstScreenPosition ()) /** From the video memory. */);
}

// ---
void TEXASINSTRUMENTS::TMS99xxFamily::drawGraphicsSpritesAndDetectCollisions 
	(unsigned short x, unsigned short y, const std::tuple <MCHEmul::UByte, MCHEmul::UByte, MCHEmul::UByte>& data)
{
	switch (_TMS99xxFamilyRegisters -> graphicMode ())
	{
		case TEXASINSTRUMENTS::TMS99xxFamilyRegisters::_GRAPHICIMODE:
			{
				drawGraphicsScreenGraphicsMode (x, y, data);
			}

			break;

		case TEXASINSTRUMENTS::TMS99xxFamilyRegisters::_TEXTMODE:
			{
				drawGraphicsScreenTextMode (x, y, data);
			}

			break;

		case TEXASINSTRUMENTS::TMS99xxFamilyRegisters::_MULTICOLORMODE:
			{
				drawGraphicsScreenMulticolorMode (x, y, data);
			}

			break;

		case TEXASINSTRUMENTS::TMS99xxFamilyRegisters::_GRAPHICIIMODE:
			{
				drawGraphicsScreenGraphicsMode (x, y, data);
			}

			break;

		default:
			{
				_LOG ("Graphic mode:" + 
					  std::to_string (_TMS99xxFamilyRegisters -> graphicMode ()) + " not implemented");

				assert (false);
			}

			break;
	}
}

// ----
void TEXASINSTRUMENTS::TMS99xxFamily::drawGraphicsScreenGraphicsMode (unsigned short x, unsigned short y,
			 const std::tuple <MCHEmul::UByte, MCHEmul::UByte, MCHEmul::UByte>& data)
{
	// Which is the foreground color?
	unsigned int fC = (unsigned int) ((std::get <2> (data).value () & 0xf0) >> 4);
	// Which is the background color?
	unsigned int bC = (unsigned int) std::get <2> (data).value () & 0x0f;
	// Is there any pixel to draw?
	bool p = std::get <1> (data).bit (7 - ((x - _raster.hData ().firstScreenPosition ()) % 8));
	// If there is color defined for a pixel to be drawn...draw it!
	if (fC != 0 && p)	_screenMemory -> setPixel (x, y, fC);
	// If there is color defined for a pixel not drawn...draw it too! 
	if (bC != 0 && !p)	_screenMemory -> setPixel (x, y, bC);
}

// ---
void TEXASINSTRUMENTS::TMS99xxFamily::drawGraphicsScreenTextMode (unsigned short x, unsigned short y,
			 const std::tuple <MCHEmul::UByte, MCHEmul::UByte, MCHEmul::UByte>& data)
{
	unsigned short xS = x - _raster.hData ().firstScreenPosition ();
	if (xS < 8 || xS >= 248 /** (40 * 6) + 8 first pixels = 248. */)
		_screenMemory -> setPixel (x, y, _TMS99xxFamilyRegisters -> backDropColor ());
	else
		_screenMemory -> setPixel (x, y,
			std::get <1> (data).bit (7 - ((xS - 8) % 6))
				? _TMS99xxFamilyRegisters -> textColor ()
				: _TMS99xxFamilyRegisters -> backDropColor ());
}

// ---
void TEXASINSTRUMENTS::TMS99xxFamily::drawGraphicsScreenMulticolorMode (unsigned short x, unsigned short y,
			 const std::tuple <MCHEmul::UByte, MCHEmul::UByte, MCHEmul::UByte>& data)
{
	// The colot to apply will depend on the group of 4 pixels...
	unsigned int cl = 0;
	unsigned short xS = x - _raster.hData ().firstScreenPosition ();
	if (((xS >> 2) % 1) == 0) cl = (unsigned int) ((std::get <2> (data).value () & 0xf0) >> 4); // even block...
	else cl = (unsigned int) (std::get <2> (data).value () & 0x0f); // odd block...
	_screenMemory -> setPixel (x, y, cl);
}

// ---
void TEXASINSTRUMENTS::TMS99xxFamily::drawEvents ()
{
	// Draw the border events...
	unsigned int cEvent = std::numeric_limits <unsigned int>::max ();
	
	// TODO
	
	if (cEvent != std::numeric_limits <unsigned int>::max ())
		screenMemory () -> setHorizontalLine // Draw at least two pixels when the events has happpened...
			(_raster.hData ().currentVisiblePosition (), _raster.vData ().currentVisiblePosition (), 2, cEvent);
}

// ---
void TEXASINSTRUMENTS::TMS99xxFamily::debugTMS9918ACycle (MCHEmul::CPU* cpu, unsigned int i)
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeCompleteLine (className (), cpu -> clockCycles () - (i * _clockFactor), "Info Cyle", 
		MCHEmul::Attributes ({ 
			{ "Raster",
				std::to_string (_raster.currentColumnAtBase0 ()) + "," +
				std::to_string (_raster.currentLineAtBase0 ()) } }));
}

// ---
void TEXASINSTRUMENTS::TMS99xxFamily::debugVideoNoActive ()
{
	assert (_deepDebugFile != nullptr);

	// TODO
}

// ---
TEXASINSTRUMENTS::TMS9929A::TMS9929A (TEXASINSTRUMENTS::TMS99xxFamilyRegisters* reg, unsigned int cF, int iId)
	: TMS99xxFamily
		(_VRASTERDATA, _HRASTERDATA, reg, cF, iId,
		 { { "Name", "TMS9929A" },
		   { "Manufacturer", "TexasInstruments" },
		   { "Year", "1982" } })
{
	// Nothing else...
}

// ---
TEXASINSTRUMENTS::TMS9918A::TMS9918A (TEXASINSTRUMENTS::TMS99xxFamilyRegisters* reg, unsigned int cF, int iId)
	: TMS99xxFamily
		(_VRASTERDATA, _HRASTERDATA, reg, cF, iId,
		 { { "Name", "TMS9918A/TMS9928A" },
		   { "Manufacturer", "TexasInstruments" },
		   { "Year", "1982" } })
{
	// Nothing else...
}
