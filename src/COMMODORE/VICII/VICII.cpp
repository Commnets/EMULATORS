#include <COMMODORE/VICII/VICII.hpp>
#include <F6500/IRQInterrupt.hpp>

// ---
const MCHEmul::Address COMMODORE::VICII::_COLORMEMORY ({ 0x00, 0xd8 }, false);
const MCHEmul::RasterData COMMODORE::VICII_PAL::_VRASTERDATA (0, 16, 51, 250, 299, 311, 312, 4, 4);
const MCHEmul::RasterData COMMODORE::VICII_PAL::_HRASTERDATA 
	(404, 480, 24, 343, 380, 403, 504 /** For everyting to run, it has to be divisible by 8. */, 7, 9);
const MCHEmul::RasterData COMMODORE::VICII_NTSC::_VRASTERDATA (27, 41, 51, 250, 12, 26, 262, 4, 4);
const MCHEmul::RasterData COMMODORE::VICII_NTSC::_HRASTERDATA 
	(412, 488, 24, 343, 388, 411, 512 /** For everything to run, it has to be divisible by 8. */, 7, 9);
// This two positions are fized...
const MCHEmul::Address COMMODORE::VICII::_MEMORYPOSIDLE1 = MCHEmul::Address ({ 0xff, 0x39 }, false);
const MCHEmul::Address COMMODORE::VICII::_MEMORYPOSIDLE2 = MCHEmul::Address ({ 0xff, 0x3f }, false);

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
	  _incCyclesPerRasterLine (cRL - COMMODORE::VICII_PAL::_CYCLESPERRASTERLINE),
	  _raster (vd, hd, 8 /** step. */),
	  _lastCPUCycles (0),
	  _format (nullptr),
	  _cycleInRasterLine (1),
	  _videoActive (true),
	  _lastVBlankEntered (false),
	  _lastBadLineScrollY (-1), _newBadLineCondition (false), _badLineStopCyclesAdded (false),
	  _vicGraphicInfo (),
	  _vicSpriteInfo ()
{
	assert (_cyclesPerRasterLine >= 63);

	setClassName ("VICII");

	_format = SDL_AllocFormat (SDL_PIXELFORMAT_ARGB8888);

	for (size_t i = 0; i < 8; _vicSpriteInfo [i++] = VICSpriteInfo ());
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
	
	_cycleInRasterLine = 1;

	_videoActive = true;

	_lastVBlankEntered = false;

	_lastBadLineScrollY = -1;
	_newBadLineCondition = false;
	_badLineStopCyclesAdded = false;

	_vicGraphicInfo = VICGraphicInfo ();
	for (size_t i = 0; i < 8; _vicSpriteInfo [i++] = VICSpriteInfo ());

	return (true);
}

// ---
bool COMMODORE::VICII::simulate (MCHEmul::CPU* cpu)
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
				_VICIIRegisters -> verticalScrollPosition () && // aligned with the scrollY
			_lastBadLineScrollY != (int) _VICIIRegisters -> verticalScrollPosition (); //..and obvious if that situation in the scroll changed
		
		if (result)
			_lastBadLineScrollY = _VICIIRegisters -> verticalScrollPosition ();

		return (result);
	};

	auto isAboutToReadSpriteInfo = [&]() -> bool
	{
		return (
			(_cycleInRasterLine == 2								&& _vicSpriteInfo [5]._active) ||
			(_cycleInRasterLine == 4								&& _vicSpriteInfo [6]._active) ||
			(_cycleInRasterLine == 6								&& _vicSpriteInfo [7]._active) ||
			(_cycleInRasterLine == (55 + _incCyclesPerRasterLine)	&& _vicSpriteInfo [0]._active) ||
			(_cycleInRasterLine == (57 + _incCyclesPerRasterLine)	&& _vicSpriteInfo [1]._active) ||
			(_cycleInRasterLine == (59 + _incCyclesPerRasterLine)	&& _vicSpriteInfo [2]._active) ||
			(_cycleInRasterLine == (61 + _incCyclesPerRasterLine)	&& _vicSpriteInfo [3]._active) ||
			(_cycleInRasterLine == (63 + _incCyclesPerRasterLine)	&& _vicSpriteInfo [4]._active));
	};

	// If the "video reset flag" is actived nothing is done...
	if (_VICIIRegisters -> videoResetActive ())
		return (true);

	// Adapt the size of the display zone to the parameters specificied in the register...
	// The zone where sprites and texts are finally visible is call the "screen zone"
	_raster.reduceDisplayZone
		(!_VICIIRegisters -> textDisplay25RowsActive (), !_VICIIRegisters -> textDisplay40ColumnsActive ());

	// The simulation has to be repeated as many time as cycles have spent since the last invocation...
	for (unsigned int i = (cpu -> clockCycles  () - _lastCPUCycles); i > 0; i--)
	{
		if (deepDebugActive ())
		{
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
				<< std::to_string ((int) _VICIIRegisters -> graphicModeActive ())
				<< ", Memory:["
				<< "Bk=" << std::to_string ((int) _VICIIRegisters -> bank ()) << ","
				<< "SM=$" << MCHEmul::removeAll0 (_VICIIRegisters -> screenMemory ().asString
					(MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) << ","
				<< "CM=$" << MCHEmul::removeAll0 (_VICIIRegisters -> charDataMemory ().asString
					(MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) << ","
				<< "BM=$" << MCHEmul::removeAll0 (_VICIIRegisters -> bitmapMemory ().asString
					(MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) << "]\n";

			// Draws lines where there is a IRQ interruption...
			unsigned short lrt = 
				_raster.lineInVisibleZone (_VICIIRegisters -> IRQRasterLineAt ());
			if (lrt <= _raster.vData ().lastVisiblePosition ())
			{
				unsigned char clrt = _VICIIRegisters -> backgroundColor () == 15 
					? 0 : _VICIIRegisters -> backgroundColor () + 1; /** to be visible. */
				screenMemory () -> setHorizontalLine (0, lrt, _raster.visibleColumns (), clrt);
			}
		}

		// Whether the video is active or not is only checked at the very first bad line...
		_videoActive = (_raster.currentLine () == _FIRSTBADLINE) 
			? !_VICIIRegisters -> blankEntireScreen () : _videoActive; // The value can change at any cycle of this first bad line...

		// Is there a bad line situation?
		// A bad line can happen at any cycle within the raster line.
		// When happened, the graphics are read and the situation is latched because maybe additional stop cycles could be needed.
		if (isNewBadLine ())
		{
			if (deepDebugActive ())
				*_deepDebugFile << "\t\t\t\tBad line situation\n";

			_newBadLineCondition = true;		// latched...
			_badLineStopCyclesAdded = false;	// ...the cycles have to be added...

			_vicGraphicInfo._idleState = false; // No longer in "idle" state but in the "screen" one!
		}

		// When VICII is about to read sprites or graphics info (bad line),
		// the CPU has to stop 3 cycles in advance (just for READ activities) for those activities,
		// unless it was stopped previously and that stop situation were still valid...
		// In the case of graphics that stop only happens when the situation arise in the "screen cycles" (40)
		if (!cpu -> stopped () && 
			(isAboutToReadSpriteInfo () || 
				(_newBadLineCondition && (_cycleInRasterLine >= 12 && _cycleInRasterLine < 52))))
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
				_vicGraphicInfo._VCBASE = _vicGraphicInfo._VC = 0;
				_vicGraphicInfo._RC = 0;
			}
			// In any other line number, VC start back to count from the value in VCBASE.
			// VCBASE is actualized only then RC reaches 8. @see rasterCycle 58 treatment.
			else 
				_vicGraphicInfo._VC = _vicGraphicInfo._VCBASE;

			// If the current line is where a IR has been set...
			if (_raster.currentLine () == _VICIIRegisters -> IRQRasterLineAt ())
				_VICIIRegisters -> activateRasterIRQ (); // ...the interrupt is activated (but not necessary launched!)
		}

		// Per cycle, the IRQ condition is checked! 
		// (many reasons during the cycle can unchain the IRQ interrupt)
		int cI = -1;
		if ((cI = (int)_VICIIRegisters -> reasonIRQCode ()) != 0)
			cpu -> requestInterrupt (
				F6500::IRQInterrupt::_ID, 
				cpu -> clockCycles  () - i, 
				this,
				cI);
	}

	// When the raster enters the non visible part of the screen,
	// a notification is sent (to the Screen class usually) 
	// just to draw the screen...
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

	_VICIIRegisters -> setCurrentRasterLine (_raster.currentLine ()); 

	_lastCPUCycles = cpu -> clockCycles ();

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
MCHEmul::UBytes COMMODORE::VICII::screenMemorySnapShot (MCHEmul::CPU* cpu) const
{ 	
	// Usually in this point the active view should be the CPU one, 
	// but just in case, it is checked and guranteed....
	// Same in the rest
	int aVID = cpu -> memoryRef () -> activeView () -> id ();
	if (aVID != _VICIIView)
		cpu -> memoryRef () -> setActiveView (_VICIIView);

	MCHEmul::UBytes result = cpu -> memoryRef () -> values 
		(_VICIIRegisters -> screenMemory (), 0x03e8 /** 1000 positions = 40 x 25. */);

	cpu -> memoryRef () -> setActiveView (aVID);

	return (result);
}

// ---
MCHEmul::UBytes COMMODORE::VICII::colorMemorySnapShot (MCHEmul::CPU* cpu) const
{ 
	int aVID = cpu -> memoryRef () -> activeView () -> id ();
	if (aVID != _VICIIView)
		cpu -> memoryRef () -> setActiveView (_VICIIView);

	MCHEmul::UBytes result = cpu -> memoryRef () -> values 
		(_COLORMEMORY, 0x03e8 /** 1000 positions = 40 x 25. */); 

	cpu -> memoryRef () -> setActiveView (aVID);

	return (result);
}

// ---
MCHEmul::UBytes COMMODORE::VICII::bitmapMemorySnapShot (MCHEmul::CPU* cpu) const
{ 
	int aVID = cpu -> memoryRef () -> activeView () -> id ();
	if (aVID != _VICIIView)
		cpu -> memoryRef () -> setActiveView (_VICIIView);

	MCHEmul::UBytes result = cpu -> memoryRef () -> values 
		(_VICIIRegisters -> bitmapMemory (), 0x1f40 /** 8000 positions = 40 x 25 x 8. */); 

	cpu -> memoryRef () -> setActiveView (aVID);

	return (result);
}

// ---
MCHEmul::UBytes COMMODORE::VICII::spritesMemorySnapShot (MCHEmul::CPU* cpu, 
	const std::vector <size_t>& sprs) const
{
	int aVID = cpu -> memoryRef () -> activeView () -> id ();
	if (aVID != _VICIIView)
		cpu -> memoryRef () -> setActiveView (_VICIIView);

	std::vector <MCHEmul::UByte> result;
	for (size_t i = 0; i < 8; i++)
	{
		if (!sprs.empty () && 
			std::find (sprs.begin (), sprs.end (), (i + 1)) == sprs.end ())
			continue;

		MCHEmul::UBytes sprDt = cpu -> memoryRef () -> values (_VICIIRegisters -> initAddressBank () + 
			((size_t) memoryRef () -> value (_VICIIRegisters -> spritePointersMemory () + i).value () << 6), 0x3f);
		result.insert (result.end (), sprDt.bytes ().begin (), sprDt.bytes ().end ());
	}

	cpu -> memoryRef () -> setActiveView (aVID);

	return (MCHEmul::UBytes (result));
}

// ---
MCHEmul::Strings COMMODORE::VICII::spritesDrawSnapshot (MCHEmul::CPU* cpu, 
	const std::vector <size_t>& sprs) const
{
	int aVID = cpu -> memoryRef () -> activeView () -> id ();
	if (aVID != _VICIIView)
		cpu -> memoryRef () -> setActiveView (_VICIIView);

	MCHEmul::Strings result;
	for (size_t i = 0; i < 8; i++)
	{
		if (!sprs.empty () && 
			std::find (sprs.begin (), sprs.end (), (i + 1)) == sprs.end ())
			continue;

		MCHEmul::Address sprAdd = _VICIIRegisters -> initAddressBank () + 
			((size_t) memoryRef () -> value (_VICIIRegisters -> spritePointersMemory () + i).value () << 6);
		std::string dt = std::to_string (i + 1) + "---\n$" +
			MCHEmul::removeAll0 (sprAdd.asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) + "\n";
		MCHEmul::UBytes sprDt = cpu -> memoryRef () -> values (sprAdd, 0x3f);
		for (size_t j = 0; j < 21; j++) // 21 lines per sprite...
		{
			if (j != 0)
				dt += "\n";

			for (size_t k = 0; k < 3; k++) // 3 bytes per line...
			{
				if (_VICIIRegisters -> spriteMulticolorMode (i)) // Can be monocolor or multicolor...
				{
					for (size_t l = 0; l < 8; l += 2)
					{
						switch ((sprDt [(j * 3) + k].value () & (0x03 << (6 - l))) >> (6 - l))
						{
							case 0x00: dt += " "; break;
							case 0x01: dt += "O"; break;
							case 0x02: dt += "X"; break;
							case 0x03: dt += "#"; break;
							default: assert (false); break; // It should be here, but just in case...
						}
					}
				}
				else
					for (size_t l = 0; l < 8; l++)
						dt += ((sprDt [(j * 3) + k].value () & (1 << (7 - l))) != 0x00) ? "#" : " ";
			}
		}

		result.emplace_back (std::move (dt));
	}

	result.emplace_back ("---");

	cpu -> memoryRef () -> setActiveView (aVID);

	return (result);
}

// ---
MCHEmul::Strings COMMODORE::VICII::charsDrawSnapshot (MCHEmul::CPU* cpu,
	const std::vector <size_t>& chrs) const
{
	int aVID = cpu -> memoryRef () -> activeView () -> id ();
	if (aVID != _VICIIView)
		cpu -> memoryRef () -> setActiveView (_VICIIView);

	MCHEmul::Strings result;
	for (size_t i = 0; i < 256; i++)
	{
		if (!chrs.empty () && 
			std::find (chrs.begin (), chrs.end (), i) == chrs.end ())
			continue;

		MCHEmul::Address chrAdd = _VICIIRegisters -> charDataMemory () + (i << 3);
		std::string dt = std::to_string (i) + "---\n$" +
			MCHEmul::removeAll0 (chrAdd.asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) + "\n";
		MCHEmul::UBytes chrDt = cpu -> memoryRef () -> values (chrAdd, 0x08);
		for (size_t j = 0; j < 8; j++) // 8 lines per character...
		{
			if (j != 0)
				dt += "\n";

			if (_VICIIRegisters -> graphicMulticolorTextModeActive ())
			{
				for (size_t l = 0; l < 8; l += 2)
				{ 
					switch ((chrDt [j].value () & (0x03 << (6 - l))) >> (6 - l))
					{
						case 0x00: dt += " "; break;
						case 0x01: dt += "O"; break;
						case 0x02: dt += "X"; break;
						case 0x03: dt += "#"; break;
						default: assert (false); break; // It should be here, but just in case...
					}
				}
			}
			else // Including extended (in terms of definition)...
			{
				for (size_t l = 0; l < 8; l++)
					dt += ((chrDt [j].value () & (1 << (7 - l))) != 0x00) ? "#" : " ";
			}
		}

		result.emplace_back (std::move (dt));
	}

	result.emplace_back ("---");

	cpu -> memoryRef () -> setActiveView (aVID);

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
unsigned int COMMODORE::VICII::treatRasterCycle ()
{
	unsigned int result = 0;

	// Read graphics zone?
	bool rG = false;
	switch (_cycleInRasterLine)
	{
		// Read the sprite 3 to 7 data?
		case 1:
		case 3:
		case 5:
		case 7:
		case 9:
			{
				// Is there sprite info avaliable?
				unsigned short nSR = (((_cycleInRasterLine - 1) >> 1) + 3); // 3, 4, 5, 6 or 7
				if (readSpriteData (nSR))
				{ 
					result = 2;	// VICII has to read three bytes, meaning 2 clock cycles stopped more... 

					if (deepDebugActive ())
						*_deepDebugFile
							<< "\t\t\t\tReading info sprite:" << std::to_string (nSR + 1)
							<< " [" << _vicSpriteInfo [nSR]._graphicsLineSprites.asString 
								(MCHEmul::UByte::OutputFormat::_HEXA, ' ') << "]"
							<< "\n";
				}
			}

			break;

		// In raster cycle 14 the graphics information moves...
		case 14:
			{
				_vicGraphicInfo._VC = _vicGraphicInfo._VCBASE;
				_vicGraphicInfo._VLMI = 0;

				if (_newBadLineCondition)
					_vicGraphicInfo._RC = 0;
			}

			break;

		// Between the raster cycle 15 and 55 the graphics are read...
		// ...But in the cycle 15 also...
		// After reading info of the sprites, cycles 58 - 62 (previos raster line) + 1 - 9 (this line),
		// the information about the active sprites is actualized...
		case 15:
			{
				for (size_t i = 0; i < 8; i++)
				{
					if (_vicSpriteInfo [i]._active &&
						((_vicSpriteInfo [i]._expansionY && 
							(!(_vicSpriteInfo [i]._ff = !_vicSpriteInfo [i]._ff))) || 
							// Note that when expansion is on, the line attribute is incremented very two raster lines...
						(!_vicSpriteInfo [i]._expansionY)))
					{ 
						// When the line treated of the sprite is the last one, the sprite is desactivated...
						if (_vicSpriteInfo [i]._line++ == 21)
							_vicSpriteInfo [i] = VICSpriteInfo (); // Starts back...
					}
				}
			}

			break;

		// Just read the graphics...
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
		case 48:
		case 49:
		case 50:
		case 51:
		// 52 is a little bit special...
		case 53:
		case 54:
		case 55:
			{
				rG = true;
			}

			break;

		// The raster cycle 55/56 (we have choosen 55) is very important for sprite behaviour.
		// At this point the active sprites are defined.
		// Notice that this identification is done before two events:
		// The VICII to decide whether it has or not to stop 3 cycles because sprite info is going to be read (first sprite at cycle 55),
		// and the sprite info is actually read (first sprite at cycle 58)
		// That the reason to choose 52, when in the real VICII this happen at cycles 55/56.
		case 52:
			{
				for (size_t i = 0; i < 8; i++)
				{
					if (_VICIIRegisters -> spriteEnable (i) &&
						_raster.currentLine () == (unsigned short) _VICIIRegisters -> spriteYCoord (i))
						_vicSpriteInfo [i] = VICSpriteInfo (true /** active. */, 0 /** first line. */, 
							_VICIIRegisters -> spriteDoubleHeight (i) /** double height? */);
				}

				rG = true;
			}

			break;

		// In cycle 58 again the graphical info is updated...
		case 58:
			{
				if (_vicGraphicInfo._RC == 7)
				{
					_vicGraphicInfo._VCBASE = _vicGraphicInfo._VC;
					if (!_newBadLineCondition) // When RC gets 7 and there is no bad condition, then the idle state is set back...
						_vicGraphicInfo._idleState = true;
				}

				if (++_vicGraphicInfo._RC == 8)
					_vicGraphicInfo._RC = 0; // Only from 0 to 7...
			}

			break;

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
				result = 56 - _cycleInRasterLine;
			}

			readVideoMatrixAndColorRAM ();

			if (deepDebugActive ())
				*_deepDebugFile
					<< "\t\t\t\tReading Video Matrix & Color RAM" 
					<< " [" << _vicGraphicInfo._lastScreenCodeDataRead.asString (MCHEmul::UByte::OutputFormat::_HEXA) 
					<< ", " << _vicGraphicInfo._lastColorDataRead.asString (MCHEmul::UByte::OutputFormat::_HEXA) << "]"
					<< "\n";
		}

		readGraphicalInfo ();

		if (deepDebugActive ())
			*_deepDebugFile
				<< "\t\t\t\tReading Graphics"
				<< " [" << _vicGraphicInfo._lastGraphicDataRead.asString (MCHEmul::UByte::OutputFormat::_HEXA) << "]"
				<< "\n";

		if (!_vicGraphicInfo._idleState)
			_vicGraphicInfo._VC++;	
		_vicGraphicInfo._VLMI++;
	}

	return (result);
}

// ---
void COMMODORE::VICII::drawVisibleZone (MCHEmul::CPU* cpu)
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
				_VICIIRegisters -> foregroundColor ());

		return;
	}

	// In other case...
	// Everyting is the color of the background initially...
	// ..and it will be covered with the foreground (border) later if needed..
	// This is how the VICII works...
	screenMemory () -> setHorizontalLine ((size_t) cav, (size_t) rv,
		(cav + 8) > _raster.visibleColumns () ? (_raster.visibleColumns () - cav) : 8, 
			_VICIIRegisters -> backgroundColor ());

	// Now the information is drawn,...
	// ...and also the collisions detected at the same time
	drawGraphicsSpritesAndDetectCollisions (
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
			_VICIIRegisters -> foregroundColor ());
	}

	// The IRQ related with the lightpen is calculated...
	unsigned short dx, dy;
	_raster.currentDisplayPosition (dx, dy);
	unsigned short lpx, lpy;
	_VICIIRegisters -> currentLightPenPosition (lpx, lpy);
	if (dy == lpy && (lpx >= dx && lpx < (dx + 8))) // The beam moves every 8 pixels...
		_VICIIRegisters -> activateLightPenOnScreenIRQ ();
}

// ---
void COMMODORE::VICII::drawGraphicsSpritesAndDetectCollisions (const COMMODORE::VICII::DrawContext& dC)
{
	// This varible keeps info about the text/graphics:
	// Whether the 8 pixels to draw are foreground or background...
	// ...and the color of the ones that are not finally background!
	// And also info to control later the collision with sprites!
	COMMODORE::VICII::DrawResult colGraphics = std::move (drawGraphics (dC));

	// The info about the sprites is moved into this variable too...
	std::vector <MCHEmul::UByte> sprCollData (8, MCHEmul::UByte::_0);
	for (size_t i = 0; i < 8; i++)
		if (_vicSpriteInfo [i]._active)
			sprCollData [i] = std::move (drawSpriteOver (i, colGraphics._spriteColor [i]));

	// The graphical info is moved to the screen...
	drawResultToScreen (colGraphics, dC);
	// ...and the collisions are also detected...
	detectCollisions (colGraphics._collisionData, sprCollData);
}

// ---
COMMODORE::VICII::DrawResult COMMODORE::VICII::drawGraphics (const COMMODORE::VICII::DrawContext& dC)
{
	// To draw is only valid in the graphical state...
	if (_vicGraphicInfo._idleState)
		return (COMMODORE::VICII::DrawResult ());

	// The "display" column being involved...
	// In cb, the SCROLLX is involved, so it could be negative! starting from -7, 
	// When e.g. the raster is at the very first "screen "dislay" column, 
	// there are no reductions is the screen (display == screen) and SCROLLX = 0x00
	int cb = (int) dC._RCA - (int) dC._ICD - (int) dC._SC;
	// To draw just only when there is a possibility of at least draw a pixel!
	if (cb <= -8)
		return (COMMODORE::VICII::DrawResult ());

	// At this point rc positive for sure, and cb could be negative...
	// Never invoke the methods within the swith case statements direcly
	// a crash might be generated...

	COMMODORE::VICII::DrawResult result;
	switch (_VICIIRegisters -> graphicModeActive ())
	{
		case COMMODORE::VICIIRegisters::GraphicMode::_CHARMODE:
			{
				result = std::move (drawMonoColorChar (cb));
			}

			break;

		case COMMODORE::VICIIRegisters::GraphicMode::_MULTICOLORCHARMODE:
			{
				result = std::move (drawMultiColorChar (cb));
			}

			break;

		case COMMODORE::VICIIRegisters::GraphicMode::_EXTENDEDBACKGROUNDMODE:
			{
				result = std::move (drawMultiColorExtendedChar (cb));
			}

			break;

		case COMMODORE::VICIIRegisters::GraphicMode::_BITMAPMODE:
			{
				result = std::move (drawMonoColorBitMap (cb));
			}

			break;

		case COMMODORE::VICIIRegisters::GraphicMode::_MULTICOLORBITMAPMODE:
			{
				result = std::move (drawMultiColorBitMap (cb));
			}

			break;

		case COMMODORE::VICIIRegisters::GraphicMode::_INVALIDTEXMODE:
			{
				// Like multicolor char mode, but invalid...
				result = std::move (drawMultiColorChar (cb, result._invalid = true));
			}

			break;

		case COMMODORE::VICIIRegisters::GraphicMode::_INVALIDBITMAPMODE1:
			{
				// Like moncocolor bitmap mode, but invalid...
				result = std::move (drawMonoColorBitMap (cb, result._invalid = true));
			}
			break;

		case COMMODORE::VICIIRegisters::GraphicMode::_INVALIDBITMAPMODE2:
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
COMMODORE::VICII::DrawResult COMMODORE::VICII::drawMonoColorChar (int cb)
{
	COMMODORE::VICII::DrawResult result;

	for (int i = 0; i < 8 /** To paint always 8 pixels */; i++)
	{
		int pp = cb + i;
		if (pp < 0)
			continue;
		if (pp >= 320)
			break;

		size_t iBy = ((size_t) pp) >> 3; // To determine the byte...
		size_t iBt = 7 - (((size_t) pp) % 8); // From MSB to LSB...

		if (_vicGraphicInfo._graphicData [iBy].bit (iBt))
		{
			result._collisionData.setBit (7 - i, true);

			result._foregroundColorData [i] = 
				(unsigned int) (_vicGraphicInfo._colorData [iBy].value () & 0x0f /** Useful nibble. */);
		}
		// When 0, it is background...
		// Not necessary to specify neither collision information
		// nor the color of the pixels as it will be always the basic background color,
		// that has already been set to the value of $d021 in the main loop...
	}

	return (result);
}

// ---
COMMODORE::VICII::DrawResult COMMODORE::VICII::drawMultiColorChar (int cb, bool inv)
{
	COMMODORE::VICII::DrawResult result;

	for (unsigned short i = 0 ; i < 8 /** To paint always 8 pixels but in blocks of 2. */; i += 2)
	{
		int pp = cb + i;
		if (pp < -1)
			continue;
		if (pp >= 320)
			break;

		// After this pp can be -1...

		size_t iBy = 0;
		unsigned char cs = 0;
		if (pp >= 0)
		{
			iBy = ((size_t) pp) >> 3; 
			size_t iBt = 3 - ((((size_t) pp) % 8) >> 1);
			cs = (_vicGraphicInfo._graphicData [iBy].value () >> (iBt << 1)) & 0x03; // 0, 1, 2 or 3
		}
		// This is the case when pp == -1...
		else
			cs = (_vicGraphicInfo._graphicData [0].value () >> 6) & 0x03; // 0, 1, 2 or 3

		// If 0, the pixel should be drawn (and considered) as background 
		// and it is already the default status tha comes from the parent method...
		if (cs == 0x00)
			continue;

		// The way the pixels are going to be drawn will depend on the information in the color memory
		// If the most significant bit of the low significant nibble of the color memory is set to 1
		// the data will be managed in a monocolor way...
		if ((_vicGraphicInfo._colorData [iBy] & 0x08) == 0x00) 
		{
			unsigned int fc = 
				inv 
					? 0x00 // When invalid all pixels are black...
					: _vicGraphicInfo._colorData [iBy].value () & 0x07;

			// ...and remember we are dealing with pairs of pixels...

			switch (cs)
			{
				case 0x01:
					{
						result._backgroundColorData [i + 1] = fc;
					}

					break;

				case 0x02:
					{
						result._collisionData.setBit (7 - i, true);

						result._foregroundColorData [i] = fc;
					}

					break;

				case 0x03:
					{
						result._collisionData.setBit (7 - i, true);
						result._collisionData.setBit (6 - i, true);

						result._foregroundColorData [i] = fc;
						result._foregroundColorData [i + 1] = fc;
					}

					break;

				default:
					break;
			}
		}
		// But if it is 1, 
		// then it will be draw as in the multicolor version...
		else
		{
			unsigned int fc = 
				inv 
					? 0x00 
					: (unsigned int) ((cs == 0x03) 
						? (_vicGraphicInfo._colorData [iBy].value () & 0x07)
						: _VICIIRegisters -> backgroundColor (cs));

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
COMMODORE::VICII::DrawResult COMMODORE::VICII::drawMultiColorExtendedChar (int cb)
{
	// The mode is quite similar to the standard text mode, 
	// with the difference the 0 pixels (background) can have different background colors...

	COMMODORE::VICII::DrawResult result;

	for (int i = 0; i < 8 /** To paint always 8 pixels */; i++)
	{
		int pp = cb + i;
		if (pp < 0)
			continue;
		if (pp >= 320)
			break;

		size_t iBy = ((size_t) pp) >> 3 /** To determine the byte. */;
		size_t iBt = 7 - (((size_t) pp) % 8); /** From MSB to LSB. */
		// The color of the pixel 0 is determined by the 2 MSBites of the char code...
		bool bS = _vicGraphicInfo._graphicData [iBy].bit (iBt); // To know whether the bit is 1 or 0...
		unsigned int cs = ((_vicGraphicInfo._screenCodeData [iBy].value () & 0xc0) >> 6) & 0x03; // 0, 1, 2, or 3
		unsigned int fc = 
			bS 
				? (_vicGraphicInfo._colorData [iBy].value () & 0x0f) 
				: _VICIIRegisters -> backgroundColor (cs);

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
COMMODORE::VICII::DrawResult COMMODORE::VICII::drawMonoColorBitMap (int cb, bool inv)
{
	COMMODORE::VICII::DrawResult result;

	for (int i = 0; i < 8 /** To paint always 8 pixels. */; i++)
	{
		int pp = cb + i;
		if (pp < 0)
			continue;
		if (pp >= 320)
			break;

		size_t iBy = ((size_t) pp) >> 3; // To determine the byte...
		size_t iBt = 7 - (((size_t) pp) % 8); // From MSB to LSB...
		bool bS = _vicGraphicInfo._graphicData [iBy].bit (iBt);
		unsigned int fc = 
			inv 
				? 0x00 // When invalid, all pixels are black...
				: bS 
					? (_vicGraphicInfo._screenCodeData [iBy].value () & 0xf0) >> 4	// If the bit is 1, the color is determined by the MSNibble
					: (_vicGraphicInfo._screenCodeData [iBy].value () & 0x0f);		// ...and for LSNibble if it is 0...

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
COMMODORE::VICII::DrawResult COMMODORE::VICII::drawMultiColorBitMap (int cb, bool inv)
{
	COMMODORE::VICII::DrawResult result;

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
			cs = (_vicGraphicInfo._graphicData [iBy].value () >> (iBt << 1)) & 0x03; // 0, 1, 2 or 3
		}
		// This is the case when pp == -1...
		else
			cs = (_vicGraphicInfo._graphicData [0].value () >> 6) & 0x03; // 0, 1, 2 or 3

		// If 0, the pixel should be drawn (and considered) as background 
		// and it is already the default status tha comes from the parent method...
		if (cs == 0x00)
			continue;

		unsigned fc = // The value 0x00 is not tested....
				inv
					? 0x00 // When invalid all pixels are black...
					: (cs == 0x01) // The color is the defined in the video matrix, high nibble...
						? (_vicGraphicInfo._screenCodeData [iBy].value () & 0xf0) >> 4
						: ((cs == 0x02) // The color is defined in the video matrix, low nibble...
							? (_vicGraphicInfo._screenCodeData [iBy].value () & 0x0f)
							: (_vicGraphicInfo._colorData [iBy].value () & 0x0f)); // The color is defined in color matrix...

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
MCHEmul::UByte COMMODORE::VICII::drawSpriteOver (size_t spr, unsigned int* d)
{
	return ((_vicSpriteInfo [spr]._graphicsLineSprites.size () == 0)
		? MCHEmul::UByte::_0
		: (_VICIIRegisters -> spriteMulticolorMode (spr)
			? drawMultiColorSpriteOver (_raster.currentColumn (), _raster.currentLine (), spr, d)
			: drawMonoColorSpriteOver (_raster.currentColumn (), _raster.currentLine (), spr, d)));
}

// ---
MCHEmul::UByte COMMODORE::VICII::drawMonoColorSpriteOver (unsigned short c, unsigned short r, size_t spr, unsigned int* d)
{
	MCHEmul::UByte result = MCHEmul::UByte::_0;

	// Horizontal info about the sprite
	unsigned short dW	= _VICIIRegisters -> spriteDoubleWidth (spr) ? 2 : 1;
	unsigned short x	= _VICIIRegisters -> spriteXCoord (spr) + 4;
	unsigned short wX	= 24 /** normal width in pixels. */ * dW;
	unsigned short dW8	= 8 * dW; // 8 or 16
	// Vertical info about the sprite
	/** Because the position 50 correspond at the raster line position 51. */
	unsigned short y	= _VICIIRegisters -> spriteYCoord (spr) + 1;
	unsigned short wY	= _VICIIRegisters -> spriteDoubleHeight (spr) ? 42 : 21;

	if (r < y || r >= (y + wY))
		return (result); // Not visible in the vertical zone...
	if ((c + 8 /** pixels */) < x || c >= (x + wX))
		return (result); // Not visible in the horizontal zone...

	for (unsigned short i = 0; 
			i < 8 /** always to draw 8 pixels */; i += dW /** the size of the pixels block. */)
	{
		unsigned short pp = (c + i);
		if (pp < x)
			continue; // Not visible...
		if (pp >= (x + wX))
			break; // No more draws...

		// To determine the initial byte (iBy) and bit (iBt) with the info about the sprite...
		// The bit moves from 7 to 0, and the byte increases...
		size_t iBy = (size_t) ((pp - x) / dW8);
		size_t iBt = (size_t) (7 - (((pp - x) % dW8) / dW));
		bool dP = _vicSpriteInfo [spr]._graphicsLineSprites [iBy].bit (iBt);
		if (!dP)
			continue; // The point is not visible...

		for (size_t j = 0; j < (size_t) dW; j++)
		{
			result.setBit (7 - (i + j), true);

			d [i + j] = _VICIIRegisters -> spriteColor (spr);
		}
	}

	return (result);
}

// ---
MCHEmul::UByte COMMODORE::VICII::drawMultiColorSpriteOver (unsigned short c, unsigned short r, size_t spr, unsigned int* d)
{
	MCHEmul::UByte result = MCHEmul::UByte::_0;

	// Horizontal info about the sprite
	unsigned short dW	= _VICIIRegisters -> spriteDoubleWidth (spr) ? 2 : 1;
	unsigned short x	= _VICIIRegisters -> spriteXCoord (spr) + 4;
	unsigned short wX	= 24 /** normal width in pixels. */ * dW;
	unsigned short dW8	= 8 * dW; // 8 or 16
	// Vertical info about the sprite
	unsigned short y	= _VICIIRegisters -> spriteYCoord (spr) + 1;
	unsigned short wY	= _VICIIRegisters -> spriteDoubleHeight (spr) ? 42 : 21;

	if (r < y || r >= (y + wY))
		return (result); // Not visible in the vertical zone...
	if ((c + 8 /** pixels */) < x || c >= (x + wX))
		return (result); // Not visible in the horizontal zone...

	for (unsigned short i = 0; i < 8 /** pixels. */; i += (2 * dW))
	{
		unsigned short pp = (c + i);
		if (pp < x)
			continue; // Not visible...
		if (pp >= (x + wX))
			break; // No more draws...

		// To determine the initial byte (iBy) and bit (iBt) with the info about the sprite...
		// The bit to select moves from 0 to 3, represeting the pair of bits (0,1), (2,3), (4,5), (6,7)
		size_t iBy = (size_t) ((pp - x) / dW8);
		size_t iBt = (size_t) (3 - (((pp - x) % dW8) / (2 * dW)));
		unsigned char cs = (_vicSpriteInfo [spr]._graphicsLineSprites [iBy].value () >> (iBt << 1)) & 0x03;
		if (cs == 0)
			continue; // The point has no color...

		unsigned int fc = 
			(unsigned int) ((cs == 0x01) 
				? _VICIIRegisters -> spriteSharedColor (0)
				: ((cs == 0x02) 
					? _VICIIRegisters -> spriteColor (spr)
					: _VICIIRegisters -> spriteSharedColor (1)));

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
	// Pixel by pixel..
	for (size_t i = 0; i < 8; i++)
	{
		size_t pos = (size_t) dC._RCA + i;

		// If the graphic mode was invalid...
		if (cT._invalid)
		{
			// the pixel will be always black by default...
			screenMemory () -> setPixel (pos, (size_t) dC._RR, 0x00 /** black. */);

			continue;
		}

		// The background is drawn...
		// Although the background was already drawn in the main loop
		// as a consequence of managing graphics pixels other than 0 (in multicolor modes e.g.) might also 
		// be part of the background...
		if (cT._backgroundColorData [i] != _U0)
			screenMemory () -> setPixel (pos, (size_t) dC._RR, cT._backgroundColorData [i]);

		// Looks for the most priority sprite to draw, if any...
		size_t nSpr = 0;
		while (nSpr != 8 && cT._spriteColor [nSpr][i] == _U0) nSpr++;
		// If that sprite didn't exist...
		if (nSpr == 8)
		{
			// ...and there were a foreground not transparent on top, 
			// the sprite pixel would be drawn!
			if (cT._foregroundColorData [i] != _U0)
				screenMemory () -> setPixel (pos, (size_t) dC._RR, cT._foregroundColorData [i]);
		}
		// But, if the sprite existed...
		else
		{
			// ...the sprite would be drawn...
			screenMemory () -> setPixel (pos, (size_t) dC._RR, cT._spriteColor [nSpr][i]);
			// ..and if it had less priority than the foreground, 
			// the foreground is drawn on top...
			if (_VICIIRegisters -> spriteToForegroundPriority (nSpr) &&
				cT._foregroundColorData [i] != _U0)
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
COMMODORE::VICII_PAL::VICII_PAL (int vV)
	: COMMODORE::VICII (
		 _VRASTERDATA, _HRASTERDATA, vV, COMMODORE::VICII_PAL::_CYCLESPERRASTERLINE,
		 { { "Name", "VIC-II (PAL) Video Chip Interface II" },
		   { "Code", "6569/8565/8566" },
		   { "Manufacturer", "MOS Technology INC/Commodore Semiconductor Group (CBM)"},
		   { "Year", "1980" } })
{
	// Nothing else to do...
}

// ---
unsigned int COMMODORE::VICII_PAL::treatRasterCycle ()
{
	unsigned int result = result = COMMODORE::VICII::treatRasterCycle ();

	switch (_cycleInRasterLine)
	{
		// Read sprite 0 to 2 data?
		// At the last cycles...
		case 58:
		case 60:
		case 62:
			{
				// Is there sprite info available?
				unsigned short nSR = ((_cycleInRasterLine - 58) >> 1); // 0, 1 or 2
				if (readSpriteData (nSR))
				{
					result += 2; // VICII has to read three bytes, Meaning 2 clock cycles stopped more... 

					if (deepDebugActive ())
						*_deepDebugFile
							<< "\t\t\t\tReading info sprite:" << std::to_string (nSR + 1)
							<< " [" << _vicSpriteInfo [nSR]._graphicsLineSprites.asString 
								(MCHEmul::UByte::OutputFormat::_HEXA, ' ') << "]"
							<< "\n";
				}
			}

			break;

		// Cycle 63 is left...

		default:
			break;
	}

	return (result);
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
unsigned int COMMODORE::VICII_NTSC::treatRasterCycle ()
{
	unsigned int result = COMMODORE::VICII::treatRasterCycle ();

	switch (_cycleInRasterLine)
	{
		// Read sprite 0 to 2 data?
		// At the last cycles...
		case 60:
		case 62:
		case 64:
			{
				// Is there sprite info available?
				unsigned short nSR = ((_cycleInRasterLine - 60) >> 1); // 0, 1 or 2
				if (readSpriteData (nSR))
				{
					result += 2;	// VICII has to read three bytes, Meaning 2 clock cycles stopped more... 

					if (deepDebugActive ())
						*_deepDebugFile
							<< "\t\t\t\tReading info sprite:" << std::to_string (nSR + 1) 
							<< " [" << _vicSpriteInfo [nSR]._graphicsLineSprites.asString 
								(MCHEmul::UByte::OutputFormat::_HEXA, ' ') << "]"
							<< "\n";
				}
			}

			break;

		// Cycle 64 is left...

		default:
			break;
	}

	return (result);
}
