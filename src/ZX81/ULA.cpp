#include <ZX81/ULA.hpp>
#include <ZX81/ULARegisters.hpp>
#include <ZX81/ZX81.hpp>
#include <ZX81/OSIO.hpp>
#include <FZ80/CZ80.hpp>
#include <FZ80/NMIInterrupt.hpp>

// ---
/** Standard PAL requires 64us to draw a line. \n
	ULA runs in parallel with CPU and runs at double speed than the CPU. \n
	CPU runs at 3,25Mhz and ULA runs at 6,50Mhz. \n
	CPU need 207 cycles (CPU type) to draw a line, so ULA needs 414 cycles (ULA type). \n
	The structure of a line is the following: \n
	-----------------------------------------------------------------------------\n
	|Zone				|Time aprox	|ULA Cicles (at 6.5 MHz, double than CPU)	|\n
	|------------------	|-----------|-------------------------------------------|\n
	|**HSYNC**			|\~4.7 µs	|4.7 µs × 6.5 MHz ≈ **30.6 → 31 cycles**		|\n
	|**Back Porch**		|\~5.7 µs	|5.7 µs × 6.5 MHz ≈ **37.1 → 36 cycles**		|\n
	|**Video Active**	|\~52 µs		|52 µs × 6.5 MHz ≈ **338 cycles**				|\n
	|**Front Porch**	|\~1.5 µs	|1.5 µs × 6.5 MHz ≈ **9.75 → 9 cycles**		|\n
	|------------------	|-----------|-------------------------------------------|\n
	|**TOTAL**			|\~63.9 µs	|**414 cycles** (rounded)					|\n
	-----------------------------------------------------------------------------\n 
	Standard PAL requires 312 lines to draw a frame. \n
	The structure of the frame is the following: \n
	-----------------------------------------------------------------\n 
	|Zone							|Lines aprox.	|Time aprox.	|\n
	|-------------------------------|--------------	|-------------	|\n
	|**Vertical sync pulse** (VSYNC)|\~5 lines		|\~320 µs		|\n
	|**Vertical back porch**		|\~25 lines		|\~1.6 ms		|\n
	|**Zona de imagen activa**		|\~284 lines	|\~18.2 ms		|\n
	|**Vertical front porch**		|\~3 lines		|\~192 µs		|\n
	|-------------------------------|--------------	|-------------	|\n
	|**TOTAL**						|**312 líneas**	|**20 ms**		|\n
	-----------------------------------------------------------------\n 
	\n
	Where the border and the drawing zone that is seen is the phisical screen is actually, 
	cannot be defined in advance!, like happens in e.g. VICII (C64). \n
	It is really the software and the CPU the ones building up the screen: \n
	---
	When the address bus connected to ULA have bit 14 set (1) and bit 15 reset (0),
	the ULA will know that the CPU is accesing the video memory. \n
	Then uses the info at the data bus to load the shift register and start to shift it left (see below). \n
	Just after loading that info the ULA put back "0" in the data bus (unless the bit 6 of the data is on)
	to "confuse" the CPU that will execute a NOP operation. \n
	Take a look to ZX81::MemoryVideoCode class. \n
	If the bit 6 is set, the ULA doesn't change the value in the data bus (usually equivalent to HALT), \n
	The value read is used (together with the LINECNTRL register) to build up 
	the address where to read the value to load in the shift register. \n
	Then every clock (ULAwise) the register shifts one position left and draws a pixel according to the value shifted. \n
	When the shift ends, there won't be more pixels to draw until another byte were loaded. \n
	So, the drawing zone will start where the CPU starts to "run" things in the video memory.
	---
	In which line starts the drawing is not directly controlled actually! \n
	What it is really controlled is when the VSYNC starts only. \n
	It starts when the port FE is read. This is the zone of the ROM called VSYC Routine. \n
	When that happens a signal is sent to ULA to reset the vertical raster,
	LINECTRL is set to 0 and "locked" temporally. \n
	That routine VSYNC finishes, a OUT port FD is sent to activate (in ULA) the NMI interrupts. \n
	This port also indicates that the VSYNC zone has finishes and LINENCTRL is unlocked to count. \n
	----
	In MCHEmul, raster is thought to be "linked" to the different zones of a screen (border, drawing, etc.),
	And also used to draw the window that content the whole simulation. \n
	The only important value is where the visible parts starts because 
	it has to be aligned with the INT routine that draws the screen. \n
	142 "cycles" is enough to left a 8 width pixels border. \n
	After all being said, the only behaviour that could be use is the second one. */
/** 32 lines border up/down/left/right by default in both PAL & NTSC. */
const MCHEmul::RasterData ZX81::ULA_PAL::_VRASTERDATA
	(0, 46 /** 30 + 16 starts visible. */, 46 /** = starts screen. */, 269 /** +16+192+16 (24 char lines * 8) end screen. */,
	 269 /** = end visible part. */, 311 /** +26 retrace. */, 311 /** = end. */, 312 /* total */, 0, 0);
const MCHEmul::RasterData ZX81::ULA_PAL::_HRASTERDATA
	(0, 125 /** +128 starts visible. */, 125 /** = starts screen, */, 413 /** +19+256+14 (32 chars * 8) end screen. */,
	 413 /** = end visible part. */, 413 /** = retrace. */, 413 /** = end. */, 414 /** total. */, 0, 0);
// const MCHEmul::RasterData ZX81::ULA_NTSC::_VRASTERDATA (0, 15, 15, 246, 246, 261, 261, 262, 0, 0);
const MCHEmul::RasterData ZX81::ULA_NTSC::_VRASTERDATA (0, 22, 22, 245, 245, 261, 261, 262, 0, 0);
const MCHEmul::RasterData ZX81::ULA_NTSC::_HRASTERDATA (0, 125, 125, 413, 413, 413, 413, 414, 0, 0);

// ---
ZX81::ULA::ULA (const MCHEmul::RasterData& vd, const MCHEmul::RasterData& hd, ZX81::Type t, 
		int vV, const MCHEmul::Attributes& attrs)
	: MCHEmul::GraphicalChip (_ID, 
		{ { "Name", "ULA" },
		  { "Code", "2C184E" },
		  { "Manufacturer", "Ferranti" },
		  { "Year", "1980" } }),
	  _ULARegisters (new ZX81::ULARegisters (t)),
	  _type (t),
	  _ULAView (vV),
	  _raster (vd, hd, 1 /** The step is 1 pixel. */),
	  _showEvents (false),
	  _charLoadDelayPixels (t == ZX81::Type::_ZX80 ? 8 : 7),
	  _pendingCharacters (),
	  _nextPendingCharacter (0),
	  _lineSyncPosition (hd.totalPositions () - _charLoadDelayPixels - 1),
	  _lineSyncActive (false),
	  _simulationStarted (false),
	  _lastCPUCycles (0),
	  _format (nullptr),
	  _HALTBefore (false),
	  _INTActive (false), _NMIActive (false), _HALTActive (false), _LINECNTRLTo0 (false), _LINECNTRLTo0Draw (0),
	  _writePort (false), _readPortFE (false),
	  _NMIGeneratorOn (false), _NMIGeneratorOff (false)
{
	setClassName ("ULA");

	_pendingCharacters.reserve (64);

	_format = SDL_AllocFormat (SDL_PIXELFORMAT_ARGB8888);
}

// ---
ZX81::ULA::~ULA ()
{
	SDL_FreeFormat (_format);
}

// ---
bool ZX81::ULA::initialize ()
{
	assert (MCHEmul::GraphicalChip::memoryRef () != nullptr);

	if (!MCHEmul::GraphicalChip::initialize ())
		return (false);

	restartRaster ();

	// Notice that all attributes related with drawing signal are not inialized
	// to avoid that when restart a new ulaevents instruction must be commanded!

	_ULARegisters -> initialize ();

	_pendingCharacters.clear ();
	_nextPendingCharacter = 0;
	_simulationStarted = false;
	_lastCPUCycles = 0;

	// Events null...
	_HALTBefore = false;
	_INTActive = false; _NMIActive = false; _HALTActive = false;
	_LINECNTRLTo0 = false; _LINECNTRLTo0Draw = 0;
	_writePort = false; _readPortFE = false;
	_NMIGeneratorOn = false; _NMIGeneratorOff = false;
	
	return (true);
}

// ---
bool ZX81::ULA::simulate (MCHEmul::CPU* cpu)
{
	// First time?
	if (!_simulationStarted)
	{ 
		_lastCPUCycles = cpu -> clockCycles ();

		_simulationStarted = true;

		return (true);
	}

	// Simulate the visulization...
	for (unsigned int i = 0;
			i < ((cpu -> clockCycles  () - _lastCPUCycles) << 1 /** ULA cycles = 2 * CPU Cycles. */); 
			i++)
	{
		unsigned int c = _lastCPUCycles + (i >> 1);
		unsigned char p = (unsigned char) (i & 1);
		// A due load precedes this pixel's shift; the old pattern survives until then.
		loadPendingCharData (c, p);

		_IFDEBUG debugULACycle (cpu, i);

		// Draws the screen to the display at the beginning of everything...
		if (_raster.vData ().currentPositionAtBase0 () == 0 &&
			_raster.hData ().currentPositionAtBase0 () == 0)
			MCHEmul::GraphicalChip::notify (MCHEmul::Event (_GRAPHICSREADY));

		// Decide whether it is needed to draw or not the events...
		if (_showEvents)
		{
			// Keep track of different situations to be drawn later if needed...
			// When the LINE of Control is 0...
			if (_ULARegisters -> LINECNTRL () == 7)
				_LINECNTRLTo0 = true; // the value _LINECNTRLTo0Draw is not initialized...
			// When a INT interrupt is in execution...
			if (!_INTActive.peekValue () && 
				cpu -> programCounter ().internalRepresentation () == 0x038)
				_INTActive = true;
			// When a NMI interrupt is in execution...
			if (!_NMIActive.peekValue () &&
				cpu -> programCounter ().internalRepresentation () == 0x066)
				_NMIActive = true;
			// When the HALT situation is active...
			if (cpu -> lastInstruction () -> code () == 0x076)
			{
				if (!_HALTActive.peekValue ())
					_HALTActive = true;
			}
			else
				_HALTBefore = false;
		}

		bool iV = drawInVisibleZone (cpu);
		unsigned short x = 0, y = 0;
		if (iV)
			_raster.currentVisiblePosition (x, y);

		unsigned short hB = _raster.hData ().currentPositionAtBase0 ();
		unsigned short vB = _raster.vData ().currentPositionAtBase0 ();

		bool eH = _ULARegisters -> INTack (c);
		bool internalSync = (hB == _lineSyncPosition);
		bool wasActive = _lineSyncActive;
		bool rE = _raster.hData ().add (1);

		if (eH)
		{
			// Complete the positions skipped by external horizontal alignment.
			if (iV && (x + 1) < _raster.visibleColumns ())
				_screenMemory -> setHorizontalLine
					(x + 1, y, _raster.visibleColumns () - (x + 1), 1);

			// INT acceptance starts the tail interval, not the next presentation row.
			// Do not derive hardware events from retrace flags produced by this jump.
			_raster.hData ().add
				((int) (_lineSyncPosition + 1) -
				 (int) (_raster.hData ().currentPositionAtBase0 ()));

			// External alignment also takes precedence over a coincident wrap.
			rE = false;
		}

		if (eH || internalSync)
		{
			unsigned char lB = _ULARegisters -> LINECNTRL ();
			bool applied = !wasActive;

			// Coarse line model: a second source during this tail realigns the
			// return, but does not count another logical line or request another NMI.
			if (applied)
			{
				_ULARegisters -> incLINECTRL ();

				// ZX80 never enables this generator.
				if (_ULARegisters -> NMIGenerator ())
					cpu -> requestInterrupt (FZ80::NMIInterrupt::_ID, c, this, 1);
			}

			_lineSyncActive = true;

			_IFDEBUG debugLineSync
				(c, p, eH, internalSync, wasActive, applied, hB, lB);
		}

		if (rE)
		{
			// add() already wrapped horizontally. Logical line state was updated
			// at sync; only the presentation row advances at this later instant.
			_raster.vData ().add (1);
			_lineSyncActive = false;

			_IFDEBUG debugLineAdvance (c, p, hB, vB);
		}

		// Notice that the VSYNC doesn't happen here as it is the port output the one launching that...
		// (@see ZX81::PortManager class)

		// If the status of the casette signal has changed, it has to be notified...
		if (_ULARegisters -> MICSignalChanged ()) // After checked the value returns false...
			notify (MCHEmul::Event (MCHEmul::DatasetteIOPort::_WRITE, _ULARegisters -> MICSignal () ? 1 : 0));
	}

	_lastCPUCycles = cpu -> clockCycles ();

	return (true);
}

// ---
MCHEmul::InfoStructure ZX81::ULA::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::GraphicalChip::getInfoStructure ());

	result.remove ("Memory"); // This info is not neccesary...
	result.add ("ULARegisters",	std::move (_ULARegisters -> getInfoStructure ()));
	result.add ("Raster",		std::move (_raster.getInfoStructure ()));

	return (result);
}

// ---
void ZX81::ULA::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	auto pressOrReleaseKey = [=](SDL_Scancode sc, bool pressed) -> void
		{
			const ZX81::InputOSSystem::Keystrokes& ks = 
				((ZX81::InputOSSystem*) n) -> keystrokesFor (sc);
			if (!ks.empty ()) // The key has to be defined...
				for (const auto& j : ks)
					_ULARegisters -> setKeyboardStatus (j.first, j.second, pressed);
		};

	switch (evnt.id ())
	{
		case MCHEmul::InputOSSystem::_KEYBOARDKEYPRESSED:
			{
				pressOrReleaseKey (std::static_pointer_cast <MCHEmul::InputOSSystem::KeyboardEvent> 
					(evnt.data ()) -> _key, true);
			}

			break;

		case MCHEmul::InputOSSystem::_KEYBOARDKEYRELEASED:
			{
				pressOrReleaseKey (std::static_pointer_cast <MCHEmul::InputOSSystem::KeyboardEvent> 
					(evnt.data ()) -> _key, false);
			}

			break;

		// The rest of the events are not taken here into account!

		default:
			break;
	}
}

// ---
MCHEmul::ScreenMemory* ZX81::ULA::createScreenMemory ()
{
	unsigned int* cP = new unsigned int [16];
	// The colors are partially transparents to allow the blending...
	cP [0]  = SDL_MapRGBA (_format, 0x00, 0x00, 0x00, 0xe0); // Black
	cP [1]  = SDL_MapRGBA (_format, 0xff, 0xff, 0xff, 0xe0); // White

	// These other colors doesn't exist in ZX81, but are used to draw borders, bebug information, etc...
	cP [2]  = SDL_MapRGBA (_format, 0xff, 0x00, 0x00, 0xff); // Red
	cP [3]  = SDL_MapRGBA (_format, 0x00, 0xff, 0xff, 0xff); // Cyan
	cP [4]  = SDL_MapRGBA (_format, 0xff, 0x00, 0xff, 0xff); // Violet
	cP [5]  = SDL_MapRGBA (_format, 0x00, 0xff, 0x00, 0xff); // Green
	cP [6]  = SDL_MapRGBA (_format, 0x00, 0x00, 0xff, 0xff); // Blue
	cP [7]  = SDL_MapRGBA (_format, 0xff, 0xff, 0x00, 0xff); // Yellow
	cP [8]  = SDL_MapRGBA (_format, 0xff, 0x00, 0x7f, 0xff); // Light Pink
	cP [9]  = SDL_MapRGBA (_format, 0x80, 0x00, 0x80, 0xff); // Purple
	cP [10] = SDL_MapRGBA (_format, 0xff, 0xa5, 0x00, 0xff); // Orange
	cP [11] = SDL_MapRGBA (_format, 0x60, 0x60, 0x60, 0xff); // Dark Grey
	cP [12] = SDL_MapRGBA (_format, 0x8a, 0x8a, 0x8a, 0xff); // Light Grey
	cP [13] = SDL_MapRGBA (_format, 0x32, 0xc8, 0x32, 0xff); // Green Lime
	cP [14] = SDL_MapRGBA (_format, 0x00, 0xa0, 0xff, 0xff); // Light Blue
	cP [15] = SDL_MapRGBA (_format, 0xff, 0x33, 0x33, 0xff); // Light Red

	return (new MCHEmul::ScreenMemory (numberColumns (), numberRows (), cP));
}

// --
bool ZX81::ULA::drawInVisibleZone (MCHEmul::CPU* cpu)
{
	bool d = false;
	bool p = _ULARegisters -> shiftOutData (d);

	if (!_raster.isInVisibleZone ())
		return (false); // Nothing else can be done...

	unsigned short x = 0, y = 0;
	_raster.currentVisiblePosition (x, y);
	// Draws the pixel..
	_screenMemory -> setPixel (x, y, 1);
	if (p && _ULARegisters -> syncOutputWhite () &&
		(_ULARegisters -> reverseVideo () ^ d)) // normal video and pixel on?, or inverse video and pixels off?
			_screenMemory -> setPixel (x, y, 0); // ...then put it in black...

	// Draws the events if any...
	if (_showEvents)
	{
		// First because if could hide the rst of the events...
		if (_LINECNTRLTo0 && ++_LINECNTRLTo0Draw == 3) 
			{ _LINECNTRLTo0Draw = 0; _screenMemory -> setPixel (x, y, 4); }	// Violet
		if (_HALTActive)
			{ _screenMemory -> setPixel (x, y, !_HALTBefore ? 13 /** Fist HALT. */ : 12); _HALTBefore = true; }
		if (_INTActive)			_screenMemory -> setPixel (x, y, 2);	// Red
		if (_NMIActive)			_screenMemory -> setPixel (x, y, 7);	// Yellow
		// Writting and reading to the specific ports is also detected...
		if (_writePort)			_screenMemory -> setPixel (x, y, 5);	// Green
		if (_NMIGeneratorOn)	_screenMemory -> setPixel (x, y, 6);	// Blue
		if (_NMIGeneratorOff)	_screenMemory -> setPixel (x, y, 8);	// Light Pink
		if (_readPortFE)		_screenMemory -> setPixel (x, y, 3);	// Cyan
	}

	return (true);
}

// ---
void ZX81::ULA::captureCharData (MCHEmul::CPU* cpu,
	const MCHEmul::UByte& dt)
{
	unsigned int c = cpu -> clockCycles ();

	// Preserve the first capture instead of skipping its interval on first simulate().
	if (!_simulationStarted)
	{
		_lastCPUCycles = c;
		_simulationStarted = true;
	}

	// This reference describes the initial opcode fetch. Nested prefix fetches
	// still need their own M1 timestamp. Queuing preserves multiple captures in
	// a CPU batch, but does not reconstruct intervening memory or port changes.
	_pendingCharacters.push_back
		({ c,
		   c + (_charLoadDelayPixels >> 1),
		   (unsigned char) (_charLoadDelayPixels & 1),
		   static_cast <FZ80::CZ80*> (cpu) -> iRegister ().values () [0].value (),
		   dt });

	_IFDEBUG debugCharCapture (_pendingCharacters.back ());
}

// ---
void ZX81::ULA::loadPendingCharData (unsigned int c, unsigned char p)
{
	while (_nextPendingCharacter < _pendingCharacters.size ())
	{
		const PendingCharacter& ch = _pendingCharacters [_nextPendingCharacter];

		// Nearby pending timestamps remain within half the CPU-clock range.
		int distance = (int) (c - ch._loadClock);
		if (distance < 0 || (distance == 0 && p < ch._loadPhase))
			break;

		MCHEmul::MemoryView* oV = memoryRef () -> activeView ();
		memoryRef () -> setActiveView (_ULAView);

		// I belongs to the capture; the line counter belongs to this load phase.
		// For ROM character patterns, video hardware supplies A0-A8:
		// LINECNTRL supplies A0-A2 and character bits 0-5 supply A3-A8.
		// Discard I0 so that CPU A8 cannot override character bit 5.
		// This address formation does not represent RAM-based I:R video fetches.
		MCHEmul::Address a (2,
			((unsigned int) (ch._i & 0b11111110) << 8) |
			((unsigned int) (ch._code.value () & 0b00111111) << 3) |
			(unsigned int) (_ULARegisters -> LINECNTRL ()));
		MCHEmul::UByte pattern = memoryRef () -> value (a);

		memoryRef () -> setActiveView (oV -> id ());

		_IFDEBUG debugCharLoad (ch, c, p, a, pattern, false, false);

		bool accepted = _ULARegisters -> loadSHIFTRegister (pattern);
		if (accepted)
			_ULARegisters -> setReverseVideo (ch._code.bit (7));

		_IFDEBUG debugCharLoad (ch, c, p, a, pattern, true, accepted);

		// A rejected load is reported and consumed, never silently delayed again.
		_nextPendingCharacter++;
	}

	if (_nextPendingCharacter == _pendingCharacters.size () &&
		_nextPendingCharacter != 0)
	{
		_pendingCharacters.clear ();
		_nextPendingCharacter = 0;
	}
}

// ---
void ZX81::ULA::restartRaster ()
{
	_raster.initialize ();

	_lineSyncActive = false;
}

// ---
void ZX81::ULA::debugULACycle (MCHEmul::CPU* cpu, unsigned int i)
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeCompleteLine (className (), _lastCPUCycles + (i >> 1), "Info Cycle",
		{ { "Raster position",
			std::to_string (_raster.currentColumnAtBase0 ()) + "," +
			std::to_string (_raster.currentLineAtBase0 ()) },
		  { "Internal status",
			"NMI=" + std::string ((_ULARegisters -> NMIGenerator () ? "NMI_ON" : "NMI_OFF")) + "," +
			"ZONE=" + std::string ((_ULARegisters -> syncOutputWhite () ? "WHITE" : "BLACK")) + "," + 
			"LNCTRL=" + std::to_string (_ULARegisters -> LINECNTRL ()) + "," +
			"LineSyncActive=" + std::to_string (_lineSyncActive) },
		  { "Shift register",
			"Data=" + std::to_string (_ULARegisters -> SHIFTRegister ().value ()) + "," +
			"PendingBits=" + std::to_string (_ULARegisters -> pendingSHIFTBits ()) + "," +
			"Inverse=" + std::to_string (_ULARegisters -> reverseVideo ()) },
		  { "Timing",
			"PixelPhase=" + std::to_string (i & 1) + "," +
			"State=BeforeShift" } });
}

// ---
void ZX81::ULA::debugPortRead (unsigned short ab, unsigned char id,
	const MCHEmul::UByte& v, bool ms) const
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeCompleteLine
		(className (), _lastCPUCycles, "Port Read",
		{ { "Raster position",
			"Column=" + std::to_string (_raster.currentColumnAtBase0 ()) + "," +
			"Row=" + std::to_string (_raster.currentLineAtBase0 ()) },
		  { "Port",
			"Access=" + std::string (ms ? "READ" : "PEEK") + "," +
			"Address=" + std::to_string (ab) + "," +
			"ID=" + std::to_string (id) + "," +
			"Data=" + std::to_string (v.value ()) + "," +
			"D6=" + std::to_string (v.bit (6)) + "," +
			"D7=" + std::to_string (v.bit (7)) },
		  { "Internal status",
			"NTSC=" + std::to_string (_ULARegisters -> NTSC ()) + "," +
			"EAR=" + std::to_string (_ULARegisters -> EARSignal ()) + "," +
			"MIC=" + std::to_string (_ULARegisters -> MICSignal ()) + "," +
			"VSYNC=" + std::to_string (_ULARegisters -> inVSync ()) + "," +
			"LNCTRL=" + std::to_string (_ULARegisters -> LINECNTRL ()) + "," +
			"LNBlocked=" + std::to_string (_ULARegisters -> LINECTRLBlocked ()) },
		  { "Timing",
			"Reference=LastULASimulatedCPUClock,State=AfterRead" } });
}

// ---
void ZX81::ULA::debugCharCapture (const PendingCharacter& ch) const
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeCompleteLine
		(className (), ch._captureClock, "Character Capture",
		{ { "Character",
			"Code=" + std::to_string (ch._code.value ()) + "," +
			"I=" + std::to_string (ch._i) },
		  { "Timing",
			"CaptureClock=" + std::to_string (ch._captureClock) + "," +
			"LoadClock=" + std::to_string (ch._loadClock) + "," +
			"LoadPhase=" + std::to_string (ch._loadPhase) + "," +
			"DelayPixels=" + std::to_string (_charLoadDelayPixels) + "," +
			"Reference=InitialOpcodeFetch" } });
}

// ---
void ZX81::ULA::debugCharLoad (const PendingCharacter& ch,
	unsigned int c, unsigned char p, const MCHEmul::Address& a,
	const MCHEmul::UByte& pattern, bool after, bool accepted) const
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeCompleteLine
		(className (), c, "Character Load",
		{ { "Raster position",
			"Column=" + std::to_string (_raster.currentColumnAtBase0 ()) + "," +
			"Row=" + std::to_string (_raster.currentLineAtBase0 ()) },
		  { "Character",
			"Code=" + std::to_string (ch._code.value ()) + "," +
			"I=" + std::to_string (ch._i) + "," +
			"LNCTRL=" + std::to_string (_ULARegisters -> LINECNTRL ()) },
		  { "Pattern",
			"Address=" + std::to_string (a.value ()) + "," +
			"Data=" + std::to_string (pattern.value ()) + "," +
			"Accepted=" + (after ? std::to_string (accepted) : std::string ("NA")) },
		  { "Shift register",
			"Data=" + std::to_string (_ULARegisters -> SHIFTRegister ().value ()) + "," +
			"PendingBits=" + std::to_string (_ULARegisters -> pendingSHIFTBits ()) + "," +
			"Inverse=" + std::to_string (_ULARegisters -> reverseVideo ()) },
		  { "Timing",
			"CaptureClock=" + std::to_string (ch._captureClock) + "," +
			"ScheduledClock=" + std::to_string (ch._loadClock) + "," +
			"ScheduledPhase=" + std::to_string (ch._loadPhase) + "," +
			"ActualClock=" + std::to_string (c) + "," +
			"ActualPhase=" + std::to_string (p) + "," +
			"Late=" + std::to_string (c != ch._loadClock || p != ch._loadPhase) + "," +
			"Stage=" + std::string (after ? "AfterLoad" : "BeforeLoad") } });
}

// ---
void ZX81::ULA::debugLineSync (unsigned int c, unsigned char p,
	bool external, bool internal, bool wasActive, bool applied,
	unsigned short hB, unsigned char lB) const
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeCompleteLine
		(className (), c, "Line Sync",
		{ { "Raster position",
			"HBefore=" + std::to_string (hB) + "," +
			"HAfter=" + std::to_string (_raster.hData ().currentPositionAtBase0 ()) + "," +
			"Row=" + std::to_string (_raster.vData ().currentPositionAtBase0 ()) },
		  { "Cause",
			"ExternalSync=" + std::to_string (external) + "," +
			"InternalSync=" + std::to_string (internal) + "," +
			"AlreadyActive=" + std::to_string (wasActive) + "," +
			"Applied=" + std::to_string (applied) + "," +
			"INTackClock=" + (external ? std::to_string (_ULARegisters -> INTackClock ())
				: std::string ("NA")) },
		  { "Internal status",
			"LNBefore=" + std::to_string (lB) + "," +
			"LNAfter=" + std::to_string (_ULARegisters -> LINECNTRL ()) + "," +
			"LNBlocked=" + std::to_string (_ULARegisters -> LINECTRLBlocked ()) + "," +
			"NMIGenerator=" + std::to_string (_ULARegisters -> NMIGenerator ()) + "," +
			"PendingBits=" + std::to_string (_ULARegisters -> pendingSHIFTBits ()) },
		  { "Timing",
			"PixelPhase=" + std::to_string (p) + "," +
			"TailPixels=" + std::to_string (_charLoadDelayPixels) + "," +
			"State=AfterLineSync" } });
}

// ---
void ZX81::ULA::debugLineAdvance (unsigned int c, unsigned char p,
	unsigned short hB, unsigned short vB) const
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeCompleteLine
		(className (), c, "Line Advance",
		{ { "Raster position",
			"HBefore=" + std::to_string (hB) + "," +
			"HAfter=" + std::to_string (_raster.hData ().currentPositionAtBase0 ()) + "," +
			"VBefore=" + std::to_string (vB) + "," +
			"VAfter=" + std::to_string (_raster.vData ().currentPositionAtBase0 ()) },
		  { "Internal status",
			"LNCTRL=" + std::to_string (_ULARegisters -> LINECNTRL ()) + "," +
			"PendingBits=" + std::to_string (_ULARegisters -> pendingSHIFTBits ()) },
		  { "Timing",
			"PixelPhase=" + std::to_string (p) + "," +
			"State=AfterHorizontalWrap" } });
}

// ---
ZX81::ULA_PAL::ULA_PAL (ZX81::Type t, int vV)
	: ZX81::ULA (_VRASTERDATA, _HRASTERDATA, t, vV,
		 { { "Name", "ULA" },
		   { "Code", "2C184E (PAL)" },
		   { "Manufacturer", "Ferranti"},
		   { "Year", "1980" } })
{
	_ULARegisters -> setNTSC (false);
}

// ---
ZX81::ULA_NTSC::ULA_NTSC (ZX81::Type t, int vV)
	: ZX81::ULA (_VRASTERDATA, _HRASTERDATA, t, vV,
		 { { "Name", "ULA" },
		   { "Code", "2C184E (PAL)" },
		   { "Manufacturer", "Ferranti"},
		   { "Year", "1980" } })
{
	_ULARegisters -> setNTSC (true);
}
