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
	  _graphicsCharData (MCHEmul::UBytes::_E), 
	  _graphicsBitmapData (MCHEmul::UBytes::_E),
	  _graphicsColorData (MCHEmul::UBytes::_E),
	  _graphicsSprites (8, MCHEmul::UBytes::_E),
	  _spritesEnabled (),
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
	_graphicsCharData = MCHEmul::UBytes::_E;
	_graphicsBitmapData = MCHEmul::UBytes::_E;
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

	for (size_t i = (cpu -> clockCycles  () - _lastCPUCycles); i > 0 ; i--)
	{
		_videoActive = (_raster.currentLine () == _FIRSTBADLINE) 
			? !_VICIIRegisters -> blankEntireScreen () : _videoActive; // Only at first bad line it can change its value...

		if (_isNewRasterLine)
		{
			if (isBadRasterLine () /** @see definition above. */)
			{
				// This is not exactly what VICII does, but it could be a good aproximation...
				readGraphicsInfoAt (_raster.currentLine () - 
					_FIRSTBADLINE - _VICIIRegisters -> verticalScrollPosition ());

				cpu -> addClockCycles (_CPUCYCLESWHENREADGRAPHS + 
					((unsigned int) _spritesEnabled.size () << 1)); // The cost of reading in terms of cycles...
																	// This will slow down for a while later the cpu, 
																	// that is what actually should happen!
			}

			// At the beginning of a new line, an interrupt could be generated...
			if (_raster.currentLine () == _VICIIRegisters -> IRQRasterLineAt ())
				_VICIIRegisters -> activateRasterAtLineIRQ ();

			_isNewRasterLine = false;
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
					(_raster.vData ().currentPositionAtBase0 () - 
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

				// Draw the graphics, including the sprites...
				// The method also detects the collisions!
				drawGraphicsAndDetectCollisions ({
					/** _ICD */ _raster.hData ().firstDisplayPosition (),		// DISLAY: The original...
					/** _ICS */ _raster.hData ().firstScreenPosition (),		// SCREEN: And the real one (after reduction size)
					/** _LCD */ _raster.hData ().lastDisplayPosition (),		// DISPLAY: The original...
					/** _LCS */ _raster.hData ().lastScreenPosition (),			// SCREEN: And the real one (after reduction size)
					/** _SC	 */ _VICIIRegisters -> horizontalScrollPosition (),	// From 0 - 7 
					/** _RC	 */ cv,												// Where the horizontal raster is (not adjusted to 8)
					/** _RCA */ cav,											// Where the horizontal raster is (adjusted to 8)
					/** _IRD */ _raster.vData ().firstDisplayPosition (),		// DISPLAY: The original... 
					/** _IRS */ _raster.vData ().firstScreenPosition (),		// SCREEN:  And the real one (after reduction size)
					/** _LRD */ _raster.vData ().lastDisplayPosition (),		// DISPLAY: The original...
					/** _LRS */ _raster.vData ().lastScreenPosition (),			// SCREEN: And the real one (after reduction size)
					/** _SR	 */ _VICIIRegisters -> verticalScrollPosition (),	// From 0 - 7 (taken into account in bad lines)
					/** _RR	 */ rv												// Where the vertical raster is...
					});
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
				notify (MCHEmul::Event (MCHEmul::Screen::_GRAPHICSREADY)); 
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
	if (_VICIIRegisters -> hasVICIIToGenerateIRQ () && 
		!cpu -> interrupt (F6500::IRQInterrupt::_ID) -> active ())
		cpu -> interrupt (F6500::IRQInterrupt::_ID) -> setActive (true);

	return (true);
}

// ---
MCHEmul::InfoStructure COMMODORE::VICII::getInfoStructure () const
{
	MCHEmul::InfoStructure result = MCHEmul::GraphicalChip::getInfoStructure ();

	result.remove ("Memory"); // This info is not neccesary...
	result.add ("VICIIRegisters",	_VICIIRegisters -> getInfoStructure ());
	result.add ("Raster",			_raster.getInfoStructure ());

	return (result);
}

// ---
void COMMODORE::VICII::readGraphicsInfoAt (unsigned short gl)
{
	unsigned short chrLine = gl >> 3;

	memoryRef () -> setActiveView (_VICIIView);

	// In real VIC II color is read at the same time than the graphics data
	// The color memory is always at the same location (only visible from VICII)
	readColorDataAt (chrLine);

	// Depending on the graphics mode either char data or bit data is loaded
	readScreenCodeDataAt (chrLine); // load _graphicsScreenCodeData...
	if (_VICIIRegisters -> textMode ()) 
		readCharDataFor (_graphicsScreenCodeData, _VICIIRegisters -> graphicExtendedColorTextModeActive ());
	else readBitmapDataAt (gl);

	// Only data for active sprites is read
	readSpriteData ();

	memoryRef () -> setCPUView ();
}

// ---
void COMMODORE::VICII::drawGraphicsAndDetectCollisions (const COMMODORE::VICII::DrawContext& dC)
{
	// If no graphic has been loaded, it is not needed to continue...
	if (_graphicsColorData.size () == 0)
		return; // It could happen at the first lines of the screen when the vertical SCROLL is active...

	// The graphical column being involved...
	int c = dC._RCA - dC._ICD;
	// In cb, the SCROLLX is involved, so it could be negative! starting from -7, 
	// When e.g. the raster is at the very first screen position, 
	// there are no reductions is the screen (display == screen) and SCROLLX = 0x07
	int cb = c - dC._SC;

	// The graphical line being involved...
	int r = dC._RR - dC._IRD;  
	// rc is the line with in the graphics cache to be drawn...
	// In rc, the SCROLLY is involved, so it could be also negative moving from -7,
	// When e.g. the raster is at the very first screen line,
	// there are no reductions in the screen (display == screen) and SCROLL = 0x07,
	// 0x03 is the difference between the FIRTBADLINE = 0x30 and the first visible line = 0x33 (with no reduction)
	int rc = r - dC._SR + 0x03; 
	// If negative, then no graphics has to be drawn...
	if (rc < 0)	return;
	// Otherwise the position to draw will be within one the 8 lines read...
	rc = (rc % 8);

	// The interim function to draw the sprites...
	std::vector <MCHEmul::UByte> colSprites (8, MCHEmul::UByte::_0);
	auto drawSprites = [&](bool f /** false if they don have priorit against the background. */) -> void
	{
		// The sprite 0 has the highest priority...
		for (const auto& i : _spritesEnabled)
		{
			if ((f && !_VICIIRegisters -> spriteToForegroundPriority (i)) ||
				(!f && _VICIIRegisters -> spriteToForegroundPriority (i)))
			{
				if (_VICIIRegisters -> spriteMulticolorMode (i)) colSprites [i] = drawMultiColorSprite (c, r, i, dC);
				else colSprites [i] = drawMonoColorSprite (c, r, i, dC);
			}
		}
	};

	drawSprites (false);

	MCHEmul::UByte colGraphics = MCHEmul::UByte::_0;
	switch (_VICIIRegisters -> graphicModeActive ())
	{
		case COMMODORE::VICIIRegisters::GraphicMode::_CHARMODE:
			colGraphics = drawMonoColorChar (cb, rc, _graphicsCharData, _graphicsColorData, dC);
			break;

		case COMMODORE::VICIIRegisters::GraphicMode::_MULTICOLORCHARMODE:
			colGraphics = drawMultiColorChar (cb, rc, _graphicsCharData, _graphicsColorData, dC);
			break;

		case COMMODORE::VICIIRegisters::GraphicMode::_EXTENDEDBACKGROUNDMODE:
			colGraphics = drawMultiColorExtendedChar (cb, rc, _graphicsScreenCodeData, _graphicsCharData, _graphicsColorData, dC);
			break;

		case COMMODORE::VICIIRegisters::GraphicMode::_BITMAPMODE:
			colGraphics = drawMonoColorBitMap (cb, rc, _graphicsScreenCodeData, _graphicsBitmapData, dC);
			break;

		case COMMODORE::VICIIRegisters::GraphicMode::_MULTICOLORBITMAPMODE:
			colGraphics = drawMultiColorBitMap (cb, rc, _graphicsScreenCodeData, _graphicsBitmapData, _graphicsColorData, dC);
			break;

		case COMMODORE::VICIIRegisters::GraphicMode::_INVALIDTEXMODE:
			colGraphics = drawMultiColorChar (cb, rc, _graphicsCharData, _graphicsColorData, dC, true /** everything black. */);
			break;

		case COMMODORE::VICIIRegisters::GraphicMode::_INVALIDBITMAPMODE1:
			colGraphics = drawMonoColorBitMap (cb, rc, _graphicsScreenCodeData, _graphicsBitmapData, dC, true /** everything black. */);
			break;

		case COMMODORE::VICIIRegisters::GraphicMode::_INVALIDBITMAPMODE2:
			colGraphics = drawMultiColorBitMap 
				(cb, rc, _graphicsScreenCodeData, _graphicsBitmapData, _graphicsColorData, dC, true /* everything black. */);
			break;

		default:
			assert (0); // Not possible...
			break;
	}

	drawSprites (true);

	// Now it is time to detect collisions...
	// First among the graphics and the sprites
	bool cGS = false;
	for (size_t i = 0; i < _spritesEnabled.size (); i++)
	{ 
		if ((colSprites [_spritesEnabled [i]].value () & 
			 colGraphics.value ()) != 0x00)
		{
			cGS = true;

			_VICIIRegisters -> setSpriteCollisionWithDataHappened (_spritesEnabled [i]);
		}
	}

	if (cGS) _VICIIRegisters -> activateSpritesCollisionWithDataIRQ ();
	
	// ...and among sprites...
	bool cSS = false;
	for (size_t i = 0; i < _spritesEnabled.size (); i++)
	{
		for (size_t j = i + 1; j < _spritesEnabled.size (); j++)
		{
			if ((colSprites [_spritesEnabled [i]].value () & 
				 colSprites [_spritesEnabled [j]].value ()) != 0x00)
			{ 
				cSS = true;

				_VICIIRegisters -> setSpriteCollision (_spritesEnabled [i]);
				_VICIIRegisters -> setSpriteCollision (_spritesEnabled [j]);
			}
		}
	}

	if (cSS) _VICIIRegisters -> activateSpritesCollisionIRQ ();
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

	return (new MCHEmul::ScreenMemory (_raster.visibleColumns (), _raster.visibleLines (), cP));
}

// ---
MCHEmul::UByte COMMODORE::VICII::drawMonoColorChar (int cb, int r,
	const MCHEmul::UBytes& bt, const MCHEmul::UBytes& clr, const COMMODORE::VICII::DrawContext& dC)
{
	MCHEmul::UByte result = MCHEmul::UByte::_0;

	for (int i = 0; i < 8 /** To paint always 8 pixels */; i++)
	{
		int pp = cb + i;
		if (pp < 0)
			continue;

		size_t iBy = ((size_t) pp) >> 3 /** To determine the byte. */;
		size_t iBt = 7 - (((size_t) pp) % 8); /** From MSB to LSB. */
		unsigned short pos = dC._RCA + i;
		if (bt [(iBy << 3) + (size_t) r].bit (iBt) && 
			(pos >= dC._ICS && pos <= dC._LCS))
		{ 
			result.setBit (7 - i, true);
			screenMemory () -> setPixel ((size_t) pos, (size_t) dC._RR, 
				(unsigned int) (clr [iBy].value () & 0x0f /** Useful nibble. */));
		}
	}

	return (result);
}

// ---
MCHEmul::UByte COMMODORE::VICII::drawMultiColorChar (int cb, int r,
	const MCHEmul::UBytes& bt, const MCHEmul::UBytes& clr, const COMMODORE::VICII::DrawContext& dC, bool blk)
{
	MCHEmul::UByte result = MCHEmul::UByte::_0;

	for (unsigned short i = 0 ; i < 8 /** To paint always 8 pixels but in block of 2. */; i += 2)
	{
		int pp = cb + i;
		if (pp < 0)
			continue;

		size_t iBy = ((size_t) pp) >> 3; 
		size_t iBt = 3 - ((((size_t) pp) % 8) >> 1);
		unsigned char cs = (bt [(iBy << 3) + (size_t) r].value () >> (iBt << 1)) & 0x03; // 0, 1, 2 or 3
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
			if ((cs & 0x02) == 0x02 /** if set. */ && (pos >= dC._ICS && pos < dC._LCS)) 
				screenMemory () -> setPixel ((size_t) pos, (size_t) dC._RR, fc);
			if ((cs & 0x01) == 0x01 /** if set. */ && ((pos + 1) >= dC._ICS && (pos + 1) < dC._LCS)) 
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
MCHEmul::UByte COMMODORE::VICII::drawMultiColorExtendedChar (int cb, int r,
	const MCHEmul::UBytes& sc, const MCHEmul::UBytes& bt, const MCHEmul::UBytes& clr, const COMMODORE::VICII::DrawContext& dC)
{
	MCHEmul::UByte result = MCHEmul::UByte::_0;

	for (int i = 0; i < 8 /** To paint always 8 pixels */; i++)
	{
		int pp = cb + i;
		if (pp < 0)
			continue;

		size_t iBy = ((size_t) pp) >> 3 /** To determine the byte. */;
		size_t iBt = 7 - (((size_t) pp) % 8); /** From MSB to LSB. */
		// The color of the pixel 0 is determined by the 2 MSBites of the char code...
		bool bS = bt [(iBy << 3) + (size_t) r].bit (iBt); // To know whether the bit is 1 or 0...
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
MCHEmul::UByte COMMODORE::VICII::drawMonoColorBitMap (int cb, int r,
	const MCHEmul::UBytes& sc, const MCHEmul::UBytes& bt, const COMMODORE::VICII::DrawContext& dC, bool blk)
{
	MCHEmul::UByte result = MCHEmul::UByte::_0;

	for (int i = 0; i < 8 /** To paint always 8 pixels. */; i++)
	{
		int pp = cb + i;
		if (pp < 0)
			continue;

		size_t iBy = ((size_t) pp) >> 3 /** To determine the byte. */;
		size_t iBt = 7 - (((size_t) pp) % 8); /** From MSB to LSB. */
		bool bS = bt [(iBy << 3) + (size_t) r].bit (iBt);
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
MCHEmul::UByte COMMODORE::VICII::drawMultiColorBitMap (int cb, int r,
	const MCHEmul::UBytes& sc, const MCHEmul::UBytes& bt, const MCHEmul::UBytes& clr, const COMMODORE::VICII::DrawContext& dC, bool blk)
{
	MCHEmul::UByte result = MCHEmul::UByte::_0;

	for (unsigned short i = 0 ; i < 8 /** To paint always 8 pixels but in block of 2. */; i += 2)
	{
		int pp = cb + i;
		if (pp < 0)
			continue;

		size_t iBy = ((size_t) pp) >> 3; 
		size_t iBt = 3 - ((((size_t) pp) % 8) >> 1);
		unsigned char cs = (bt [(iBy << 3) + (size_t) r].value () >> (iBt << 1)) & 0x03; // 0, 1, 2 or 3
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
						? (sc [iBy].value () & 0x0f)
						: clr [iBy].value () & 0x0f); // The color is defined in color matrix...
		if (pos >= dC._ICS && pos <= dC._LCS) 
			screenMemory () -> setPixel ((size_t) pos, (size_t) dC._RR, fc);
		if ((pos + 1) >= dC._ICS && (pos + 1) <= dC._LCS) 
			screenMemory () -> setPixel (((size_t) pos + 1), (size_t) dC._RR, fc);
	}

	return (result);
}

// ---
MCHEmul::UByte COMMODORE::VICII::drawMonoColorSprite (int c, int r, size_t spr, const DrawContext& dC)
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
		size_t iBy = (size_t) (((r - rY) / (int) dH) * 3 /** 3 bytes per sprite row info. */ + ((pp - cX) / (int) dW8));
		size_t iBt = (size_t) (7 - (((pp - cX) % (int) dW8) / (int) dW));
		bool dP = _graphicsSprites [spr][iBy].bit (iBt);
		if (!dP)
			continue; // The point is not visible...

		unsigned short pos = dC._RCA + i;
		for (size_t j = 0; j < (size_t) dW; j++)
		{
			if ((pos + j) >= dC._ICS && (pos + j) <= dC._LCS)
			{ 
				result.setBit (7 - (i + j), true);
				screenMemory () -> setPixel ((size_t) (pos + j), (size_t) dC._RR, 
					_VICIIRegisters -> spriteColor (spr) & 0x0f /** useful nibble. */);
			}
		}
	}

	return (result);
}

// ---
MCHEmul::UByte COMMODORE::VICII::drawMultiColorSprite (int c, int r, size_t spr, const DrawContext& dC)
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
		size_t iBy = (size_t) (((r - rY) / (int) dH) * 3 /** 3 bytes per sprite row info. */ + ((pp - cX) / (int) dW8));
		size_t iBt = (size_t) (3 - (((pp - cX) % (int) dW8) / (2 * (int) dW)));
		unsigned char cs = (_graphicsSprites [spr][iBy].value () >> (iBt << 1)) & 0x03;
		if (cs == 0)
			continue; // The point has no color...

		unsigned int fc = (unsigned int) ((cs == 0x01) 
			? _VICIIRegisters -> spriteSharedColor (0) 
			: ((cs == 0x02) ? _VICIIRegisters -> spriteColor (spr) : _VICIIRegisters -> spriteSharedColor (1)));
		unsigned short pos = dC._RCA + i;
		for (size_t j = 0; j < (size_t) (2 * dW); j++)
		{
			if ((pos + j) >= dC._ICS && (pos + j) <= dC._LCS)
			{
				result.setBit (7 - (i + j), true); result.setBit (6 - (i + j), true);
				screenMemory () -> setPixel ((size_t) (pos + j), (size_t) dC._RR, fc);
			}
		}
	}

	return (result);
}

// ---
void COMMODORE::VICII::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	// To set the bank...
	if (evnt.id () >= _BANK0SET && evnt.id () <= _BANK3SET)
		setBank (evnt.id () - _BANK0SET);
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
