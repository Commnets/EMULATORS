#include <COMMODORE/VICII/VICII.hpp>
#include <F6500/IRQInterrupt.hpp>

// ---
/** At the information is in https://www.cebix.net/VIC-Article.txt. */
const MCHEmul::RasterData COMMODORE::VICII_PAL::_VRASTERDATA 
	(0, 16, 51, 250, 289, 311 /** When the reatrace starts. */, 311 /** When the retrace finishes. */, 312, 4, 4);
/** The real VICII does actions in both phases of the clock (it really uses a clock running at double speed and creates the CPU clock from that) \n
	When the raster beam is in the visible zone 4 pixels are drawn in each phase of the cycle, so 8 pixels per CPU cycle.
	As the documentation describes, the graphics are read from cycle 15 onwards (in a badline). \n
	The first char code is actually read at the 2nd phase (up) of the cycle 15 and the graphic data is read at the 1st phase of the cycle 16. \n
	The simulation doesn't manage a clock with two phases, so both actions are programmed to happen at cycle 16 (see below). That is from cycle 16 to cycle 55 (40).
	At the beginning of that cycle 16 the raster will haved moved: (16 - 1) * 8 = 120 "pixels" since the counter of cycles started.
	Drawing of the content should start at position (as defined): 24, that's 124 "pixels" since the counter of cycles started (504 - 404 + 24). */
const MCHEmul::RasterData COMMODORE::VICII_PAL::_HRASTERDATA 
	(404, 496 /** The 2nd HBI finishes and then the drawing border starts. 
				  In the documentation it happens at 480, but it wouldn't make a border with the same with than the right one! */, 
		24, 343, 375, 379 /** 1st horizontal blanking interval (HBI) */, 403 /** 1st HBI finishes., */,
		504 /** For everyting to run, it has to be divisible by 8. */, 7, 9);
const MCHEmul::RasterData COMMODORE::VICII_NTSC::_VRASTERDATA 
	(27, 41, 51, 250, 2, 26, 26, 262, 4, 4); // Same meaning than in the case of the PAL system...
const MCHEmul::RasterData COMMODORE::VICII_NTSC::_HRASTERDATA 
	(412, 504, 24, 343, 375, 379, 411, 512, 7, 9); // Same meaning than in the case of the PAL system...
// This two positions are fized...
// are the ones where the VICII reads information from when it is not in the visible part!
const MCHEmul::Address COMMODORE::VICII::_MEMORYPOSIDLE1 = MCHEmul::Address ({ 0xff, 0x39 }, false);
const MCHEmul::Address COMMODORE::VICII::_MEMORYPOSIDLE2 = MCHEmul::Address ({ 0xff, 0x3f }, false);

// ---
COMMODORE::VICII::VICII (int intId, MCHEmul::PhysicalStorageSubset* cR, const MCHEmul::Address& cRA,
		const MCHEmul::RasterData& vd, const MCHEmul::RasterData& hd, 
		int vV, unsigned short cRL, const MCHEmul::Attributes& attrs)
	: MCHEmul::GraphicalChip (_ID, attrs),
	  _interruptId (intId),
	  _colorRAM (cR), _colorRAMAddress (cRA),
	  _VICIIRegisters (nullptr), 
	  _VICIIView (vV),
	  _cyclesPerRasterLine (cRL),
	  _IRQrasterPosition (0), // Assigned within the constructor of the specific version of the VICII...
	  _incCyclesPerRasterLine (cRL - COMMODORE::VICII_PAL::_CYCLESPERRASTERLINE),
	  _raster (vd, hd, 8 /** @see above. This is the step. */),
	  _drawRasterInterruptPositions (false), _drawSpritesBorder (false), _drawOtherEvents (false),
	  _lastCPUCycles (0),
	  _format (nullptr),
	  _cycleInRasterLine (1),
	  _videoActive (true),
	  _lastVBlankEntered (false),
	  _lastBadLineScrollY (-1), _newBadLineCondition (false), _badLineStopCyclesAdded (false),
	  _vicGraphicInfo (),
	  _vicSpriteInfo (),
	  _eventStatus { false, false }
{
	// At this point the color RAM can be nullptr, 
	// but never when the VIC starts to work!

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

	assert (_colorRAM != nullptr); // It must have a reference...

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

	_VICIIRegisters -> linkToRaster (&_raster);
	_VICIIRegisters -> initialize (); // The raster is not reinitialized there...

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
	// Details about the emulation of the VICII:
	// VICII does activities in both phases of the clock.
	// In a bad line, the VICII reads the screen code in the phase 2 (up) of the clock (when the CPU is usually doing things)...
	// ...and the graphics data in the phase 1
	// The VICII starts to read in the cycle 15 the screen code (phase 2) and the graphics data in the phase 1 of the next cycle 16.
	// IMPORTANT: The simulation has been built to do both actions in the cycle 16.
	// The graphic is then sent to the screen in two moments: 4 pixels in the phase 2 of the cycle 16, and 4 more in the phase 1 of the cycle 17.
	// IMPORTANT: The simulation has been done to do both activities in the cycle 17.
	// The same rational is applied for the next: So the graphics are read from cycle 16 to 55 (included)...
	// ...and the screen is drawn from cycle 17 to 56.

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
			_vicGraphicInfo._ROW >= _FIRSTBADLINE && 
			_vicGraphicInfo._ROW <= _LASTBADLINE &&
			(unsigned char) (_vicGraphicInfo._ROW & 0x07) /** The three last bits. */ == 
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

	// If the "video reset flag" is actived nothing should be done...
	// This is a very strange bit "RES" at the register $d016 of the VIC.
	// There is no too much documentation about it.
	// In "Mapping the C64" says this bit "disconnect" the full operation of the VICII, 
	// and that in some old C64s the screen is also turn in black...
	// However seems that something is anyway done...
	// and what it is done without any doubt is to actualize the raster 
	// because it is a signal comming from the TV not from the VICII....
	// So far just the condition is logged...
	if (_VICIIRegisters -> videoResetActive ())
		_IFDEBUG debugDisconnected (cpu);

	// Adapt the size of the display zone to the parameters specificied in the register...
	// The zone where sprites and texts are finally visible is call the "screen zone"
	_raster.reduceDisplayZone
		(!_VICIIRegisters -> textDisplay25RowsActive (), !_VICIIRegisters -> textDisplay40ColumnsActive ());

	// If the right register has been modified....
	if (_VICIIRegisters -> interruptsEnabledBack ())
		cpu -> interrupt (_interruptId) -> 
			setNewInterruptRequestAdmitted (false); // ...the interrupts are admitted again...

	// The simulation has to be repeated as many time as cycles have spent since the last invocation...
	for (unsigned int i = (cpu -> clockCycles  () - _lastCPUCycles); i > 0; i--)
	{
		_IFDEBUG debugVICIICycle (cpu, i);

		// Whether the video is active or not is only checked at the very first bad line...
		_videoActive = (_vicGraphicInfo._ROW == _FIRSTBADLINE) 
			? !_VICIIRegisters -> blankEntireScreen () : _videoActive; // The value can change at any cycle of this first bad line...

		// Is there a bad line situation?
		// A bad line can happen at any cycle within the raster line.
		// When happened, the graphics are read and the situation is latched because maybe additional stop cycles could be needed.
		if (isNewBadLine ())
		{
			_IFDEBUG debugBadLine ();

			_newBadLineCondition = true;		// latched...
			_badLineStopCyclesAdded = false;	// ...the cycles have to be added...

			_vicGraphicInfo._idleState = false; // No longer in "idle" state but in the "screen" one!

			// To keep track of the event later, if needed...
			_eventStatus._badLine = 
				_raster.vData ().currentVisiblePosition ();
		}

		// When VICII is about to read sprites or graphics info (bad line),
		// the CPU has to stop 3 cycles in advance (just for READ activities) for those activities,
		// unless it was stopped previously and that stop situation were still valid...
		// In the case of graphics that stop only happens when the situation arise in the "screen cycles" (40)
		// When a badline happens (usual bad line) to stop the CPU is requested at cycle 12, then it would be stopped cycles 13, 14, and 15
		// At cycle 16 the code reaches this point and the condition will also be met so a new stop will be requested (3 cycles more)
		// However, later the graphics will start to be read an a new stop condition will arise....(1)
		if (!cpu -> stopped () && 
			(isAboutToReadSpriteInfo () || 
				(_newBadLineCondition && (_cycleInRasterLine >= 12 && _cycleInRasterLine < 52))))
			cpu -> setStop (true, MCHEmul::InstructionDefined::_CYCLEREAD /** only read in not allowed. */, 
				cpu -> clockCycles () - i, 3);

		// Treat the right cycle...
		// ...and as a consequence the CPU can be also stopped...
		// (1)...so this condition will overwrite the previous one!
		unsigned int cS = 0;
		if ((cS = treatRasterCycle ()) > 0)
			cpu -> setStop (true, MCHEmul::InstructionDefined::_CYCLEALL /** fully stopped. */, cpu -> clockCycles () - i, (int) cS);

		// Draws the graphics & border if it has to do so...
		if (_raster.isInVisibleZone ())
			drawVisibleZone (cpu);

		// Move 8 pixels right in the raster line and jump to other line is possible...
		// Notice that the variable _isNewRasterLine becomes true when a new line comes...
		// Always when there is a new line the Raster IRQ has to be checked, 
		// and the situation is flagged into the register if true...
		// Whether finally a IRQ is or not actually launched is something that is determined later per cycle
		// just to take into account other issuing possibilities like two sprites collision analized later.
		_cycleInRasterLine++; // First move to the next raster cycle...

		// Moves the horizontal raster...
		bool cLine = _raster.hData ().add (1 * _raster.step ());
		// Does the raster reached the retrace start position...
		if (_raster.hData ().retraceJustOverPassed ())
			_raster.vData ().next (); // ...in this case, the row has to be changed...
		// ...and has it reached the last position defined (that is when the retrace finishes)?
		if (cLine)
		{
			_cycleInRasterLine = 1;

			_lastBadLineScrollY = -1;
			_newBadLineCondition = false;
			_badLineStopCyclesAdded = false;

			// At the beginning of the counter....
			_vicGraphicInfo._ROW = _raster.currentLine ();
			if (_vicGraphicInfo._ROW == 0)
			{ 
				// The internal variables counting the lines, etc...
				// are set back to 0...
				_vicGraphicInfo._VCBASE = _vicGraphicInfo._VC = 0;
				_vicGraphicInfo._RC = 0;

				// ...but also the position of the light pen is "fixed"...
				// ...and a interruption is also launched at that position related with the light pen!...
				// ...is the lightpen is active for sure!
				fixLightPenPosition ();
			}
			// In any other line number, VC start back to count from the value in VCBASE.
			// VCBASE is actualized only then RC reaches 8. @see rasterCycle 58 treatment.
			else 
				_vicGraphicInfo._VC = _vicGraphicInfo._VCBASE;
		}

		// If the current line is where a IRQ has been set...
		// ..and also the column of the raster is where it should be launched...
		if (_vicGraphicInfo._ROW == _VICIIRegisters -> IRQRasterLineAt () &&
			_raster.currentColumn () == _IRQrasterPosition)
			_VICIIRegisters -> activateRasterIRQ (); // ...the interrupt is activated (but not necessary launched!)

		// Latch the light pen position (reading the mouse)
		// if it is within the window...
		latchLightPenPosition ();

		// Per cycle, the IRQ condition is checked! 
		// (many reasons during the cycle can unchain the IRQ interrupt)
		int cI = -1;
		if ((cI = (int)_VICIIRegisters -> reasonIRQCode ()) != 0)
			cpu -> requestInterrupt (
				_interruptId,
				cpu -> clockCycles  () - i, 
				this,
				cI);
	}

	// When the raster enters the non visible part of the screen,
	// a notification is sent (to the Screen class usually) 
	// just to draw the screen...
	if (_raster.isInFirstVBlankZone ())
	{
		if (!_lastVBlankEntered)
		{
			_lastVBlankEntered = true;

			notify (MCHEmul::Event (_GRAPHICSREADY)); 
		}
	}
	else
		_lastVBlankEntered = false;

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
	result.add ("VICIIInternal",	std::move (_vicGraphicInfo.getInfoStructure ()));
	result.add ("Badline",			std::string (_newBadLineCondition ? "YES" : "NO"));
	result.add ("Cycle",			_cycleInRasterLine);

	return (result);
}

// ---
MCHEmul::UBytes COMMODORE::VICII::screenMemorySnapShot (MCHEmul::CPU* cpu) const
{ 	
	// Usually in this point the active view should be the CPU one, 
	// But the one needed is the VICII, so it is changed if neded...
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
	// Remember that VICII has no a direct view of the color RAM
	// Because it access directly to it when access the memory to get a char instead...
	// So the only way to see that part of the memory is through out the CPU view
	// The position of the ColorRAM is fixed in the CPU view and it is received as parameter
	// when the VICII chip is built!
	int aVID = cpu -> memoryRef () -> activeView () -> id ();
	if (aVID == _VICIIView)
		cpu -> memoryRef () -> setCPUView (); // Change to the main one...

	// Gets the data from the memory directly...
	MCHEmul::UBytes result = cpu -> memoryRef () -> values 
		(_colorRAMAddress, 0x03e8 /** 1000 positions = 40 x 25. */); 
	// ...but in each byte of the color RAM only the low nibble cares,
	// ...the upper one is a random value, so it must be "eliminated" for the clarity of the user...
	for (size_t i = 0; i < result.size (); i++)
		result [i] = result [i] & 0x0f;

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
							case 0x00: dt += "  "; break;
							case 0x01: dt += "OO"; break;
							case 0x02: dt += "XX"; break;
							case 0x03: dt += "##"; break;
							// This situation doesn't happen...
							default: break;
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
						case 0x00: dt += "  "; break;
						case 0x01: dt += "OO"; break;
						case 0x02: dt += "XX"; break;
						case 0x03: dt += "##"; break;
						// This situation doesn't happen...
						default: break;
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

		// If the mouse is moved, keep track of its position
		// it will be used later to simulate dthe light pen!
		// @see "simulate" method in in this class...
		case MCHEmul::InputOSSystem::_MOUSEMOVED:
			{
				// Where is the mouse?
				int x = std::dynamic_pointer_cast <MCHEmul::InputOSSystem::MouseMovementEvent> 
					(evnt.data ()) -> _x;
				int y = std::dynamic_pointer_cast <MCHEmul::InputOSSystem::MouseMovementEvent> 
					(evnt.data ()) -> _y;

				// Is the mouse in the window?
				if (x == 0 || 
					y == 0 ||
					(unsigned short) x >= _raster.visibleColumns () ||
					(unsigned short) y >= _raster.visibleLines ())
					_VICIIRegisters -> setMousePosition (-1, -1); // No in the window...
				else
					_VICIIRegisters -> setMousePosition (x, y);
			}

			break;

		// The lightpen actives when the right button of the mouse is pressed...
		case MCHEmul::InputOSSystem::_MOUSEBUTTONPRESSED:
			{
				if (std::dynamic_pointer_cast <MCHEmul::InputOSSystem::MouseButtonEvent> 
					(evnt.data ()) -> _buttonId == 1 /** Left. Right would be 3. */)
					_VICIIRegisters -> setLigthPenActive (true);
			}
			
			break;

		// ...and stop being active when released...
		case MCHEmul::InputOSSystem::_MOUSEBUTTONRELEASED:
			{
				if (std::dynamic_pointer_cast <MCHEmul::InputOSSystem::MouseButtonEvent> 
					(evnt.data ()) -> _buttonId == 1 /** Left. */)
					_VICIIRegisters -> setLigthPenActive (false);
			}

			break;

		default:
			break;
	}
}

// ---
MCHEmul::ScreenMemory* COMMODORE::VICII::createScreenMemory ()
{
	unsigned int* cP = new unsigned int [37];

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

	// Colors used for the borders and so! (16)
	// Same than the original ones, but with full light!
	cP [16] = SDL_MapRGBA (_format, 0x00, 0x00, 0x00, 0xff); // Black
	cP [17] = SDL_MapRGBA (_format, 0xff, 0xff, 0xff, 0xff); // White
	cP [18] = SDL_MapRGBA (_format, 0x92, 0x4a, 0x40, 0xff); // Red
	cP [19] = SDL_MapRGBA (_format, 0x84, 0xc5, 0xcc, 0xff); // Cyan
	cP [20] = SDL_MapRGBA (_format, 0x93, 0x51, 0xb6, 0xff); // Violet
	cP [21] = SDL_MapRGBA (_format, 0x72, 0xb1, 0x4b, 0xff); // Green
	cP [22] = SDL_MapRGBA (_format, 0x48, 0x3a, 0xaa, 0xff); // Blue
	cP [23] = SDL_MapRGBA (_format, 0xd5, 0xdf, 0x7c, 0xff); // Yellow
	cP [24] = SDL_MapRGBA (_format, 0x99, 0x69, 0x2d, 0xff); // Brown
	cP [25] = SDL_MapRGBA (_format, 0x67, 0x52, 0x00, 0xff); // Light Red
	cP [26] = SDL_MapRGBA (_format, 0xc1, 0x81, 0x78, 0xff); // Orange
	cP [27] = SDL_MapRGBA (_format, 0x60, 0x60, 0x60, 0xff); // Dark Grey
	cP [28] = SDL_MapRGBA (_format, 0x8a, 0x8a, 0x8a, 0xff); // Medium Grey
	cP [29] = SDL_MapRGBA (_format, 0xb3, 0xec, 0x91, 0xff); // Light Green
	cP [30] = SDL_MapRGBA (_format, 0x86, 0x7a, 0xde, 0xff); // Light Blue
	cP [31] = SDL_MapRGBA (_format, 0xb3, 0xb3, 0xb3, 0xff); // Light Grey

	// Colors used for events!
	cP [32] = SDL_MapRGBA (_format, 0X00, 0Xf5, 0xff, 0xff); // light Cyan
	cP [33] = SDL_MapRGBA (_format, 0Xfc, 0Xe7, 0x00, 0xff); // light yellow
	cP [34] = SDL_MapRGBA (_format, 0Xff, 0X6d, 0x28, 0xff); // light orange
	cP [35] = SDL_MapRGBA (_format, 0Xea, 0X04, 0x7e, 0xff); // light purple
	cP [36] = SDL_MapRGBA (_format, 0X3e, 0Xc7, 0x0b, 0xff); // light green

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

					_IFDEBUG debugReadingSpriteInfo (nSR);
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
					// This cycle happens at the beginning of every raster line...
					// From 0 to 20 (21 lines)...
					if (_vicSpriteInfo [i]._line == 21)
					{
						_vicSpriteInfo [i]._active = false;

						_IFDEBUG debugSpriteDrawFinishes (i);
					}

					// Read also cycle 55 onwards info
					// because if expansion flip - flop hadn't active, 
					// the "line" will increment every two raster lines...
					if (_VICIIRegisters -> expansionYFlipFlop (i))
						_vicSpriteInfo [i]._line++;

					// When this last instruction is executed and finally the sprite becomes no active
					// the value of the flip flop is always "true":
					// When the sprite is not expanded, it will remain as true from the beginning...
					// When the sprite is expanded, it start as false (@see cycle 55) and finished in true...
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
					// As the documentacion describes:
					// DMA activation is one thing and Sprite activation is another different one.
					// In the cycle 55 the DMA is created if it wasn't before.
					// In the cycle 55/56/58 (in PAL VICII) if the DMA is active, the sprite associated is activated too.
					// So when DMA is "created" the sprite is active, and also in the other sense...

					// Simulation of the cycle 55...
					if (_VICIIRegisters -> spriteDoubleHeight (i))
						_VICIIRegisters -> invertExpansionYFlipFlop (i); 
					// The flip flop is inverted if at this point the sprite has still double height...

					// Simulation of the cycle 55/56/58...
					if (_VICIIRegisters -> spriteEnable (i) &&
						_vicGraphicInfo._ROW == (unsigned short) _VICIIRegisters -> spriteYCoord (i))
					{
						_vicSpriteInfo [i]._active = true;
						_vicSpriteInfo [i]._line = 0;
						_vicSpriteInfo [i]._drawing = false; _vicSpriteInfo [i]._xS = 0;
						// _expansionY attribute is used later to draw and know what the exactly height is...
						if (_vicSpriteInfo [i]._expansionY = _VICIIRegisters -> spriteDoubleHeight (i))
							_VICIIRegisters -> setExpansionYFlipFlop (i, false);

						_IFDEBUG debugSpriteDrawToStart (i);
					}
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

			_IFDEBUG debugReadingVideoMatrix ();
		}

		readGraphicalInfo ();

		_IFDEBUG debugReadingGraphics ();

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

	// actualize the status of the border...
	actualizeMainBorderStatus (cav, rv);

	// If the video is not active, 
	// then everything will have the border color...
	if (!_videoActive)
	{
		_IFDEBUG debugVideoNoActiveAt (cav);

		screenMemory () -> setHorizontalLine ((size_t) cav, (size_t) rv,
			(cav + 8) >= _raster.visibleColumns () ? (_raster.visibleColumns () - cav) : 8, 
				_VICIIRegisters -> foregroundColor ());

		return;
	}

	// In other case...
	// Everyting is the color of the background initially...
	// ..and it will be covered with the foreground (border) later if needed..
	// This is how the VICII works...
	screenMemory () -> setHorizontalLine ((size_t) cav, (size_t) rv,
		(cav + 8) >= _raster.visibleColumns () ? (_raster.visibleColumns () - cav) : 8, 
			_VICIIRegisters -> backgroundColor ());

	// Now the information is drawn,...
	// ...and also the collisions detected at the same time
	drawGraphicsSpritesAndDetectCollisions (
		{
			/** _ICD */ _raster.hData ().firstDisplayPosition (),		// DISPLAY: The original...
			/** _SC	 */ _VICIIRegisters -> horizontalScrollPosition (),	// From 0 - 7
			/** _RC  */ cv,												// Not adjusted with in the window...
			/** _RCA */ cav,											// Where the horizontal raster is (adjusted to 8) inside the window
			/** _RR	 */ rv												// Where the vertical raster is inside the window (it is not the chip raster line)
		});

	// If the raster is not in the very visible zone...
	// it is time to cover with the border...
	if (_vicGraphicInfo._ffMBorder || 
		_vicGraphicInfo._ffLBorder || _vicGraphicInfo._ffRBorder /** The temporal ones. */)
	{
		screenMemory () -> setHorizontalLine ((size_t) _vicGraphicInfo._ffMBorderBegin, 
			(size_t) rv, (size_t) _vicGraphicInfo._ffMBorderPixels, _VICIIRegisters -> foregroundColor ());
		if (_vicGraphicInfo._ffLBorder)
			{ _vicGraphicInfo._ffLBorder = false; _vicGraphicInfo._ffMBorder = false; }
		if (_vicGraphicInfo._ffRBorder)
			{ _vicGraphicInfo._ffRBorder = false; _vicGraphicInfo._ffMBorder = true; }
	}

	// If there were requested to draw the position where the Raster Interrupt is generated...
	// All draw are replace by a line with a color different of the background!
	// The line is only drawn if that position where within the visible zone!
	if (_drawRasterInterruptPositions)
	{
		unsigned short lrt = 
			_raster.lineInVisibleZone (_VICIIRegisters -> IRQRasterLineAt ());
		if (lrt <= _raster.vData ().lastVisiblePosition ())
			screenMemory () -> setHorizontalLine ((size_t) cav, (size_t) lrt,
				(cav + 8) > _raster.visibleColumns () ? (_raster.visibleColumns () - cav) : 8, 
					_VICIIRegisters -> backgroundColor () == 15 
						? 0 : _VICIIRegisters -> backgroundColor () + 1 /** to be visible. */);
	}

	// The draw around the sprites is drawn as part of the sprite draw routine...

	// If it activated to draw other events that happen during the interation of the VICII...
	if (_drawOtherEvents)
		drawOtherEvents ();
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
	for (int i = 7; i >= 0; i--)
	{
		if (_vicSpriteInfo [(size_t) i]._active)
			colGraphics._collisionSpritesData [(size_t) i] = 
				std::move (drawSpriteOver ((size_t) i, colGraphics._spriteColor, 
					colGraphics._spriteColorOwner));
	}

	// The graphical info is moved to the screen...
	drawResultToScreen (colGraphics, dC);
	// ...and the collisions are also detected...
	detectCollisions (colGraphics);
}

// ---
void COMMODORE::VICII::drawOtherEvents ()
{
	// Draw the border events...
	unsigned int cEvent = std::numeric_limits <unsigned int>::max ();
	if (_eventStatus._ffVBorderChange.positiveEdge ()) 
		cEvent = 32; // Auxiliar...
	if (_eventStatus._ffVBorderChange.negativeEdge ()) 
		cEvent = 33; // Auxiliar...
	if (_eventStatus._ffMBorderChange.positiveEdge ())
		cEvent = 34; // The main indication for the border...
	if (_eventStatus._ffMBorderChange.negativeEdge ())
		cEvent = 35; // The main indication for the border...
	if (cEvent != std::numeric_limits <unsigned int>::max ())
		screenMemory () -> setHorizontalLine 
			(_vicGraphicInfo._ffMBorderBegin, _raster.vData ().currentVisiblePosition (), 2, cEvent);

	// Draw the bad line event...
	// From the moment the condition is identified and a full line!
	if (_eventStatus._badLine != std::numeric_limits <unsigned short>::max ())
	{ 
		if (_raster.vData ().currentVisiblePosition () == _eventStatus._badLine)
			screenMemory () -> setHorizontalLine 
				(_raster.hData ().currentVisiblePosition (), _eventStatus._badLine, 2, 36); // in points and draw in auxiliar color...
		else
			_eventStatus._badLine = std::numeric_limits <unsigned short>::max ();
	}
}

// ---
COMMODORE::VICII::DrawResult COMMODORE::VICII::drawGraphics (const COMMODORE::VICII::DrawContext& dC)
{
	/** IMPORTANT NOTE:
		When it is in idle state it also drawn
		With the content of a fixed position depending on the active bacnk and always in color 0. */

	/** IMPORTANT NOTE:
		When the raster cycle is 16, _RC = 28 and _RCA = 24.
		The VIC should start to draw at that cycle. 
		As the code is now, using _RCA instead _RC to determine the initial position to start the drawing,
		that wouldn't happen until cycle 17. */

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

	_IFDEBUG debugDrawPixelAt (cb);

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
			continue; // The pixel is not still visible...
		if (pp >= 320)
			break; // No more pixels to draw...

		size_t iBy = ((size_t) pp) >> 3; // To determine the byte...
		size_t iBt = 7 - (((size_t) pp) % 8); // From MSB to LSB...

		if (_vicGraphicInfo._graphicData [iBy].bit (iBt))
		{
			result._collisionGraphicData.setBit (7 - i, true);

			result._foregroundColorData [i] = _vicGraphicInfo._idleState 
				? 0x00 // In idle state the color is always 0...
				: (unsigned int) (_vicGraphicInfo._colorData [iBy].value () & 0x0f /** Useful nibble. */);
		}

		// When false, it is background...
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

	for (unsigned short i = 0 ; i < 8 /** To paint always 8 pixels. */; i++)
	{
		int pp = cb + i;
		if (pp < 0)
			continue; // The pixel is not still visible...
		if (pp >= 320)
			break; // No more pixels to draw...

		size_t iBy = 0;
		iBy = ((size_t) pp) >> 3; 
		size_t iBt = 3 - ((((size_t) pp) % 8) >> 1);
		unsigned char cs = (_vicGraphicInfo._graphicData [iBy].value () >> (iBt << 1)) & 0x03; // 0, 1, 2 or 3

		// If 0, the pixel should be drawn (and considered) as background 
		// and it is already the default status tha comes from the parent method...
		if (cs == 0x00)
			continue;

		// The way the pixels are going to be drawn will depend on the information in the color memory
		// If the most significant bit of the low significant nibble of the color memory is set to 1
		// the data will be managed in a monocolor way...
		if ((_vicGraphicInfo._colorData [iBy] & 0x08) == 0x00 || 
			 _vicGraphicInfo._idleState) // also when idle state...
		{
			unsigned int fc = 
				(inv || _vicGraphicInfo._idleState)
					? 0x00 // When invalid or idle state all pixels are black...
					: _vicGraphicInfo._colorData [iBy].value () & 0x07;

			// ...and remember we are dealing with pairs of pixels...

			switch (cs)
			{
				case 0x01:
					{
						if ((pp % 2) == 1) // El pixel to be drawn has to be odd...
							result._backgroundColorData [i] = fc;
					}

					break;

				case 0x02:
					{
						if ((pp % 2) == 0) // El pixel to be draw has to be even...
						{
							result._collisionGraphicData.setBit (7 - i, true);

							result._foregroundColorData [i] = fc;
						}
					}

					break;

				case 0x03:
					{
						result._collisionGraphicData.setBit (7 - i, true);

						result._foregroundColorData [i] = fc;
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
				result._backgroundColorData [i] = fc;
			// ..while the other two are part of the foreground...
			// ..and also included in the collision info!
			else
			{
				result._collisionGraphicData.setBit (7 - i, true);

				result._foregroundColorData [i] = fc;
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
			continue; // The pixel is not still visible...
		if (pp >= 320)
			break; // No more pixels to draw...

		size_t iBy = ((size_t) pp) >> 3 /** To determine the byte. */;
		size_t iBt = 7 - (((size_t) pp) % 8); /** From MSB to LSB. */
		// The color of the pixel 0 is determined by the 2 MSBites of the char code...
		bool bS = _vicGraphicInfo._graphicData [iBy].bit (iBt); // To know whether the bit is 1 or 0...
		unsigned int cs = ((_vicGraphicInfo._screenCodeData [iBy].value () & 0xc0) >> 6) & 0x03; // 0, 1, 2, or 3
		unsigned int fc = _vicGraphicInfo._idleState 
			? 0x00 // In idle state the color is always 0...
			: (bS 
				? (_vicGraphicInfo._colorData [iBy].value () & 0x0f) 
				: _VICIIRegisters -> backgroundColor (cs));

		if (bS)
		{
			result._collisionGraphicData.setBit (7 - i, true);

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
			continue; // The pixel is not still visible...
		if (pp >= 320)
			break; // No more pixels to draw...

		size_t iBy = ((size_t) pp) >> 3; // To determine the byte...
		size_t iBt = 7 - (((size_t) pp) % 8); // From MSB to LSB...
		bool bS = _vicGraphicInfo._graphicData [iBy].bit (iBt);
		unsigned int fc = 
			(inv || _vicGraphicInfo._idleState)
				? 0x00 // When invalid or idle state, all pixels are black...
				: bS 
					? (_vicGraphicInfo._screenCodeData [iBy].value () & 0xf0) >> 4	// If the bit is 1, the color is determined by the MSNibble
					: (_vicGraphicInfo._screenCodeData [iBy].value () & 0x0f);		// ...and for LSNibble if it is 0...

		if (bS)
		{
			result._collisionGraphicData.setBit (7 - i, true);

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

	for (unsigned short i = 0 ; i < 8 /** To paint always 8 pixels. */; i++)
	{
		int pp = cb + i;
		if (pp < 0) // The pixel is not still visible...
			continue;
		if (pp >= 320)
			break; // No more pixels to draw...

		size_t iBy = 0;
		iBy = ((size_t) pp) >> 3; 
		size_t iBt = 3 - ((((size_t) pp) % 8) >> 1);
		unsigned char cs = (_vicGraphicInfo._graphicData [iBy].value () >> (iBt << 1)) & 0x03; // 0, 1, 2 or 3

		// If 0, the pixel should be drawn (and considered) as background 
		// and it is already the default status tha comes from the parent method...
		if (cs == 0x00)
			continue;

		unsigned fc = // The value 0x00 is not tested....
				(inv || _vicGraphicInfo._idleState)
					? 0x00 // When invalid or idle state all pixels are black...
					: (cs == 0x01) // The color is the defined in the video matrix, high nibble...
						? (_vicGraphicInfo._screenCodeData [iBy].value () & 0xf0) >> 4
						: ((cs == 0x02) // The color is defined in the video matrix, low nibble...
							? (_vicGraphicInfo._screenCodeData [iBy].value () & 0x0f)
							: (_vicGraphicInfo._colorData [iBy].value () & 0x0f)); // The color is defined in color matrix...

		// The combination "01" is managed as background also...
		// ...the 0x00 has already been jumped an then treated as background!
		if (cs == 0x01)
			result._backgroundColorData [i] = fc;
		// ...while the rest as managed as foreground...
		else
		{
			result._collisionGraphicData.setBit (7 -i, true);

			result._foregroundColorData [i] = fc;
		}
	}

	return (result);
}

// ---
MCHEmul::UByte COMMODORE::VICII::drawSpriteOver (size_t spr, unsigned int* d, size_t* dO)
{
	return ((_vicSpriteInfo [spr]._graphicsLineSprites.size () == 0)
		? MCHEmul::UByte::_0
		: (_VICIIRegisters -> spriteMulticolorMode (spr)
			? drawMultiColorSpriteOver (_raster.currentColumn (), _vicGraphicInfo._ROW, spr, d, dO)
			: drawMonoColorSpriteOver (_raster.currentColumn (), _vicGraphicInfo._ROW, spr, d, dO)));
}

// ---
MCHEmul::UByte COMMODORE::VICII::drawMonoColorSpriteOver (unsigned short c, unsigned short r, size_t spr, 
	unsigned int* d, size_t* dO)
{
	MCHEmul::UByte result = MCHEmul::UByte::_0;

	// Horizontal info about the sprite
	unsigned short dW	= _VICIIRegisters -> spriteDoubleWidth (spr) ? 2 : 1;
	unsigned short x	= (_vicSpriteInfo [spr]._drawing) 
		? _vicSpriteInfo [spr]._xS : (_VICIIRegisters -> spriteXCoord (spr) + 4);
	unsigned short wX	= 24 /** normal width in pixels. */ * dW;
	unsigned short dW8	= dW << 3; // 8 or 16
	unsigned short wY	= _vicSpriteInfo [spr]._expansionY ? 42 : 21;

	// When the code reaches this position _vicSpriteInfo._active is true
	// Which means the bits have to be shifted from the composition register
	// No need to check whether the y position is within or not the limits
	if ((c + 8 /** pixels */) < x || c >= (x + wX))
		return (result); // Not visible in the horizontal zone...

	for (unsigned short i = 0; i < 8 /** always to draw 8 pixels */; i++)
	{
		unsigned short pp = (c + i); // The exact pixel to draw...
		if (pp < x)
			continue; // Not visible...
		if (pp >= (x + wX))
			break; // No more draws...

		// The sprite starts to be drawn...
		// ...if it isn0't already before...
		if (!_vicSpriteInfo [spr]._drawing)
		{
			_vicSpriteInfo [spr]._drawing = true; _vicSpriteInfo [spr]._xS = x; 

			_IFDEBUG debugDrawSpriteAt (spr, x, r);
		}

		// To determine the initial byte (iBy) and bit (iBt) with the info about the sprite...
		// The bit moves from 7 to 0, and the byte increases...
		size_t iBy = (size_t) ((pp - x) / dW8);
		size_t iBt = (size_t) (7 - (((pp - x) % dW8) / dW));

		// Once the bit has been used, it is put back to false...
		// ..simulating the behaviour of the shift register used by the VIC
		// ...to select the bit to paint...
		int iByA = (int) iBy;
		int iBtA = ((int) iBt) + 1;
		if (iBtA >= 8) { iBtA = 0; iByA--; }
		if (iBtA > 0)
			_vicSpriteInfo [spr]._graphicsLineSprites [(size_t) iByA].
				setBit ((size_t) iBtA, false);

		// Draw the border if needed...
		// The border is drawn before the pixel itself to appreciate the sprite fully!
		if (_drawSpritesBorder) 
		{
			// Because the position 50 correspond at the raster line position 51...
			unsigned short y = _VICIIRegisters -> spriteYCoord (spr) + 1;
			if ((pp == x || pp == (x + wX - 1)) || 
				(r == y || r == (y + wY - 1)))
					{ d [i] = 32 /** cyan. */; dO [i] = spr; }
		}

		if (!_vicSpriteInfo [spr]._graphicsLineSprites [iBy].bit (iBt))
			continue; // The point is not visible...

		result.setBit (7 - i, true);
		d [i] = _VICIIRegisters -> spriteColor (spr);
		dO [i] = spr;
	}

	return (result);
}

// ---
MCHEmul::UByte COMMODORE::VICII::drawMultiColorSpriteOver (unsigned short c, unsigned short r, size_t spr, 
	unsigned int* d, size_t* dO)
{
	MCHEmul::UByte result = MCHEmul::UByte::_0;

	// Horizontal info about the sprite
	unsigned short dW	= _VICIIRegisters -> spriteDoubleWidth (spr) ? 2 : 1;
	unsigned short x	= (_vicSpriteInfo [spr]._drawing) 
		? _vicSpriteInfo [spr]._xS : _VICIIRegisters -> spriteXCoord (spr) + 4;
	unsigned short wX	= 24 /** normal width in pixels. */ * dW;
	unsigned short dW8	= dW << 3; // 8 or 16
	unsigned short dW2  = dW << 1; // 4 or 2
	unsigned short wY	= _vicSpriteInfo [spr]._expansionY ? 42 : 21;

	// When the code reaches this position _vicSpriteInfo._active is true
	// Which means the bits have to be shifted from the composition register
	// No need to check whether the y position is within or not the limits
	if ((c + 8 /** pixels */) < x || c >= (x + wX))
		return (result); // Not visible in the horizontal zone...

	for (unsigned short i = 0; i < 8 /** always to draw 8 pixels. */; i++)
	{
		unsigned short pp = (c + i); // The exact pixels to draw...
		if (pp < x)
			continue; // Not visible...
		if (pp >= (x + wX))
			break; // No more draws...

		// The sprite starts to be drawn...
		// ...if it isn0't already before...
		if (!_vicSpriteInfo [spr]._drawing)
		{ 
			_vicSpriteInfo [spr]._drawing = true; _vicSpriteInfo [spr]._xS = x; 

			_IFDEBUG debugDrawSpriteAt (spr, x, r);
		}

		// To determine the initial byte (iBy) and bit (iBt) with the info about the sprite...
		// The bit to select moves from 0 to 3, represeting the pair of bits (0,1), (2,3), (4,5), (6,7)
		size_t iBy = (size_t) ((pp - x) / dW8);
		size_t iBt = (size_t) (3 - (((pp - x) % dW8) / dW2));

		// Once the bits have been used, it is put back to false...
		// ..simulating the behaviour of the shift register...
		// ...used within the VIC to paint...
		int iByA = (int) iBy;
		int iBtA = ((int) iBt) + 1;
		if (iBtA >= 4) { iBtA = 0; iByA--; }
		if (iBtA > 0)
		{
			_vicSpriteInfo [spr]._graphicsLineSprites [(size_t) iByA].
				setBit (((size_t) iBtA << 1), false);
			_vicSpriteInfo [spr]._graphicsLineSprites [(size_t) iByA].
				setBit (((size_t) iBtA << 1) + 1, false);
		}

		// Draw the border if needed...
		// The border is drawn before the pixel itself to appreciate the sprite fully!
		if (_drawSpritesBorder)
		{
			// Because the position 50 correspond at the raster line position 51...
			unsigned short y = _VICIIRegisters -> spriteYCoord (spr) + 1;
			if ((pp == x || pp == (x + wX - 1)) ||
				(r == y || r == (y + wY - 1)))
					{ d [i] = 32 /** cyan. */; dO [i] = spr; }
		}

		unsigned char cs = 
			(_vicSpriteInfo [spr]._graphicsLineSprites [iBy].value () >> (iBt << 1)) & 0x03;
		if (cs == 0)
			continue; // The point has no color...

		result.setBit (7 - i, true);

		d [i] = 
			(unsigned int) ((cs == 0x01) 
				? _VICIIRegisters -> spriteSharedColor (0)
				: ((cs == 0x02) 
					? _VICIIRegisters -> spriteColor (spr)
					: _VICIIRegisters -> spriteSharedColor (1)));
;
		dO [i] = spr;
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
		if (cT._backgroundColorData [i] != MCHEmul::_U0)
			screenMemory () -> setPixel (pos, (size_t) dC._RR, cT._backgroundColorData [i]);

		// If that sprite didn't exist at that pixel..
		if (cT._spriteColorOwner [i] == MCHEmul::_S0)
		{
			// ...and there were a foreground not transparent on top, 
			// the sprite pixel would be drawn!
			if (cT._foregroundColorData [i] != MCHEmul::_U0)
				screenMemory () -> setPixel (pos, (size_t) dC._RR, cT._foregroundColorData [i]);
		}
		// But, if the sprite existed...
		else
		{
			// ...the sprite would be drawn...
			screenMemory () -> setPixel (pos, (size_t) dC._RR, cT._spriteColor [i]);
			// ..and if it had less priority than the foreground, 
			// the foreground is drawn on top...
			if (_VICIIRegisters -> spriteToForegroundPriority (cT._spriteColorOwner [i]) &&
				cT._foregroundColorData [i] != MCHEmul::_U0)
					screenMemory () -> setPixel (pos, (size_t) dC._RR, cT._foregroundColorData [i]);
		}
	}
}

// ---
void COMMODORE::VICII::detectCollisions (const DrawResult& cT)
{
	// Now it is time to detect collisions...
	// First among the graphics and the sprites
	bool cGS = false;
	for (size_t i = 0; i < 8 && !cGS; i++)
		// ...at the first collision detected, the check stops...
		if ((cGS = (cT._collisionSpritesData [i].value () & cT._collisionGraphicData.value ()) != 0x00)) 
			_VICIIRegisters -> setSpriteCollisionWithDataHappened (i);
	if (cGS) 
		_VICIIRegisters -> activateSpriteCollisionWithDataIRQ ();
	
	// ...and among sprites...
	bool cSS = false;
	for (size_t i = 0; i < 8; i++)
	{
		for (size_t j = i + 1; j < 8; j++)
		{
			if ((cSS = 
					((cT._collisionSpritesData [i].value () & 
					  cT._collisionSpritesData [j].value ()) != 0x00)))
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
COMMODORE::VICII_PAL::VICII_PAL (int intId, MCHEmul::PhysicalStorageSubset* cR, 
		const MCHEmul::Address& cRA, int vV)
	: COMMODORE::VICII (intId, cR, cRA,
		 _VRASTERDATA, _HRASTERDATA, vV, COMMODORE::VICII_PAL::_CYCLESPERRASTERLINE,
		 { { "Name", "VIC-II (PAL) Video Chip Interface II" },
		   { "Code", "6569(NMOS 63 cycles)/8565(HMOS)/8566(HMOS in C128)" },
		   { "Manufacturer", "MOS Technology INC/Commodore Semiconductor Group (CBM)"},
		   { "Year", "1982-1983 (depending on version)" } })
{
	// This value - the initial position in the row divided by 8 must have a rest of 0
	_IRQrasterPosition = 404;
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

					_IFDEBUG debugReadingSpriteInfo (nSR);
				}
			}

			break;

		// Manages the situation of the border...
		case 63:
			{ 
				actualizeVerticalBorderStatus ();
			}

			break;

		default:
			break;
	}

	return (result);
}

// ---
COMMODORE::VICII_NTSC::VICII_NTSC (int intId, MCHEmul::PhysicalStorageSubset* cR,
		const MCHEmul::Address& cRA, int vV)
	: COMMODORE::VICII (intId, cR, cRA,
		 _VRASTERDATA, _HRASTERDATA, vV, 64,
		 { { "Name", "VIC-II (NTSC) Video Chip Interface II" },
		   { "Code", "6567R56A(NMOS 64 cycles)/8582(HMOS)/8564(HMOS in C128)" },
		   { "Manufacturer", "MOS Technology INC/Commodore Semiconductor Group (CBM)"},
		   { "Year", "1983" } })
{
	// This value - the initial position in the row divided by 8 must have a rest of 0
	_IRQrasterPosition = 412;
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

					_IFDEBUG debugReadingSpriteInfo (nSR);
				}

				// Manages the situation of the border...
				actualizeVerticalBorderStatus ();
			}

			break;

		default:
			break;
	}

	return (result);
}

// ---
void COMMODORE::VICII::debugDisconnected (MCHEmul::CPU* cpu)
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeCompleteLine ("VICII", cpu -> clockCycles (), "Disconnected");
}

// ---
void COMMODORE::VICII::debugVICIICycle (MCHEmul::CPU* cpu, unsigned int i)
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeCompleteLine (className (), cpu -> clockCycles () - i, "Info Cycle",
		{ { "Raster position",
			"Column=" + std::to_string (_raster.currentColumn ()) + "(" + 
				std::to_string (_raster.currentColumnAtBase0 ()) + ")," +
			"Row=" + std::to_string (_raster.currentLine ()) + "(" + 
				std::to_string (_raster.currentLineAtBase0 ()) + ")" },
		  { "Internal",
			"IDLE=" + std::to_string (_vicGraphicInfo._idleState) + "," +
			"VCBASE=" + std::to_string (_vicGraphicInfo._VCBASE) + "," +
			"VC=" + std::to_string (_vicGraphicInfo._VC) + "," +
			"VLMI=" + std::to_string (_vicGraphicInfo._VLMI) + "," +
			"RC=" + std::to_string (_vicGraphicInfo._RC) + "," +
			"ROW=" + std::to_string (_vicGraphicInfo._ROW) + "," +
			"Badline=" + std::to_string (_newBadLineCondition) + "," +
			"Cycle=" + std::to_string (_cycleInRasterLine) },
		  { "Border", 
			"Main=" + std::to_string (_vicGraphicInfo._ffMBorder) + "," +
			"Vertical=" + std::to_string (_vicGraphicInfo._ffVBorder) + "," +
			"Border=" + std::to_string (_vicGraphicInfo._ffMBorderBegin) + "," +
			"Pixels=" + std::to_string (_vicGraphicInfo._ffMBorderPixels) },
		  { "Graphics mode",
			std::to_string ((int) _VICIIRegisters -> graphicModeActive ()) },
		  { "Memory location", 
			"Bank=" + std::to_string ((int) _VICIIRegisters -> bank ()) + "," +
			"Screen=$" + MCHEmul::removeAll0 (_VICIIRegisters -> screenMemory ().asString
				(MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) + "," +
			"Characters=$" + MCHEmul::removeAll0 (_VICIIRegisters -> charDataMemory ().asString
				(MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) + "," +
			"Bitmap=$" + MCHEmul::removeAll0 (_VICIIRegisters -> bitmapMemory ().asString
				(MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) } });
}

// ---
void COMMODORE::VICII::debugBadLine ()
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeLineData ("Bad line situation");
}

// ---
void COMMODORE::VICII::debugReadingSpriteInfo (size_t nS)
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeLineData ("Reading info sprite " + 
		std::to_string (nS) + "[" + _vicSpriteInfo [nS]._graphicsLineSprites.asString 
			(MCHEmul::UByte::OutputFormat::_HEXA, ' ') + "]");
}

// ---
void COMMODORE::VICII::debugSpriteDrawFinishes (size_t nS)
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeLineData ("Sprite draw finishes " + std::to_string (nS));
}

// ---
void COMMODORE::VICII::debugSpriteDrawToStart (size_t nS)
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeLineData ("Sprite active to be drawn " + std::to_string (nS));
}

// ---
void COMMODORE::VICII::debugReadingVideoMatrix ()
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeLineData ("Reading Video Matrix & Color RAM [" +
					_vicGraphicInfo._lastScreenCodeDataRead.asString (MCHEmul::UByte::OutputFormat::_HEXA) + ", " +
					_vicGraphicInfo._lastColorDataRead.asString (MCHEmul::UByte::OutputFormat::_HEXA) + "]");
}

// ---
void COMMODORE::VICII::debugReadingGraphics ()
{
	assert (_deepDebugFile != nullptr);
	
	_deepDebugFile -> writeLineData ("Reading Graphics [" + 
		_vicGraphicInfo._lastGraphicDataRead.asString (MCHEmul::UByte::OutputFormat::_HEXA) + "]");
}

// ---
void COMMODORE::VICII::debugVideoNoActiveAt (unsigned short cav)
{
	assert (_deepDebugFile != nullptr);
	
	_deepDebugFile -> writeLineData ("Video no active at pixel " + std::to_string (cav) +
		", foreground color " + std::to_string ((unsigned int) _VICIIRegisters -> foregroundColor ()));
}

// ---
void COMMODORE::VICII::debugDrawPixelAt (unsigned short cb)
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeLineData ("Drawing pixels at " + std::to_string (cb) +
		", background color " + std::to_string ((unsigned int) _VICIIRegisters -> backgroundColor ()));
}

// ---
void COMMODORE::VICII::debugDrawSpriteAt (size_t nS, unsigned short x, unsigned short r)
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeLineData ("Drawing sprite " + std::to_string (nS) +
		" at " + std::to_string (x) + "," + std::to_string (r));
}

// ---
MCHEmul::InfoStructure COMMODORE::VICII::VICGraphicInfo::getInfoStructure () const
{
	MCHEmul::InfoStructure result;

	result.add ("VCBASE",	_VCBASE);
	result.add ("VC",		_VC);
	result.add ("RC",		_RC);
	result.add ("VLMI",		_VLMI);
	result.add ("ROW",		_ROW);
	result.add ("IDLE",		std::string (_idleState ? "YES" : "NO"));

	return (result);
}
