#include <C64/VICII.hpp>
#include <C64/C64.hpp>
#include <F6500/incs.hpp>

const MCHEmul::Address C64::VICII::_COLORMEMORY ({ 0xd8, 0x00 });
const C64::VICII::RasterData C64::VICII_NTSC::_VRASTERDATA (27, 41, 51, 250, 12, 26, 262, 8);
const C64::VICII::RasterData C64::VICII_NTSC::_HRASTERDATA (412, 488, 22, 342, 388, 411, 511, 16);
const C64::VICII::RasterData C64::VICII_PAL::_VRASTERDATA (0, 16, 51, 250, 299, 311, 312, 8);
const C64::VICII::RasterData C64::VICII_PAL::_HRASTERDATA (404, 480, 19, 338, 380, 403, 502, 16);

// ---
void C64::VICII::RasterData::reduceDisplayZone (bool s)
{
	if (_displayZoneReducted == s)
		return; // If nothing changes, nothing to do...

	if (_displayZoneReducted = s)
	{
		_firstDisplayPosition	+= (_positionsToReduce >> 1);
		_lastDisplayPosition	-= (_positionsToReduce >> 1);
	}
	else
	{
		_firstDisplayPosition	-= (_positionsToReduce >> 1);
		_lastDisplayPosition	+= (_positionsToReduce >> 1);
	}
}

// ---
bool C64::VICII::RasterData::next ()
{
	bool result = false;

	_currentPosition++;

	if (_currentPosition >= _maxPositions)
		_currentPosition = 0;

	if (toBase0 (_currentPosition) == 0)
	{
		_currentPosition = _firstPosition;

		result = true;
	}

	return (result);
}

// ---
C64::VICII::VICII (const C64::VICII::RasterData& vd, const C64::VICII::RasterData& hd, 
		const MCHEmul::Attributes& attrs)
	: MCHEmul::GraphicalChip (_ID, attrs),
	  _raster (vd, hd),
	  _lastCPUCycles (0),
	  _format (nullptr),
	  _graphicsCharCodeData (MCHEmul::UBytes::_E), 
	  _graphicsCharData (MCHEmul::UBytes::_E), 
	  _graphicsBitmapData (MCHEmul::UBytes::_E),
	  _graphicsColorData (MCHEmul::UBytes::_E),
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
		dynamic_cast <C64::VICIIRegisters*> (memoryRef () -> block (C64::Commodore64::_VICREGS_MEMORY))))
	{
		_lastError = MCHEmul::_INIT_ERROR;

		return (false);
	}

	_VICIIRegisters -> initialize ();

	_raster.initialize ();

	_lastCPUCycles = 0;

	_graphicsCharCodeData = MCHEmul::UBytes::_E; 
	_graphicsCharData = MCHEmul::UBytes::_E;
	_graphicsBitmapData = MCHEmul::UBytes::_E;
	_graphicsColorData = MCHEmul::UBytes::_E;

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
			(_raster.currentLine () & 0x07 /** The three last bits. */) == _VICIIRegisters -> verticalScrollPosition ()); };

	if (_VICIIRegisters -> vicIItoGenerateIRQ ())
		cpu -> interrupt (F6500::IRQInterrupt::_ID) -> setActive (true);

	// Rduce the visible zone if any... The info is passed to the raster!
	_raster.reduceDisplayZone
		(!_VICIIRegisters -> textDisplay25RowsActive (), !_VICIIRegisters -> textDisplay40ColumnsActive ());

	for (size_t i = (cpu -> clockCycles  () - _lastCPUCycles); i > 0 ; i--)
	{
		if (_raster.isAtBeginningLine ())
		{
			if (isBadRasterLine () /** @see definition above. */)
			{
				// This is not exactly what VICII does, but it could be a good aproximation...
				// Not all graphic info is read actually at th beginning of a bad line, only video and color matrix...
				// The rest of the info is read in further cycles depending on the type of graphic mode
				readGraphicsInfo ();

				cpu -> addClockCycles (_CPUCYCLESWHENEADGRAPHS); // The cost of reading in terms of cycles...
																 // This will slow down for a while later the cpu, 
																 // that is what actually should happen!
			}

			if (_VICIIRegisters -> rasterIRQActive () && 
				_raster.currentLine () == _VICIIRegisters -> IRQRasterLineAt ())
			{
				_VICIIRegisters -> setRasterAtLine (true);

				cpu -> interrupt (F6500::IRQInterrupt::_ID) -> setActive (true);
			}
		}

		if (_raster.isInVisibleZone ())
		{
			unsigned short x,y;
			_raster.currentVisiblePosition (x, y);
			screenMemory () -> setPixel (x, y, _VICIIRegisters -> borderColor ());

			if (_raster.isInDisplayZone () && 
				!_VICIIRegisters -> videoResetActive ())
			{
				drawGraphics ();

				drawSprites ();
			}
		}

		_raster.next ();

		if (_raster.isInLastVBlank ())
		{
			if (!_lastVBlankEntered)
				setGraphicsReady (_lastVBlankEntered = true); // The limit of the visible screen has been reached!
		}
		else
			_lastVBlankEntered = false;
	}

	// It might have been incremented after reading graphics...
	_lastCPUCycles = cpu -> clockCycles (); 

	// To store back th info in the VIC Registers...
	_VICIIRegisters -> setCurrentRasterLine (_raster.currentLine ()); 

	return (true);
}

// ---
void C64::VICII::readGraphicsInfo ()
{
	unsigned short graphLine = _raster.currentLine () - _raster._FIRSTBADLINE;
	unsigned short chrLine = graphLine >> 3;

	_graphicsCharCodeData	= readCharCodeDataAt (chrLine);
	_graphicsCharData		= readCharDataFor (_graphicsCharCodeData);
	_graphicsBitmapData		= readBitmapDataAt (graphLine);
	_graphicsColorData		= readColorDataAt (chrLine);
}

// ---
void C64::VICII::drawGraphics ()
{
	switch (_VICIIRegisters -> graphicModeActive ())
	{
		case C64::VICIIRegisters::GraphicMode::_CHARMODE:
		case C64::VICIIRegisters::GraphicMode::_MULTICOLORCHARMODE:
			drawCharMode ();
			break;
	
		case C64::VICIIRegisters::GraphicMode::_BITMAPMODE:
		case C64::VICIIRegisters::GraphicMode::_MULTICOLORBITMAPMODE:
			drawBitMapMode ();
			break;
	
		default:
			// Graphic mode not supported...yet
			break;
	}
}

// ---
void C64::VICII::drawCharMode ()
{
}

// ---
void C64::VICII::drawBitMapMode ()
{
}

// ---
void C64::VICII::drawSprites ()
{
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

	for (auto i : chrs.values ())
	{
		std::vector <MCHEmul::UByte> chrDt = memoryRef () -> values 
			(_VICIIRegisters -> charMemory () /** The key. */ + (size_t) (i.value () << 3), 8).values ();
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
			memoryRef () -> values (_VICIIRegisters -> bitmapMemory () + (size_t) (cL + (i << 3)), 8).values ();
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
