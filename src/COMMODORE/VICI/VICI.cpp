#include <COMMODORE/VICI/VICI.hpp>

// ---
/** This values have been calculated from the info at: http://tinyvga.com/6561. */
// PAL SYSTEM
/** VERTICAL RASTER: \n
	---------------- \n
	312 lines in total (as it is in the documentation). \n
	284 visible lines (also in the documentation), so 28 invisible lines (312 - 284 = 28). \n
	8 * 22 = 176 screen lines in total. \n
	The granularity of the vertical movement of the screen is 2 pixels. \n
	When $01 = 14 the first line of the screen is visible (at the second visible line), 
	so all invisible lines are at the top. \n
	And when it is 155 only 1 pixel of the first row is visible,
	so it there will have moved = (155 - 14 + 1) * 2 = 284 pixels down. \n
	The usual value of the register $01 is 38, so the up margin is = ((38-14+1)*2)-1(the one visible before)=49pixels. \n
	And the down margin = 284 - (49 + 176) = 59. \n
	And the rest up tpo 312 is not visible = 312 - (28 + 49 + 176 + 59) = 0!
	So: 0,28,77,252,311,311. Total 312. \n
	\n
=>	However as the drawing is controlled by internal counters, 
	the only thing that matters actually is is total visible positions = 312 - 28 = 284 
	and the size of the non visible zone. = 28
	So: 0,28,28,311,311,311. Total 312. \n
	\n
	HORIZONTAL RASTER:\n
	------------------\n
	In one side:
	The VICI is connected to a clock at 4.433.618Hz. \n
	It reduces the speed and set CPU Clock at /4 speed = 1.108.404Hz. \n
	It is said that the screen is 233 pixels width (visible part). \n
	We could initially suppose than the screen has then (233 - (23 * 8)) / 2 = 24,5 
	pixels border per side in average. Or 23 and 25 each it was centered.
	\n
	In the other side:
	Every line takes 71 CPU cycles. \n
	The sensibility of the screen movement (looking registers $00 e.g) is 4 pixels. \n
	So in each CPU cycle only 4 pixels should be drawn. \n
	It is said (in the documentation) that when the value of the register $00 is $00, and the number of columns is 22,
	the first visible pixel of the display is the 4th pixel of the 3rd column. \n
	When that value is 5, the first pixel seen is the first pixel of the first column.
	If the screen started to be drawn at the very first cycle in each line,
	we could say that there would be 20 pixels hidden. and because the usual value of this register is 12, 
	the size of the left border would be = 32 pixels (12 - 5 + 1) * 4. \n
	This number doesn't match with the one that roughly was calculated above (24). 
	The number of pixels of the usual screen are in this case 176 pixels (22 * 8).
	So the right margin is: 233 - (32 + 176) = 25 pixels.
	\n
	Finally: \n
	(71 * 4) - (20 HIDDEN LEFT + 32 MARGIN + 176 SCREEN + 25 MARGIN) = 31 hidden on the right... \n
	I would like to "duplicate" the size of the horizontal screen, so: \n
	(71 * 8) - (40 + 64 + 352 + 50) = 62 hidden on the right... \n
	Or: 0,40,104,455,505,567. Total 568.
	\n
=>	However, as the drawing is controlled by internal counters, 
	the only thing that matters actually is is total visible positions = 233 * 2 = 466 
	and the size of the non visible zone. = 40
	So: 0,40,40,506,506,567. Total 568. */
const MCHEmul::RasterData COMMODORE::VICI_PAL::_VRASTERDATA	(0, 28, 28, 311, 311, 311, 311, 312, 0, 0);
const MCHEmul::RasterData COMMODORE::VICI_PAL::_HRASTERDATA	(0, 40, 40, 506, 506, 567, 567, 568, 0, 0);
// NTSC SYSTEM
/** VERTICAL RASTER: \n
	---------------- \n
	261 lines in total (as it is in the documentation). \n
	233 visible lines (also in the documentation), so 28 invisible lines (312 - 284 = 28). \n
	8 * 22 = 176 screen lines in total. \n
	The granularity of the vertical movement of the screen is 2 pixels. \n
	When $01 = 14 the first line of the screen is visible (at the second visible line), 
	so all invisible lines are at the top. \n
	And when it is 130 only 1 pixel of the first row is visible,
	so it there will have moved = (130 - 14 + 1) * 2 = 234 pixels down. \n
	The usual value of the register $01 is 38, so the up margin is = ((38-14+1)*2)-1(the one visible before)=49pixels. \n
	And the down margin = 233 - (49 + 176) = 8. \n
	And the rest up too 261 is not visible = 261 - (28 + 49 + 176 + 8) = 0!
	So: 0,28,77,252,260,260 \n
	\n
=>	However as the drawing is controlled by internal counters, 
	the only thing that matters actually is is total visible positions = 261 - 28 = 233 
	and the size of the non visible zone. = 28
	So: 0,28,28,260,260,260. 
	\n
	HORIZONTAL RASTER:\n
	------------------\n
	In one side:
	The VICI is connected to a clock at 14.318.181Hz. \n
	It reduces the speed and set CPU Clock at /14 speed = 1.020.000Hz. \n
	It is said that the screen is 210 pixels width (visible part). \n
	We could initially suppose than the screen has then (210 - (23 * 8)) / 2 = 13 pixels border per side.
	\n
	In the other side:
	Every line takes 65 CPU cycles. \n
	The sensinility of the screen movement (looking registers $00 e.g) is 4 pixels. \n
	So in each CPU cycle only 4 pixels should be drawn. \n
	It is said (in the documentation) that when the value of the register $00 is $00, and the number of columns is 22,
	the first visible pixel of the display is the 4th pixel of the 1st column. \n
	When that value is 1, the first pixel seen is the first pixel of the first column.
	If the screen started to be drawn at the very first cycle in each line,
	we could say that there would be 4 pixels hidden and, because the usual value of this register is 6,
	the size of the left border would be = 16 pixels (5 - 1 + 1) * 4 \n
	This number doesn't match with the one that roughly was calculated above (13). 
	The number of pixels of the usual screen are in this case 176 pixels (22 * 8).
	So the right margin is: 210 - (16 + 176) = 18 pixels.
	\n
	Finally: \n
	(65 * 4) - (4 HIDDEN LEFT + 16 MARGIN + 176 SCREEN + 18 MARGIN) = 46 hidden on the right... \n
	I would like to "duplicate" the size of the horizontal screen, so: \n
	(65 * 8) - (8 + 32 + 352 + 36) = 92 hidden on the right... \n
	Or: 0,8,40,391,427,519.
	\n
=>	However, as the drawing is controlled by internal counters, 
	the only thing that matters actually is is total visible positions = 210 * 2 = 420 
	and the size of the non visible zone. = 8
	So: 0,8,8,427,427,519. */
const MCHEmul::RasterData COMMODORE::VICI_NTSC::_VRASTERDATA (0, 28, 28, 260, 260, 260, 260, 261, 0, 0);
const MCHEmul::RasterData COMMODORE::VICI_NTSC::_HRASTERDATA (0, 8, 8, 427, 427, 519, 519, 520, 0, 0);
const MCHEmul::Address COMMODORE::VICI::_MEMORYPOSOFF = MCHEmul::Address ({ 0x00, 0x1c }, true /* already in little - endian. */);

// ---
COMMODORE::VICI::SoundFunction::SoundFunction (MCHEmul::SoundLibWrapper* sW)
	: MCHEmul::SoundChip (_ID, 
		{ { "Name", "SoundFunction" },
		  { "Code", "Inside 6560 (NTSC), 6561/6561E/6561-101 (PAL)" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Year", "1977" } },
		sW),
	  _lastCPUCycles (0)
{ 
	// Take care that the sound emulation library was not null at all...
	// ...and also belonging to the right type..
	assert (dynamic_cast <COMMODORE::VICISoundLibWrapper*> (soundWrapper ()) != nullptr);

	setClassName ("VICISoundFunction");
}

// ---
bool COMMODORE::VICI::SoundFunction::initialize ()
{
	if (!MCHEmul::SoundChip::initialize ())
		return (false);

	_lastCPUCycles = 0;

	return (true);
}

// ---
bool COMMODORE::VICI::SoundFunction::simulate (MCHEmul::CPU* cpu)
{
	// First time?
	if (_lastCPUCycles == 0)
	{ 
		_lastCPUCycles = cpu -> clockCycles (); // Nothing to do...

		return (true);
	}

	if (soundWrapper () != nullptr)
	{
		for (unsigned int i = (cpu -> clockCycles () - _lastCPUCycles); i > 0 ; i--)
		{
			_IFDEBUG debugVICISoundCycle (cpu, i);

			// The number of bytes that can come from the wrapper might not be fixed...
			MCHEmul::UBytes data;
			if (soundWrapper () -> getData (cpu, data))
			{
				for (size_t j = 0; j < data.size (); j++)
				{
					char dt = (char) data [j].value ();
					if (soundMemory () -> addSampleData (&dt, sizeof (char)))
						MCHEmul::SoundChip::notify (MCHEmul::Event (_SOUNDREADY));
				}
			}
		}
	}

	_lastCPUCycles = cpu -> clockCycles ();

	return (true);
}

// ---
MCHEmul::InfoStructure COMMODORE::VICI::SoundFunction::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::SoundChip::getInfoStructure ());

	result.remove ("Memory"); // This info is not neccesary...
	result.add ("SoundLibWrapper",	std::move (soundWrapper () -> getInfoStructure ())); // To know which library is behing...

	return (result);
}

// ---
void COMMODORE::VICI::SoundFunction::debugVICISoundCycle (MCHEmul::CPU* cpu, unsigned int i)
{
	assert (_deepDebugFile != nullptr);

	MCHEmul::InfoStructure sidStr = std::move (soundWrapper () -> getInfoStructure ());
	MCHEmul::InfoStructure voicesStr = std::move (sidStr.infoStructure ("VOICES"));
	
	// Create the structure of attributes...
	MCHEmul::Attributes voicesAttrs;
	for (const auto& e : voicesStr.infoStructures ())
	{ 
		std::string w;
		MCHEmul::InfoStructure wStr = std::move (e.second.infoStructure ("WAVES"));
		for (size_t ct = 0; ct < 3; ct++)
			w += ((ct != 0) ? "," : "") + std::string ("Wave ") + std::to_string (ct) + "=" +
				wStr.infoStructure (std::to_string (ct)).attribute ("TYPEANDFREQUENCY");
		std::string v = e.second.attribute ("ADSR");
		voicesAttrs.insert (MCHEmul::Attributes::value_type ("Voice " + e.first, 
			e.second.attribute ("ACTIVE") + "," + v + "," + w));
	}

	_deepDebugFile -> writeCompleteLine (className (), 
		cpu -> clockCycles () - i, "Info Cycle", voicesAttrs);
}

// ---
COMMODORE::VICI::VICI (const MCHEmul::RasterData& vd, const MCHEmul::RasterData& hd, 
		int vV, unsigned short cRL, MCHEmul::SoundLibWrapper* sW, const MCHEmul::Attributes& attrs)
	: MCHEmul::GraphicalChip (_ID, 
		{ { "Name", "VICI" },
		  { "Code", "6560 (NTSC), 6561/6561E/6561-101 (PAL)" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Year", "1977" } }),
	  _soundFunction (new COMMODORE::VICI::SoundFunction (sW)),
	  _VICIRegisters (nullptr),
	  _VICIView (vV),
	  _cyclesPerRasterLine (cRL),
	  _raster (vd, hd, 8 /** The step is here 8 pixels */),
	  _maxXVisible (0), // Defined better later...
	  _lastCPUCycles (0),
	  _format (nullptr),
	  _cycleInRasterLine (0),
	  _lastVBlankEntered (false),
	  _scrX1 (0), _scrY1 (0), _scrX2 (0), _scrY2 (0)
{
	assert (_cyclesPerRasterLine >= 65);

	setClassName ("VICI");

	_format = SDL_AllocFormat (SDL_PIXELFORMAT_ARGB8888);

	unsigned short x1, y1, x2, y2;
	_raster.screenPositions (x1, y1, x2, y2);
	_maxXVisible = (x2 - x1);
}

// ---
COMMODORE::VICI::~VICI ()
{
	/**
		delete (_soundFunction);
		It is not invoked, because the sound function is
		managed as a different chip from the computer on regards simulation.
		That chip is owned by the computer and then destroyed by it. */

	SDL_FreeFormat (_format);
}

// ---
bool COMMODORE::VICI::initialize ()
{
	assert (MCHEmul::GraphicalChip::memoryRef () != nullptr);

	// The sound function is not initialized, because at Computer level
	// it is treated as an independent chip, that will be also initialized!
	if (!MCHEmul::GraphicalChip::initialize ())
		return (false);

	// Gets the memory block dedicated to the VICI
	if (!(_VICIRegisters = 
		dynamic_cast <COMMODORE::VICIRegisters*> 
			(memoryRef () -> subset (COMMODORE::VICIRegisters::_VICREGS_SUBSET))))
	{
		MCHEmul::GraphicalChip::_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	_raster.initialize ();

	_VICIRegisters -> setSoundLibWrapper (_soundFunction -> soundWrapper ());
	_VICIRegisters -> linkToRaster (&_raster);
	_VICIRegisters -> initialize ();

	_lastCPUCycles = 0;
	
	_cycleInRasterLine = 1;

	_lastVBlankEntered = false;

	_scrX1 = _scrY1 = _scrX2 = _scrY2 = 0;

	_vicGraphicInfo.initialize (_VICIRegisters);

	return (true);
}

// ---
bool COMMODORE::VICI::simulate (MCHEmul::CPU* cpu)
{
	// First time?
	if (_lastCPUCycles == 0)
	{ 
		_lastCPUCycles = cpu -> clockCycles (); // Nothing to do...

		return (true);
	}

	// Simulate the visulization...
	for (unsigned int i = (cpu -> clockCycles  () - _lastCPUCycles); i > 0; i--)
	{
		_IFDEBUG debugVICICycle (cpu, i);

		// Read the graphics and draw the visible zone, 
		// if it is the case...
		readGraphicsAndDrawVisibleZone ();
		// ...and then treat the current cycle...
		treatRasterCycle ();
		// ...and also moves 8 pixels right in the raster line and jump to other line is possible...
		if (_raster.moveCycles (1))
			_cycleInRasterLine = 1;

		// When the raster enters the non visible part of the screen,
		// a notification is sent (to the Screen class usually) 
		// just to draw the screen...
		if (_raster.isInFirstVBlankZone ())
		{
			if (!_lastVBlankEntered)
			{
				_lastVBlankEntered = true;

				notify (MCHEmul::Event (_GRAPHICSREADY));

				_vicGraphicInfo.initialize (_VICIRegisters);
			}
		}
		else
			_lastVBlankEntered = false;
	}

	_lastCPUCycles = cpu -> clockCycles ();

	return (true);

	// Note that the _soundFunction is not simulated here...
	// It is added like another different chip on regards simulation
	// The framework need a sound chip always!
}

// ---
MCHEmul::InfoStructure COMMODORE::VICI::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::GraphicalChip::getInfoStructure ());

	result.remove ("Memory"); // This info is not neccesary...
	result.add ("VICIRegisters",		std::move (_VICIRegisters -> getInfoStructure ()));
	result.add ("Raster",				std::move (_raster.getInfoStructure ()));
	result.add ("VICISoundFunction",	std::move (_soundFunction -> getInfoStructure ()));

	return (result);
}

// ---
MCHEmul::Strings COMMODORE::VICI::charsDrawSnapshot (MCHEmul::CPU* cpu,
	const std::vector <size_t>& chrs) const
{
	MCHEmul::Strings result;
	for (size_t i = 0; i < 256; i++)
	{
		if (!chrs.empty () && 
			std::find (chrs.begin (), chrs.end (), i) == chrs.end ())
			continue;

		MCHEmul::Address chrAdd = _VICIRegisters -> charDataMemory () + (i << 3);
		std::string dt = std::to_string (i) + "---\n$" +
			MCHEmul::removeAll0 (chrAdd.asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) + "\n";
		MCHEmul::UBytes chrDt = cpu -> memoryRef () -> values (chrAdd, 0x08);
		for (size_t j = 0; j < 8; j++) // 8 lines per character...
		{
			if (j != 0)
				dt += "\n";

			for (size_t l = 0; l < 8; l++)
				dt += ((chrDt [j].value () & (1 << (7 - l))) != 0x00) ? "#" : " ";
		}

		result.emplace_back (std::move (dt));
	}

	result.emplace_back ("---");

	return (result);
}

// ---
void COMMODORE::VICI::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	// To set the bank...
	switch (evnt.id ())
	{
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
MCHEmul::ScreenMemory* COMMODORE::VICI::createScreenMemory ()
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
void COMMODORE::VICI::readGraphicsAndDrawVisibleZone ()
{
	// The graphical info is always read!
	readVideoMatrixAndColorRAM ();
	readGraphicalInfo ();

	// Calculate the screen position, if it is needed...
	// If the screen position has not been reached yet, or it is already out of it, the previous value is maintained...
	calculateScreenPositions ();

	// ..and draws the graphics & border if it has to do so...
	if (_raster.isInVisibleZone ())
		drawVisibleZone ();
}

// ---
void COMMODORE::VICI::treatRasterCycle ()
{
	// At the very last cycle, the row is moved...
	if (_cycleInRasterLine == _cyclesPerRasterLine)
		_vicGraphicInfo.nextVertical (_VICIRegisters);
	else
	// In any raster cycles, expect the last one, the column is then moved...
	if (_cycleInRasterLine < _cyclesPerRasterLine)
		_vicGraphicInfo.nextHorizontal (_VICIRegisters);

	// ...then finally moves to the next cycle...
	_cycleInRasterLine++;
}

// ---
void COMMODORE::VICI::drawVisibleZone ()
{
	// These two variables are very key.
	// They hold the position of the raster within the VISIBLE ZONE.
	// It is the left up corner of the "computer screen" will be cv = 0 & rv = 0...
	unsigned short cv, rv; 
	_raster.currentVisiblePosition (cv, rv);

	// In other case...
	// Everyting is the color of the border initially...
	// ..and it will be covered with the screen later if needed..
	// This is how the VICI works...I guess!
	screenMemory () -> setHorizontalLine ((size_t) cv, (size_t) rv,
		(cv + 8) > _raster.visibleColumns () ? (_raster.visibleColumns () - cv) : 8, 
			_VICIRegisters -> borderColor ());

	// Now the information is drawn...
	if (_vicGraphicInfo.draw ())
		drawGraphics (
			{
				/** _ICD */ _raster.hData ().firstDisplayPosition (),
				/** _LCD */ _raster.hData ().lastDisplayPosition (),
				/** _RC	 */ cv,		// Where the horizontal raster is (not adjusted to 8) inside the window
				/** _IRD */ _raster.vData ().firstDisplayPosition (),
				/** _LRD */ _raster.vData ().lastDisplayPosition (),
				/** _RR	 */ rv		// Where the vertical raster is inside the window (it is not the chip raster line)
			});

	// The position related with the lightpen is calculated...
	unsigned short dx, dy;
	_raster.currentDisplayPosition (dx, dy);
	unsigned short lpx, lpy;
	_VICIRegisters -> currentLightPenPosition (lpx, lpy);
}

// ---
void COMMODORE::VICI::drawGraphics (const COMMODORE::VICI::DrawContext& dC)
{
	// First of all, the screen is drawn!...
	screenMemory () -> setHorizontalLine ((size_t) dC._RC, (size_t) dC._RR,
		(dC._RC + 8) > _raster.visibleColumns () ? (_raster.visibleColumns () - dC._RC) : 8, 
			_VICIRegisters -> backgroundColor ());

	// Multicolor mode?
	// where the color of the pixels are determined by combination 
	// of the pair of bits in the char data definition memory...
	// or just high resolution one?
	// where the color of the pixels is determined by any individual bit in
	// in the char definition memory...
	int cb = (int) dC._RC - (int) dC._ICD;
	drawResultToScreen (
		(((_vicGraphicInfo._colorData.value () & 0x08) == 0x00) // MSB low nibble on? = multicolorMode...
			? drawHighResolutionMode (cb)
			: drawMulticolorMode (cb)), dC);
}

// ---
COMMODORE::VICI::DrawResult COMMODORE::VICI::drawHighResolutionMode (int cb)
{
	COMMODORE::VICI::DrawResult result;

	for (unsigned short i = 0 ; 
			i < 8 /** To paint always 8 pixels but in blocks of 2. */; i += 2)
	{
		if ((cb + i) > _maxXVisible)
			break;

		if (_vicGraphicInfo._graphicData [3 - (i >> 1)])
		{
			// If the pixel is set and the inverse mode is not active...
			// ...the color will be in the foreground...
			if (!_VICIRegisters -> inverseMode ())
			{
				result._foregroundColorData [i] = 
				result._foregroundColorData [i + 1] = _vicGraphicInfo._colorData.value () & 0x07;
			}
		}
		else
		{
			// But if the pixels is off and the mode inverse is active...
			// ...the color will be in the background...
			if (_VICIRegisters -> inverseMode ())
			{
				result._backgroundColorData [i] = 
				result._backgroundColorData [i + 1] = _vicGraphicInfo._colorData.value () & 0x07;
			}
		}
	}

	return (result);
}

// ---
COMMODORE::VICI::DrawResult COMMODORE::VICI::drawMulticolorMode (int cb)
{
	COMMODORE::VICI::DrawResult result;

	for (unsigned short i = 0 ; 
			i < 8 /** To paint always 8 pixels but in blocks of 2. */; i += 2)
	{
		if ((cb + i) > _maxXVisible)
			break;

		unsigned char cs = 
			(_vicGraphicInfo._graphicData.value () >> (2 - (i >> 2))) & 0x03; // 0, 1, 2, 3?
		// The value 0 means background, and it has already been drawn!
		if (cs == 0x00)
			continue;

		// Depending on the value of the par of bits, the rule will be one or another...
		unsigned int fc = 
			(cs == 0x01)
				? _VICIRegisters -> borderColor ()
				: ((cs == 0x10)
					? _vicGraphicInfo._colorData.value () & 0x07
					: _VICIRegisters -> auxiliarColor ());

		result._foregroundColorData [i] = 
		result._foregroundColorData [i + 1] = fc;
	}

	return (result);
}

// ---
void COMMODORE::VICI::drawResultToScreen (const COMMODORE::VICI::DrawResult& cT, const COMMODORE::VICI::DrawContext& dC)
{
	// The four pixels to draw...
	for (size_t i = 0; i < 8; i++)
	{
		size_t pos = (size_t) dC._RC + i;

		// And then the background pixels...
		if (cT._backgroundColorData [i] != ~0)
			screenMemory () -> setPixel (pos, (size_t) dC._RR, cT._backgroundColorData [i]);
		// and the foreground ones finally...
		if (cT._foregroundColorData [i] != ~0)
			screenMemory () -> setPixel (pos, (size_t) dC._RR, cT._foregroundColorData [i]);
	}
}

// ---
void COMMODORE::VICI::debugVICICycle (MCHEmul::CPU* cpu, unsigned int i)
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeCompleteLine (className (), cpu -> clockCycles () - i, "Info Cycle",
		{ { "Raster", 
				std::to_string (_raster.currentColumnAtBase0 ()) + "," +
				std::to_string (_raster.currentLineAtBase0 ()) + "," +
				std::to_string (_cycleInRasterLine) },
		  { "Memory",
				"Screen=$" + MCHEmul::removeAll0 (_VICIRegisters -> screenMemory ().asString
					(MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) + "," +
				"Characters=$" + MCHEmul::removeAll0 (_VICIRegisters -> charDataMemory ().asString
					(MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) + "," +
				"Color=$" + MCHEmul::removeAll0 (_VICIRegisters -> colourMemory ().asString
				(MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) } });
}

// ---
COMMODORE::VICI_PAL::VICI_PAL (int vV, MCHEmul::SoundLibWrapper* wS)
	: COMMODORE::VICI (
		 _VRASTERDATA, _HRASTERDATA, vV, COMMODORE::VICI_PAL::_CYCLESPERRASTERLINE, wS,
		 { { "Name", "VIC-I (PAL) Video Chip Interface" },
		   { "Code", "6561/6561E/6561-101" },
		   { "Manufacturer", "MOS Technology INC/Commodore Semiconductor Group (CBM)"},
		   { "Year", "1977" } })
{
	// Nothing else to do...
}

// ---
COMMODORE::VICI_NTSC::VICI_NTSC (int vV, MCHEmul::SoundLibWrapper* wS)
	: COMMODORE::VICI (
		 _VRASTERDATA, _HRASTERDATA, vV, _CYCLESPERRASTERLINE, wS,
			{ { "Name", "VICI" },
			  { "Code", "6560 (NTSC)" },
			  { "Manufacturer", "Commodore Business Machines CBM" },
			  { "Year", "1977" } })
{
	// Nothing else to do...
}
