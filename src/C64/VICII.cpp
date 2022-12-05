#include <C64/VICII.hpp>
#include <C64/C64.hpp>
#include <C64/Memory.hpp>
#include <F6500/incs.hpp>

// ---
const MCHEmul::Address C64::VICII::_COLORMEMORY ({ 0x00, 0xd8 }, false);
const C64::VICII::RasterData C64::VICII_NTSC::_VRASTERDATA (27, 41, 51, 250, 12, 26, 262, 4, 4);
const C64::VICII::RasterData C64::VICII_NTSC::_HRASTERDATA (412, 488, 24, 343, 388, 411, 512, 7, 9);
const C64::VICII::RasterData C64::VICII_PAL::_VRASTERDATA (0, 16, 51, 250, 299, 311, 312, 4, 4);
const C64::VICII::RasterData C64::VICII_PAL::_HRASTERDATA (404, 480, 24, 343, 380, 403, 504, 7, 9);

// ---
C64::VICII::RasterData::RasterData (
	unsigned short fp,
	unsigned short fvp,
	unsigned short fdp,
	unsigned short ldp,
	unsigned short lvp,
	unsigned short lp,
	unsigned short mp,
	unsigned short pr1,
	unsigned short pr2
					  )
				: MCHEmul::InfoClass ("RasterData"),
				  _firstPosition (fp), _firstVisiblePosition (fvp), _firstDisplayPosition (fdp), 
				  _lastDisplayPosition (ldp), _lastVisiblePosition (lvp), _lastPosition (lp),
				  _originalFirstDisplayPosition (fdp), _originalLastDisplayPosition (ldp),
				  _maxPositions (mp),
				  _positionsToReduce1 (pr1), _positionsToReduce2 (pr2),
				  _currentPosition (fp),
				  _displayZoneReducted (false)
{
	_firstPosition_0				= toBase0 (_firstPosition);
	_firstVisiblePosition_0			= toBase0 (_firstVisiblePosition);
	_firstDisplayPosition_0			= toBase0 (_firstDisplayPosition);
	_originalFirstDisplayPosition_0 = toBase0 (_firstDisplayPosition);
	_lastDisplayPosition_0			= toBase0 (_lastDisplayPosition);
	_originalLastDisplayPosition_0	= toBase0 (_lastDisplayPosition);
	_lastVisiblePosition_0			= toBase0 (_lastVisiblePosition);
	_lastPosition_0					= toBase0 (_lastPosition);

	_currentPosition_0				= toBase0 (_currentPosition_0);

	assert (_lastPosition_0 == (_maxPositions - 1));
}

// ---
bool C64::VICII::RasterData::add (unsigned short i)
{
	bool result = false;

	int cP = (int)_currentPosition_0;
	cP += i; // Can move to the next (o nexts) lines...
	if (result = (cP >= (int) _maxPositions))
		while (cP >= (int) _maxPositions)
			cP -= (int) _maxPositions;

	cP += (int) _firstPosition;
	if (cP >= (int) _maxPositions)
		cP -= (int) _maxPositions;

	_currentPosition = (unsigned short) cP;
	_currentPosition_0 = toBase0 (_currentPosition);

	return (result);
}

// ---
void C64::VICII::RasterData::reduceDisplayZone (bool s)
{
	if (_displayZoneReducted == s)
		return; // If nothing changes, nothing to do...

	if (_displayZoneReducted = s)
	{
		_firstDisplayPosition	+= _positionsToReduce1;
		_firstDisplayPosition_0	+= _positionsToReduce1;
		_lastDisplayPosition	-= _positionsToReduce2;
		_lastDisplayPosition_0	-= _positionsToReduce2;
	}
	else
	{
		_firstDisplayPosition	-= _positionsToReduce1;
		_firstDisplayPosition_0	-= _positionsToReduce1;
		_lastDisplayPosition	+= _positionsToReduce2;
		_lastDisplayPosition_0	+= _positionsToReduce2;
	}
}

// ---
MCHEmul::InfoStructure C64::VICII::RasterData::getInfoStructure () const
{
	MCHEmul::InfoStructure result;

	result.add ("POSITION",		_currentPosition);
	result.add ("POSITION0",	_currentPosition_0);
	result.add ("FIRST",		_firstPosition_0);
	result.add ("LAST",			_lastPosition_0);

	return (result);
}

// ---
MCHEmul::InfoStructure C64::VICII::Raster::getInfoStructure () const
{
	MCHEmul::InfoStructure result;

	result.add ("RasterX", _hRasterData.getInfoStructure ());
	result.add ("RasterY", _vRasterData.getInfoStructure ());

	return (result);
}

// ---
C64::VICII::VICII (const C64::VICII::RasterData& vd, const C64::VICII::RasterData& hd, 
		const MCHEmul::Attributes& attrs)
	: MCHEmul::GraphicalChip (_ID, attrs),
	  _VICIIRegisters (nullptr), 
	  _raster (vd, hd),
	  _drawBorder (false),
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
C64::VICII::~VICII ()
{
	SDL_FreeFormat (_format);
}

// ---
bool C64::VICII::initialize ()
{
	assert (memoryRef () != nullptr);

	if (!MCHEmul::GraphicalChip::initialize ())
		return (false);

	// Gets the memory block dedicated to the VICII
	if (!(_VICIIRegisters = 
		dynamic_cast <C64::VICIIRegisters*> (memoryRef () -> subset (C64::Memory::_VICREGS_SUBSET))))
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
	for (size_t i = 0; i < 8; _graphicsSprites [i++] = MCHEmul::UBytes::_E);

	_isNewRasterLine = true; // The first...
	_videoActive = true;

	_lastVBlankEntered = false;

	return (true);
}

// ---
bool C64::VICII::simulate (MCHEmul::CPU* cpu)
{
	// Notice that the bad line detection routine takes into account 
	// the value of the YSCROLL register as the graphics information to be shown 
	// is loaded at the beginning of every bad line...
	auto isBadRasterLine = [=]() -> bool
		{ return (_videoActive && _raster.isInPotentialBadLine () && 
			((_raster.currentLine () - 0x03) & 0x07 /** The three last bits. */) == _VICIIRegisters -> verticalScrollPosition ()); };

	if (_VICIIRegisters -> vicIItoGenerateIRQ ())
		cpu -> interrupt (F6500::IRQInterrupt::_ID) -> setActive (true);

	// Rduce the visible zone if any... The info is passed to the raster!
	_raster.reduceDisplayZone
		(!_VICIIRegisters -> textDisplay25RowsActive (), !_VICIIRegisters -> textDisplay40ColumnsActive ());

	for (size_t i = (cpu -> clockCycles  () - _lastCPUCycles); i > 0 ; i--)
	{
		_videoActive = (_raster.currentLine () == _raster._FIRSTBADLINE) 
			? !_VICIIRegisters -> videoResetActive () : _videoActive; // Only at first bad line it can change its value...

		if (_isNewRasterLine)
		{
			if (isBadRasterLine () /** @see definition above. */)
			{
				// This is not exactly what VICII does, but it could be a good aproximation...
				readGraphicsInfoAt (_raster.currentLine () - 
					_raster._FIRSTBADLINE - _VICIIRegisters -> verticalScrollPosition ());

				cpu -> addClockCycles (_CPUCYCLESWHENREADGRAPHS + 
					((unsigned int) _spritesEnabled.size () << 1)); // The cost of reading in terms of cycles...
																	// This will slow down for a while later the cpu, 
																	// that is what actually should happen!
			}

			// At the beginning of a new line, an interrupt could be generated...
			if (_VICIIRegisters -> rasterIRQActive () && 
				_raster.currentLine () == _VICIIRegisters -> IRQRasterLineAt ())
			{
				_VICIIRegisters -> setRasterAtLine (true); // The interrupt has been issued!

				cpu -> interrupt (F6500::IRQInterrupt::_ID) -> setActive (true);
			}

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
				(cav + 8) > _raster.visibleColumns () ? (_raster.visibleColumns () - cav) : 8, _VICIIRegisters -> borderColor ());

			// When the raster is in the display zone but in the screen vertical zone too
			// and for sure the vide is active, then everything has to happen!
			if (_raster.isInDisplayZone () && 
				_raster.vData ().isInScreenZone () &&
				_videoActive)
			{ 
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
					screenMemory () -> setHorizontalLine ((size_t) cav, (size_t) rv, lbk, 
						_VICIIRegisters -> backgroundColor ());
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
				setGraphicsReady (_lastVBlankEntered = true); // The limit of the visible screen has been reached!
															  // so it is time to actualize the graphics...
		}
		else
			_lastVBlankEntered = false;
	}

	// It might have been incremented after reading graphics...
	_lastCPUCycles = cpu -> clockCycles (); 

	// To store back the info in the VIC Registers...
	_VICIIRegisters -> setCurrentRasterLine (_raster.currentLine ()); 

	// Just to highlight (in black) the borders of the visible zone...
	if (_drawBorder)
	{
		unsigned short x1, y1, x2, y2;
		_raster.screenPositions (x1, y1, x2, y2);
		screenMemory () -> setHorizontalLine ((size_t) x1 - 1, (size_t) y1 - 1, (size_t) x2 - x1 + 3, 0);
		screenMemory () -> setHorizontalLine ((size_t) x1 - 1, (size_t) y2 + 1, (size_t) x2 - x1 + 3, 0);
		screenMemory () -> setVerticalLine ((size_t) x1 - 1, (size_t) y1 - 1, (size_t) y2 - y1 + 3, 0);
		screenMemory () -> setVerticalLine ((size_t) x2 + 1, (size_t) y1 - 1, (size_t) y2 - y1 + 3, 0);
	}

	return (true);
}

// ---
MCHEmul::InfoStructure C64::VICII::getInfoStructure () const
{
	MCHEmul::InfoStructure result = MCHEmul::GraphicalChip::getInfoStructure ();

	result.remove ("Memory"); // This info is not neccesary...
	result.add ("VICIIRegisters",	_VICIIRegisters -> getInfoStructure ());
	result.add ("Raster",			_raster.getInfoStructure ());

	return (result);
}

// ---
void C64::VICII::readGraphicsInfoAt (unsigned short gl)
{
	unsigned short chrLine = gl >> 3;
	dynamic_cast <C64::Memory*> (memoryRef ()) -> setVICIIView ();

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

	dynamic_cast <C64::Memory*> (memoryRef ()) -> setCPUView ();
}

// ---
void C64::VICII::drawGraphicsAndDetectCollisions (const C64::VICII::DrawContext& dC)
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
	// there are no reductions in the screen (display == scree) and SCROLL = 0x07,
	int rc = r - dC._SR;
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
		case C64::VICIIRegisters::GraphicMode::_CHARMODE:
			colGraphics = drawMonoColorChar (cb, rc, _graphicsCharData, _graphicsColorData, dC);
			break;

		case C64::VICIIRegisters::GraphicMode::_MULTICOLORCHARMODE:
			colGraphics = drawMultiColorChar (cb, rc, _graphicsCharData, _graphicsColorData, dC);
			break;

		case C64::VICIIRegisters::GraphicMode::_EXTENDEDBACKGROUNDMODE:
			colGraphics = drawMultiColorExtendedChar (cb, rc, _graphicsScreenCodeData, _graphicsCharData, _graphicsColorData, dC);
			break;

		case C64::VICIIRegisters::GraphicMode::_BITMAPMODE:
			colGraphics = drawMonoColorBitMap (cb, rc, _graphicsScreenCodeData, _graphicsBitmapData, dC);
			break;

		case C64::VICIIRegisters::GraphicMode::_MULTICOLORBITMAPMODE:
			colGraphics = drawMultiColorBitMap (cb, rc, _graphicsScreenCodeData, _graphicsBitmapData, _graphicsColorData, dC);
			break;

		case C64::VICIIRegisters::GraphicMode::_INVALIDTEXMODE:
			colGraphics = drawMultiColorChar (cb, rc, _graphicsCharData, _graphicsColorData, dC, true /** everything black. */);
			break;

		case C64::VICIIRegisters::GraphicMode::_INVALIDBITMAPMODE1:
			colGraphics = drawMonoColorBitMap (cb, rc, _graphicsScreenCodeData, _graphicsBitmapData, dC, true /** everything black. */);
			break;

		case C64::VICIIRegisters::GraphicMode::_INVALIDBITMAPMODE2:
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
		_VICIIRegisters -> setSpriteCollisionWithDataHappened 
			(_spritesEnabled [i], cGS |= (colSprites [_spritesEnabled [i]].value () & colGraphics.value ()) != 0x00);
	_VICIIRegisters -> setSpritesCollisionWithData (cGS);
	// ...and among sprites...
	bool cSS = false;
	for (size_t i = 0; i < _spritesEnabled.size (); i++)
		for (size_t j = i + 1; j < _spritesEnabled.size (); j++)
			_VICIIRegisters -> setSpriteCollision 
				(_spritesEnabled [i], 
				 cSS |= (colSprites [_spritesEnabled [i]].value () & colSprites [_spritesEnabled [j]].value ()) != 0x00);
	_VICIIRegisters -> setSpritesCollision (cSS);
}

// ---
MCHEmul::ScreenMemory* C64::VICII::createScreenMemory ()
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
	cP [8]  = SDL_MapRGB (_format, 0x67, 0x52, 0x00); // Brown
	cP [9]  = SDL_MapRGB (_format, 0xc3, 0x3d, 0x00); // Light Red
	cP [10] = SDL_MapRGB (_format, 0xc1, 0x81, 0x78); // Orange
	cP [11] = SDL_MapRGB (_format, 0x60, 0x60, 0x60); // Dark Grey
	cP [12] = SDL_MapRGB (_format, 0x8a, 0x8a, 0x8a); // Medium Grey
	cP [13] = SDL_MapRGB (_format, 0xb3, 0xec, 0x91); // Light Green
	cP [14] = SDL_MapRGB (_format, 0x86, 0x7a, 0xde); // Light Blue
	cP [15] = SDL_MapRGB (_format, 0xb3, 0xb3, 0xb3); // Light Grey

	return (new MCHEmul::ScreenMemory (_raster.visibleColumns (), _raster.visibleLines (), cP));
}

// ---
const MCHEmul::UBytes& C64::VICII::readCharDataFor (const MCHEmul::UBytes& chrs, bool eM) const
{
	std::vector <MCHEmul::UByte> dt;
	for (const auto& i : chrs.bytes ())
	{
		std::vector <MCHEmul::UByte> chrDt = memoryRef () -> bytes 
			(_VICIIRegisters -> charDataMemory () /** The key. */ + 
				(((size_t) i.value () & (eM ? 0x3f : 0xff)) 
					/** In the extended graphics mode there is only 64 chars possible. */ << 3), 8);
		dt.insert (dt.end (), chrDt.begin (), chrDt.end ());
	}

	return (_graphicsCharData = MCHEmul::UBytes (dt));
}

// ---
const MCHEmul::UBytes& C64::VICII::readBitmapDataAt (unsigned short l) const
{
	std::vector <MCHEmul::UByte> dt;
	unsigned short cL = l * _GRAPHMAXCHARCOLUMNS;
	for (unsigned short i = 0; i < _GRAPHMAXCHARCOLUMNS; i++)
	{
		std::vector <MCHEmul::UByte> btDt = 
			memoryRef () -> bytes (_VICIIRegisters -> bitmapMemory () + (cL + ((size_t) i << 3)), 8);
		dt.insert (dt.end (), btDt.begin (), btDt.end ());
	}

	return (_graphicsBitmapData = MCHEmul::UBytes (dt));
}

// ---
const std::vector <MCHEmul::UBytes>& C64::VICII::readSpriteData () const
{
	// The list of sprites enabled is used later to draw them
	// Only the sprite numbers in the list are then draw, and they are drawn in the order they are in this list
	// So the last one must be the one with the highest priority, and this is the number 0!
	_spritesEnabled = { }; // The list of the sprites enabled...

	MCHEmul::Address sP = _VICIIRegisters -> spritePointersMemory ();
	for (int /** can be negative. */ i = 7; i >= 0; i--)
	{ 
		if (_VICIIRegisters -> spriteEnable ((size_t) i)) // Read data only if the sprite is active...
		{ 
			_spritesEnabled.push_back ((size_t) i);
			_graphicsSprites [(size_t) i] = 
				MCHEmul::UBytes (memoryRef () -> bytes (_VICIIRegisters -> initAddressBank () + 
					((size_t) memoryRef () -> value (sP + (size_t) i).value () << 6 /** 64 blocks. */), 63 /** size in bytes. */));
		}
	}

	return (_graphicsSprites);
}

// ---
MCHEmul::UByte C64::VICII::drawMonoColorChar (int cb, int r,
	const MCHEmul::UBytes& bt, const MCHEmul::UBytes& clr, const C64::VICII::DrawContext& dC)
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
MCHEmul::UByte C64::VICII::drawMultiColorChar (int cb, int r,
	const MCHEmul::UBytes& bt, const MCHEmul::UBytes& clr, const C64::VICII::DrawContext& dC, bool blk)
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

		// The combinations 0x00 and 0x01 are considered as background!
		// So they coan not be used in collisions...
		if (cs == 0x10 || cs == 0x11)
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
MCHEmul::UByte C64::VICII::drawMultiColorExtendedChar (int cb, int r,
	const MCHEmul::UBytes& sc, const MCHEmul::UBytes& bt, const MCHEmul::UBytes& clr, const C64::VICII::DrawContext& dC)
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
		unsigned int cs = ((sc [iBy].value () & 0xc0) >> 6) & 0x03;
		unsigned short pos = dC._RCA + i;
		unsigned int fc = (bS ? clr [iBy].value () : _VICIIRegisters -> backgroundColor (cs)) & 0x0f /** useful nibble. */;
		if (pos >= dC._ICS && pos <= dC._LCS)
			screenMemory () -> setPixel ((size_t) pos, (size_t) dC._RR, fc);
	}

	return (result);
}

// ---
MCHEmul::UByte C64::VICII::drawMonoColorBitMap (int cb, int r,
	const MCHEmul::UBytes& sc, const MCHEmul::UBytes& bt, const C64::VICII::DrawContext& dC, bool blk)
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
MCHEmul::UByte C64::VICII::drawMultiColorBitMap (int cb, int r,
	const MCHEmul::UBytes& sc, const MCHEmul::UBytes& bt, const MCHEmul::UBytes& clr, const C64::VICII::DrawContext& dC, bool blk)
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

		// The combinations 0x00 and 0x01 are considered as background!
		// So they cannot be used in collisions...
		if (cs == 0x10 || cs == 0x11)
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
					: ((cs == 0x10) // The color is defined in the video matrix, low nibble...
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
MCHEmul::UByte C64::VICII::drawMonoColorSprite (int c, int r, size_t spr, const DrawContext& dC)
{
	MCHEmul::UByte result = MCHEmul::UByte::_0;

	// Horizontal info about the sprite
	unsigned short dW	= _VICIIRegisters -> spriteDoubleWidth (spr) ? 2 : 1;
	unsigned short x	= _VICIIRegisters -> spriteXCoord (spr);
	unsigned short wX	= 24 /** normal width in pixels. */ * dW;
	unsigned short dW8	= 8 * dW; // 8 or 16
	// Vertical info about the sprite
	unsigned short dH	= _VICIIRegisters -> spriteDoubleHeight (spr) ? 2 : 1;
	unsigned short y	= _VICIIRegisters -> spriteYCoord (spr);
	unsigned short wY	= 21 /** normal height in pixels. */ * dH;

	if (r < (int) y || r >= (int) (y + wY))
		return (result); // Not visible in the vertical zone...
	if ((c + 8 /** pixels */) < (int) x || c >= (int) (x + wX))
		return (result); // Not visible in the horizontal zone...

	for (unsigned int i = 0; 
			i < 8 /** always to draw 8 pixels */; i += dW /** the size of the pixels block. */)
	{
		size_t pp = (c + i);
		if (pp < (size_t) x)
			continue; // Not visible...
		if (pp >= (size_t) (x + wX))
			break; // No more draws...

		// To determine the initial byte (iBy) and bit (iBt) with the info about the sprite...
		// The bit moves from 7 to 0, and the byte increases...
		size_t iBy = ((r - y) / dH) * 3 /** 3 bytes per sprite row info. */ + ((pp - x) / dW8);
		size_t iBt = 7 - (((pp - x) % dW8) / dW);
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
MCHEmul::UByte C64::VICII::drawMultiColorSprite (int c, int r, size_t spr, const DrawContext& dC)
{
	MCHEmul::UByte result = MCHEmul::UByte::_0;

	// Horizontal info about the sprite
	unsigned short dW	= _VICIIRegisters -> spriteDoubleWidth (spr) ? 2 : 1;
	unsigned short x	= _VICIIRegisters -> spriteXCoord (spr);
	unsigned short wX	= 24 /** normal width in pixels. */ * dW;
	unsigned short dW8	= 8 * dW; // 8 or 16
	// Vertical info about the sprite
	unsigned short dH	= _VICIIRegisters -> spriteDoubleHeight (spr) ? 2 : 1;
	unsigned short y	= _VICIIRegisters -> spriteYCoord (spr);
	unsigned short wY	= 21 /** normal height in pixels. */ * dH;

	if (r < (int) y || r >= (int) (y + wY))
		return (result); // Not visible in the vertical zone...
	if ((c + 8 /** pixels */) < (int) x || c >= (int) (x + wX))
		return (result); // Not visible in the horizontal zone...

	for (unsigned int i = 0; i < 8 /** pixels. */; i += (2 * dW))
	{
		size_t pp = (c + i);
		if (pp < (size_t) x)
			continue; // Not visible...
		if (pp >= (size_t) (x + wX))
			break; // No more draws...

		// To determine the initial byte (iBy) and bit (iBt) with the info about the sprite...
		// The bit to select moves from 0 to 3, represeting the pair of bits (0,1), (2,3), (4,5), (6,7)
		size_t iBy = ((r - y) / dH) * 3 /** 3 bytes per sprite row info. */ + ((pp - x) / dW8);
		size_t iBt = 3 - (((pp - x) % dW8) / (2 * dW));
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
C64::VICII_NTSC::VICII_NTSC ()
	: C64::VICII (
		 _VRASTERDATA, _HRASTERDATA,
		 { { "Name", "VIC-II (NTSC) Video Chip Interface II" },
		   { "Code", "6567/8562/8564" },
		   { "Manufacturer", "MOS Technology INC/Commodore Semiconductor Group (CBM)"},
		   { "Year", "1980" } })
{
	// Nothing else to do...
}

// ---
C64::VICII_PAL::VICII_PAL ()
	: C64::VICII (
		 _VRASTERDATA, _HRASTERDATA,
		 { { "Name", "VIC-II (PAL) Video Chip Interface II" },
		   { "Code", "6569/8565/8566" },
		   { "Manufacturer", "MOS Technology INC/Commodore Semiconductor Group (CBM)"},
		   { "Year", "1980" } })
{
	// Nothing else to do...
}
