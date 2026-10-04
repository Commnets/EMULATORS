#include <ZX81/ULA.hpp>
#include <ZX81/ULARegisters.hpp>
#include <ZX81/ZX81.hpp>
#include <ZX81/OSIO.hpp>
#include <FZ80/CZ80.hpp>
#include <FZ80/NMIInterrupt.hpp>

// ---
/** Presentation geometry, not a broadcast-standard porch specification. \n
	The horizontal raster has 414 positions; visible columns span 125..413
	(289 pixels). PAL has 312 rows with visible rows 46..269 (224 rows); NTSC has
	262 rows with visible rows 22..245 (224 rows). Endpoints are inclusive. \n
	ROM execution and the character pipeline determine actual image placement.
	Delayed pattern loading and the trailing interval preserve presentation alignment. \n
	ZX81's separate generator uses a 207-T compatibility baseline and HSYNC at ULA
	counts 32..63. Crop bounds do not specify HSYNC or physical porch durations;
	414 clocks at 6.5 MHz are not exactly 64 microseconds. \n
	An actual even-port read starts VSYNC when NMI is disabled; any output ends it.
	A1-low output disables ZX81 NMI, then A0-low enables it. ZX80 has no NMI generator.
	See PortManager for decoding and side effects. */
const MCHEmul::RasterData ZX81::ULA_PAL::_VRASTERDATA
	(0, 46 /** First visible row. */, 46 /** = starts screen. */, 269 /** Last screen row, inclusive. */,
	 269 /** = end visible part. */, 311 /** Vertical retrace position. */, 311 /** = end. */, 312 /* total */, 0, 0);
const MCHEmul::RasterData ZX81::ULA_PAL::_HRASTERDATA
	(0, 125 /** First visible column. */, 125 /** = starts screen, */, 413 /** Last screen column, inclusive. */,
	 413 /** = end visible part. */, 413 /** = retrace. */, 413 /** = end. */, 414 /** total. */, 0, 0);
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
	  _lineSyncPosition (hd.totalPositions () - (t == ZX81::Type::_ZX80 ? 8 : 7) - 1),
	  _lineSyncActive (false),
	  _horizontalCounter (4),
	  _hSyncActive (false),
	  _horizontalResetPending (false),
	  _horizontalResetClock (0),
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
bool ZX81::ULA::aboutToGenerateNMIAfterCycles (unsigned int nC)
{
	if (_type == ZX81::Type::_ZX80 || !_ULARegisters -> NMIGenerator () || nC == 0)
		return (false);

	unsigned int pixels = nC << 1;
	unsigned int distance = (_HSYNCSTART + _HORIZONTALPERIOD - _horizontalCounter) % _HORIZONTALPERIOD;
	if (!_horizontalResetPending)
		return (distance < pixels);

	// The stored counter describes the next pixel, at _lastCPUCycles phase zero.
	// A reset wins over a sync edge at exactly the same instant.
	unsigned int resetDistance = (_horizontalResetClock - _lastCPUCycles) << 1;
	return ((distance < pixels && distance < resetDistance) ||
		(resetDistance < pixels && _HSYNCSTART < (pixels - resetDistance)));
}

// ---
unsigned int ZX81::ULA::haltNMIWaitCycles
	(unsigned int requestClock, unsigned int acceptanceClock) const
{
	if (_type == ZX81::Type::_ZX80)
		return (0);

	unsigned int elapsed = acceptanceClock - requestClock;
	// Only a boundary of the four-T HALT cycle is covered. Larger latencies
	// can result from CPU/chip batching and need a more detailed bus model.
	if (elapsed > 4)
		return (0);

	// Picozx81's four-T phase compensation: 14 + 3 - elapsed.
	// This extends the effective response; it is not a 17-T HSYNC pulse
	// or a pin-level WAIT simulation. Ordinary instructions remain outside
	// this approximation, which must not depend on the game or raster crop.
	return (17 - elapsed);
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
		bool eH = _ULARegisters -> INTack (c);
		if (_type != ZX81::Type::_ZX80)
		{
			if (eH)
			{
				_horizontalResetPending = true;
				_horizontalResetClock = c + _INTACKDELAY;
				_IFDEBUG debugHorizontalTiming (c, p, "INT Response", "CPUResponse",
					_horizontalCounter, _ULARegisters -> LINECNTRL (), false);
			}
			processHorizontalSync (cpu, c, p);
		}

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

		bool internalSync = (hB == _lineSyncPosition);
		bool wasActive = _lineSyncActive;
		bool rE = _raster.hData ().add (1);

		if (eH)
		{
			// Complete the positions skipped by external horizontal alignment.
			if (iV && (x + 1) < _raster.visibleColumns ())
				_screenMemory -> setHorizontalLine
					(x + 1, y, _raster.visibleColumns () - (x + 1), 1);

			// Presentation alignment preserves the existing text coordinates.
			// On ZX81 this does not reset the hardware counter or start HSYNC.
			// Do not derive hardware events from retrace flags produced by this jump.
			_raster.hData ().add
				((int) (_lineSyncPosition + 1) -
				 (int) (_raster.hData ().currentPositionAtBase0 ()));

			// External alignment also takes precedence over a coincident wrap.
			rE = false;
		}

		if (_type == ZX81::Type::_ZX80 && (eH || internalSync))
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
			// separately at sync; this wrap advances only the presentation row.
			_raster.vData ().add (1);
			_lineSyncActive = false;

			_IFDEBUG debugLineAdvance (c, p, hB, vB);
		}

		if (_type != ZX81::Type::_ZX80)
			advanceHorizontalCounter ();

		// VSYNC starts on a qualifying port read and ends on output; PortManager applies both.
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

	FZ80::CZ80* cZ80 = static_cast <FZ80::CZ80*> (cpu);
	unsigned char i = cZ80 -> iRegister ().values () [0].value ();
	unsigned char r = cZ80 -> rRegister ().values () [0].value ();

	// Refresh presents I:R before this M1 increments R.
	// Capture that address unchanged; anticipating the increment skips
	// the first WRX pattern byte and reads one byte beyond the intended row.
	// The CPU remains responsible for updating its own R register.
	unsigned short refreshAddress = (unsigned short)
		(((unsigned int) (i) << 8) | (unsigned int) (r));

	// Preserve this M1's refresh address even if the CPU advances before
	// the ULA consumes the capture. Prefix fetch timestamps retain the
	// existing approximation; this does not reconstruct intervening bus activity.
	_pendingCharacters.push_back
		({ c,
		   c + (_charLoadDelayPixels >> 1),
		   (unsigned char) (_charLoadDelayPixels & 1),
		   i,
		   refreshAddress,
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

		MCHEmul::Address a (2, ch._refreshAddress);
		bool ramRefresh =
			static_cast <ZX81::Memory*> (memoryRef ()) ->
				canReadRAM16KDuringRefresh (a);

		// A refresh-capable expansion receives the captured CPU I:R address.
		// Otherwise retain the existing character path for compatibility;
		// this fallback does not model an electrically undriven refresh bus.
		// In the character path, the ULA supplies A0-A8, including I0's position.
		if (!ramRefresh)
			a = MCHEmul::Address (2,
				((unsigned int) (ch._i & 0b11111110) << 8) |
				((unsigned int) (ch._code.value () & 0b00111111) << 3) |
				(unsigned int) (_ULARegisters -> LINECNTRL ()));
		MCHEmul::UByte pattern = memoryRef () -> value (a);

		memoryRef () -> setActiveView (oV -> id ());

		_IFDEBUG debugCharLoad (ch, c, p, a, pattern, ramRefresh, false, false);

		bool accepted = _ULARegisters -> loadSHIFTRegister (pattern);
		if (accepted)
			_ULARegisters -> setReverseVideo (ch._code.bit (7));

		_IFDEBUG debugCharLoad (ch, c, p, a, pattern, ramRefresh, true, accepted);

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

	initializeHorizontalTiming ();
}

// ---
void ZX81::ULA::processHorizontalSync (MCHEmul::CPU* cpu, unsigned int c, unsigned char p)
{
	if (_horizontalResetPending && c == _horizontalResetClock && p == 0)
	{
		unsigned short before = _horizontalCounter;
		_horizontalResetPending = false;
		// A bus acknowledge restarts the generator, not the sync pulse.
		if (_hSyncActive)
		{
			_hSyncActive = false;

			_IFDEBUG debugHorizontalTiming (c, p, "HSync End", "INTAcknowledge",
				before, _ULARegisters -> LINECNTRL (), false);
		}

		_horizontalCounter = 0;

		_IFDEBUG debugHorizontalTiming (c, p, "Horizontal Reset", "INTAcknowledge",
			before, _ULARegisters -> LINECNTRL (), false);
	}

	if (_horizontalCounter == _HSYNCSTART)
	{
		unsigned char lB = _ULARegisters -> LINECNTRL ();
		_hSyncActive = true;
		_ULARegisters -> incLINECTRL ();
		bool nmiRequested = _ULARegisters -> NMIGenerator ();
		if (nmiRequested)
			cpu -> requestInterrupt (FZ80::NMIInterrupt::_ID, c, this, 1);

		_IFDEBUG debugHorizontalTiming (c, p, "HSync Start", "Counter",
			_horizontalCounter, lB, nmiRequested);
	}
	else
	if (_horizontalCounter == _HSYNCEND)
	{
		_hSyncActive = false;

		_IFDEBUG debugHorizontalTiming (c, p, "HSync End", "Counter",
			_horizontalCounter, _ULARegisters -> LINECNTRL (), false);
	}
}

// ---
void ZX81::ULA::initializeHorizontalTiming ()
{
	unsigned short before = _horizontalCounter;
	// Presentation H=406 denotes response start; H=410 denotes bus acknowledge.
	// Thus presentation H=0 corresponds to generator count 4, not zero.
	// Keep the coarse VSYNC restart: exact I/O timing and pin-level WAIT are
	// not modeled. HALT/NMI compensation is separate; this is not an INT edge.
	_horizontalCounter = 4;
	_hSyncActive = false;
	_horizontalResetPending = false;
	_horizontalResetClock = 0;
	if (_type != ZX81::Type::_ZX80)
		_IFDEBUG debugHorizontalTiming (_lastCPUCycles, 0, "Horizontal Reset",
			"InitializationOrVSync", before, _ULARegisters -> LINECNTRL (), false);
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
		  { "Refresh",
			"R=" + std::to_string (ch._refreshAddress & 0xff) + "," +
			"Address=" + std::to_string (ch._refreshAddress) },
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
	const MCHEmul::UByte& pattern, bool ramRefresh, bool after, bool accepted) const
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
		  { "Refresh",
			"R=" + std::to_string (ch._refreshAddress & 0xff) + "," +
			"Address=" + std::to_string (ch._refreshAddress) },
		  { "Pattern",
			"Source=" + std::string
				(ramRefresh ? "RAMRefresh" : "CharacterPath") + "," +
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
void ZX81::ULA::debugHorizontalTiming (unsigned int c, unsigned char p,
	const char* event, const char* cause, unsigned short before,
	unsigned char lB, bool nmiRequested) const
{
	assert (_deepDebugFile != nullptr);
	_deepDebugFile -> writeCompleteLine (className (), c, event,
		{ { "Generator",
			"Before=" + std::to_string (before) + "," +
			"After=" + std::to_string (_horizontalCounter) + "," +
			"HSync=" + std::to_string (_hSyncActive) + "," +
			"ResetPending=" + std::to_string (_horizontalResetPending) },
		  { "Raster position",
			"Column=" + std::to_string (_raster.hData ().currentPositionAtBase0 ()) + "," +
			"Row=" + std::to_string (_raster.vData ().currentPositionAtBase0 ()) },
		  { "Internal status",
			"LNBefore=" + std::to_string (lB) + "," +
			"LNAfter=" + std::to_string (_ULARegisters -> LINECNTRL ()) + "," +
			"LNBlocked=" + std::to_string (_ULARegisters -> LINECTRLBlocked ()) + "," +
			"NMIGenerator=" + std::to_string (_ULARegisters -> NMIGenerator ()) + "," +
			"NMIRequested=" + std::to_string (nmiRequested) },
		  { "Timing",
			"PixelPhase=" + std::to_string (p) + "," +
			"Cause=" + std::string (cause) + "," +
			"ResponseClock=" + std::to_string (_ULARegisters -> INTackClock ()) + "," +
			"ResetClock=" + std::to_string (_horizontalResetClock) + "," +
			"State=AfterEvent" } });
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
