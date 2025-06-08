#include <TEXASINSTRUMENTS/TMS99xxFamily/TMS99xxFamily.hpp>
#include <TEXASINSTRUMENTS/TMS99xxFamily/TMS99xxFamilyRegisters.hpp>

// ---
// PAL Systems...
const MCHEmul::RasterData TEXASINSTRUMENTS::TMS9929A::_VRASTERDATA
	(0, 0 /** First visible position. */, 54 /** 54 top border. */, 245 /** 192 lines to draw. */, 
	 298 /** 53 bottom. */, 299 /** Retrace init. */, 311 /** End of VSYNC. */, 312, 0, 0);
const MCHEmul::RasterData TEXASINSTRUMENTS::TMS9929A::_HRASTERDATA
	(0, 0 /** First visible position. */, 32 /** 32 left border. */, 287 /** 256 columns to draw. 32 chars (8 pixels each). */, 
	 319 /** 32 right border. */, 320 /** Retrace Init. */, 341 /** End of SYNC. */, 342, 0, 0);
// NTSC is explained in similar terms than PAL...
const MCHEmul::RasterData TEXASINSTRUMENTS::TMS9918A::_VRASTERDATA
	(0, 0, 27, 218, 245, 246, 261, 262, 0, 0); // Sorter...
const MCHEmul::RasterData TEXASINSTRUMENTS::TMS9918A::_HRASTERDATA
	(0, 0, 32, 287, 319, 320, 341, 342, 0, 0);

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
	  _internalRegisters (nullptr),
	  _2CYCLE (false), // To control when the raster moves forward...
	  _spriteInfo (32, TEXASINSTRUMENTS::TMS99xxFamily::SpriteInfo ()) // 32 positions empty...
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

	_2CYCLE = false;

	for (size_t i = 0; i < 32; 
		_spriteInfo [i++] = TEXASINSTRUMENTS::TMS99xxFamily::SpriteInfo ());

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
	// It is needed 2 cycles to draw a pixel
	for (unsigned int i = 
			((cpu -> clockCycles  () - _lastCPUCycles) * _clockFactor); 
		i > 0; i--)
	{
		// Only every 2 cycles the rest of the code is taken,
		// Otherwise it continues iterating...
		// This is the way to reach the _clockFactor (usually 3)/2 speed drawing pixels against the CPU!
		if (!(_2CYCLE = !_2CYCLE))
			continue;

		_IFDEBUG debugTMS9918ACycle (cpu, i);

		// Read the graphics and draw the visible zone, if it is the case...
		// ...marks also whether the event have to be drawn or not!
		if (_raster.isInVisibleZone ())
		{
			// Collision detected?
			// The only way to delete the collision bit is to read the status info...
			bool cD = readGraphicInfoAndDrawVisibleZone (cpu); // It has to be executed anyway...
			_TMS99xxFamilyRegisters -> setSpriteCollisionDetected 
				(_TMS99xxFamilyRegisters -> spriteCollisionDetected () || cD);

			if (_showEvents)
				drawEvents ();
		}

		// If the starting position is reached...
		if (_raster.vData ().currentPositionAtBase0 () == 0 &&
			_raster.hData ().currentPositionAtBase0 () == 0)
		{
			// Set the bit 7 in the status register (INT)
			// This register is used to indicate that the screen has been updated...
			// The only way to clear that bit is to read the status register...
			_TMS99xxFamilyRegisters -> setSreenUpdateHappen ();
			// Set the bit 5 off in the status register
			// This register is used to indicate that a fifth register situation has been detected...
			// The only way this bit is clear is when there is no a fifth sprite detected!
			_TMS99xxFamilyRegisters -> setFifthSpriteDetected (false);
			_TMS99xxFamilyRegisters -> setFifthSpriteNotDrawn (0xff);
	
			// ...a notification to draw the screen is also generated...
			MCHEmul::GraphicalChip::notify (MCHEmul::Event (_GRAPHICSREADY));

			// Refresh the information about the sprites
			// The information won't change until the next frame...
			for (size_t i = 0; i < 32; i++) // The number of sprites...
				_spriteInfo [i] = TEXASINSTRUMENTS::TMS99xxFamily::SpriteInfo 
					(_TMS99xxFamilyRegisters -> readSpriteDefinition ((unsigned char) i));
		}

		// It may do a specific action at the position where the raster is currently...
		actionPerRasterLineAndCyle ();
		// ...before moving the raster one additional position...
		_raster.moveCycles (1);

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
	result.add ("Raster",					std::move (_raster.getInfoStructure ()));

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

// ---
void TEXASINSTRUMENTS::TMS99xxFamily::actionPerRasterLineAndCyle ()
{
	// This is done whatever the graphics mode is!
	// Even if the text mode i sturned on. The sprites won't be brawn in that case for sure...
	if (_raster.isInVisibleZone () &&
		_raster.hData ().currentPositionAtBase0 () == 0)
	{
		// Variables used in the loop...
		bool fF = false; // Fifth found?
		size_t n = 0; // Number of sprites visibles...
		unsigned short vL = _raster.vData ().currentVisiblePosition ();

		// Lets found the ones that are visible!
		// If there were a sprite with its position defined at line 208 or more
		// The system stops the process...
		size_t i = 0;
		unsigned short lB, uB;
		_raster.firstScreenPosition (lB, uB);
		for (;i < 32 /** 32 max. */ && 
			  (_spriteInfo [i]._definition._posY < 209 /* Until one after this position was found. */ ||
			   _spriteInfo [i]._definition._posY >= 223) &&
			  !fF /** or the fifth in the line was found. */; i++)
		{
			// If it is not visible, then contunue looking for the next one, if possible!
			// Otherwise the infomation about which pixel is the first obne visible is update...
			if (!_spriteInfo [i].actualizeInfoAtVisibleLine (vL, lB, uB))
				continue;

			// If it is visible, there can not be more than 4 max visible...
			// ...and if there were more, the system would stops and it wouldn't continue looking for more...
			if (n++ < 4) _spriteInfo [i]._visible = true;
			else 
			{
				_spriteInfo [i]._visible = false;
				_TMS99xxFamilyRegisters -> setFifthSpriteDetected (fF = true); // At this point n has to be less or equal to 4...
				_TMS99xxFamilyRegisters -> setFifthSpriteNotDrawn ((unsigned char) i); // The sprite that is not drawn...
			}
		}

		// The rest, if any, are not visible...
		for (; i < 32 /** 32 max. */; i++) 
			_spriteInfo [i]._visible = false;
	}
}

// --
bool TEXASINSTRUMENTS::TMS99xxFamily::readGraphicInfoAndDrawVisibleZone (MCHEmul::CPU* cpu)
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
		return (false); // No collision...

	// If the screen is in blank, 
	// ...the system doesn't continue either...
	if (_TMS99xxFamilyRegisters -> blankScreen ())
	{
		_IFDEBUG debugVideoNoActive ();

		return (false); // No collision...
	}

	// Draw the graphics and sprites and detect the collisions 
	// All that done depending on the graphical mode used too...
	unsigned short xS = 0, yS = 0;
	_raster.firstScreenPosition (xS, yS);
	return (drawGraphicsSpritesAndDetectCollisions (x, y, xS /** left border. */, yS /** up border. */,
		_TMS99xxFamilyRegisters -> readGraphicInfo 
			(x - _raster.hData ().firstScreenPosition (), // Positions within the screen part of the memory...
			 y - _raster.vData ().firstScreenPosition ()) /** From the video memory. */));
}

// ---
bool TEXASINSTRUMENTS::TMS99xxFamily::drawGraphicsSpritesAndDetectCollisions 
	(unsigned short x, unsigned short y, unsigned short xS, unsigned short yS,
	 const std::tuple <MCHEmul::UByte, MCHEmul::UByte, MCHEmul::UByte>& data)
{
	// Was there a collision between sprites?
	bool result = false;

	switch (_TMS99xxFamilyRegisters -> graphicMode ())
	{
		case TEXASINSTRUMENTS::TMS99xxFamilyRegisters::_GRAPHICIMODE:
			{
				drawGraphicsScreenGraphicsMode (x, y, data);

				result = drawSprites (x, y, xS, yS);
			}

			break;

		case TEXASINSTRUMENTS::TMS99xxFamilyRegisters::_TEXTMODE:
			{
				drawGraphicsScreenTextMode (x, y, data);

				// No sprites in this mode, so no collisions will be detected...
			}

			break;

		case TEXASINSTRUMENTS::TMS99xxFamilyRegisters::_MULTICOLORMODE:
			{
				drawGraphicsScreenMulticolorMode (x, y, data);

				result = drawSprites (x, y, xS, yS);
			}

			break;

		case TEXASINSTRUMENTS::TMS99xxFamilyRegisters::_GRAPHICIIMODE:
			{
				drawGraphicsScreenGraphicsMode (x, y, data);

				result = drawSprites (x, y, xS, yS);
			}

			break;

		case TEXASINSTRUMENTS::TMS99xxFamilyRegisters::_UNDOCUMENTED12:
			{
				// The character read will have been different...
				drawGraphicsScreenTextMode (x, y, data);

				// No sprites in this mode, so no collisions will be detected...
			}

			break;

		case TEXASINSTRUMENTS::TMS99xxFamilyRegisters::_UNDOCUMENTED23:
			{
				// The character read will have been different...
				drawGraphicsScreenMulticolorMode (x, y, data);

				result = drawSprites (x, y, xS, yS);
			}
			
			break;

		case TEXASINSTRUMENTS::TMS99xxFamilyRegisters::_UNDOCUMENTED13:
		case TEXASINSTRUMENTS::TMS99xxFamilyRegisters::_UNDOCUMENTED123:
			{
				// Here the character read, if any, makes no sense...
				drawGraphicsScreenFixPatternMode (x, y, data);

				// No sprites in this mode, so no collisions will be detected...
			}

			break;

		// It shouldn't happen, but if it does, then it is an error!
		default:
			{
				_LOG ("Graphic mode:" + 
					  std::to_string (_TMS99xxFamilyRegisters -> graphicMode ()) + " not implemented");

				assert (false);
			}

			break;
	}

	return (result);
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
	// The color to apply will depend on the group of 4 pixels...
	unsigned int cl = 0;
	unsigned short xS = x - _raster.hData ().firstScreenPosition ();
	if (((xS >> 2) % 1) == 0) cl = (unsigned int) ((std::get <2> (data).value () & 0xf0) >> 4); // even block...
	else cl = (unsigned int) (std::get <2> (data).value () & 0x0f); // odd block...
	if (cl != 0)
		_screenMemory -> setPixel (x, y, cl);
}

// ---
void TEXASINSTRUMENTS::TMS99xxFamily::drawGraphicsScreenFixPatternMode (unsigned short x, unsigned short y,
			const std::tuple <MCHEmul::UByte, MCHEmul::UByte, MCHEmul::UByte>& data)
{
	unsigned short xS = x - _raster.hData ().firstScreenPosition ();
	if (xS < 8 || xS >= 248 /** (40 * 6) + 8 first pixels = 248. */)
		_screenMemory -> setPixel (x, y, _TMS99xxFamilyRegisters -> backDropColor ());
	else
		_screenMemory -> setPixel (x, y, ((xS % 6) < 4)
			? _TMS99xxFamilyRegisters -> textColor () // 4 pixels of the same color. The value read doesn't matter...
			: _TMS99xxFamilyRegisters -> backDropColor ()); // The other two are the background color...
}

// ---
bool TEXASINSTRUMENTS::TMS99xxFamily::drawSprites 
	(unsigned short x, unsigned short y, unsigned short xS, unsigned short yS)
{
	// The have to be drawn in the reverse order, from 31 to 0...
	int nD = 0;
	for (int i = 31; i >= 0; i--)
	{
		if (_spriteInfo [i]._visible)
			if (drawSprite (x, y, xS, yS, (unsigned char) i))
				nD++;
	}

	// There were a collision in this point 
	// because there were more than one sprite drawn in the same position...
	return (nD > 1);
}

// ---
bool  TEXASINSTRUMENTS::TMS99xxFamily::drawSprite 
	(unsigned short x, unsigned short y, unsigned short xS, unsigned short yS, unsigned char nS)
{
	bool result = false;

	size_t dP = 0;
	if (_spriteInfo [nS]._definition.visibleAtVisiblePosition (x, xS, dP)) // In an independent line to load dP...
	{
		if ((result = _spriteInfo [nS]._bytes 
			[(_spriteInfo [nS]._definition._16pixels ? 1 : 0) - ((dP >> 3) /** 0 or 1 when 16 pixels on. */) 
				/** so 1 or 0 in same circunstances. */].bit (dP % 8)) && // If the pixel is to be drawn...
			_spriteInfo [nS]._definition._color != 0) // ...and the color is not 0 (transparent)...
			_screenMemory -> setPixel (x, y, _spriteInfo [nS]._definition._color); // The color 0 counts for collisions instead...

		// Draw a rectangle around the sprite if the events have to be highligthed...
		if (_showEvents)
		{
			unsigned short en = 
				(_spriteInfo [nS]._definition._enlarged ? 1 : 0);
			unsigned short d = 
				(_spriteInfo [nS]._definition._16pixels ? 1 : 0);
			unsigned short mP = 8 << (en + d); 
			int pY = int (((char) _spriteInfo [nS]._definition._posY >= 208) 
				? (char) _spriteInfo [nS]._definition._posY /** Here become negative. */ : _spriteInfo [nS]._definition._posY);
			int pX = (int) _spriteInfo [nS]._definition._posX - 
				(_spriteInfo [nS]._definition._earlyClock ? 32 : 0); // here it can be also negative...
			int vLN = (int) y - (int) yS - 1;
			int vPN = (int) x - (int) xS;
			if ((vLN == pY || vLN == ((pY + (int) mP) - 1)) ||
				(vPN == pX || vPN == ((pX + (int) mP) - 1))) 
				_screenMemory -> setPixel (x, y, 16);
		}
	}
	
	return (result);
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
