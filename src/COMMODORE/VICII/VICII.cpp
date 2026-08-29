#include <COMMODORE/VICII/VICII.hpp>

#include <algorithm>
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
	  _incCyclesPerRasterLine (cRL - COMMODORE::VICII_PAL::_CYCLESPERRASTERLINE),
	  _raster (vd, hd, 8 /** @see above. This is the step. */),
	  _drawRasterInterruptPositions (false), _drawSpritesBorder (false), _drawOtherEvents (false),
	  _lastCPUCycles (0),
	  _format (nullptr),
	  _cycleInRasterLine (1),
	  _lastVICDataRead (MCHEmul::UByte::_0),
	  _cpuOpcodeLowNibble (MCHEmul::UByte::_0),
	  _cpuStopWindowSets (),
	  _currentCPUStopWindows (nullptr), _nextCPUStopWindows (nullptr),
	  _adjustedCurrentCPUStopWindows (),
	  _spriteDMAStateMask (0),
	  _currentSpriteDMAMask (0), _nextSpriteDMAMask (0),
	  _pendingCPUTransaction (), _pendingCPUStopPrediction (),
	  _pendingRegisterWrites (),
	  _DENSeenAtLine30 (false),
	  _badLineAlreadyDetectedThisLine (false), 
	  _badLineConditionActive (false),
	  _badLinePreventedIdleThisLine (false),
	  _badLineBAAlreadyRequested (false),
	  _badLineBARequestCycle (0),
	  _badLineCAccessActive (false),
	  _badLineCAccessAllowedThisLine (false),
	  _badLineInvalidCAccessCycles (0),
	  _badLineInvalidColorData (MCHEmul::UByte::_0),
	  _badLineCAccessStartCycle (0),
	  _lastVBlankEntered (false),
	  _lightPenFrameLatched (false), _lightPenButtonPressed (false),
	  _vicGraphicInfo (),
	  _vicSpriteInfo (),
	  _eventStatus { false, false, false, false, false }
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

	// Notice that all attributes related with drawing signal are not inialized
	// to avoid that when restart a new showevents instruction must be commanded!

	_VICIIRegisters -> linkToRaster (&_raster);
	_VICIIRegisters -> initialize (); // The raster is not reinitialized there...

	// Establish the initial display limits from the register values restored by
	// VICIIRegisters::initialize (). Later changes are applied at their exact
	// buffered write cycles.
	_raster.reduceDisplayZone
		(!_VICIIRegisters -> textDisplay25RowsActive (),
		 !_VICIIRegisters -> textDisplay40ColumnsActive ());

	_lastCPUCycles = 0;
	
	_cycleInRasterLine = 1;

	_lastVICDataRead = MCHEmul::UByte::_0;
	_cpuOpcodeLowNibble = MCHEmul::UByte::_0;

	_DENSeenAtLine30 = false;
	_badLineAlreadyDetectedThisLine = false;
	_badLineConditionActive = false;
	_badLinePreventedIdleThisLine = false;
	_badLineBAAlreadyRequested = false;
	_badLineBARequestCycle = 0;
	_badLineCAccessActive = false;
	_badLineCAccessAllowedThisLine = false;
	_badLineInvalidCAccessCycles = 0;
	_badLineInvalidColorData = MCHEmul::UByte::_0;
	_badLineCAccessStartCycle = 0;

	_lastVBlankEntered = false;

	_lightPenFrameLatched = false; _lightPenButtonPressed = false;

	_vicGraphicInfo = VICGraphicInfo ();
	// PAL starts at line 0 while the NTSC raster definition starts at line 27.
	// Window selection always requires ROW to identify that physical line.
	_vicGraphicInfo._ROW = _raster.currentLine ();
	_VICIIRegisters -> setRasterReadTiming
		(_cyclesPerRasterLine, _raster.vData ().totalPositions ());
	_VICIIRegisters -> setRasterReadContext
		(_vicGraphicInfo._ROW, _cycleInRasterLine);
	_VICIIRegisters -> setNumberPositionsToInstructionEffect (0);
	for (size_t i = 0; i < 8; _vicSpriteInfo [i++] = VICSpriteInfo ());

	// Window construction is delayed until initialize () so the concrete PAL
	// or NTSC sprite timing method is available through virtual dispatch.
	initializeCPUStopWindowSets ();
	_spriteDMAStateMask = spriteDMAMask ();
	_currentSpriteDMAMask = _spriteDMAStateMask;
	_nextSpriteDMAMask = projectedSpriteDMAMaskForNextRasterLine ();
	selectCPUStopWindowsForCurrentAndNextLine ();
	_pendingCPUTransaction.reset ();
	_pendingCPUStopPrediction = CPUStopPrediction ();
	_pendingRegisterWrites.clear ();

	_eventStatus = { false, false, false, false, false };

	return (true);
}

// ---
void COMMODORE::VICII::CPUAboutToExecute (const MCHEmul::InstructionContextEventData* dt)
{
	assert (dt != nullptr);

	const MCHEmul::InstructionDefined* instruction =
		static_cast <const MCHEmul::InstructionDefined*> (dt -> _instruction);
	unsigned int clockCycles = instruction -> clockCyclesToExecute
		(dt -> _cpu, dt -> _memory, dt -> _address);
	size_t cycleStructure = (size_t) (clockCycles - instruction -> clockCycles ());

	prepareCPUStopPrediction
		(&instruction -> cycleStructure (cycleStructure),
		 &instruction -> busCycleData (cycleStructure),
		 clockCycles,
		 dt -> _cpu -> clockCycles ());

	_IFDEBUG debugCPUStopPrediction (dt);

	// Keep the CPU nibble that U16 presents to the VIC-II while AEC is high.
	_cpuOpcodeLowNibble =
		MCHEmul::UByte ((unsigned char)
			(dt -> _instruction -> code () & MCHEmul::UByte::_0F));
}

// ---
void COMMODORE::VICII::CPUAboutToExecute
	(const MCHEmul::InterruptContextEventData* dt)
{
	assert (dt != nullptr);

	prepareCPUStopPrediction
		(&dt -> _interrupt -> cycleStructure (),
		 &dt -> _interrupt -> busCycleData (),
		 dt -> _interrupt -> cyclesToLaunch (),
		 dt -> _cpu -> clockCycles ());

	_IFDEBUG debugCPUStopPrediction (dt);

	// _cpuOpcodeLowNibble is intentionally not changed here. During interrupt
	// entry the real low data-bus nibble depends on the particular read or
	// stack-write cycle; that requires cycle-specific bus values.
}

// ---
bool COMMODORE::VICII::simulate (MCHEmul::CPU* cpu)
{
	// simulate() manages synchronization and the batch of CPU cycles elapsed
	// since its previous invocation. simulateRasterCycle() owns the complete
	// ordered hardware pipeline represented by each individual cycle.
	const unsigned int currentCPUCycles = cpu -> clockCycles ();

	// First invocation: synchronize the VIC-II with the CPU cycle counter.
	// No VIC-II activity is simulated yet because there is no elapsed cycle
	// interval to process.
	if (_lastCPUCycles == 0)
	{ 
		_lastCPUCycles = currentCPUCycles;

		return (true);
	}

	// Extract once per VIC-II simulation batch. If a previous transaction is
	// still physically completing, the method preserves its pending commands.
	extractPendingRegisterWrites ();

	// The VIC-II "video reset" bit is logged as a disconnected/video-reset
	// situation. Raster timing is still advanced because the raster position is
	// considered an external timing reference of the video system.
	if (_VICIIRegisters -> videoResetActive ())
		_IFDEBUG debugDisconnected (cpu);

	// If the interrupt-enable/status register has been written in a way that
	// re-arms interrupt admission, propagate that condition to the CPU interrupt
	// controller before evaluating new IRQ causes in the current batch of cycles.
	if (_VICIIRegisters -> interruptsEnabledBack ())
		cpu -> interrupt (_interruptId) -> 
			setNewInterruptRequestAdmitted (false);

	// Simulate one VIC-II cycle for every CPU cycle elapsed since the previous
	// invocation. The captured clock cannot change while the VIC-II processes
	// this batch, so absolute cycles advance directly without reading it again.
	for (unsigned int i = currentCPUCycles - _lastCPUCycles, 
			cC = _lastCPUCycles; i > 0; i--, cC++)
		simulateRasterCycle (cpu, cC, i);

	// Outside the per-cycle loop, notify the screen subsystem once when entering
	// the first VBlank zone. This keeps the previous behaviour: the notification
	// is associated with the batch of simulated cycles, not with every internal
	// VIC-II cycle.
	notifyGraphicsReadyIfNeeded ();

	// Synchronize the VIC-II cycle counter with the CPU after processing all
	// pending cycles.
	_lastCPUCycles = currentCPUCycles;

	return (true);
}

// ---
void COMMODORE::VICII::simulateRasterCycle
	(MCHEmul::CPU* cpu, unsigned int cC, unsigned int i)
{
	// The real chip works with two bus phases per CPU cycle. This emulator keeps
	// one CPU-cycle scheduling unit, but preserves their causal order explicitly:
	// VIC-II bus activity observes the pre-write register state, the CPU write
	// becomes effective between output pixels 3 and 4, and the later comparators
	// and output pixels observe the post-write state.

	// Phase 1: observe the state at the beginning of the cycle.
	// Events appended to this record describe activity occurring during cC;
	// their resulting state is visible in the following cycle snapshot.
	_IFDEBUG debugVICIICycle (cpu, i);

	// Phase 2: evaluate DMA and CPU-bus arbitration.
	// This includes:
	// - latching whether DEN has been seen at raster line $30,
	// - evaluating the current bad-line condition,
	// - entering display state immediately for regular bad lines, while a
	//   late transition from idle is deferred until the current bus cycle
	//   has completed,
	// - latching a c-access sequence if the condition appears inside
	//   the c-access start window. Cycle-14 sequences are validated there;
	//   later sequences are handled as DMA-delay/VSP-like c-access attempts.
	treatBadLineStateAtCurrentCycle ();

	if (_cycleInRasterLine == 55 || _cycleInRasterLine == 56)
	{
		// Sprite DMA is decided before BA/RDY arbitration. Sprite 0 data is
		// fetched at cycle 58, so cycle 55 must already request the BA lead.
		treatSpriteDMAStartAtCurrentCycle ();

		const unsigned char spriteStateMask = spriteDMAMask ();
		const unsigned char projectedSpriteMask =
			projectedSpriteDMAMaskForNextRasterLine ();
		if (spriteStateMask != _spriteDMAStateMask ||
			projectedSpriteMask != _nextSpriteDMAMask)
		{
			// DMA can start in cycles 55/56 after the early sprite slots have
			// passed. Preserve those historical slots and add the newly active
			// state only to the still relevant current-line window selection.
			_currentSpriteDMAMask |= spriteStateMask;
			_spriteDMAStateMask = spriteStateMask; _nextSpriteDMAMask = projectedSpriteMask;
			selectCPUStopWindowsForCurrentAndNextLine ();
			if (_badLineCAccessActive &&
				_badLineCAccessStartCycle != _BADLINE_START_FIRST_CYCLE)
				actualizeCPUStopWindowsAfterBadLineChange ();
			else if (recalculatePendingCPUStopPrediction ())
				_IFDEBUG debugCPUStopPredictionRecalculated ("SpriteDMAChange");
		}
	}

	// A CPU stop prediction is exceptional. Avoid entering the out-of-line
	// requester during ordinary VIC-II cycles.
	if (_pendingCPUStopPrediction._stopRequired)
		requestPredictedCPUStopIfNeeded (cpu, cC);

	// Phase 3: execute VIC-II bus activity and capture pre-write output state.
	// In shared graphics cycles, the phi1 g-access first consumes the matrix/color
	// entry prepared previously, advances VC/VLMI and then the phi2 c-access
	// prepares the following entry. Every access observes the register values
	// effective before the CPU write of cC.
	treatRasterBusCycle ();

	// On the first cycle of a late Bad Line Condition, phi1 has already made
	// an idle g-access before phi2 performs the first c-access. Enter display
	// state after both phases so the following g-access consumes matrix slot 0.
	if (_badLineCAccessActive &&
		_badLineCAccessStartCycle > 14 &&
		_cycleInRasterLine == firstBadLineCAccessCycle () &&
		idleStateActive ())
		enterScreenState ();

	// Cycle 16 can advance MCBASE and terminate DMA after the early sprite
	// slots have already occurred. Keep the current-line slot mask intact,
	// but refresh the actual state and the next-line projection immediately.
	if (_cycleInRasterLine == 16)
	{
		_spriteDMAStateMask = spriteDMAMask ();
		const unsigned char nextSpriteMask =
			projectedSpriteDMAMaskForNextRasterLine ();
		if (nextSpriteMask != _nextSpriteDMAMask)
		{
			_nextSpriteDMAMask = nextSpriteMask;
			selectCPUStopWindowsForCurrentAndNextLine ();
			if (_badLineCAccessActive)
				actualizeCPUStopWindowsAfterBadLineChange ();
			else if (recalculatePendingCPUStopPrediction ())
				_IFDEBUG debugCPUStopPredictionRecalculated
					("SpriteDMAProjectionChange");
		}
	}

	// Raster coordinates are independent from the CPU register write. Capture
	// them first so the same context can preserve both output states of cC.
	unsigned short cv = 0, rv = 0, cav = 0;
	const bool rasterVisible = _raster.isInVisibleZone ();
	if (rasterVisible)
	{
		_raster.currentVisiblePosition (cv, rv);
		// Adjusted to draw...
		cav = (cv >> 3) << 3;
	}

	DrawContext dC (_raster.hData ().firstDisplayPosition (), cv, cav, rv);
	if (rasterVisible)
		captureOutputState (dC._beforeCPUWrite);

	// The phi1 comparators observe the register values effective before the
	// CPU write of the current VIC-II cycle. Each border pixel is recorded in
	// the draw context so its state cannot be changed retroactively.
	if (rasterVisible)
		actualizeMainBorderStatus
			(dC, 0, DrawContext::_FIRSTPIXELAFTERCPUWRITE);
	treatSpriteHorizontalStartAtCurrentCycle
		(0, DrawContext::_FIRSTPIXELAFTERCPUWRITE);

	// Phase 4: apply the CPU register write belonging to this absolute cycle.
	// The VIC-II memory accesses performed in phi1 have therefore observed
	// the register value that was active at the beginning of cC.
	bool horizontalDisplayZoneChanged = false, registerWriteApplied = false;
	if (!_pendingRegisterWrites.empty ())
		registerWriteApplied = executePendingRegisterWriteAt
			(cC, &dC._registerEffect, &horizontalDisplayZoneChanged);

	// A $d019 write can clear interrupt flags during phi2. The final IRQ
	// evaluation of this cycle must observe that acknowledgement.
	if (registerWriteApplied &&
		_VICIIRegisters -> interruptsEnabledBack ())
		cpu -> interrupt (_interruptId) ->
			setNewInterruptRequestAdmitted (false);

	// A second state is needed only when the CPU write changes an output value.
	// Every other cycle reuses the initial state without copying it.
	if (rasterVisible &&
		dC._registerEffect._applied && dC._registerEffect._affectsOutput)
		captureOutputState (dC._afterCPUWrite);

	// Phase 5: evaluate post-write comparators and compose the output slice.
	// The phi2 comparators observe writes performed by the CPU during the
	// current VIC-II cycle. A transition already latched in phi1 is retained.
	if (rasterVisible)
		actualizeMainBorderStatus
			(dC, DrawContext::_FIRSTPIXELAFTERCPUWRITE, _raster.step ());
	treatSpriteHorizontalStartAtCurrentCycle
		(DrawContext::_FIRSTPIXELAFTERCPUWRITE, _raster.step ());

	// Bauer vertical-border rules are evaluated once at the final cycle of
	// each raster line. _cyclesPerRasterLine selects cycle 63 or 64 without
	// duplicating this behaviour in the PAL and NTSC implementations.
	if (_cycleInRasterLine == _cyclesPerRasterLine)
		actualizeVerticalBorderStatus ();

	// The drawing path uses the data already fetched by the raster-cycle phase
	// and then applies sprites, priority, collisions and border overlay.
	if (rasterVisible)
		drawVisibleZone (cpu, dC);

	// Phase 6: apply deferred geometry and advance the raster.
	// CSEL changes the host-side horizontal display limits only after the current
	// grouped eight-pixel slice has completed.
	if (horizontalDisplayZoneChanged)
		_raster.hData ().reduceDisplayZone
			(!_VICIIRegisters -> textDisplay40ColumnsActive ());

	// Advance the raster beam by one VIC-II cycle. New-line and new-frame
	// housekeeping is performed by advanceRasterPosition().
	advanceRasterPosition ();

	// Phase 7: evaluate position-dependent events and request IRQ.
	// The IRQ flag is activated at the new raster position; the actual CPU IRQ
	// request is performed after every source has been evaluated.
	treatRasterIRQAtCurrentPosition ();

	// The light pen can latch the mouse position at most once per frame.
	treatLightPenAtCurrentRasterPosition ();

	// Raster, light-pen and collision sources have now had the opportunity to
	// set their flags, so the final request observes all of them.
	requestIRQIfNeeded (cpu, cC);
}

// ---
MCHEmul::InfoStructure COMMODORE::VICII::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::GraphicalChip::getInfoStructure ());

	result.remove ("Memory"); // This info is not neccesary...
	result.add ("VICIIRegisters",				std::move (_VICIIRegisters -> getInfoStructure ()));
	result.add ("Raster",						std::move (_raster.getInfoStructure ()));
	result.add ("VICIIInternal",				std::move (_vicGraphicInfo.getInfoStructure ()));
	result.add ("DENSeenAtLine30",				std::string (_DENSeenAtLine30 ? "YES" : "NO"));
	result.add ("BadlineCondition",				std::string (_badLineConditionActive ? "YES" : "NO"));
	result.add ("BadlinePreventedIdleThisLine", std::string (_badLinePreventedIdleThisLine ? "YES" : "NO"));
	result.add ("BadlineDetected",				std::string (_badLineAlreadyDetectedThisLine ? "YES" : "NO"));
	result.add ("BadlineBARequested",			std::string (_badLineBAAlreadyRequested ? "YES" : "NO"));
	result.add ("BadlineBARequestCycle",		_badLineBARequestCycle);
	result.add ("BadlineFirstCAccessCycle",		firstBadLineCAccessCycle ());
	result.add ("BadlineCAccess",				std::string (_badLineCAccessActive ? "YES" : "NO"));
	result.add ("BadlineCAccessAllowed",		std::string (_badLineCAccessAllowedThisLine ? "YES" : "NO"));
	result.add ("BadlineInvalidCAccessCycles",	_badLineInvalidCAccessCycles);
	result.add ("BadlineCAccessStartCycle",		_badLineCAccessStartCycle);
	result.add ("Cycle",						_cycleInRasterLine);
	result.add ("LastVICDataRead",				_lastVICDataRead.asString (MCHEmul::UByte::OutputFormat::_HEXA));

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

			std::string byDt ="";
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
				{
					for (size_t l = 0; l < 8; l++)
						dt += ((sprDt [(j * 3) + k].value () & (1 << (7 - l))) != 0x00) ? "#" : " ";
				}

				// Accumulate the byte info...
				byDt += "$" + sprDt [(j * 3) + k].asString (MCHEmul::UByte::OutputFormat::_HEXA, 2);
			}

			// ...and draws the data info...
			// It is an addition of the 3 bytes printed out per line...
			dt += "|" + byDt;
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

			// Draws the data info...
			dt += "|$" + chrDt [j].asString (MCHEmul::UByte::OutputFormat::_HEXA, 2);
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
				if (x < 0 || 
					y < 0 ||
					(unsigned short) x >= _raster.visibleColumns () ||
					(unsigned short) y >= _raster.visibleLines ())
					_VICIIRegisters -> setMousePosition (-1, -1); // No in the window...
				else
					_VICIIRegisters -> setMousePosition (x, y);
			}

			break;

		// The lightpen actives when the left button of the mouse is pressed...
		case MCHEmul::InputOSSystem::_MOUSEBUTTONPRESSED:
			{
				if (std::dynamic_pointer_cast <MCHEmul::InputOSSystem::MouseButtonEvent>
					(evnt.data ()) -> _buttonId == 1 /** Left. Right would be 3. */)
					 _lightPenButtonPressed = true;
			}
			
			break;

		// ...and stop being active when released...
		case MCHEmul::InputOSSystem::_MOUSEBUTTONRELEASED:
			{
				if (std::dynamic_pointer_cast <MCHEmul::InputOSSystem::MouseButtonEvent> 
					(evnt.data ()) -> _buttonId == 1 /** Left. */)
					_lightPenButtonPressed = false;
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
	cP [8]  = SDL_MapRGBA (_format, 0x99, 0x69, 0x2d, 0xe0); // Orange
	cP [9]  = SDL_MapRGBA (_format, 0x67, 0x52, 0x00, 0xe0); // Brown
	cP [10] = SDL_MapRGBA (_format, 0xc1, 0x81, 0x78, 0xe0); // Light Red
	cP [11] = SDL_MapRGBA (_format, 0x60, 0x60, 0x60, 0xe0); // Dark Gray
	cP [12] = SDL_MapRGBA (_format, 0x8a, 0x8a, 0x8a, 0xe0); // Medium Gray
	cP [13] = SDL_MapRGBA (_format, 0xb3, 0xec, 0x91, 0xe0); // Light Green
	cP [14] = SDL_MapRGBA (_format, 0x86, 0x7a, 0xde, 0xe0); // Light Blue
	cP [15] = SDL_MapRGBA (_format, 0xb3, 0xb3, 0xb3, 0xe0); // Light Gray

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
	cP [33] = SDL_MapRGBA (_format, 0Xfc, 0Xe7, 0x00, 0xff); // light Yellow
	cP [34] = SDL_MapRGBA (_format, 0Xff, 0X6d, 0x28, 0xff); // light Orange
	cP [35] = SDL_MapRGBA (_format, 0Xea, 0X04, 0x7e, 0xff); // light Purple
	cP [36] = SDL_MapRGBA (_format, 0X3e, 0Xc7, 0x0b, 0xff); // light Green

	return (new MCHEmul::ScreenMemory (numberColumns (), numberRows (), cP));
}

// ---
void COMMODORE::VICII::initializeCPUStopWindowSets ()
{
	for (size_t i = 0; i < _cpuStopWindowSets.size (); i++)
		buildCPUStopWindowSet
			((i & 0x0100) != 0, (unsigned char) (i & 0x00ff),
			 _cpuStopWindowSets [i]);
}

// ---
void COMMODORE::VICII::buildCPUStopWindowSet
	(bool bL, unsigned char sM, COMMODORE::VICII::CPUStopWindows& result) const
{
	result.clear ();
	result.reserve (9);

	if (bL)
		addBadLineCPUStopWindow (result);

	for (size_t i = 0; i < 8; i++)
		if ((sM & (1 << i)) != 0)
			addSpriteCPUStopWindow (i, result);

	mergeCPUStopWindows (result);
}

// ---
void COMMODORE::VICII::addBadLineCPUStopWindow
	(COMMODORE::VICII::CPUStopWindows& result) const
{
	result.emplace_back
		(_BADLINE_START_FIRST_CYCLE,
		 _BADLINE_START_FIRST_CYCLE + 3,
		 _BADLINE_START_LAST_CYCLE,
		 true);
}

// ---
void COMMODORE::VICII::mergeCPUStopWindows
	(COMMODORE::VICII::CPUStopWindows& result) const
{
	if (result.empty ())
		return;

	std::sort
		(result.begin (), result.end (),
		 [] (const CPUStopWindow& w1, const CPUStopWindow& w2) -> bool
			{ return (w1._firstBACycle < w2._firstBACycle); });

	size_t destination = 0;
	for (size_t source = 1; source < result.size (); source++)
	{
		CPUStopWindow& current = result [destination];
		const CPUStopWindow& next = result [source];

		// BA remaining low across adjacent demands means that the 6510 cannot
		// restart an opcode read between them, so they form one effective stop.
		if (next._firstBACycle <= current._lastCycle + 1)
		{
			current._firstAECCycle =
				std::min (current._firstAECCycle, next._firstAECCycle);
			current._lastCycle = std::max (current._lastCycle, next._lastCycle);
			current._badLineSource |= next._badLineSource;
			current._spriteSourceMask |= next._spriteSourceMask;
		}
		else
			result [++destination] = next;
	}

	result.resize (destination + 1);
}

// ---
unsigned char COMMODORE::VICII::spriteDMAMask () const
{
	unsigned char result = 0;
	for (size_t i = 0; i < 8; i++)
		if (_vicSpriteInfo [i]._DMAActive)
			result |= (1 << i);

	return (result);
}

// ---
unsigned char COMMODORE::VICII::projectedSpriteDMAMaskForNextRasterLine () const
{
	unsigned char result = 0;
	for (size_t i = 0; i < 8; i++)
	{
		if (!_vicSpriteInfo [i]._DMAActive)
			continue;

		// Sprites 3..7 fetch at the beginning of the next line, before MCBASE
		// can terminate DMA at cycle 16. Sprites 0..2 fetch afterwards and must
		// be omitted when the two counter steps will consume their final row.
		if (i >= 3 ||
			!_VICIIRegisters -> expansionYFlipFlop (i) ||
			_vicSpriteInfo [i]._MCBASE + 3 < 63)
			result |= (1 << i);
	}

	return (result);
}

// ---
void COMMODORE::VICII::selectCPUStopWindowsForCurrentAndNextLine ()
{
	// ROW must already represent the raster line whose current stop-window
	// pointer is going to be selected. Using Raster directly prevents a stale
	// graphics ROW from transferring a bad-line window to the following line.
	assert (_vicGraphicInfo._ROW == _raster.currentLine ());

	_currentCPUStopWindows = &_cpuStopWindowSets [cpuStopWindowSetIndex
		(badLineConditionForRasterLine (_raster.currentLine ()),
		 _currentSpriteDMAMask)];
	_nextCPUStopWindows = &_cpuStopWindowSets [cpuStopWindowSetIndex
		(badLineConditionForRasterLine (_raster.nextLine ()),
		 _nextSpriteDMAMask)];
}

// ---
void COMMODORE::VICII::actualizeCPUStopWindowsAfterBadLineChange ()
{
	const unsigned short firstBACycle = !_badLineCAccessActive
		? 0
		: ((_badLineCAccessStartCycle <= 14)
			? _badLineCAccessStartCycle
			: firstBadLineCAccessCycle ());

	_badLineBAAlreadyRequested = firstBACycle != 0;
	_badLineBARequestCycle = firstBACycle;

	const CPUStopWindows* noBadLineWindows =
		&_cpuStopWindowSets [cpuStopWindowSetIndex
			(false, _currentSpriteDMAMask)];
	if (!_badLineCAccessActive &&
		_currentCPUStopWindows == noBadLineWindows)
		return;

	// The immutable set remains usable while the condition follows the normal
	// cycle-12 start. Only a late or aborted sequence needs the reusable copy.
	if ((_badLineConditionActive &&
		 _cycleInRasterLine < _BADLINE_START_FIRST_CYCLE) ||
		(_badLineCAccessActive &&
		 _badLineCAccessStartCycle == _BADLINE_START_FIRST_CYCLE))
		_currentCPUStopWindows = &_cpuStopWindowSets [cpuStopWindowSetIndex
			(true, _currentSpriteDMAMask)];
	else
	{
		_adjustedCurrentCPUStopWindows = *noBadLineWindows;

		// Once a c-access sequence has started, clearing the instantaneous Bad
		// Line Condition does not retroactively release its already committed BA
		// interval. An aborted cycle-12/13 sequence is cleared at cycle 14 by the
		// existing graphics-fetch state machine and therefore adds no interval.
		//
		// A sequence latched after cycle 14 starts its BA lead together with its
		// first attempted c-access, in the cycle following recognition. AEC becomes
		// effective after the three initial invalid c-access attempts. Sequences
		// already latched by cycle 14 retain their original BA start cycle.
		if (_badLineCAccessActive && firstBACycle != 0 &&
			firstBACycle <= _BADLINE_START_LAST_CYCLE)
		{
			_adjustedCurrentCPUStopWindows.emplace_back
				(firstBACycle,
				 firstBACycle + 3,
				 _BADLINE_START_LAST_CYCLE,
				 true);

			mergeCPUStopWindows (_adjustedCurrentCPUStopWindows);
		}

		_currentCPUStopWindows = &_adjustedCurrentCPUStopWindows;
	}

	if (recalculatePendingCPUStopPrediction ())
		_IFDEBUG debugCPUStopPredictionRecalculated ("BadLineChange");
}

// ---
bool COMMODORE::VICII::CPUStopWindowAt
	(CPURasterCycle c, const CPUStopWindows& currentWindows,
	 const CPUStopWindows& nextWindows, CPUStopWindow& result) const
{
	for (const auto& i : currentWindows)
		if (i.BAActiveAt (c))
		{
			result = i;

			return (true);
		}

	const CPURasterCycle nextCycle = c - _cyclesPerRasterLine;
	for (const auto& i : nextWindows)
		if (i.BAActiveAt (nextCycle))
		{
			result = CPUStopWindow
				(i._firstBACycle + _cyclesPerRasterLine,
				 i._firstAECCycle + _cyclesPerRasterLine,
				 i._lastCycle + _cyclesPerRasterLine,
				 i._badLineSource,
				 i._spriteSourceMask);

			return (true);
		}

	return (false);
}

// ---
COMMODORE::VICII::CPUStopPrediction COMMODORE::VICII::calculateCPUStopPrediction
	(const MCHEmul::CycleStructure& cS, const MCHEmul::BusCycleData& bD,
	 unsigned int nC, CPURasterCycle startCycle,
	 const CPUStopWindows& currentWindows, const CPUStopWindows& nextWindows) const
{
	assert (cS.size () == nC && bD._numberCycles == nC);
	assert (bD._numberWriteCycles <= _MAXCPUTRANSACTIONWRITES);

	CPUStopPrediction result;
	result._valid = true;

	CPURasterCycle realCycle = startCycle;
	size_t instructionCycle = 0;
	size_t writeIndex = 0;
	CPUStopWindow window;

	while (instructionCycle < nC)
	{
		const bool readCycle =
			bD.nextReadCycle (instructionCycle) == instructionCycle;
		const bool writeCycle =
			writeIndex < bD._numberWriteCycles &&
			bD.writeCycle (writeIndex) == instructionCycle;

		if (CPUStopWindowAt
			(realCycle, currentWindows, nextWindows, window))
		{
			// BA immediately holds reads. Writes are allowed only during the lead
			// interval before AEC removes the CPU address and data bus drivers.
			if (readCycle || !writeCycle || !window.CPUOwnsBusAt (realCycle))
			{
				if (!result._stopRequired)
				{
					result._stopRequired = true;
					result._firstStopCycle = realCycle;
				}

				realCycle = window._lastCycle + 1;
				continue;
			}
		}

		if (writeCycle)
			result._positionsToWriteEffects [writeIndex++] =
				(unsigned int) (realCycle - startCycle);

		instructionCycle++;
		realCycle++;
	}

	assert (writeIndex == bD._numberWriteCycles);

	result._instructionEffectCycle = realCycle - 1;

	// The next opcode fetch is an implicit read. It must be considered even
	// when the instruction itself completed immediately before BA went low.
	while (CPUStopWindowAt (realCycle, currentWindows, nextWindows, window))
	{
		if (!result._stopRequired)
		{
			result._stopRequired = true;
			result._firstStopCycle = realCycle;
		}

		realCycle = window._lastCycle + 1;
	}

	result._firstNormalCycle = realCycle;
	result._cyclesToStop = (unsigned int)
		(result._firstNormalCycle - (startCycle + (CPURasterCycle) nC));
	result._positionsToInstructionEffect = (unsigned int)
		(result._instructionEffectCycle - startCycle);

	return (result);
}

// ---
void COMMODORE::VICII::prepareCPUStopPrediction
	(const MCHEmul::CycleStructure* cS,
	 const MCHEmul::BusCycleData* bD, unsigned int nC,
	 unsigned int startCPUCycle)
{
	assert (cS != nullptr);
	assert (bD != nullptr);
	assert (cS -> size () == nC);
	assert (bD -> _numberCycles == nC);
	assert (bD -> _numberWriteCycles <= _MAXCPUTRANSACTIONWRITES);
	assert (_pendingRegisterWrites.empty ());

	_pendingCPUTransaction._cycleStructure = cS;
	_pendingCPUTransaction._busCycleData = bD;
	_pendingCPUTransaction._clockCycles = nC;
	_pendingCPUTransaction._startCycle = _cycleInRasterLine;
	_pendingCPUTransaction._startCPUCycle = startCPUCycle;

	_pendingCPUStopPrediction = calculateCPUStopPrediction
		(*cS, *bD, nC, _pendingCPUTransaction._startCycle,
		 *_currentCPUStopWindows, *_nextCPUStopWindows);

	_VICIIRegisters -> setRasterReadContext
		(_vicGraphicInfo._ROW, _cycleInRasterLine);
	_VICIIRegisters -> setNumberPositionsToInstructionEffect
		(_pendingCPUStopPrediction._positionsToInstructionEffect);
}

// ---
bool COMMODORE::VICII::recalculatePendingCPUStopPrediction ()
{
	if (!_pendingCPUTransaction.valid () ||
		_pendingCPUStopPrediction._stopRequested)
		return (false);

	_pendingCPUStopPrediction = calculateCPUStopPrediction
		(*_pendingCPUTransaction._cycleStructure,
		 *_pendingCPUTransaction._busCycleData,
		 _pendingCPUTransaction._clockCycles,
		 _pendingCPUTransaction._startCycle,
		 *_currentCPUStopWindows, *_nextCPUStopWindows);

	_VICIIRegisters -> setNumberPositionsToInstructionEffect
		(_pendingCPUStopPrediction._positionsToInstructionEffect);

	return (true);
}

// ---
void COMMODORE::VICII::requestPredictedCPUStopIfNeeded
	(MCHEmul::CPU* cpu, unsigned int cC)
{
	if (!_pendingCPUTransaction.valid () ||
		!_pendingCPUStopPrediction._stopRequired ||
		_pendingCPUStopPrediction._stopRequested ||
		_cycleInRasterLine != _pendingCPUStopPrediction._firstStopCycle)
		return;

	cpu -> setStop
		(true, MCHEmul::InstructionDefined::_CYCLEALL, cC,
		 (int) _pendingCPUStopPrediction._cyclesToStop);

	_pendingCPUStopPrediction._stopRequested = true;

	_IFDEBUG debugCPUStopRequested (cC);
}

// ---
void COMMODORE::VICII::extractPendingRegisterWrites ()
{
	// A non-empty collection belongs to a transaction that is still physically
	// completing while the CPU remains stopped.
	if (!_pendingRegisterWrites.empty ())
		return;

	_pendingRegisterWrites =
		MCHEmul::Memory::configuration ().
			extractMemorySetCommandsBuffered (_VICIIRegisters);

	if (_pendingRegisterWrites.empty ())
		return;

	assert (_pendingCPUTransaction.valid ());
	assert (_pendingCPUTransaction._busCycleData != nullptr);

	// This correspondence is valid for the 6500: an instruction writing VIC-II
	// registers directs all its data-write cycles to the same effective address.
	assert (_pendingRegisterWrites.size () ==
		_pendingCPUTransaction._busCycleData -> _numberWriteCycles);
}

// ---
bool COMMODORE::VICII::executePendingRegisterWriteAt
	(unsigned int cC, COMMODORE::VICII::DrawContext::RegisterEffect* rE, bool* hDZC)
{
	assert (rE != nullptr);
	assert (hDZC != nullptr);
	*hDZC = false;

	if (_pendingRegisterWrites.empty ())
		return (false);

	assert (_pendingCPUTransaction.valid ());
	assert (_pendingCPUTransaction._busCycleData != nullptr);
	assert (_pendingRegisterWrites.size () <=
		_pendingCPUTransaction._busCycleData -> _numberWriteCycles);

	// The number of already consumed writes is derived from the commands that
	// remain, avoiding a separate cursor synchronized with the vector.
	const size_t writeIndex =
		_pendingCPUTransaction._busCycleData -> _numberWriteCycles -
		_pendingRegisterWrites.size ();

	if (cC !=
		_pendingCPUTransaction._startCPUCycle +
		_pendingCPUStopPrediction._positionsToWriteEffects [writeIndex])
		return (false);

	MCHEmul::SetMemoryCommand& registerWrite = _pendingRegisterWrites [0];
	const size_t registerPosition = registerWrite.position () % 0x40;
	rE -> _registerPosition = registerPosition;
	rE -> _previousValue = _VICIIRegisters -> storedRegisterValue (registerPosition);
	rE -> _newValue = registerWrite.value ();
	rE -> _affectsOutput = registerAffectsOutputState (registerPosition);

	registerWrite.execute ();
	rE -> _applied = true;

	// The register value becomes visible during the CPU phase of cC. A CSEL
	// change is therefore available to the later comparator phase, while its
	// host-side horizontal display geometry remains deferred until this slice
	// has been composed.
	*hDZC = (registerPosition == 0x16);

	_IFDEBUG debugVICIIRegisterWriteApplied
		(cC, &registerWrite, writeIndex, _pendingRegisterWrites.size () - 1);

	_pendingRegisterWrites.erase (_pendingRegisterWrites.begin ());

	return (true);
}

// ---
void COMMODORE::VICII::treatRasterBusCycle ()
{
	// Read graphics zone?
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
					_IFDEBUG debugReadingSpriteInfo (nSR);
				}
			}

			break;

		// In raster cycle 14 the graphics information moves...
		case 14:
			{
				treatGraphicFetchStartCycle ();
			}

			break;

		// Sprite MCBASE is advanced in two steps. The graphic c/g access for
		// these cycles is still handled later by treatGraphicAccessCycle().
		case 15:
			{
				treatSpriteCounterCycle15 ();
			}

			break;

		// Second MCBASE update step. If this reaches the end of the 63-byte
		// sprite definition, DMA stops but the current visible line is kept.
		case 16:
			{
				treatSpriteCounterCycle16 ();
			}

			break;

		// DMA is decided in cycles 55/56 before the sprite data slot.
		// At cycle 58 MC is loaded from MCBASE and display can start.
		case 58:
			{
				treatSpriteDisplayStartCycle ();

				treatGraphicRowEndCycle ();
			}

			break;

		default:
			break;
	}

	treatGraphicAccessCycle ();
}

// ---
void COMMODORE::VICII::drawVisibleZone
	(MCHEmul::CPU* cpu, const COMMODORE::VICII::DrawContext& dC)
{
	// A slice can cross either horizontal comparator. Determine whether every
	// visible pixel is covered without deriving the answer from the final
	// flip-flop state, which describes only the last pixel of the slice.
	size_t visiblePixels;
	visiblePixels = (dC._RCA + 8) >= _raster.visibleColumns ()
		? (size_t) (_raster.visibleColumns () - dC._RCA)
		: 8;
	bool borderCoversEntireSlice = true;
	for (size_t i = 0; i < visiblePixels && borderCoversEntireSlice; i++)
		borderCoversEntireSlice = dC.mainBorderAtPixel (i);

	drawOutputStateLine (dC, (size_t) dC._RCA, (size_t) dC._RR,
		visiblePixels, borderCoversEntireSlice);

	// The priority-multiplexer output is only copied when some part of the
	// current slice is not covered by the main border.
	drawGraphicsSpritesAndDetectCollisions
		(dC, !borderCoversEntireSlice);

	// The main border is the final and highest-priority video layer. Draw each
	// contiguous span so a transition in either half-cycle retains its exact
	// pixel position and the corresponding before/after border color.
	for (size_t i = 0; !borderCoversEntireSlice && i < visiblePixels; )
	{
		if (!dC.mainBorderAtPixel (i))
		{
			i++;
			continue;
		}

		const size_t firstBorderPixel = i;
		while (i < visiblePixels && dC.mainBorderAtPixel (i)) i++;
		drawOutputStateLine (dC, (size_t) dC._RCA + firstBorderPixel,
			(size_t) dC._RR, i - firstBorderPixel, true);
	}

	// If there were requested to draw the position where the Raster Interrupt is generated...
	// All draw are replace by a line with a color different of the background!
	// The line is only drawn if that position where within the visible zone!
	if (_drawRasterInterruptPositions)
	{
		unsigned short lrt = 
			_raster.lineInVisibleZone (_VICIIRegisters -> IRQRasterLineAt ());
		if (lrt <= _raster.vData ().lastVisiblePosition ())
		{
			const COMMODORE::VICII::DrawContext::OutputState& oS =
				dC.outputStateAfterCPUWrite ();
			unsigned int cl = oS._backgroundColors [0] == 15
				? 0 : oS._backgroundColors [0] + 1; /** to be visible. */
			screenMemory () -> setHorizontalLine ((size_t) dC._RCA, (size_t) lrt,
				(dC._RCA + 8) > _raster.visibleColumns () ?
					(_raster.visibleColumns () - dC._RCA) : 8, cl);
			if (dC._RCA >= 8 && dC._RCA < 40 && lrt > 6)
				screenMemory () -> setString ((size_t) 0, (size_t) (lrt - 6), 
					std::to_string ((unsigned int) _VICIIRegisters -> IRQRasterLineAt ()) + " " +
					std::to_string ((unsigned int) lrt), cl);
		}
	}

	// The draw around the sprites is done as part of the sprite draw routine...

	// If it activated to draw other events that happen during the interation of the VICII...
	if (_drawOtherEvents)
		drawOtherEvents (dC._RCA, dC._RR);
}

// ---
void COMMODORE::VICII::drawOutputStateLine
	(const COMMODORE::VICII::DrawContext& dC,
	 size_t p, size_t r, size_t n, bool border)
{
	if (n == 0)
		return;
	assert (p >= (size_t) dC._RCA && p + n <= (size_t) dC._RCA + 8);

	const size_t transitionPosition =
		(size_t) dC._RCA + DrawContext::_FIRSTPIXELAFTERCPUWRITE;

	// Most cycles have no visual write and retain the previous single call.
	if (!dC.outputChangesDuringSlice () || p + n <= transitionPosition)
	{
		screenMemory () -> setHorizontalLine (p, r, n, border
			? dC._beforeCPUWrite._borderColor
			: dC._beforeCPUWrite._backgroundColors [0]);

		return;
	}

	if (p >= transitionPosition)
	{
		screenMemory () -> setHorizontalLine (p, r, n, border
			? dC._afterCPUWrite._borderColor
			: dC._afterCPUWrite._backgroundColors [0]);

		return;
	}

	const size_t beforePixels = transitionPosition - p;
	screenMemory () -> setHorizontalLine (p, r, beforePixels, border
		? dC._beforeCPUWrite._borderColor
		: dC._beforeCPUWrite._backgroundColors [0]);
	screenMemory () -> setHorizontalLine
		(transitionPosition, r, n - beforePixels, border
			? dC._afterCPUWrite._borderColor
			: dC._afterCPUWrite._backgroundColors [0]);
}

// ---
void COMMODORE::VICII::drawGraphicsSpritesAndDetectCollisions 
	(const COMMODORE::VICII::DrawContext& dC, bool dTS)
{
	// This varible keeps info about the text/graphics:
	// Whether the 8 pixels to draw are foreground or background...
	// ...and the color of the ones that are not finally background!
	// And also info to control later the collision with sprites!
	COMMODORE::VICII::DrawResult colGraphics;
	const bool outputTransition = dC.outputChangesDuringSlice ();
	const size_t firstLimit = outputTransition
		? DrawContext::_FIRSTPIXELAFTERCPUWRITE : 8;

	drawGraphics (dC, dC._beforeCPUWrite, 0, firstLimit, colGraphics);
	if (outputTransition)
		drawGraphics (dC, dC._afterCPUWrite,
			DrawContext::_FIRSTPIXELAFTERCPUWRITE, 8, colGraphics);

	// The info about the sprites is moved into this variable too...
	MCHEmul::UByte sCF = MCHEmul::UByte::_0; // to know whether there were at least one sprite drawn!
	for (int i = 7; i >= 0; i--)
	{
		if (_vicSpriteInfo [(size_t) i]._displayActive)
		{
			colGraphics._collisionSpritesData [(size_t) i] = 
				drawSpriteOver ((size_t) i, dC, colGraphics._spriteColor,
					colGraphics._spriteColorOwner);

			// If a sprite has drawn something, the bit with its number will be set!
			// It can be later used to even speed up more the detection of the collisions...
			if (colGraphics._collisionSpritesData [(size_t) i] != MCHEmul::UByte::_0)
				sCF.setBit ((size_t) i, true);
		}
	}

	// The graphical info is moved to the screen...
	// ..if it has to!
	if (dTS)
	{
		drawResultToScreen (colGraphics, dC, dC._beforeCPUWrite, 0, firstLimit);
		if (outputTransition)
			drawResultToScreen (colGraphics, dC, dC._afterCPUWrite,
				DrawContext::_FIRSTPIXELAFTERCPUWRITE, 8);
	}

	// ...and the collisions are also detected...
	// when there were sprites drawn!
	// It is done just to speed up the while drawn cycle a lot!
	if (sCF != MCHEmul::UByte::_0)
		detectCollisions (colGraphics, sCF, dC._verticalBorderData);
}

// ---
void COMMODORE::VICII::drawGraphics
	(const COMMODORE::VICII::DrawContext& dC,
	 const COMMODORE::VICII::DrawContext::OutputState& oS,
	 size_t fP, size_t lP, COMMODORE::VICII::DrawResult& result)
{
	assert (fP < lP && lP <= 8);

	/** IMPORTANT NOTE:
		When it is in idle state it also drawn
		With the content of a fixed position depending on the active bacnk and always in color 0. \n
		When the raster cycle is 16, _RC = 28 and _RCA = 24.
		The VIC should start to draw at that cycle. 
		As the code is now, using _RCA instead _RC to determine the initial position to start the drawing,
		that wouldn't happen until cycle 17. */

	// The "display" column being involved...
	// In cb, the SCROLLX is involved, so it could be negative! starting from -7, 
	// When e.g. the raster is at the very first "screen "dislay" column, 
	// there are no reductions is the screen (display == screen) and SCROLLX = 0x00
	int cb = (int) dC._RCA - (int) dC._ICD - (int) oS._horizontalScroll;
	// To draw just only when there is a possibility of at least draw a pixel!
	if (cb <= -8)
		return;

	// At this point rc positive for sure, and cb could be negative...
	// Never invoke the methods within the swith case statements direcly
	// a crash might be generated...

	// Record the raster position and graphics-buffer interval selected for this
	// output span. A visual CPU write therefore emits one phi1 and one phi2 record.
	_IFDEBUG debugDrawPixelAt
		(dC._RCA, cb, oS._horizontalScroll, fP, lP);

	switch (oS._graphicMode)
	{
		case COMMODORE::VICIIRegisters::GraphicMode::_CHARMODE:
			{
				drawMonoColorChar (cb, oS, fP, lP, result);
			}

			break;

		case COMMODORE::VICIIRegisters::GraphicMode::_MULTICOLORCHARMODE:
			{
				drawMultiColorChar (cb, oS, fP, lP, result);
			}

			break;

		case COMMODORE::VICIIRegisters::GraphicMode::_EXTENDEDBACKGROUNDMODE:
			{
				drawMultiColorExtendedChar (cb, oS, fP, lP, result);
			}

			break;

		case COMMODORE::VICIIRegisters::GraphicMode::_BITMAPMODE:
			{
				drawMonoColorBitMap (cb, oS, fP, lP, result);
			}

			break;

		case COMMODORE::VICIIRegisters::GraphicMode::_MULTICOLORBITMAPMODE:
			{
				drawMultiColorBitMap (cb, oS, fP, lP, result);
			}

			break;

		case COMMODORE::VICIIRegisters::GraphicMode::_INVALIDTEXMODE:
			{
				// Like multicolor char mode, but invalid...
				drawMultiColorChar (cb, oS, fP, lP, result, true);
				for (size_t i = fP; i < lP; i++)
					result._invalidGraphicData.setBit (7 - i, true);
			}

			break;

		case COMMODORE::VICIIRegisters::GraphicMode::_INVALIDBITMAPMODE1:
			{
				// Like moncocolor bitmap mode, but invalid...
				drawMonoColorBitMap (cb, oS, fP, lP, result, true);
				for (size_t i = fP; i < lP; i++)
					result._invalidGraphicData.setBit (7 - i, true);
			}
			break;

		case COMMODORE::VICIIRegisters::GraphicMode::_INVALIDBITMAPMODE2:
			{
				// Like multicolor bitmap mode, but invalid...
				drawMultiColorBitMap (cb, oS, fP, lP, result, true);
				for (size_t i = fP; i < lP; i++)
					result._invalidGraphicData.setBit (7 - i, true);
			}

			break;

		default:
			assert (false); // Not possible...the code shouldn't pass over this point!
			break;
	}
}

// ---
void COMMODORE::VICII::drawMonoColorChar
	(int cb, const COMMODORE::VICII::DrawContext::OutputState& oS,
	 size_t fP, size_t lP, COMMODORE::VICII::DrawResult& result)
{
	const bool idle = oS._idleState;

	for (size_t i = fP; i < lP; i++)
	{
		int pp = cb + (int) i;
		if (pp < 0)
			continue; // The pixel is not still visible...
		if (pp >= 320)
			break; // No more pixels to draw...

		size_t iBy = ((size_t) pp) >> 3; // To determine the byte...
		size_t iBt = 7 - (((size_t) pp) % 8); // From MSB to LSB...

		if (_vicGraphicInfo._graphicData [iBy].bit (iBt))
		{
			result._collisionGraphicData.setBit (7 - i, true);

			result._foregroundColorData [i] = idle 
				? 0x00 // In idle state the color is always 0...
				: (unsigned int) (_vicGraphicInfo._colorDrawData [iBy].value () & 0x0f /** Useful nibble. */);
		}

		// When false, it is background...
		// Not necessary to specify neither collision information
		// nor the color of the pixels as it will be always the basic background color,
		// that has already been set to the value of $d021 in the main loop...
	}
}

// ---
void COMMODORE::VICII::drawMultiColorChar
	(int cb, const COMMODORE::VICII::DrawContext::OutputState& oS,
	 size_t fP, size_t lP, COMMODORE::VICII::DrawResult& result, bool inv)
{
	const bool idle = oS._idleState;

	for (size_t i = fP; i < lP; i++)
	{
		int pp = cb + (int) i;
		if (pp < 0)
			continue; // The pixel is not still visible...
		if (pp >= 320)
			break; // No more pixels to draw...

		size_t iBy = ((size_t) pp) >> 3;
		size_t iBt = 3 - ((((size_t) pp) % 8) >> 1);
		unsigned char cs = (_vicGraphicInfo._graphicData [iBy].value () >> (iBt << 1)) & 0x03; // 0, 1, 2 or 3

		// If it is invalid all colors are black, including the background...
		// Rememeber that at this point that background will have been already drawn...
		if (inv)
			result._backgroundColorData [i] = 0x00;

		// If 0, the pixel should be drawn (and considered) as background 
		// and it is already the default status tha comes from the parent method...
		if (cs == 0x00)
			continue;

		// The way the pixels are going to be drawn will depend on the information in the color memory
		// If the most significant bit of the low significant nibble of the color memory is set to 0
		// the data will be managed in a monolor way...
		if ((_vicGraphicInfo._colorDrawData [iBy] & 0x08) == 0x00 ||
			 idle) // The idle state is treated as monocolor...
		{
			unsigned int fc = 
				(inv || idle) // also in idle state it is black...
					? 0x00 // When invalid or idle all pixels are black...
					: _vicGraphicInfo._colorDrawData [iBy].value () & 0x07;

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
					? 0x00 // unless the mode is invalid where everything is black...
					: (unsigned int) ((cs == 0x03) 
						? (_vicGraphicInfo._colorDrawData [iBy].value () & 0x07)
						: oS._backgroundColors [cs]);

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
}

// ---
void COMMODORE::VICII::drawMultiColorExtendedChar
	(int cb, const COMMODORE::VICII::DrawContext::OutputState& oS,
	 size_t fP, size_t lP, COMMODORE::VICII::DrawResult& result)
{
	// The mode is quite similar to the standard text mode, 
	// with the difference the 0 pixels (background) can have different background colors...

	const bool idle = oS._idleState;

	for (size_t i = fP; i < lP; i++)
	{
		int pp = cb + (int) i;
		if (pp < 0)
			continue; // The pixel is not still visible...
		if (pp >= 320)
			break; // No more pixels to draw...

		size_t iBy = ((size_t) pp) >> 3; // To determine the byte...
		size_t iBt = 7 - (((size_t) pp) % 8); // From MSB to LSB...
		// The color of the pixel 0 is determined by the 2 MSBites of the char code...
		bool bS = _vicGraphicInfo._graphicData [iBy].bit (iBt); // To know whether the bit is 1 or 0...
		unsigned int cs = ((_vicGraphicInfo._screenCodeDrawData [iBy].value () & 0xc0) >> 6) & 0x03; // 0, 1, 2, or 3
		unsigned int fc = idle 
			? 0x00 // In idle state the color is always 0...
			: (bS 
				? (_vicGraphicInfo._colorDrawData [iBy].value () & 0x0f)
				: oS._backgroundColors [cs]);

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
}

// ---
void COMMODORE::VICII::drawMonoColorBitMap
	(int cb, const COMMODORE::VICII::DrawContext::OutputState& oS,
	 size_t fP, size_t lP, COMMODORE::VICII::DrawResult& result, bool inv)
{
	const bool idle = oS._idleState;

	for (size_t i = fP; i < lP; i++)
	{
		int pp = cb + (int) i;
		if (pp < 0)
			continue; // The pixel is not still visible...
		if (pp >= 320)
			break; // No more pixels to draw...

		size_t iBy = ((size_t) pp) >> 3; // To determine the byte...
		size_t iBt = 7 - (((size_t) pp) % 8); // From MSB to LSB...
		bool bS = _vicGraphicInfo._graphicData [iBy].bit (iBt);
		unsigned int fc = 
			(inv || idle)
				? 0x00 // When invalid or idle state, all pixels are black...
				: bS 
					? (_vicGraphicInfo._screenCodeDrawData [iBy].value () & 0xf0) >> 4	// If the bit is 1, the color is determined by the MSNibble
					: (_vicGraphicInfo._screenCodeDrawData [iBy].value () & 0x0f);		// ...and for LSNibble if it is 0...

		// If the mode is invalid all bits must be invalid...
		// Reme,ber that at this point the background will have been already drawn
		// so it is needed to draw it back all black!
		if (inv)
			result._backgroundColorData [i] = 0x00;

		// And then attending to the status of the pixels itself
		// it would be drawn either as background or foregrand and 
		// taken into account for the collision purposes
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
}

// ---
void COMMODORE::VICII::drawMultiColorBitMap
	(int cb, const COMMODORE::VICII::DrawContext::OutputState& oS,
	 size_t fP, size_t lP, COMMODORE::VICII::DrawResult& result, bool inv)
{
	const bool idle = oS._idleState;

	for (size_t i = fP; i < lP; i++)
	{
		int pp = cb + (int) i;
		if (pp < 0) // The pixel is not still visible...
			continue;
		if (pp >= 320)
			break; // No more pixels to draw...

		size_t iBy = ((size_t) pp) >> 3;
		size_t iBt = 3 - ((((size_t) pp) % 8) >> 1);
		unsigned char cs = (_vicGraphicInfo._graphicData [iBy].value () >> (iBt << 1)) & 0x03; // 0, 1, 2 or 3

		// If the mode is invalid all bits must be invalid...
		// Reme,ber that at this point the background will have been already drawn
		// so it is needed to draw it back all black!
		if (inv)
			result._backgroundColorData [i] = 0x00;

		// If 0, the pixel should be drawn (and considered) as background 
		// and it is already the default status tha comes from the parent method...
		if (cs == 0x00)
			continue;

		unsigned fc = // The value 0x00 is not tested....
				(inv || idle)
					? 0x00 // When invalid or idle state all pixels are black...
					: (cs == 0x01) // The color is the defined in the video matrix, high nibble...
						? (_vicGraphicInfo._screenCodeDrawData [iBy].value () & 0xf0) >> 4
						: ((cs == 0x02) // The color is defined in the video matrix, low nibble...
							? (_vicGraphicInfo._screenCodeDrawData [iBy].value () & 0x0f)
							: (_vicGraphicInfo._colorDrawData [iBy].value () & 0x0f)); // The color is defined in color matrix...

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
}

// ---
MCHEmul::UByte COMMODORE::VICII::drawSpriteOver
	(size_t spr, const COMMODORE::VICII::DrawContext& dC, unsigned int* d, size_t* dO)
{
	MCHEmul::UByte result = MCHEmul::UByte::_0;
	if (!_vicSpriteInfo [spr]._drawing ||
		_vicSpriteInfo [spr]._graphicsLineSprites.size () == 0)
		return (result);

	const bool outputTransition = dC.outputChangesDuringSlice ();
	const size_t firstLimit = outputTransition
		? DrawContext::_FIRSTPIXELAFTERCPUWRITE : 8;
	const unsigned short column = _raster.currentColumn (), row = _vicGraphicInfo._ROW;

	if (((dC._beforeCPUWrite._spriteMulticolorMask >> spr) & 0x01) != 0)
		drawMultiColorSpriteOver (column, row, spr, dC._beforeCPUWrite,
			0, firstLimit, d, dO, result);
	else
		drawMonoColorSpriteOver (column, row, spr, dC._beforeCPUWrite,
			0, firstLimit, d, dO, result);

	if (outputTransition)
	{
		if (((dC._afterCPUWrite._spriteMulticolorMask >> spr) & 0x01) != 0)
			drawMultiColorSpriteOver (column, row, spr, dC._afterCPUWrite,
				DrawContext::_FIRSTPIXELAFTERCPUWRITE, 8, d, dO, result);
		else
			drawMonoColorSpriteOver (column, row, spr, dC._afterCPUWrite,
				DrawContext::_FIRSTPIXELAFTERCPUWRITE, 8, d, dO, result);
	}

	return (result);
}

// ---
void COMMODORE::VICII::drawMonoColorSpriteOver
	(unsigned short c, unsigned short r, size_t spr,
	 const COMMODORE::VICII::DrawContext::OutputState& oS, size_t fP, size_t lP,
	 unsigned int* d, size_t* dO, MCHEmul::UByte& result)
{
	// Horizontal info about the sprite
	unsigned short dW	= ((oS._spriteDoubleWidthMask >> spr) & 0x01) != 0 ? 2 : 1;
	unsigned short x	= _vicSpriteInfo [spr]._xS, dW8 = dW << 3; // 8 or 16
	unsigned short wX	= 24 /** normal width in pixels. */ * dW;
	unsigned short wY	= _vicSpriteInfo [spr]._expansionY ? 42 : 21;

	// This method is reached only for display-active sprites.
	// Vertical eligibility has already been decided by the sprite DMA/display logic.
	// Which means the bits have to be shifted from the composition register
	// No need to check whether the y position is within or not the limits
	for (size_t i = fP; i < lP; i++)
	{
		unsigned short pp = c + (unsigned short) i,
			pS = (pp >= x)
			? pp - x
			: ((_cyclesPerRasterLine << 3) + pp - x);

		if (pS >= wX)
			continue;

		// To determine the initial byte (iBy) and bit (iBt) with the info about the sprite...
		// The bit moves from 7 to 0, and the byte increases...
		size_t iBy = (size_t) (pS / dW8);
		size_t iBt = (size_t) (7 - ((pS % dW8) / dW));

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
			if ((pS == 0 || pS == (wX - 1)) ||
				(r == y || r == (y + wY - 1)))
					{ d [i] = 32 /** cyan. */; dO [i] = spr; }
		}

		if (!_vicSpriteInfo [spr]._graphicsLineSprites [iBy].bit (iBt))
			continue; // The point is not visible...

		result.setBit (7 - i, true);
		d [i] = oS._spriteColors [spr];
		dO [i] = spr;
	}
}

// ---
void COMMODORE::VICII::drawMultiColorSpriteOver
	(unsigned short c, unsigned short r, size_t spr,
	 const COMMODORE::VICII::DrawContext::OutputState& oS, size_t fP, size_t lP,
	 unsigned int* d, size_t* dO, MCHEmul::UByte& result)
{
	// Horizontal info about the sprite
	unsigned short dW	= ((oS._spriteDoubleWidthMask >> spr) & 0x01) != 0 ? 2 : 1;
	unsigned short x	= _vicSpriteInfo [spr]._xS,
		dW8 = dW << 3, dW2 = dW << 1; // 8 or 16, and 2 or 4
	unsigned short wX	= 24 /** normal width in pixels. */ * dW;
	unsigned short wY	= _vicSpriteInfo [spr]._expansionY ? 42 : 21;

	// This method is reached only for display-active sprites.
	// Vertical eligibility has already been decided by the sprite DMA/display logic.
	// Which means the bits have to be shifted from the composition register
	// No need to check whether the y position is within or not the limits
	for (size_t i = fP; i < lP; i++)
	{
		unsigned short pp = c + (unsigned short) i,
			pS = (pp >= x)
			? pp - x
			: ((_cyclesPerRasterLine << 3) + pp - x);

		if (pS >= wX)
			continue;

		// To determine the initial byte (iBy) and bit (iBt) with the info about the sprite...
		// The bit to select moves from 0 to 3, represeting the pair of bits (0,1), (2,3), (4,5), (6,7)
		size_t iBy = (size_t) (pS / dW8);
		size_t iBt = (size_t) (3 - ((pS % dW8) / dW2));

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
			if ((pS == 0 || pS == (wX - 1)) ||
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
				? oS._spriteSharedColors [0]
				: ((cs == 0x02) 
					? oS._spriteColors [spr]
					: oS._spriteSharedColors [1]));
		dO [i] = spr;
	}
}

// ---
void COMMODORE::VICII::drawResultToScreen
	(const COMMODORE::VICII::DrawResult& cT, const COMMODORE::VICII::DrawContext& dC,
	 const COMMODORE::VICII::DrawContext::OutputState& oS, size_t fP, size_t lP)
{
	assert (fP < lP && lP <= 8);

	// Pixel by pixel..
	for (size_t i = fP; i < lP; i++)
	{
		size_t pos = (size_t) dC._RCA + i;
		const size_t spriteOwner = cT._spriteColorOwner [i];

		// The vertical border flip-flop suppresses graphics data at the pixel
		// multiplexer, but sprite output continues. The main-border layer, when
		// active, is applied afterwards and still covers both of them.
		if (dC.verticalBorderAtPixel (i))
		{
			if (spriteOwner != MCHEmul::_S0)
				screenMemory () -> setPixel
					(pos, (size_t) dC._RR, cT._spriteColor [i]);

			continue;
		}

		// Invalid graphic modes generate black graphics, but sprites still pass
		// through the priority multiplexer using the underlying foreground mask.
		if (cT._invalidGraphicData.bit (7 - i))
		{
			// The graphic sequencer output is black by default...
			screenMemory () -> setPixel (pos, (size_t) dC._RR, 0x00 /** black. */);

			if (spriteOwner != MCHEmul::_S0 &&
				(((oS._spritePriorityMask >> spriteOwner) & 0x01) == 0 ||
				 !cT._collisionGraphicData.bit (7 - i)))
				// ...but a sprite remains visible when it has priority or the
				// underlying graphic pixel is background.
				screenMemory () -> setPixel (pos, (size_t) dC._RR, cT._spriteColor [i]);

			continue;
		}

		// The background is drawn...
		// Although the background was already drawn in the main loop
		// as a consequence of managing graphics pixels other than 0 (in multicolor modes e.g.) might also 
		// be part of the background...
		if (cT._backgroundColorData [i] != MCHEmul::_U0)
			screenMemory () -> setPixel (pos, (size_t) dC._RR, cT._backgroundColorData [i]);

		// If that sprite didn't exist at that pixel..
		if (spriteOwner == MCHEmul::_S0)
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
			if (((oS._spritePriorityMask >> spriteOwner) & 0x01) != 0 &&
				cT._foregroundColorData [i] != MCHEmul::_U0)
					screenMemory () -> setPixel (pos, (size_t) dC._RR, cT._foregroundColorData [i]);
		}
	}
}

// ---
void COMMODORE::VICII::detectCollisions
	(const DrawResult& cT, const MCHEmul::UByte& sD,
	 const MCHEmul::UByte& verticalBorderData)
{
	// Now it is time to detect collisions...
	// First among graphics and sprites. Graphics data is suppressed only at
	// pixels covered by the vertical border; sprite-sprite collisions remain
	// independent from both border flip-flops.
	MCHEmul::UByte activeGraphicData;
	activeGraphicData = cT._collisionGraphicData &
		verticalBorderData.complement ();
	if (activeGraphicData != MCHEmul::UByte::_0)
	{
		bool cGS = false;
		for (size_t i = 0; i < 8; i++)
		{
			if (!sD.bit (i))
				continue; // This sprite didn't exist at that block of pixels, 
						  // so it can't be in collision with the graphics...
				
			// ...at the first collision detected, the check stops...
			if ((cT._collisionSpritesData [i] &
				 activeGraphicData) != MCHEmul::UByte::_0)
			{
				// At least one collision between graphics 
				// and the sprite has happened...
				cGS = true; 

				// ...and marked it for that sprite...
				_VICIIRegisters -> setSpriteCollisionWithDataHappened (i);
			}
		}

		// If the collision between sprites and graphics has happened, 
		// the corresponding IRQ is activated...
		if (cGS) 
			_VICIIRegisters -> activateSpriteCollisionWithDataIRQ ();
	}
	
	// ...and among sprites...
	bool cSS = false;
	for (size_t i = 0; i < 8; i++)
	{
		if (!sD.bit (i))
			continue; // This sprite didn't exist at that block of pixels, 
					  // so it can't be in collision with the other sprites...

		for (size_t j = i + 1; j < 8; j++)
		{
			if (!sD.bit (j))
				continue; // This sprite didn't exist at that block of pixels, 
						  // so it can't be in collision with the previous one...

			if ((cT._collisionSpritesData [i] & 
				 cT._collisionSpritesData [j]) != 0x00)
			{ 
				// At leat one collision happened between sprites...
				cSS = true; 

				// ...and marked it for both of them...
				_VICIIRegisters -> setSpriteCollision (i);
				_VICIIRegisters -> setSpriteCollision (j);
			}
		}
	}

	// If the collision between sprites has happened,
	// the corresponding IRQ is activated...
	if (cSS) 
		_VICIIRegisters -> activateSpriteCollisionIRQ ();
}

// ---
void COMMODORE::VICII::drawOtherEvents (unsigned short cv, unsigned short rv)
{
	// Draw the border events...
	unsigned int cEvent = std::numeric_limits <unsigned int>::max ();
	if (_eventStatus._ffVBorderChange.positiveEdge ()) 
		cEvent = 32; // Auxiliar. Light cyan
	if (_eventStatus._ffVBorderChange.negativeEdge ()) 
		cEvent = 33; // Auxiliar. Light yellow
	if (_eventStatus._ffMBorderChange.positiveEdge ())
		cEvent = 34; // The main indication for the border (when activated). Light orange
	if (_eventStatus._ffMBorderChange.negativeEdge ())
		cEvent = 35; // The main indication for the border (when dsactivated). Light purple
	if (cEvent != std::numeric_limits <unsigned int>::max ())
		screenMemory () -> setHorizontalLine // Draw at least two pixels when the events has happpened...
			(_vicGraphicInfo._ffMBorderBegin, _raster.vData ().currentVisiblePosition (), 2, cEvent);

	// Draw the accepted bad-line condition event...
	// This marks the raster line where a Bad Line Condition was first accepted,
	// not necessarily a line where normal matrix/color c-accesses were performed.
	if (_eventStatus._badLine != std::numeric_limits <unsigned short>::max ())
	{ 
		if (_raster.vData ().currentVisiblePosition () == _eventStatus._badLine)
			screenMemory () -> setHorizontalLine 
				(_raster.hData ().currentVisiblePosition (), _eventStatus._badLine, 2, 36); // in points and draw in auxiliar color...
		else
			_eventStatus._badLine = std::numeric_limits <unsigned short>::max ();
	}

	// Draw the light pen event...
	// if it has happened...
	unsigned int cl = _VICIIRegisters -> backgroundColor () == 15 
		? 0 : _VICIIRegisters -> backgroundColor () + 1; /** to be visible. */
	auto delLPData = [&]() -> void
		{ screenMemory () -> setString ((size_t) 0, (size_t) 20, 
			MCHEmul::_SPACES.substr (8), cl); };
	if (_eventStatus._lightPenPositionLatched)
	{
		unsigned char lx, ly;
		_VICIIRegisters -> lightPenPositionLatched (&lx, &ly);
		if (cv >= 0 && cv < 40 && rv >= 20 && rv <= 28)
		{
			if (_eventStatus._lightPenPositionChanged) 
				delLPData (); // Just once...

			screenMemory () -> setString ((size_t) 0, (size_t) 20, 
				std::to_string ((unsigned int) lx) + " " +
				std::to_string ((unsigned int) ly), cl); // Always printing the new one...
		}
	}
	// ...and delete if not...
	else
		if (cv >= 0 && cv < 40 && rv >= 20 && rv <= 28) delLPData ();
}

// ---
void COMMODORE::VICII::debugDisconnected (MCHEmul::CPU* cpu)
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeCompleteLine ("VICII", cpu -> clockCycles (), "Disconnected");
}

// ---
std::string COMMODORE::VICII::debugCPUStopWindowsAsString
	(const COMMODORE::VICII::CPUStopWindows& windows) const
{
	std::string result;

	for (const auto& i : windows)
	{
		if (!result.empty ())
			result += "|";

		result +=
			std::to_string (i._firstBACycle) + "/" +
			std::to_string (i._firstAECCycle) + "/" +
			std::to_string (i._lastCycle) + "/" +
			std::to_string (i._badLineSource) + "/" +
			std::to_string ((unsigned int) i._spriteSourceMask);
	}

	return (result.empty () ? "-" : result);
}

// ---
std::string COMMODORE::VICII::debugCPUWriteEffectPositionsAsString () const
{
	if (!_pendingCPUTransaction.valid () ||
		_pendingCPUTransaction._busCycleData -> _numberWriteCycles == 0)
		return ("-");

	std::string result;

	for (size_t i = 0;
		i < _pendingCPUTransaction._busCycleData -> _numberWriteCycles; i++)
	{
		if (!result.empty ())
			result += "|";
		result += std::to_string
			(_pendingCPUStopPrediction._positionsToWriteEffects [i]);
	}

	return (result);
}

// ---
std::string COMMODORE::VICII::debugCPUStopPredictionAsString () const
{
	return (
		"Valid=" +
			std::to_string (_pendingCPUStopPrediction._valid) + "," +
		"Required=" +
			std::to_string (_pendingCPUStopPrediction._stopRequired) + "," +
		"Requested=" +
			std::to_string (_pendingCPUStopPrediction._stopRequested) + "," +
		"FirstStopCycle=" +
			std::to_string (_pendingCPUStopPrediction._firstStopCycle) + "," +
		"EffectCycle=" +
			std::to_string (_pendingCPUStopPrediction._instructionEffectCycle) + "," +
		"FirstNormalCycle=" +
			std::to_string (_pendingCPUStopPrediction._firstNormalCycle) + "," +
		"StopCycles=" +
			std::to_string (_pendingCPUStopPrediction._cyclesToStop) + "," +
		"WriteEffectPositions=" +
			debugCPUWriteEffectPositionsAsString () + "," +
		"EffectPositions=" +
			std::to_string
				(_pendingCPUStopPrediction._positionsToInstructionEffect));
}

// ---
void COMMODORE::VICII::debugCPUStopPrediction
	(const MCHEmul::InstructionContextEventData* dt)
{
	assert (_deepDebugFile != nullptr);
	assert (dt != nullptr);

	const MCHEmul::InstructionDefined* instruction =
		static_cast <const MCHEmul::InstructionDefined*> (dt -> _instruction);

	size_t cycleStructure = static_cast <size_t>
		(_pendingCPUTransaction._clockCycles - instruction -> clockCycles ());
	const MCHEmul::BusCycleData& busData =
		*_pendingCPUTransaction._busCycleData;

	_deepDebugFile -> writeCompleteLine
		(className (), dt -> _cpu -> clockCycles (),
		 "CPU transaction prediction",
		{ { "Transaction",
			"Type=Instruction," +
			std::string ("Address=$") +
				MCHEmul::removeAll0 (dt -> _address.asString
					(MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) + "," +
			"Code=" + std::to_string (instruction -> code ()) + "," +
			"Cycles=" +
				std::to_string (_pendingCPUTransaction._clockCycles) + "," +
			"Structure=" + std::to_string (cycleStructure) },
		  { "Bus cycle data",
			"Cycles=" + std::to_string (busData._numberCycles) + "," +
			"Reads=" + std::to_string (busData._numberReadCycles) + "," +
			"Writes=" + std::to_string (busData._numberWriteCycles) + "," +
			"TrailingWrites=" +
				std::to_string (busData._trailingWriteCycles) + "," +
			"MaxConsecutiveWrites=" +
				std::to_string (busData._maximumConsecutiveWriteCycles) + "," +
			"LastCycleType=" +
				std::to_string (busData._lastCycleType) },
		  { "Raster origin",
			"Row=" + std::to_string (_raster.currentLine ()) + "," +
			"Cycle=" + std::to_string (_cycleInRasterLine) },
		  { "Stop windows",
			"Current=" +
				debugCPUStopWindowsAsString (*_currentCPUStopWindows) + "," +
			"Next=" +
				debugCPUStopWindowsAsString (*_nextCPUStopWindows) + "," +
			"ActualSpriteMask=" +
				std::to_string ((unsigned int) _spriteDMAStateMask) + "," +
			"CurrentSpriteMask=" +
				std::to_string ((unsigned int) _currentSpriteDMAMask) + "," +
			"NextSpriteMask=" +
				std::to_string ((unsigned int) _nextSpriteDMAMask) },
		  { "Prediction",
			debugCPUStopPredictionAsString () } });
}

// ---
void COMMODORE::VICII::debugCPUStopPrediction
	(const MCHEmul::InterruptContextEventData* dt)
{
	assert (_deepDebugFile != nullptr);
	assert (dt != nullptr);

	const MCHEmul::BusCycleData& busData =
		dt -> _interrupt -> busCycleData ();

	_deepDebugFile -> writeCompleteLine
		(className (), dt -> _cpu -> clockCycles (),
		 "CPU transaction prediction",
		{ { "Transaction",
			"Type=Interrupt," +
			std::string ("Id=") +
				std::to_string (dt -> _interrupt -> id ()) + "," +
			"Cycles=" +
				std::to_string (_pendingCPUTransaction._clockCycles) + "," +
			"Structure=0" },
		  { "Bus cycle data",
			"Cycles=" + std::to_string (busData._numberCycles) + "," +
			"Reads=" + std::to_string (busData._numberReadCycles) + "," +
			"Writes=" + std::to_string (busData._numberWriteCycles) + "," +
			"TrailingWrites=" +
				std::to_string (busData._trailingWriteCycles) + "," +
			"MaxConsecutiveWrites=" +
				std::to_string (busData._maximumConsecutiveWriteCycles) + "," +
			"LastCycleType=" +
				std::to_string (busData._lastCycleType) },
		  { "Raster origin",
			"Row=" + std::to_string (_raster.currentLine ()) + "," +
			"Cycle=" + std::to_string (_cycleInRasterLine) },
		  { "Stop windows",
			"Current=" +
				debugCPUStopWindowsAsString (*_currentCPUStopWindows) + "," +
			"Next=" +
				debugCPUStopWindowsAsString (*_nextCPUStopWindows) + "," +
			"ActualSpriteMask=" +
				std::to_string ((unsigned int) _spriteDMAStateMask) + "," +
			"CurrentSpriteMask=" +
				std::to_string ((unsigned int) _currentSpriteDMAMask) + "," +
			"NextSpriteMask=" +
				std::to_string ((unsigned int) _nextSpriteDMAMask) },
		  { "Prediction",
			debugCPUStopPredictionAsString () } });
}

// ---
void COMMODORE::VICII::debugCPUStopPredictionRecalculated
	(const std::string& reason)
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeLineData
		(MCHEmul::Attributes (
			{ { "CPU stop recalculation",
				"Reason=" + reason + "," +
				"Row=" + std::to_string (_raster.currentLine ()) + "," +
				"Cycle=" + std::to_string (_cycleInRasterLine) },
			  { "Stop windows",
				"Current=" +
					debugCPUStopWindowsAsString (*_currentCPUStopWindows) + "," +
				"Next=" +
					debugCPUStopWindowsAsString (*_nextCPUStopWindows) + "," +
				"ActualSpriteMask=" +
					std::to_string ((unsigned int) _spriteDMAStateMask) + "," +
				"CurrentSpriteMask=" +
					std::to_string ((unsigned int) _currentSpriteDMAMask) + "," +
				"NextSpriteMask=" +
					std::to_string ((unsigned int) _nextSpriteDMAMask) },
			  { "Prediction",
				debugCPUStopPredictionAsString () } }));
}

// ---
void COMMODORE::VICII::debugCPUStopRequested (unsigned int cC) const
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeCompleteLine
		(className (), cC, "CPU stop applied",
		{ { "Raster position",
			"Row=" + std::to_string (_raster.currentLine ()) + "," +
			"Cycle=" + std::to_string (_cycleInRasterLine) },
		  { "Stop",
			"Cycles=" +
				std::to_string (_pendingCPUStopPrediction._cyclesToStop) + "," +
			"FirstStopCycle=" +
				std::to_string (_pendingCPUStopPrediction._firstStopCycle) },
		  { "Prediction",
			debugCPUStopPredictionAsString () } });
}

// ---
void COMMODORE::VICII::debugVICIIRegisterWriteApplied
	(unsigned int cC, const MCHEmul::SetMemoryCommand* command,
	 size_t writeIndex, size_t pendingAfter)
{
	assert (_deepDebugFile != nullptr);
	assert (command != nullptr);

	_deepDebugFile -> writeCompleteLine
		(className (), cC, "VIC-II register write",
		{ { "Raster position",
			"Column=" + std::to_string (_raster.currentColumn ()) + "," +
			"Row=" + std::to_string (_raster.currentLine ()) + "," +
			"Cycle=" + std::to_string (_cycleInRasterLine) },
		  { "Register write",
			"Address=$" + MCHEmul::removeAll0
				((command -> subset () -> initialAddress () + command -> position ()).
					asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) + "," +
			"Register=$" +
				MCHEmul::UByte ((unsigned char) (command -> position () % 0x40)).
					asString (MCHEmul::UByte::OutputFormat::_HEXA, 2) + "," +
			"Value=$" + command -> value ().asString
				(MCHEmul::UByte::OutputFormat::_HEXA, 2) + "," +
			"WriteIndex=" + std::to_string (writeIndex) + "," +
			"PendingAfter=" + std::to_string (pendingAfter) },
		  { "CPU stop prediction",
			"StartCPUCycle=" +
				std::to_string (_pendingCPUTransaction._startCPUCycle) + "," +
			"WritePosition=" + std::to_string
				(_pendingCPUStopPrediction._positionsToWriteEffects [writeIndex]) + "," +
			debugCPUStopPredictionAsString () } });
}

// ---
void COMMODORE::VICII::debugVICIICycle
	(MCHEmul::CPU* cpu, unsigned int i)
{
	assert (_deepDebugFile != nullptr);

	CPUStopWindow activeWindow;
	const bool baLow = CPUStopWindowAt
		(_cycleInRasterLine,
		 *_currentCPUStopWindows,
		 *_nextCPUStopWindows,
		 activeWindow);
	const bool aecLow = baLow &&
		!activeWindow.CPUOwnsBusAt (_cycleInRasterLine);
	const std::string activeWindowData = baLow
		? "FirstBA=" + std::to_string (activeWindow._firstBACycle) + "," +
		  "FirstAEC=" + std::to_string (activeWindow._firstAECCycle) + "," +
		  "LastCycle=" + std::to_string (activeWindow._lastCycle) + "," +
		  "BadLineSource=" + std::to_string (activeWindow._badLineSource) + "," +
		  "SpriteSourceMask=" +
			std::to_string ((unsigned int) activeWindow._spriteSourceMask)
		: "FirstBA=-,FirstAEC=-,LastCycle=-,BadLineSource=0,SpriteSourceMask=0";

	std::string spriteY, spriteMCBASE, spriteMC;
	unsigned char spriteEnabledMask = 0, spriteDisplayMask = 0,
		spriteYExpansionMask = 0, spriteYExpansionFlipFlopMask = 0;
	for (size_t sprite = 0; sprite < 8; sprite++)
	{
		if (sprite != 0)
		{
			spriteY += '|';
			spriteMCBASE += '|';
			spriteMC += '|';
		}

		spriteY += std::to_string
			((unsigned int) _VICIIRegisters -> spriteYCoord (sprite));
		spriteMCBASE += std::to_string
			((unsigned int) _vicSpriteInfo [sprite]._MCBASE);
		spriteMC += std::to_string
			((unsigned int) _vicSpriteInfo [sprite]._MC);
		if (_VICIIRegisters -> spriteEnable (sprite))
			spriteEnabledMask |= (1 << sprite);
		if (_vicSpriteInfo [sprite]._displayActive)
			spriteDisplayMask |= (1 << sprite);
		if (_VICIIRegisters -> spriteDoubleHeight (sprite))
			spriteYExpansionMask |= (1 << sprite);
		if (_VICIIRegisters -> expansionYFlipFlop (sprite))
			spriteYExpansionFlipFlopMask |= (1 << sprite);
	}

	_deepDebugFile -> writeCompleteLine (className (), cpu -> clockCycles () - i, "Info Cycle",
		{ { "Raster position",
				"Column=" + std::to_string (_raster.currentColumn ()) + 
					"(" + std::to_string (_raster.currentColumnAtBase0 ()) + ")," +
				"Row=" + std::to_string (_raster.currentLine ()) + 
					"(" + std::to_string (_raster.currentLineAtBase0 ()) + ")," +
				std::to_string (_cycleInRasterLine) },
		  { "Internal",
			"IDLE=" + std::to_string (_vicGraphicInfo._idleState) + "," +
			"VCBASE=" + std::to_string (_vicGraphicInfo._VCBASE) + "," +
			"VC=" + std::to_string (_vicGraphicInfo._VC) + "," +
			"VLMI=" + std::to_string (_vicGraphicInfo._VLMI) + "," +
			"GAccessIndex=" + std::to_string (_vicGraphicInfo._GAccessIndex) + "," +
			"RC=" + std::to_string (_vicGraphicInfo._RC) + "," +
			"ROW=" + std::to_string (_vicGraphicInfo._ROW) + "," +
			"LastVICDataRead=" + _lastVICDataRead.asString (MCHEmul::UByte::OutputFormat::_HEXA) + "," +
			"DENSeenAtLine30=" + std::to_string (_DENSeenAtLine30) + "," +
			"BadlineCondition=" + std::to_string (_badLineConditionActive) + "," +
			"BadlinePreventedIdleThisLine=" + std::to_string (_badLinePreventedIdleThisLine) + "," +
			"BadlineDetected=" + std::to_string (_badLineAlreadyDetectedThisLine) + "," +
			"BadlineBARequested=" + std::to_string (_badLineBAAlreadyRequested) + "," +
			"BadlineBARequestCycle=" + std::to_string (_badLineBARequestCycle) + "," +
			"BadlineFirstCAccessCycle=" + std::to_string (firstBadLineCAccessCycle ()) + "," +
			"BadlineCAccess=" + std::to_string (_badLineCAccessActive) + "," +
			"BadlineCAccessAllowed=" + std::to_string (_badLineCAccessAllowedThisLine) + "," +
			"BadlineInvalidCAccessCycles=" + std::to_string (_badLineInvalidCAccessCycles) + "," +
			"BadlineCAccessStartCycle=" + std::to_string (_badLineCAccessStartCycle) + "," +
			"Cycle=" + std::to_string (_cycleInRasterLine) },
		  { "CPU bus ownership",
			"BALow=" + std::to_string (baLow) + "," +
			"AECLow=" + std::to_string (aecLow) + "," +
			activeWindowData + "," +
			"CurrentWindows=" +
				std::to_string (_currentCPUStopWindows -> size ()) + "," +
			"NextWindows=" +
				std::to_string (_nextCPUStopWindows -> size ()) + "," +
			"AdjustedCurrent=" +
				std::to_string
					(_currentCPUStopWindows == &_adjustedCurrentCPUStopWindows) + "," +
			"ActualSpriteMask=" +
				std::to_string ((unsigned int) _spriteDMAStateMask) + "," +
			"CurrentSpriteMask=" +
				std::to_string ((unsigned int) _currentSpriteDMAMask) + "," +
			"NextSpriteMask=" +
				std::to_string ((unsigned int) _nextSpriteDMAMask) },
		  { "CPU stop prediction",
			"Pending=" +
				std::to_string (_pendingCPUTransaction.valid ()) + "," +
			"RegisterWritesPending=" +
				std::to_string (_pendingRegisterWrites.size ()) + "," +
			"StartCycle=" +
				std::to_string (_pendingCPUTransaction._startCycle) + "," +
			"StartCPUCycle=" +
				std::to_string (_pendingCPUTransaction._startCPUCycle) + "," +
			"NominalCycles=" +
				std::to_string (_pendingCPUTransaction._clockCycles) + "," +
			debugCPUStopPredictionAsString () },
		  { "Sprite DMA state",
			"EnabledMask=" + std::to_string ((unsigned int) spriteEnabledMask) + "," +
			"ActiveMask=" + std::to_string ((unsigned int) spriteDMAMask ()) + "," +
			"DisplayMask=" + std::to_string ((unsigned int) spriteDisplayMask) + "," +
			"YExpansionMask=" +
				std::to_string ((unsigned int) spriteYExpansionMask) + "," +
			"YExpansionFlipFlopMask=" +
				std::to_string ((unsigned int) spriteYExpansionFlipFlopMask) + "," +
			"Y=" + spriteY + "," +
			"MCBASE=" + spriteMCBASE + "," +
			"MC=" + spriteMC },
		  { "Border", 
			"Main=" + std::to_string (_vicGraphicInfo._ffMBorder) + "," +
			"Vertical=" + std::to_string (_vicGraphicInfo._ffVBorder) + "," +
			"LastEventAt=" + std::to_string (_vicGraphicInfo._ffMBorderBegin) },
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
void COMMODORE::VICII::debugSpriteDMAStateChange (size_t nS, bool active)
{
	assert (_deepDebugFile != nullptr);
	assert (nS < 8);

	_deepDebugFile -> writeLineData
		("Sprite DMA [Sprite=" + std::to_string (nS) +
		 ",Active=" + std::to_string (active) +
		 ",Y=" + std::to_string
			((unsigned int) _VICIIRegisters -> spriteYCoord (nS)) +
		 ",MCBASE=" + std::to_string
			((unsigned int) _vicSpriteInfo [nS]._MCBASE) +
		 ",MC=" + std::to_string
			((unsigned int) _vicSpriteInfo [nS]._MC) +
		 ",YExpansion=" + std::to_string
			(_VICIIRegisters -> spriteDoubleHeight (nS)) +
		 ",YExpansionFlipFlop=" + std::to_string
			(_VICIIRegisters -> expansionYFlipFlop (nS)) + "]");
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
void COMMODORE::VICII::debugReadingVideoMatrix (bool invalidCAccess)
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeLineData (std::string ("Reading Video Matrix & Color RAM [") +
		"Screen=" + _vicGraphicInfo._lastScreenCodeDataRead.asString (MCHEmul::UByte::OutputFormat::_HEXA) + "," +
		"Color=" + _vicGraphicInfo._lastColorDataRead.asString (MCHEmul::UByte::OutputFormat::_HEXA) + "," +
		"VLMI=" + std::to_string (_vicGraphicInfo._VLMI) + "," +
		"VC=" + std::to_string (_vicGraphicInfo._VC) + "," +
		"Invalid=" + std::to_string (invalidCAccess) + "]");
}

// ---
void COMMODORE::VICII::debugReadingGraphics ()
{
	assert (_deepDebugFile != nullptr);
	
	_deepDebugFile -> writeLineData (std::string ("Reading Graphics [") +
		"Data=" + _vicGraphicInfo._lastGraphicDataRead.asString (MCHEmul::UByte::OutputFormat::_HEXA) + "," +
		"GAccessIndex=" + std::to_string (_vicGraphicInfo._GAccessIndex) + "," +
		"VLMI=" + std::to_string (_vicGraphicInfo._VLMI) + "," +
		"VC=" + std::to_string (_vicGraphicInfo._VC) + "," +
		"RC=" + std::to_string ((unsigned int) _vicGraphicInfo._RC) + "," +
		"Idle=" + std::to_string (_vicGraphicInfo._idleState) + "]");
}

// ---
void COMMODORE::VICII::debugDrawPixelAt
	(unsigned short cav, int cb, unsigned char xS, size_t fP, size_t lP)
{
	assert (_deepDebugFile != nullptr);
	assert (fP < lP && lP <= 8);

	int firstGraphicPixel = cb + (int) fP,
		lastGraphicPixel = cb + (int) lP - 1;
	std::string span =
		"Raster=" + std::to_string (cav) + "," +
		"FirstPixel=" + std::to_string (fP) + "," +
		"LastPixel=" + std::to_string (lP - 1) + "," +
		"XScroll=" + std::to_string ((unsigned int) xS) + "," +
		"GraphicPixel=" + std::to_string (firstGraphicPixel) + ",";

	// Report OUT only when the complete span lies beyond the graphics buffer.
	if (lastGraphicPixel < 0 || firstGraphicPixel >= 320)
		_deepDebugFile -> writeLineData (std::string ("Drawing pixels [") +
			span +
			"FirstByte=OUT,LastByte=OUT]");
	else
	{
		int firstPixel = firstGraphicPixel < 0 ? 0 : firstGraphicPixel,
			lastPixel = lastGraphicPixel > 319 ? 319 : lastGraphicPixel;
		_deepDebugFile -> writeLineData (std::string ("Drawing pixels [") +
			span +
			"FirstByte=" + std::to_string (firstPixel >> 3) + "," +
			"LastByte=" + std::to_string (lastPixel >> 3) + "]");
	}
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

	result.add ("VCBASE",		_VCBASE);
	result.add ("VC",			_VC);
	result.add ("VLMI",			_VLMI);
	result.add ("GAccessIndex",	_GAccessIndex);
	result.add ("RC",			_RC);
	result.add ("ROW",			_ROW);
	result.add ("IDLE",			std::string (_idleState ? "YES" : "NO"));

	return (result);
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
	// Nothing else...
}

// ---
void COMMODORE::VICII_PAL::addSpriteCPUStopWindow
	(size_t nS, COMMODORE::VICII::CPUStopWindows& result) const
{
	assert (nS < 8);

	static const CPURasterCycle firstBA [8] = { 55, 57, 59, -2, 0, 2, 4, 6 };
	static const CPURasterCycle firstAEC [8] = { 58, 60, 62, 1, 3, 5, 7, 9 };

	result.emplace_back
		(firstBA [nS], firstAEC [nS], firstAEC [nS] + 1,
		 false, (unsigned char) (1 << nS));
}

// ---
void COMMODORE::VICII_PAL::treatRasterBusCycle ()
{
	COMMODORE::VICII::treatRasterBusCycle ();

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
					_IFDEBUG debugReadingSpriteInfo (nSR);
				}
			}

			break;

		default:
			break;
	}

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
	// Nothing else...
}

// ---
void COMMODORE::VICII_NTSC::addSpriteCPUStopWindow
	(size_t nS, COMMODORE::VICII::CPUStopWindows& result) const
{
	assert (nS < 8);

	// The emulated NTSC model is the 64-cycle 6567R56A. Its extra idle slot
	// moves the late sprite 0..2 accesses without shifting the early 3..7 slots.
	static const CPURasterCycle firstBA [8] = { 56, 58, 60, -2, 0, 2, 4, 6 };
	static const CPURasterCycle firstAEC [8] = { 60, 62, 64, 1, 3, 5, 7, 9 };

	result.emplace_back
		(firstBA [nS], firstAEC [nS], firstAEC [nS] + 1,
		 false, (unsigned char) (1 << nS));
}

// ---
void COMMODORE::VICII_NTSC::treatRasterBusCycle ()
{
	COMMODORE::VICII::treatRasterBusCycle ();

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
					_IFDEBUG debugReadingSpriteInfo (nSR);
				}

			}

			break;

		default:
			break;
	}

}
