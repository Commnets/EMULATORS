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
				: _firstPosition (fp), _firstVisiblePosition (fvp), _firstDisplayPosition (fdp), 
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
	if (result = (cP >= (int)_maxPositions))
		while (cP >= (int)_maxPositions)
			cP -= (int)_maxPositions;

	cP += (int)_firstPosition;
	if (cP >= (int)_maxPositions)
		cP -= (int)_maxPositions;

	_currentPosition = (unsigned short)cP;
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
std::ostream& C64::operator << (std::ostream& o, const C64::VICII::RasterData& r)
{
	o << "Pos:" << r._currentPosition << "(" << r._currentPosition_0 
	  << "),F:" << r._firstPosition_0 << ",L:" << r._lastPosition_0;

	return (o);
}

// ---
std::ostream& C64::operator << (std::ostream & o, const C64::VICII::Raster & r)
{
	o << r._vRasterData << std::endl;
	o << r._hRasterData;

	return (o);
}

// ---
C64::VICII::VICII (const C64::VICII::RasterData& vd, const C64::VICII::RasterData& hd, 
		const MCHEmul::Attributes& attrs)
	: MCHEmul::GraphicalChip (_ID, attrs),
	  _VICIIRegisters (nullptr), 
	  _raster (vd, hd),
	  _lastCPUCycles (0),
	  _format (nullptr),
	  _graphicsCharCodeData (MCHEmul::UBytes::_E), 
	  _graphicsCharData (MCHEmul::UBytes::_E), 
	  _graphicsBitmapData (MCHEmul::UBytes::_E),
	  _graphicsColorData (MCHEmul::UBytes::_E),
	  _isNewRasterLine (false),
	  _lastVBlankEntered (false)
{
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
		_lastError = MCHEmul::_INIT_ERROR;

		return (false);
	}

	_raster.initialize ();

	_lastCPUCycles = 0;

	_graphicsCharCodeData = MCHEmul::UBytes::_E; 
	_graphicsCharData = MCHEmul::UBytes::_E;
	_graphicsBitmapData = MCHEmul::UBytes::_E;
	_graphicsColorData = MCHEmul::UBytes::_E;

	_isNewRasterLine = true; // The first...

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
		{ return (!_VICIIRegisters -> videoResetActive () && _raster.isInPotentialBadLine () && 
			((_raster.currentLine () - 0x03) & 0x07 /** The three last bits. */) == _VICIIRegisters -> verticalScrollPosition ()); };

	if (_VICIIRegisters -> vicIItoGenerateIRQ ())
		cpu -> interrupt (F6500::IRQInterrupt::_ID) -> setActive (true);

	// Rduce the visible zone if any... The info is passed to the raster!
	_raster.reduceDisplayZone
		(!_VICIIRegisters -> textDisplay25RowsActive (), !_VICIIRegisters -> textDisplay40ColumnsActive ());

	for (size_t i = (cpu -> clockCycles  () - _lastCPUCycles); i > 0 ; i--)
	{
		if (_isNewRasterLine)
		{
			if (isBadRasterLine () /** @see definition above. */)
			{
				// This is not exactly what VICII does, but it could be a good aproximation...
				// Not all graphic info is read actually at th beginning of a bad line, only video and color matrix...
				// The rest of the info is read in further cycles depending on the type of graphic mode
				readGraphicsInfo ();

				cpu -> addClockCycles (_CPUCYCLESWHENREADGRAPHS); // The cost of reading in terms of cycles...
																  // This will slow down for a while later the cpu, 
																  // that is what actually should happen!
			}

			if (_VICIIRegisters -> rasterIRQActive () && 
				_raster.currentLine () == _VICIIRegisters -> IRQRasterLineAt ())
			{
				_VICIIRegisters -> setRasterAtLine (true);

				cpu -> interrupt (F6500::IRQInterrupt::_ID) -> setActive (true);
			}

			_isNewRasterLine = false;
		}

		if (_raster.isInVisibleZone ())
		{
			// READ: Important variables
			// Where is the raster in the visible part of the screen? (starting from 0)
			unsigned short rv, cv; _raster.currentVisiblePosition (cv, rv);
			// Which is the horizontal closest block of 8 pixels (from left to the right)?
			// Take into account that the step - size of the raster is always 8 pixels per cycle..
			// It always starts at 0...
			unsigned short cav = (cv >> 3) << 3;

			// Draws the border...
			screenMemory () -> setHorizontalLine ((size_t) cav, (size_t) rv,
				(cav + 8) > _raster.visibleColumns () ? (_raster.visibleColumns () - cav) : 8, _VICIIRegisters -> borderColor ());

			if (_raster.isInDisplayZone () && // In the display...
				rv >= _raster.vData ().firstScreenPosition (/** false = real. */) && // It has to be within the visible lines...
				rv <= _raster.vData ().lastScreenPosition ( /** false = real. */) &&
				!_VICIIRegisters -> videoResetActive ()) // ...and obviously with the video not reset!
			{
				// Draws the back ground, taken ito account 
				// The additional border that could exist...
				unsigned short fpxl = 0; // First pixels from the cav to be drawn...
				unsigned short lbk = 8;  // Number of pixels to be drawn...
				// With the screen? 
				if (cav <= _raster.hData ().lastScreenPosition ())
				{
					if (_raster.hData ().firstScreenPosition (/** false = real. */) > cav)
						{ fpxl += (_raster.hData ().firstScreenPosition () - cav); lbk = 8 - fpxl; }
					if ((cav + 8) > _raster.hData ().lastScreenPosition (/** false = real. */))
						{ lbk = _raster.hData ().lastScreenPosition () - cav + 1; }
					screenMemory () -> setHorizontalLine (((size_t) cav + fpxl), (size_t) rv, lbk, _VICIIRegisters -> backgroundColor ());
				}

				// Draw the graphics, and the sprites...
				C64::VICII::DrawContext dC
					= { 
						_raster.hData ().firstScreenPosition (true),		// The original...
						_raster.hData ().firstScreenPosition (),			// And the real one
						_raster.hData ().lastScreenPosition (true),			// The original...
						_raster.hData ().lastScreenPosition (),				// And the real one
						_VICIIRegisters -> horizontalScrollPosition (),		// From 0 - 7 
						cv, cav,											// Where the horizontal raster is (also adjusted)
						_raster.vData ().firstScreenPosition (true),		// The original...
						_raster.vData ().firstScreenPosition (),			// And the real one
						_raster.vData ().lastScreenPosition (true),			// The original...
						_raster.vData ().lastScreenPosition (),				// And the real one
						_VICIIRegisters -> verticalScrollPosition (),		// From 0 - 7 (taken into account in bad lines)
						rv													// Where the vertical raster is...
					  };

				drawGraphics (dC);
				drawSprites (dC);
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

	unsigned short x1, y1, x2, y2;
	_raster.screenPositions (x1, y1, x2, y2);
	screenMemory () -> setHorizontalLine ((size_t) x1 - 1, (size_t) y1 - 1, (size_t) x2 - x1 + 3, 1);
	screenMemory () -> setHorizontalLine ((size_t) x1 - 1, (size_t) y2 + 1, (size_t) x2 - x1 + 3, 1);
	screenMemory () -> setVerticalLine ((size_t) x1 - 1, (size_t) y1 - 1, (size_t) y2 - y1 + 3, 1);
	screenMemory () -> setVerticalLine ((size_t) x2 + 1, (size_t) y1 - 1, (size_t) y2 - y1 + 3, 1);

	return (true);
}

// ---
std::ostream& C64::operator << (std::ostream& o, const C64::VICII& c)
{
	o << *c._VICIIRegisters << std::endl;
	o << c._raster;

	return (o);
}

// ---
void C64::VICII::readGraphicsInfo ()
{
	unsigned short graphLine = _raster.currentLine () - _raster._FIRSTBADLINE;
	unsigned short chrLine = graphLine >> 3;

	dynamic_cast <C64::Memory*> (memoryRef ()) -> setVICIIView ();

	_graphicsCharCodeData	= readCharCodeDataAt (chrLine);
	_graphicsCharData		= readCharDataFor (_graphicsCharCodeData);
	_graphicsBitmapData		= readBitmapDataAt (graphLine);
	_graphicsColorData		= readColorDataAt (chrLine);

	dynamic_cast <C64::Memory*> (memoryRef ()) -> setCPUView ();
}

// ---
void C64::VICII::drawGraphics (const C64::VICII::DrawContext& dC)
{
	// If no graphic has been loaded, it is not needed to continue...
	if (_graphicsColorData.size () == 0)
		return; // It could happen at the first lines of the screen when the vertical SCROLL is active...

	// The graphical column being involved...
	// The SCROLLX has been applied to the calculus, so it can be negative!
	// -7 - _GRAPHMAXBITMAPCOLUMNS
	int cb = (dC._RCA - dC._ICD) - dC._SC; 

	// The graphical line being involved...
	// The SCROLLY is included in the calculus!
	// The graphical data is loaded only in a bad line but "aligned" with the value of the SCROLLY
	// So, at this point, the line being rastered could still have graphical data from the previous block of 8 rastered lines...
	// 0 - _GRAPHMAXBITMAPROWS
	size_t r = ((dC._RR - dC._IRS) % 8); 
	r = (r >= dC._SR) ? (r - dC._SR) : (8 + r - dC._SR);

	switch (_VICIIRegisters -> graphicModeActive ())
	{
		case C64::VICIIRegisters::GraphicMode::_CHARMODE:
			drawMonoColorBytes (cb, r, _graphicsCharData /** The char definition */, _graphicsColorData, dC);
			break;

		case C64::VICIIRegisters::GraphicMode::_MULTICOLORCHARMODE:
			drawMonoColorBytes (cb, r, _graphicsCharData, _graphicsColorData, dC);
			break;
	
		case C64::VICIIRegisters::GraphicMode::_BITMAPMODE:
			drawMonoColorBytes (cb, r, _graphicsBitmapData /** The bimap definition. */, _graphicsColorData, dC);

		case C64::VICIIRegisters::GraphicMode::_MULTICOLORBITMAPMODE:
			drawMonoColorBytes (cb, r, _graphicsBitmapData, _graphicsColorData, dC);
			break;

		default:
			// Not graphic mode supported, yet...
			break;
	}
}

// ---
void C64::VICII::drawSprites (const C64::VICII::DrawContext& dC)
{
	// TODO
}

// ---
MCHEmul::ScreenMemory* C64::VICII::createScreenMemory ()
{
	unsigned int* cP = new unsigned int [16];
	cP [0]  = SDL_MapRGB (_format, 0x00, 0x00, 0x00);
	cP [1]  = SDL_MapRGB (_format, 0xff, 0xff, 0xff);
	cP [2]  = SDL_MapRGB (_format, 0xab, 0x31, 0x26);
	cP [3]  = SDL_MapRGB (_format, 0x66, 0xda, 0xff);
	cP [4]  = SDL_MapRGB (_format, 0xbb, 0x3f, 0xb8);
	cP [5]  = SDL_MapRGB (_format, 0x55, 0xce, 0x58);
	cP [6]  = SDL_MapRGB (_format, 0x1d, 0x0e, 0x97);
	cP [7]  = SDL_MapRGB (_format, 0xea, 0xf5, 0x7c);
	cP [8]  = SDL_MapRGB (_format, 0xb9, 0x74, 0x18);
	cP [9]  = SDL_MapRGB (_format, 0x78, 0x53, 0x00);
	cP [10] = SDL_MapRGB (_format, 0xdd, 0x93, 0x87);
	cP [11] = SDL_MapRGB (_format, 0x5b, 0x5b, 0x5b);
	cP [12] = SDL_MapRGB (_format, 0x8b, 0x8b, 0x8b);
	cP [13] = SDL_MapRGB (_format, 0xb0, 0xf4, 0xac);
	cP [14] = SDL_MapRGB (_format, 0xaa, 0x9d, 0xef);
	cP [15] = SDL_MapRGB (_format, 0xb8, 0xb8, 0xb8);

	return (new MCHEmul::ScreenMemory (_raster.visibleColumns (), _raster.visibleLines (), cP));
}

// ---
MCHEmul::UBytes C64::VICII::readCharDataFor (const MCHEmul::UBytes& chrs) const
{
	std::vector <MCHEmul::UByte> dt;

	for (const auto& i : chrs.bytes ())
	{
		std::vector <MCHEmul::UByte> chrDt = memoryRef () -> bytes 
			(_VICIIRegisters -> charDataMemory () /** The key. */ + ((size_t) i.value () << 3), 8);
		dt.insert (dt.end (), chrDt.begin (), chrDt.end ());
	}

	return (MCHEmul::UBytes (dt));
}

// ---
MCHEmul::UBytes C64::VICII::readBitmapDataAt (unsigned short l) const
{
	std::vector <MCHEmul::UByte> dt;

	unsigned short cL = l * _GRAPHMAXCHARCOLUMNS;
	for (unsigned short i = 0; i < _GRAPHMAXCHARCOLUMNS; i++)
	{
		std::vector <MCHEmul::UByte> btDt = 
			memoryRef () -> bytes (_VICIIRegisters -> bitmapMemory () + (cL + ((size_t) i << 3)), 8);
		dt.insert (dt.end (), btDt.begin (), btDt.end ());
	}

	return (MCHEmul::UBytes (dt));
}

// ---
MCHEmul::UBytes C64::VICII::readSpriteDataAt (unsigned short l) const
{
	// TODO

	return (MCHEmul::UBytes::_E);
}

// ---
void C64::VICII::drawMonoColorBytes (int cb, size_t r, 
	const MCHEmul::UBytes& bt, const MCHEmul::UBytes& clr, const C64::VICII::DrawContext& dC)
{
	for (int i = 0; i < 8 /** To paint always 8 pixels */; i++)
	{
		if ((cb + i) < 0)
			continue;

		size_t iBy = (size_t) ((cb + i) >> 3 /** To determine the byte. */);
		size_t iBt = 7 - (size_t) ((cb + i) % 8); /** From MSB to LSB. */
		unsigned short pos = dC._RCA + i;
		if (pos <= dC._LCS && bt [(iBy << 3) + r].bit (iBt))
			screenMemory () -> setPixel ((size_t) pos, (size_t) dC._RR, 
				(unsigned int) (clr [iBy].value () & 0x0f /** Useful nibble. */));
	}
}

// ---
void C64::VICII::drawMultiColorBytes (int cb, size_t r, 
	const MCHEmul::UBytes& bt, const MCHEmul::UBytes& clr, const C64::VICII::DrawContext& dC)
{
	for (unsigned short i = 0 ; i < 8; i += 2)
	{
		if ((cb + i) < 0)
			continue;

		size_t iBy = (size_t) ((cb + i) >> 3); 
		size_t iBt = 7 - (size_t) ((cb + i) % 8);
		unsigned char cs = (bt [(iBy << 3) + r].value () >> i) & 0x03; // 0, 1, 2 or 3...
		unsigned int fc = (unsigned int) ((cs == 3) 
			? clr [iBy].value () : _VICIIRegisters -> backgroundColor (cs)) & 0x0f /** Useful nibble. */;
		unsigned short pos = dC._RCA + i;
		if (pos <= dC._LCS)	screenMemory () -> setPixel ((size_t) pos, (size_t) dC._RR, fc);
		if ((pos + 1) <= dC._LCS) screenMemory () -> setPixel (((size_t) pos + 1), (size_t) dC._RR, fc);
	}
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
