#include <iostream>
#include <string>
#include <vector>

#define SDL_MAIN_HANDLED
#include <COMMODORE/VICII/VICII.hpp>
#include <COMMODORE/VICII/VICIIRegisters.hpp>

/** One independently specified result from Bauer's BA/AEC timing rules. */
struct ExpectedPrediction final
{
	int _firstStopCycle;
	int _instructionEffectCycle;
	int _firstNormalCycle;
	unsigned int _cyclesToStop;
	std::vector <unsigned int> _writeEffectPositions;
};

/** Exposes only the pure timing calculation required by this theoretical test. */
template <class VICIIType>
class TestVICII final : public VICIIType
{
	public:
	using CPURasterCycle = typename VICIIType::CPURasterCycle;
	using CPUStopPrediction = typename VICIIType::CPUStopPrediction;
	using CPUStopWindow = typename VICIIType::CPUStopWindow;
	using CPUStopWindows = typename VICIIType::CPUStopWindows;
	using DrawContext = typename VICIIType::DrawContext;
	using DrawResult = typename VICIIType::DrawResult;
	using RegisterEffect = typename VICIIType::DrawContext::RegisterEffect;

	TestVICII ()
		: VICIIType (0, nullptr, MCHEmul::Address (2, 0), 0),
		  _registerStorage (0, MCHEmul::PhysicalStorage::Type::_RAM, 0x0400),
		  _registers
			(&_registerStorage, 0,
			 MCHEmul::Address ({ 0x00, 0xd0 }, false), 0x0400)
	{
		this -> _VICIIRegisters = &_registers;
		_registers.initialize ();
		this -> initializeCPUStopWindowSets ();
	}

	CPUStopPrediction predict
		(const MCHEmul::CycleStructure& cS, CPURasterCycle startCycle,
		 const CPUStopWindows& currentWindows) const
	{
		return (this -> calculateCPUStopPrediction
			(cS, MCHEmul::BusCycleData (cS), (unsigned int) cS.size (),
				 startCycle, currentWindows, CPUStopWindows ()));
	}

	/** Executes and prints one comparison without sharing formulas with the predictor. */
	bool testPrediction
		(const std::string& instructionName,
		 const MCHEmul::CycleStructure& cycleStructure, unsigned int executedCycles,
		 const CPUStopWindows& windows, const ExpectedPrediction& expected) const
	{
		const CPUStopPrediction prediction = predict
			(cycleStructure, 12 - (int) executedCycles, windows);
		const MCHEmul::BusCycleData busData (cycleStructure);
		bool writeEffectsMatch =
			expected._writeEffectPositions.size () == busData._numberWriteCycles;
		for (size_t i = 0;
			writeEffectsMatch && i < expected._writeEffectPositions.size (); i++)
			writeEffectsMatch =
				prediction._positionsToWriteEffects [i] ==
				expected._writeEffectPositions [i];

		const bool result = writeEffectsMatch &&
			prediction._firstStopCycle == expected._firstStopCycle &&
			prediction._instructionEffectCycle == expected._instructionEffectCycle &&
			prediction._firstNormalCycle == expected._firstNormalCycle &&
			prediction._cyclesToStop == expected._cyclesToStop;

		std::cout
			<< instructionName << " | " << executedCycles
			<< " | stop " << prediction._firstStopCycle << "/" << expected._firstStopCycle
			<< " | effect " << prediction._instructionEffectCycle << "/" << expected._instructionEffectCycle
			<< " | normal " << prediction._firstNormalCycle << "/" << expected._firstNormalCycle
			<< " | stop cycles " << prediction._cyclesToStop << "/" << expected._cyclesToStop
			<< " | writes " << (writeEffectsMatch ? "OK" : "ERROR")
			<< " | " << (result ? "OK" : "ERROR") << std::endl;

		return (result);
	}

	/** Verifies that output capture preserves the state on both sides of a
		visual register write and reuses the initial state when no write applies. */
	bool testOutputStateCapture ()
	{
		_registers.initialize ();
		_registers.setRegister (0x11, MCHEmul::UByte (0x18));
		_registers.setRegister (0x16, MCHEmul::UByte (0x08));
		_registers.setRegister (0x20, MCHEmul::UByte (0x02));
		_registers.setRegister (0x21, MCHEmul::UByte (0x03));

		DrawContext dC (0, 0, 0, 0);
		this -> captureOutputState (dC._beforeCPUWrite);
		const bool unchangedStateReused =
			&dC.outputStateAfterCPUWrite () == &dC._beforeCPUWrite &&
			!dC.outputChangesDuringSlice () &&
			&dC.outputStateAtPixel (0) == &dC._beforeCPUWrite &&
			&dC.outputStateAtPixel (7) == &dC._beforeCPUWrite;

		_registers.setRegister (0x16, MCHEmul::UByte (0x17));
		_registers.setRegister (0x1b, MCHEmul::UByte (0x01));
		_registers.setRegister (0x1c, MCHEmul::UByte (0x02));
		_registers.setRegister (0x1d, MCHEmul::UByte (0x04));
		_registers.setRegister (0x20, MCHEmul::UByte (0x05));
		_registers.setRegister (0x21, MCHEmul::UByte (0x06));
		_registers.setRegister (0x25, MCHEmul::UByte (0x07));
		_registers.setRegister (0x27, MCHEmul::UByte (0x08));
		dC._registerEffect._applied = true;
		dC._registerEffect._affectsOutput = true;
		this -> captureOutputState (dC._afterCPUWrite);
		const bool splitState =
			dC.outputChangesDuringSlice () &&
			&dC.outputStateAtPixel (0) == &dC._beforeCPUWrite &&
			&dC.outputStateAtPixel (3) == &dC._beforeCPUWrite &&
			&dC.outputStateAtPixel (4) == &dC._afterCPUWrite &&
			&dC.outputStateAtPixel (7) == &dC._afterCPUWrite;

		const bool result =
			unchangedStateReused && splitState &&
			dC._beforeCPUWrite._horizontalScroll == 0 &&
			dC._beforeCPUWrite._textDisplay40Columns &&
			dC._beforeCPUWrite._borderColor == 0x02 &&
			dC._beforeCPUWrite._backgroundColors [0] == 0x03 &&
			&dC.outputStateAfterCPUWrite () == &dC._afterCPUWrite &&
			dC._afterCPUWrite._graphicMode ==
				COMMODORE::VICIIRegisters::GraphicMode::_MULTICOLORCHARMODE &&
			dC._afterCPUWrite._horizontalScroll == 7 &&
			!dC._afterCPUWrite._textDisplay40Columns &&
			dC._afterCPUWrite._borderColor == 0x05 &&
			dC._afterCPUWrite._backgroundColors [0] == 0x06 &&
			dC._afterCPUWrite._spritePriorityMask == 0x01 &&
			dC._afterCPUWrite._spriteMulticolorMask == 0x02 &&
			dC._afterCPUWrite._spriteDoubleWidthMask == 0x04 &&
			dC._afterCPUWrite._spriteSharedColors [0] == 0x07 &&
			dC._afterCPUWrite._spriteColors [0] == 0x08;

		_registers.initialize ();
		std::cout << "VIC-II output-state capture | "
			<< (result ? "OK" : "ERROR") << std::endl;

		return (result);
	}

	/** Verifies that an invalid mode can cover only the phi2 half of a slice
		without forcing the pixels previously produced during phi1 to black. */
	bool testInvalidGraphicPixelMask () const
	{
		DrawResult drawResult;
		for (size_t i = DrawContext::_FIRSTPIXELAFTERCPUWRITE; i < 8; i++)
			drawResult._invalidGraphicData.setBit (7 - i, true);

		const bool result =
			!drawResult._invalidGraphicData.bit (7) &&
			!drawResult._invalidGraphicData.bit (4) &&
			drawResult._invalidGraphicData.bit (3) &&
			drawResult._invalidGraphicData.bit (0);

		std::cout << "VIC-II invalid-mode pixel mask | "
			<< (result ? "OK" : "ERROR") << std::endl;

		return (result);
	}

	/** Verifies that the sprite X comparator uses the pre-write value in phi1,
		the post-write value in phi2 and never repositions an already latched sprite. */
	bool testSpriteHorizontalStartPhases ()
	{
		this -> _raster.initialize ();
		while (this -> _raster.currentColumn () < 16 ||
			this -> _raster.currentColumn () > 200)
			this -> _raster.hData ().add (this -> _raster.step ());

		const unsigned short column = this -> _raster.currentColumn ();
		_registers.setRegister (0x10, MCHEmul::UByte::_0);
		_registers.setRegister (0x00, MCHEmul::UByte
			((unsigned char) (column + 2 - 4)));
		_registers.setRegister (0x02, MCHEmul::UByte
			((unsigned char) (column + 12 - 4)));
		_registers.setRegister (0x04, MCHEmul::UByte
			((unsigned char) (column + 12 - 4)));
		_registers.setRegister (0x06, MCHEmul::UByte
			((unsigned char) (column + 4 - 4)));

		for (size_t i = 0; i < 4; i++)
		{
			this -> _vicSpriteInfo [i]._displayActive = true;
			this -> _vicSpriteInfo [i]._drawing = false;
			this -> _vicSpriteInfo [i]._xS = 0;
		}

		this -> treatSpriteHorizontalStartAtCurrentCycle
			(0, DrawContext::_FIRSTPIXELAFTERCPUWRITE);
		const bool phi1Latched =
			this -> _vicSpriteInfo [0]._drawing &&
			this -> _vicSpriteInfo [0]._xS == column + 2;
		const bool boundaryNotYetLatched =
			!this -> _vicSpriteInfo [3]._drawing;

		// Move sprite 0 after it has started, sprite 1 into phi2 and sprite 2
		// into the already elapsed phi1 interval.
		_registers.setRegister (0x00, MCHEmul::UByte
			((unsigned char) (column + 6 - 4)));
		_registers.setRegister (0x02, MCHEmul::UByte
			((unsigned char) (column + 6 - 4)));
		_registers.setRegister (0x04, MCHEmul::UByte
			((unsigned char) (column + 2 - 4)));
		this -> treatSpriteHorizontalStartAtCurrentCycle
			(DrawContext::_FIRSTPIXELAFTERCPUWRITE, this -> _raster.step ());

		const bool phi1PositionPreserved =
			this -> _vicSpriteInfo [0]._xS == column + 2;
		const bool phi2Latched =
			this -> _vicSpriteInfo [1]._drawing &&
			this -> _vicSpriteInfo [1]._xS == column + 6;
		const bool elapsedPhi1Ignored =
			!this -> _vicSpriteInfo [2]._drawing;
		const bool boundaryLatchedInPhi2 =
			this -> _vicSpriteInfo [3]._drawing &&
			this -> _vicSpriteInfo [3]._xS == column + 4;
		const bool result = phi1Latched && boundaryNotYetLatched &&
			phi1PositionPreserved && phi2Latched && elapsedPhi1Ignored &&
			boundaryLatchedInPhi2;

		for (size_t i = 0; i < 4; i++)
			this -> _vicSpriteInfo [i] = typename VICIIType::VICSpriteInfo ();
		_registers.initialize ();

		std::cout << "VIC-II sprite horizontal start phases | "
			<< "phi1 " << phi1Latched << ", preserve " << phi1PositionPreserved
			<< ", phi2 " << phi2Latched << ", elapsed " << elapsedPhi1Ignored
			<< ", boundary " << boundaryNotYetLatched << "/"
			<< boundaryLatchedInPhi2 << " | "
			<< (result ? "OK" : "ERROR") << std::endl;

		return (result);
	}

	/** Verifies that write positions stay relative to the CPU transaction when
		that transaction crosses the concrete PAL or NTSC raster-line boundary. */
	bool testWritePositionAcrossRasterLine
		(const std::string& testName,
		 const MCHEmul::CycleStructure& cycleStructure) const
	{
		const CPURasterCycle startCycle = this -> cyclesPerRasterLine () - 1;
		const CPUStopPrediction prediction = predict
			(cycleStructure, startCycle, CPUStopWindows ());
		const bool result =
			prediction._positionsToWriteEffects [0] == 3 &&
			prediction._instructionEffectCycle == startCycle + 3 &&
			prediction._cyclesToStop == 0;

		std::cout
			<< testName
			<< " | start " << startCycle
			<< " | effect " << prediction._instructionEffectCycle
			<< " | write position " << prediction._positionsToWriteEffects [0]
			<< " | " << (result ? "OK" : "ERROR") << std::endl;

		return (result);
	}

	/** Advances from the final cycle of rL to the first cycle of rL + 1. \n
		DEN is considered to have been seen at line $30 so the test isolates
		the regular raster/YSCROLL bad-line relation. */
	void advanceFromRasterLine (unsigned short rL, unsigned char yScroll)
	{
		this -> _raster.initialize ();
		_registers.initialize ();
		_registers.setRegister
			(0x11, MCHEmul::UByte ((unsigned char) (0x10 | yScroll)));

		this -> _DENSeenAtLine30 = true;
		while (this -> _raster.currentLine () != rL)
			this -> _raster.vData ().next ();

		this -> _cycleInRasterLine = 1;
		this -> _vicGraphicInfo._ROW = this -> _raster.currentLine ();
		this -> _currentSpriteDMAMask = this -> _nextSpriteDMAMask = 0;
		this -> selectCPUStopWindowsForCurrentAndNextLine ();

		// Drive the real raster transition code for the complete source line.
		// The vertical retrace is crossed before the horizontal counter wraps;
		// calling only RasterData::add () would miss that coupling.
		for (unsigned short i = 0; i < this -> cyclesPerRasterLine (); i++)
			this -> advanceRasterPosition ();
	}

	bool currentWindowsContainRegularBadLine () const
	{
		return (windowsContainRegularBadLine (*this -> _currentCPUStopWindows));
	}

	bool nextWindowsContainRegularBadLine () const
	{
		return (windowsContainRegularBadLine (*this -> _nextCPUStopWindows));
	}

	/** Verifies that a buffered VIC-II store remains pending until its predicted
		absolute CPU cycle and is removed immediately after becoming effective. */
	bool testSingleRegisterWriteTiming ()
	{
		const MCHEmul::CycleStructure cycleStructure =
			{ MCHEmul::CPUCycle::_READ, MCHEmul::CPUCycle::_READ,
			  MCHEmul::CPUCycle::_READ, MCHEmul::CPUCycle::_WRITE };
		const MCHEmul::BusCycleData busData (cycleStructure);

		this -> _pendingCPUTransaction._cycleStructure = &cycleStructure;
		this -> _pendingCPUTransaction._busCycleData = &busData;
		this -> _pendingCPUTransaction._clockCycles = 4;
		this -> _pendingCPUTransaction._startCPUCycle = 100;
		this -> _pendingCPUStopPrediction = CPUStopPrediction ();
		this -> _pendingCPUStopPrediction._valid = true;
		this -> _pendingCPUStopPrediction._positionsToWriteEffects [0] = 3;
		this -> _pendingRegisterWrites.clear ();
		this -> _pendingRegisterWrites.emplace_back
			(&_registers, 0x15, MCHEmul::UByte (0x01));
		_registers.setRegister (0x15, MCHEmul::UByte::_0);
		bool horizontalDisplayZoneChanged;
		RegisterEffect pendingEffect, appliedEffect;

		const bool result =
			!this -> executePendingRegisterWriteAt
				(102, &pendingEffect, &horizontalDisplayZoneChanged) &&
			!horizontalDisplayZoneChanged &&
			!pendingEffect._applied &&
			!_registers.spriteEnable (0) &&
			this -> _pendingRegisterWrites.size () == 1 &&
			this -> executePendingRegisterWriteAt
				(103, &appliedEffect, &horizontalDisplayZoneChanged) &&
			!horizontalDisplayZoneChanged &&
			appliedEffect._applied &&
			!appliedEffect._affectsOutput &&
			appliedEffect._registerPosition == 0x15 &&
			appliedEffect._previousValue == MCHEmul::UByte::_0 &&
			appliedEffect._newValue == MCHEmul::UByte (0x01) &&
			_registers.spriteEnable (0) &&
			this -> _pendingRegisterWrites.empty ();

		this -> _pendingCPUTransaction.reset ();
		this -> _pendingCPUStopPrediction = CPUStopPrediction ();

		std::cout << "Single VIC-II write timing | "
			<< (result ? "OK" : "ERROR") << std::endl;

		return (result);
	}

	/** Verifies that the two buffered writes of a memory RMW transaction are
		applied in bus-cycle order while element zero remains the next command. */
	bool testRMWRegisterWriteTiming ()
	{
		const MCHEmul::CycleStructure cycleStructure =
			{ MCHEmul::CPUCycle::_READ, MCHEmul::CPUCycle::_READ,
			  MCHEmul::CPUCycle::_READ, MCHEmul::CPUCycle::_READ,
			  MCHEmul::CPUCycle::_WRITE, MCHEmul::CPUCycle::_WRITE };
		const MCHEmul::BusCycleData busData (cycleStructure);

		this -> _pendingCPUTransaction._cycleStructure = &cycleStructure;
		this -> _pendingCPUTransaction._busCycleData = &busData;
		this -> _pendingCPUTransaction._clockCycles = 6;
		this -> _pendingCPUTransaction._startCPUCycle = 200;
		this -> _pendingCPUStopPrediction = CPUStopPrediction ();
		this -> _pendingCPUStopPrediction._valid = true;
		this -> _pendingCPUStopPrediction._positionsToWriteEffects [0] = 4;
		this -> _pendingCPUStopPrediction._positionsToWriteEffects [1] = 5;
		this -> _pendingRegisterWrites.clear ();
		this -> _pendingRegisterWrites.emplace_back
			(&_registers, 0x15, MCHEmul::UByte (0x01));
		this -> _pendingRegisterWrites.emplace_back
			(&_registers, 0x15, MCHEmul::UByte::_0);
		_registers.setRegister (0x15, MCHEmul::UByte::_0);
		bool horizontalDisplayZoneChanged;
		RegisterEffect firstEffect, secondEffect;

		const bool firstWrite =
			this -> executePendingRegisterWriteAt
				(204, &firstEffect, &horizontalDisplayZoneChanged) &&
			!horizontalDisplayZoneChanged &&
			firstEffect._applied &&
			!firstEffect._affectsOutput &&
			firstEffect._previousValue == MCHEmul::UByte::_0 &&
			firstEffect._newValue == MCHEmul::UByte (0x01) &&
			_registers.spriteEnable (0) &&
			this -> _pendingRegisterWrites.size () == 1;
		const bool secondWrite =
			this -> executePendingRegisterWriteAt
				(205, &secondEffect, &horizontalDisplayZoneChanged) &&
			!horizontalDisplayZoneChanged &&
			secondEffect._applied &&
			!secondEffect._affectsOutput &&
			secondEffect._previousValue == MCHEmul::UByte (0x01) &&
			secondEffect._newValue == MCHEmul::UByte::_0 &&
			!_registers.spriteEnable (0) &&
			this -> _pendingRegisterWrites.empty ();
		const bool result = firstWrite && secondWrite;

		this -> _pendingCPUTransaction.reset ();
		this -> _pendingCPUStopPrediction = CPUStopPrediction ();

		std::cout << "RMW VIC-II write order | "
			<< (result ? "OK" : "ERROR") << std::endl;

		return (result);
	}

	/** Verifies that $d016 is written at its predicted CPU cycle while the
		CSEL-derived horizontal limits remain unchanged until the current video
		slice has finished. */
	bool testHorizontalDisplayZoneDeferred ()
	{
		const MCHEmul::CycleStructure cycleStructure =
			{ MCHEmul::CPUCycle::_READ, MCHEmul::CPUCycle::_READ,
			  MCHEmul::CPUCycle::_READ, MCHEmul::CPUCycle::_WRITE };
		const MCHEmul::BusCycleData busData (cycleStructure);

		this -> _pendingCPUTransaction._cycleStructure = &cycleStructure;
		this -> _pendingCPUTransaction._busCycleData = &busData;
		this -> _pendingCPUTransaction._clockCycles = 4;
		this -> _pendingCPUTransaction._startCPUCycle = 300;
		this -> _pendingCPUStopPrediction = CPUStopPrediction ();
		this -> _pendingCPUStopPrediction._valid = true;
		this -> _pendingCPUStopPrediction._positionsToWriteEffects [0] = 3;
		this -> _pendingRegisterWrites.clear ();
		this -> _pendingRegisterWrites.emplace_back
			(&_registers, 0x16, MCHEmul::UByte (0x07));
		_registers.setRegister (0x16, MCHEmul::UByte (0x08));
		this -> _raster.hData ().reduceDisplayZone (false);

		const unsigned short previousScreenPositions =
			this -> _raster.hData ().screenPositions ();
		bool horizontalDisplayZoneChanged;
		RegisterEffect registerEffect;
		const bool writeApplied = this -> executePendingRegisterWriteAt
			(303, &registerEffect, &horizontalDisplayZoneChanged);
		const bool unchangedDuringSlice =
			this -> _raster.hData ().screenPositions () == previousScreenPositions;

		if (horizontalDisplayZoneChanged)
			this -> _raster.hData ().reduceDisplayZone
				(!_registers.textDisplay40ColumnsActive ());

		const bool result =
			writeApplied && horizontalDisplayZoneChanged &&
			registerEffect._applied && registerEffect._affectsOutput &&
			registerEffect._registerPosition == 0x16 &&
			registerEffect._previousValue == MCHEmul::UByte (0x08) &&
			registerEffect._newValue == MCHEmul::UByte (0x07) &&
			unchangedDuringSlice &&
			this -> _raster.hData ().screenPositions () < previousScreenPositions;

		_registers.setRegister (0x16, MCHEmul::UByte (0x08));
		this -> _raster.hData ().reduceDisplayZone (false);
		this -> _pendingCPUTransaction.reset ();
		this -> _pendingCPUStopPrediction = CPUStopPrediction ();

		std::cout << "Deferred $d016 horizontal geometry | "
			<< (result ? "OK" : "ERROR") << std::endl;

		return (result);
	}

	/** Verifies the per-pixel main-border mask at both CSEL-dependent left
		comparators. The reduced comparator is the last pixel of its slice, while
		the normal comparator coincides with the first one. */
	bool testLeftBorderAtSliceBeginning ()
	{
		this -> _raster.initialize ();
		while (this -> _raster.currentLine () < 100)
			this -> _raster.vData ().next ();

		_registers.setRegister (0x16, MCHEmul::UByte::_0);
		this -> _raster.hData ().reduceDisplayZone (true);
		this -> _vicGraphicInfo._ffVBorder = false;
		this -> _vicGraphicInfo._ffMBorder = true;
		while (((_registers.minRasterH () +
			this -> _raster.hData ().totalPositions () -
			this -> _raster.currentColumn ()) %
			this -> _raster.hData ().totalPositions ()) >= this -> _raster.step ())
			this -> _raster.hData ().add (this -> _raster.step ());
		const unsigned short reducedRC =
			this -> _raster.hData ().currentVisiblePosition ();
		const unsigned short reducedRCA = (reducedRC >> 3) << 3;
		DrawContext reducedContext
			(this -> _raster.hData ().firstDisplayPosition (),
			 reducedRC, reducedRCA, 0);
		this -> actualizeMainBorderStatus
			(reducedContext, 0, DrawContext::_FIRSTPIXELAFTERCPUWRITE);
		this -> actualizeMainBorderStatus
			(reducedContext, DrawContext::_FIRSTPIXELAFTERCPUWRITE,
			 this -> _raster.step ());
		const bool reducedLimitDetected =
			reducedContext._mainBorderData == MCHEmul::UByte (0xfe) &&
			!this -> _vicGraphicInfo._ffMBorder;

		this -> _raster.hData ().initialize ();
		_registers.setRegister (0x16, MCHEmul::UByte (0x08));
		this -> _raster.hData ().reduceDisplayZone (false);
		this -> _vicGraphicInfo._ffMBorder = true;
		while (((_registers.minRasterH () +
			this -> _raster.hData ().totalPositions () -
			this -> _raster.currentColumn ()) %
			this -> _raster.hData ().totalPositions ()) >= this -> _raster.step ())
			this -> _raster.hData ().add (this -> _raster.step ());
		const unsigned short alignedRC =
			this -> _raster.hData ().currentVisiblePosition ();
		const unsigned short alignedRCA = (alignedRC >> 3) << 3;
		DrawContext alignedContext
			(this -> _raster.hData ().firstDisplayPosition (),
			 alignedRC, alignedRCA, 0);
		this -> actualizeMainBorderStatus
			(alignedContext, 0, DrawContext::_FIRSTPIXELAFTERCPUWRITE);
		this -> actualizeMainBorderStatus
			(alignedContext, DrawContext::_FIRSTPIXELAFTERCPUWRITE,
			 this -> _raster.step ());
		this -> _raster.hData ().add (this -> _raster.step ());
		const unsigned short nextAlignedRC =
			this -> _raster.hData ().currentVisiblePosition ();
		const unsigned short nextAlignedRCA = (nextAlignedRC >> 3) << 3;
		DrawContext nextAlignedContext
			(this -> _raster.hData ().firstDisplayPosition (),
			 nextAlignedRC, nextAlignedRCA, 0);
		this -> actualizeMainBorderStatus
			(nextAlignedContext, 0, DrawContext::_FIRSTPIXELAFTERCPUWRITE);
		this -> actualizeMainBorderStatus
			(nextAlignedContext, DrawContext::_FIRSTPIXELAFTERCPUWRITE,
			 this -> _raster.step ());
		const bool alignedLimitDetected =
			alignedContext._mainBorderData == MCHEmul::UByte (0xff) &&
			nextAlignedContext._mainBorderData == MCHEmul::UByte::_0 &&
			!this -> _vicGraphicInfo._ffMBorder;
		const bool result = reducedLimitDetected && alignedLimitDetected;
		this -> _vicGraphicInfo._ffMBorder = false;
		this -> _cycleInRasterLine = 1;

		std::cout << "Left border at slice beginning | "
			<< (result ? "OK" : "ERROR") << std::endl;

		return (result);
	}

	/** Verifies that a CSEL write affects only the phi2 comparator pass and
		cannot alter pixels or transitions already evaluated during phi1. */
	bool testMainBorderComparatorPhases ()
	{
		this -> _raster.initialize ();
		while (this -> _raster.currentLine () < 100)
			this -> _raster.vData ().next ();
		while (this -> _raster.currentColumn () != 20)
			this -> _raster.hData ().add (this -> _raster.step ());

		_registers.setRegister (0x16, MCHEmul::UByte (0x08));
		this -> _vicGraphicInfo._ffVBorder = false;
		this -> _vicGraphicInfo._ffMBorder = true;
		const unsigned short leftRC =
			this -> _raster.hData ().currentVisiblePosition ();
		const unsigned short leftRCA = (leftRC >> 3) << 3;
		DrawContext leftContext
			(this -> _raster.hData ().firstDisplayPosition (),
			 leftRC, leftRCA, 0);
		this -> actualizeMainBorderStatus
			(leftContext, 0, DrawContext::_FIRSTPIXELAFTERCPUWRITE);
		_registers.setRegister (0x16, MCHEmul::UByte::_0);
		this -> actualizeMainBorderStatus
			(leftContext, DrawContext::_FIRSTPIXELAFTERCPUWRITE,
			 this -> _raster.step ());
		this -> _raster.hData ().add (this -> _raster.step ());
		const unsigned short nextLeftRC =
			this -> _raster.hData ().currentVisiblePosition ();
		const unsigned short nextLeftRCA = (nextLeftRC >> 3) << 3;
		DrawContext nextLeftContext
			(this -> _raster.hData ().firstDisplayPosition (),
			 nextLeftRC, nextLeftRCA, 0);
		this -> actualizeMainBorderStatus
			(nextLeftContext, 0, DrawContext::_FIRSTPIXELAFTERCPUWRITE);
		this -> actualizeMainBorderStatus
			(nextLeftContext, DrawContext::_FIRSTPIXELAFTERCPUWRITE,
			 this -> _raster.step ());
		const bool passedLeftComparatorNotReplayed =
			leftContext._mainBorderData == MCHEmul::UByte (0xff) &&
			nextLeftContext._mainBorderData == MCHEmul::UByte (0xfe) &&
			!this -> _vicGraphicInfo._ffMBorder;

		this -> _raster.hData ().initialize ();
		while (this -> _raster.currentColumn () != 340)
			this -> _raster.hData ().add (this -> _raster.step ());
		_registers.setRegister (0x16, MCHEmul::UByte::_0);
		this -> _vicGraphicInfo._ffMBorder = false;
		const unsigned short rightRC =
			this -> _raster.hData ().currentVisiblePosition ();
		const unsigned short rightRCA = (rightRC >> 3) << 3;
		DrawContext rightContext
			(this -> _raster.hData ().firstDisplayPosition (),
			 rightRC, rightRCA, 0);
		this -> actualizeMainBorderStatus
			(rightContext, 0, DrawContext::_FIRSTPIXELAFTERCPUWRITE);
		_registers.setRegister (0x16, MCHEmul::UByte (0x08));
		this -> actualizeMainBorderStatus
			(rightContext, DrawContext::_FIRSTPIXELAFTERCPUWRITE,
			 this -> _raster.step ());
		this -> _raster.hData ().add (this -> _raster.step ());
		const unsigned short nextRightRC =
			this -> _raster.hData ().currentVisiblePosition ();
		const unsigned short nextRightRCA = (nextRightRC >> 3) << 3;
		DrawContext nextRightContext
			(this -> _raster.hData ().firstDisplayPosition (),
			 nextRightRC, nextRightRCA, 0);
		this -> actualizeMainBorderStatus
			(nextRightContext, 0, DrawContext::_FIRSTPIXELAFTERCPUWRITE);
		this -> actualizeMainBorderStatus
			(nextRightContext, DrawContext::_FIRSTPIXELAFTERCPUWRITE,
			 this -> _raster.step ());
		const bool newRightComparatorReachedInPhi2 =
			rightContext._mainBorderData == MCHEmul::UByte::_0 &&
			nextRightContext._mainBorderData == MCHEmul::UByte (0xff) &&
			this -> _vicGraphicInfo._ffMBorder;

		const bool result =
			passedLeftComparatorNotReplayed && newRightComparatorReachedInPhi2;
		this -> _vicGraphicInfo._ffMBorder = false;
		this -> _cycleInRasterLine = 1;

		std::cout << "Main border comparator phases | "
			<< (result ? "OK" : "ERROR") << std::endl;

		return (result);
	}

	/** Verifies Bauer's vertical-border rules at the exact left-comparator
		pixel, independently from their second evaluation at the end of the line. */
	bool testVerticalBorderAtLeftComparator ()
	{
		this -> _raster.initialize ();
		_registers.initialize ();
		_registers.setRegister (0x11, MCHEmul::UByte (0x18));
		_registers.setRegister (0x16, MCHEmul::UByte (0x08));
		while (this -> _raster.currentLine () != _registers.minRasterV ())
			this -> _raster.vData ().next ();
		while (this -> _raster.currentColumn () != 20)
			this -> _raster.hData ().add (this -> _raster.step ());

		this -> _vicGraphicInfo._ffVBorder = true;
		this -> _vicGraphicInfo._ffMBorder = true;
		const unsigned short topRC =
			this -> _raster.hData ().currentVisiblePosition ();
		const unsigned short topRCA = (topRC >> 3) << 3;
		DrawContext topContext
			(this -> _raster.hData ().firstDisplayPosition (),
			 topRC, topRCA, 0);
		this -> actualizeMainBorderStatus
			(topContext, 0, DrawContext::_FIRSTPIXELAFTERCPUWRITE);
		this -> actualizeMainBorderStatus
			(topContext, DrawContext::_FIRSTPIXELAFTERCPUWRITE,
			 this -> _raster.step ());
		this -> _raster.hData ().add (this -> _raster.step ());
		const unsigned short nextTopRC =
			this -> _raster.hData ().currentVisiblePosition ();
		const unsigned short nextTopRCA = (nextTopRC >> 3) << 3;
		DrawContext nextTopContext
			(this -> _raster.hData ().firstDisplayPosition (),
			 nextTopRC, nextTopRCA, 0);
		this -> actualizeMainBorderStatus
			(nextTopContext, 0, DrawContext::_FIRSTPIXELAFTERCPUWRITE);
		this -> actualizeMainBorderStatus
			(nextTopContext, DrawContext::_FIRSTPIXELAFTERCPUWRITE,
			 this -> _raster.step ());
		const bool openedAtTop =
			topContext._verticalBorderData == MCHEmul::UByte (0xff) &&
			topContext._mainBorderData == MCHEmul::UByte (0xff) &&
			nextTopContext._verticalBorderData == MCHEmul::UByte::_0 &&
			nextTopContext._mainBorderData == MCHEmul::UByte::_0 &&
			!this -> _vicGraphicInfo._ffVBorder &&
			!this -> _vicGraphicInfo._ffMBorder;

		this -> _raster.hData ().initialize ();
		while (this -> _raster.currentColumn () != 20)
			this -> _raster.hData ().add (this -> _raster.step ());
		while (this -> _raster.currentLine () != _registers.maxRasterV ())
			this -> _raster.vData ().next ();
		this -> _vicGraphicInfo._ffVBorder = false;
		this -> _vicGraphicInfo._ffMBorder = true;
		const unsigned short bottomRC =
			this -> _raster.hData ().currentVisiblePosition ();
		const unsigned short bottomRCA = (bottomRC >> 3) << 3;
		DrawContext bottomContext
			(this -> _raster.hData ().firstDisplayPosition (),
			 bottomRC, bottomRCA, 0);
		this -> actualizeMainBorderStatus
			(bottomContext, 0, DrawContext::_FIRSTPIXELAFTERCPUWRITE);
		this -> actualizeMainBorderStatus
			(bottomContext, DrawContext::_FIRSTPIXELAFTERCPUWRITE,
			 this -> _raster.step ());
		this -> _raster.hData ().add (this -> _raster.step ());
		const unsigned short nextBottomRC =
			this -> _raster.hData ().currentVisiblePosition ();
		const unsigned short nextBottomRCA = (nextBottomRC >> 3) << 3;
		DrawContext nextBottomContext
			(this -> _raster.hData ().firstDisplayPosition (),
			 nextBottomRC, nextBottomRCA, 0);
		this -> actualizeMainBorderStatus
			(nextBottomContext, 0, DrawContext::_FIRSTPIXELAFTERCPUWRITE);
		this -> actualizeMainBorderStatus
			(nextBottomContext, DrawContext::_FIRSTPIXELAFTERCPUWRITE,
			 this -> _raster.step ());
		const bool closedAtBottom =
			bottomContext._verticalBorderData == MCHEmul::UByte::_0 &&
			bottomContext._mainBorderData == MCHEmul::UByte (0xff) &&
			nextBottomContext._verticalBorderData == MCHEmul::UByte (0xff) &&
			nextBottomContext._mainBorderData == MCHEmul::UByte (0xff) &&
			this -> _vicGraphicInfo._ffVBorder &&
			this -> _vicGraphicInfo._ffMBorder;

		const bool result = openedAtTop && closedAtBottom;
		this -> _vicGraphicInfo._ffVBorder = false;
		this -> _vicGraphicInfo._ffMBorder = false;
		this -> _cycleInRasterLine = 1;

		std::cout << "Vertical border at left comparator | "
			<< (result ? "OK" : "ERROR") << std::endl;

		return (result);
	}

	/** Verifies the final-cycle guard used by simulate() and the vertical-border
		comparator for the concrete PAL or NTSC model. */
	bool testVerticalBorderComparatorCycle (const std::string& testName)
	{
		this -> _raster.initialize ();
		_registers.initialize ();
		_registers.setRegister (0x11, MCHEmul::UByte (0x18));
		while (this -> _raster.currentLine () != _registers.maxRasterV ())
			this -> _raster.vData ().next ();

		this -> _vicGraphicInfo._ffVBorder = false;
		this -> _cycleInRasterLine = this -> cyclesPerRasterLine () - 1;
		if (this -> _cycleInRasterLine == this -> cyclesPerRasterLine ())
			this -> actualizeVerticalBorderStatus ();
		const bool unchangedBeforeLastCycle =
			!this -> _vicGraphicInfo._ffVBorder;

		this -> _cycleInRasterLine = this -> cyclesPerRasterLine ();
		if (this -> _cycleInRasterLine == this -> cyclesPerRasterLine ())
			this -> actualizeVerticalBorderStatus ();
		const bool closedAtLastCycle = this -> _vicGraphicInfo._ffVBorder;

		while (this -> _raster.currentLine () != _registers.minRasterV ())
			this -> _raster.vData ().next ();
		this -> _cycleInRasterLine = this -> cyclesPerRasterLine () - 1;
		if (this -> _cycleInRasterLine == this -> cyclesPerRasterLine ())
			this -> actualizeVerticalBorderStatus ();
		const bool stillClosedBeforeLastCycle =
			this -> _vicGraphicInfo._ffVBorder;

		this -> _cycleInRasterLine = this -> cyclesPerRasterLine ();
		if (this -> _cycleInRasterLine == this -> cyclesPerRasterLine ())
			this -> actualizeVerticalBorderStatus ();
		const bool openedAtLastCycle = !this -> _vicGraphicInfo._ffVBorder;
		const bool result =
			unchangedBeforeLastCycle && closedAtLastCycle &&
			stillClosedBeforeLastCycle && openedAtLastCycle;

		this -> _cycleInRasterLine = 1;
		std::cout << testName << " | cycle " << this -> cyclesPerRasterLine ()
			<< " | " << (result ? "OK" : "ERROR") << std::endl;

		return (result);
	}

	/** Verifies that a sprite ending at cycle 16 keeps an early slot for
		sprites 3..7 but does not reserve a late slot for sprites 0..2. */
	bool testProjectedSpriteDMAMask ()
	{
		for (size_t i = 0; i < 8; i++)
		{
			this -> _vicSpriteInfo [i]._DMAActive = true;
			this -> _vicSpriteInfo [i]._MCBASE = 60;
		}
		_registers.setRegister (0x17, MCHEmul::UByte::_0);

		const unsigned char finishingMask =
			this -> projectedSpriteDMAMaskForNextRasterLine ();

		// Enabling Y expansion and processing cycle 55 toggles every flip-flop
		// from its known true state without exposing private register internals.
		_registers.setRegister (0x17, MCHEmul::UByte (0xff));
		this -> _cycleInRasterLine = 55;
		this -> treatSpriteDMAStartAtCurrentCycle ();
		const unsigned char heldMask =
			this -> projectedSpriteDMAMaskForNextRasterLine ();

		_registers.setRegister (0x17, MCHEmul::UByte::_0);
		for (size_t i = 0; i < 8; i++)
			this -> _vicSpriteInfo [i]._MCBASE = 59;
		const unsigned char continuingMask =
			this -> projectedSpriteDMAMaskForNextRasterLine ();

		const bool result =
			finishingMask == 0xf8 &&
			heldMask == 0xff &&
			continuingMask == 0xff;

		for (size_t i = 0; i < 8; i++)
		{
			this -> _vicSpriteInfo [i]._DMAActive = false;
			this -> _vicSpriteInfo [i]._MCBASE = 0;
		}
		this -> _cycleInRasterLine = 1;

		std::cout
			<< "Projected sprite DMA mask | finishing $"
			<< std::hex << (unsigned int) finishingMask
			<< " | held $" << (unsigned int) heldMask
			<< " | continuing $" << (unsigned int) continuingMask
			<< std::dec << " | " << (result ? "OK" : "ERROR") << std::endl;

		return (result);
	}

	/** Verifies that vertical sprite comparators reserve only the DMA slots
		that remain reachable in the current and following raster lines. */
	bool testPredictedSpriteDMAStartMasks ()
	{
		this -> _raster.initialize ();
		_registers.initialize ();
		this -> _currentSpriteDMAMask = this -> _nextSpriteDMAMask = 0;
		this -> _cycleInRasterLine = 1;

		const unsigned char currentY =
			(unsigned char) this -> _raster.currentLine ();
		const unsigned char nextY =
			(unsigned char) this -> _raster.nextLine ();

		_registers.setRegister (0x01, MCHEmul::UByte (currentY));
		_registers.setRegister (0x07, MCHEmul::UByte (currentY));
		_registers.setRegister (0x15, MCHEmul::UByte (0x09));
		const unsigned char currentLineCurrentMask =
			this -> currentSpriteDMAStopMask ();
		const unsigned char currentLineNextMask =
			this -> nextSpriteDMAStopMask ();

		_registers.setRegister (0x01, MCHEmul::UByte (nextY));
		_registers.setRegister (0x15, MCHEmul::UByte (0x01));
		const unsigned char nextLineCurrentMask =
			this -> currentSpriteDMAStopMask ();
		const unsigned char nextLineNextMask =
			this -> nextSpriteDMAStopMask ();

		_registers.setRegister (0x15, MCHEmul::UByte::_0);
		const unsigned char disabledCurrentMask =
			this -> currentSpriteDMAStopMask ();
		const unsigned char disabledNextMask =
			this -> nextSpriteDMAStopMask ();

		const bool result =
			currentLineCurrentMask == 0x01 &&
			currentLineNextMask == 0x09 &&
			nextLineCurrentMask == 0x00 &&
			nextLineNextMask == 0x01 &&
			disabledCurrentMask == 0x00 &&
			disabledNextMask == 0x00;

		_registers.initialize ();
		std::cout
			<< "Predicted sprite DMA start masks | current $"
			<< std::hex << (unsigned int) currentLineCurrentMask
			<< "/" << (unsigned int) currentLineNextMask
			<< " | next $" << (unsigned int) nextLineCurrentMask
			<< "/" << (unsigned int) nextLineNextMask
			<< std::dec << " | " << (result ? "OK" : "ERROR") << std::endl;

		return (result);
	}

	/** Verifies the independently specified staggered VIC-II graphics pipeline:
		c-access occupies cycles 15..54 and g-access cycles 16..55. */
	bool testGraphicAccessPipelineWindows ()
	{
		this -> _badLineCAccessActive = true;
		this -> _badLineCAccessAllowedThisLine = true;
		this -> _badLineCAccessStartCycle = 12;

		this -> _cycleInRasterLine = 14;
		const bool beforePipeline =
			!this -> isBadLineCAccessCycle () && !this -> isGraphicAccessCycle ();
		this -> _cycleInRasterLine = 15;
		const bool firstCAccess =
			this -> isBadLineCAccessCycle () && !this -> isGraphicAccessCycle ();
		this -> _cycleInRasterLine = 16;
		const bool firstSharedCycle =
			this -> isBadLineCAccessCycle () && this -> isGraphicAccessCycle ();
		this -> _cycleInRasterLine = 54;
		const bool lastSharedCycle =
			this -> isBadLineCAccessCycle () && this -> isGraphicAccessCycle ();
		this -> _cycleInRasterLine = 55;
		const bool lastGAccess =
			!this -> isBadLineCAccessCycle () && this -> isGraphicAccessCycle ();

		this -> _badLineCAccessStartCycle = 36;
		this -> _cycleInRasterLine = 36;
		const bool beforeLateCAccess = !this -> isBadLineCAccessCycle ();
		this -> _cycleInRasterLine = 37;
		const bool firstLateCAccess = this -> isBadLineCAccessCycle ();
		const bool result =
			beforePipeline && firstCAccess && firstSharedCycle &&
			lastSharedCycle && lastGAccess &&
			beforeLateCAccess && firstLateCAccess;

		this -> _badLineCAccessActive = false;
		this -> _badLineCAccessAllowedThisLine = false;
		this -> _badLineCAccessStartCycle = 0;
		this -> _cycleInRasterLine = 1;

		std::cout << "Staggered c/g access windows | "
			<< (result ? "OK" : "ERROR") << std::endl;

		return (result);
	}

	/** Verifies that a late bad-line sequence starts BA when it is recognized,
		attempts its first c-access one cycle later and makes AEC effective after
		the complete three-cycle BA warning. */
	bool testLateBadLineCPUStopWindow ()
	{
		this -> _badLineConditionActive = true;
		this -> _badLineCAccessActive = true;
		this -> _badLineCAccessStartCycle = 36;
		this -> _cycleInRasterLine = 36;
		this -> _currentSpriteDMAMask = 0;
		this -> actualizeCPUStopWindowsAfterBadLineChange ();

		const CPUStopWindows& windows = *this -> _currentCPUStopWindows;
		const bool middleResult = windows.size () == 1 &&
			windows [0]._firstBACycle == 36 &&
			windows [0]._firstAECCycle == 39 &&
			windows [0]._lastCycle == 54 &&
			this -> _badLineBAAlreadyRequested &&
			this -> _badLineBARequestCycle == 36 &&
			this -> firstBadLineCAccessCycle () == 37;

		this -> _badLineCAccessStartCycle = 41;
		this -> _cycleInRasterLine = 41;
		this -> actualizeCPUStopWindowsAfterBadLineChange ();

		const bool lateResult = windows.size () == 1 &&
			windows [0]._firstBACycle == 41 &&
			windows [0]._firstAECCycle == 44 &&
			windows [0]._lastCycle == 54 &&
			this -> _badLineBARequestCycle == 41 &&
			this -> firstBadLineCAccessCycle () == 42;

		this -> _badLineCAccessStartCycle = 54;
		this -> _cycleInRasterLine = 54;
		this -> actualizeCPUStopWindowsAfterBadLineChange ();

		const bool lastCycleResult = windows.size () == 1 &&
			windows [0]._firstBACycle == 54 &&
			windows [0]._firstAECCycle == 57 &&
			windows [0]._lastCycle == 54 &&
			this -> _badLineBARequestCycle == 54 &&
			this -> firstBadLineCAccessCycle () == 55;

		const bool result = middleResult && lateResult && lastCycleResult;

		this -> _badLineConditionActive = false;
		this -> _badLineCAccessActive = false;
		this -> _badLineCAccessStartCycle = 0;
		this -> _cycleInRasterLine = 1;
		this -> actualizeCPUStopWindowsAfterBadLineChange ();

		std::cout << "Late bad-line BA/AEC window | "
			<< (result ? "OK" : "ERROR") << std::endl;

		return (result);
	}

	unsigned short currentRasterLine () const
							{ return (this -> _raster.currentLine ()); }

	private:
	/** Uses Bauer's independent PAL/NTSC bad-line interval instead of the
		production constants so a common erroneous constant cannot pass the test. */
	bool windowsContainRegularBadLine (const CPUStopWindows& windows) const
	{
		for (const auto& i : windows)
			if (i._firstBACycle == 12 &&
				i._firstAECCycle == 15 && i._lastCycle == 54)
				return (true);

		return (false);
	}

	MCHEmul::PhysicalStorage _registerStorage;
	COMMODORE::VICIIRegisters _registers;
};

int main ()
{
	std::cout << "TestVICIIStop" << std::endl;
	std::cout << "Instruction | completed cycles | predicted/Bauer" << std::endl;

	TestVICII <COMMODORE::VICII_PAL> vicii;
	TestVICII <COMMODORE::VICII_NTSC> viciiNTSC;
	const TestVICII <COMMODORE::VICII_PAL>::CPUStopWindows badLineWindows =
		{ TestVICII <COMMODORE::VICII_PAL>::CPUStopWindow (12, 15, 54) };
	const MCHEmul::CycleStructure ldaAbsolute
		(4, MCHEmul::CPUCycle::_READ);
	const MCHEmul::CycleStructure staAbsolute =
		{ MCHEmul::CPUCycle::_READ, MCHEmul::CPUCycle::_READ,
		  MCHEmul::CPUCycle::_READ, MCHEmul::CPUCycle::_WRITE };
	const MCHEmul::CycleStructure rmwAbsolute =
		{ MCHEmul::CPUCycle::_READ, MCHEmul::CPUCycle::_READ,
		  MCHEmul::CPUCycle::_READ, MCHEmul::CPUCycle::_READ,
		  MCHEmul::CPUCycle::_WRITE, MCHEmul::CPUCycle::_WRITE };
	const MCHEmul::CycleStructure interrupt6500 =
	{
		MCHEmul::CPUCycle::_READ,
		MCHEmul::CPUCycle::_READ,
		MCHEmul::CPUCycle::_WRITE,
		MCHEmul::CPUCycle::_WRITE,
		MCHEmul::CPUCycle::_WRITE,
		MCHEmul::CPUCycle::_READ,
		MCHEmul::CPUCycle::_READ
	};

	const ExpectedPrediction ldaExpected [4] =
	{
		{ 12, 57, 58, 43, { } },
		{ 12, 56, 57, 43, { } },
		{ 12, 55, 56, 43, { } },
		{ 12, 11, 55, 43, { } }
	};
	const ExpectedPrediction staExpected [4] =
	{
		{ 12, 57, 58, 43, { 46 } },
		{ 12, 56, 57, 43, { 46 } },
		{ 13, 12, 55, 42, { 3 } },
		{ 12, 11, 55, 43, { 3 } }
	};

	bool result = true;
	for (unsigned int i = 0; i < 4; i++)
		result &= vicii.testPrediction
			("LDA abs", ldaAbsolute, i + 1,
			 badLineWindows, ldaExpected [i]);
	for (unsigned int i = 0; i < 4; i++)
		result &= vicii.testPrediction
			("STA abs", staAbsolute, i + 1,
			 badLineWindows, staExpected [i]);

	// A memory RMW exposes the original value and the modified value on two
	// consecutive write cycles. Exercise writes before BA, during the BA lead
	// and after a preceding read has been held until the end of the window.
	result &= vicii.testPrediction
		("RMW abs before BA", rmwAbsolute, 6,
		 badLineWindows, { 12, 11, 55, 43, { 4, 5 } });
	result &= vicii.testPrediction
		("RMW abs during BA lead", rmwAbsolute, 4,
		 badLineWindows, { 14, 13, 55, 41, { 4, 5 } });
	result &= vicii.testPrediction
		("RMW abs after AEC", rmwAbsolute, 2,
		 badLineWindows, { 12, 58, 59, 43, { 47, 48 } });

	// Starting at cycle 10, the three stack writes complete during the BA lead.
	// The following vector read stops at AEC cycle 15 and resumes at cycle 55.
	result &= vicii.testPrediction
		("IRQ/NMI 6500", interrupt6500, 2,
		 badLineWindows, { 15, 56, 57, 40, { 2, 3, 4 } });

	result &= vicii.testWritePositionAcrossRasterLine
		("PAL write across raster line", staAbsolute);
	result &= viciiNTSC.testWritePositionAcrossRasterLine
		("NTSC write across raster line", staAbsolute);
	result &= vicii.testSingleRegisterWriteTiming ();
	result &= vicii.testRMWRegisterWriteTiming ();
	result &= vicii.testHorizontalDisplayZoneDeferred ();
	result &= vicii.testOutputStateCapture ();
	result &= vicii.testInvalidGraphicPixelMask ();
	result &= vicii.testSpriteHorizontalStartPhases ();
	result &= vicii.testLeftBorderAtSliceBeginning ();
	result &= vicii.testMainBorderComparatorPhases ();
	result &= vicii.testVerticalBorderAtLeftComparator ();
	result &= vicii.testVerticalBorderComparatorCycle
		("PAL vertical border comparator");
	result &= viciiNTSC.testVerticalBorderComparatorCycle
		("NTSC vertical border comparator");
	result &= vicii.testProjectedSpriteDMAMask ();
	result &= vicii.testPredictedSpriteDMAStartMasks ();
	result &= vicii.testGraphicAccessPipelineWindows ();
	result &= vicii.testLateBadLineCPUStopWindow ();

	// YSCROLL=2: line 50 is a bad line and neither line 49 nor 51 is.
	vicii.advanceFromRasterLine (49, 2);
	std::cout << "PAL 49->" << vicii.currentRasterLine ()
		<< " | current bad line " << vicii.currentWindowsContainRegularBadLine ()
		<< " | next bad line " << vicii.nextWindowsContainRegularBadLine () << std::endl;
	result &= vicii.currentWindowsContainRegularBadLine ();
	result &= !vicii.nextWindowsContainRegularBadLine ();
	vicii.advanceFromRasterLine (50, 2);
	std::cout << "PAL 50->" << vicii.currentRasterLine ()
		<< " | current bad line " << vicii.currentWindowsContainRegularBadLine ()
		<< " | next bad line " << vicii.nextWindowsContainRegularBadLine () << std::endl;
	result &= !vicii.currentWindowsContainRegularBadLine ();

	// River Raid case: line 219 can be a bad line with YSCROLL=3, but line
	// 220 must never inherit its BA/AEC interval.
	vicii.advanceFromRasterLine (218, 3);
	std::cout << "PAL 218->" << vicii.currentRasterLine ()
		<< " | current bad line " << vicii.currentWindowsContainRegularBadLine ()
		<< " | next bad line " << vicii.nextWindowsContainRegularBadLine () << std::endl;
	result &= vicii.currentWindowsContainRegularBadLine ();
	result &= !vicii.nextWindowsContainRegularBadLine ();
	vicii.advanceFromRasterLine (219, 3);
	std::cout << "PAL 219->" << vicii.currentRasterLine ()
		<< " | current bad line " << vicii.currentWindowsContainRegularBadLine ()
		<< " | next bad line " << vicii.nextWindowsContainRegularBadLine () << std::endl;
	result &= !vicii.currentWindowsContainRegularBadLine ();

	// Exercise the same shared line transition with the 64-cycle NTSC raster.
	viciiNTSC.advanceFromRasterLine (49, 2);
	std::cout << "NTSC 49->" << viciiNTSC.currentRasterLine ()
		<< " | current bad line " << viciiNTSC.currentWindowsContainRegularBadLine ()
		<< " | next bad line " << viciiNTSC.nextWindowsContainRegularBadLine () << std::endl;
	result &= viciiNTSC.currentWindowsContainRegularBadLine ();
	result &= !viciiNTSC.nextWindowsContainRegularBadLine ();
	viciiNTSC.advanceFromRasterLine (50, 2);
	std::cout << "NTSC 50->" << viciiNTSC.currentRasterLine ()
		<< " | current bad line " << viciiNTSC.currentWindowsContainRegularBadLine ()
		<< " | next bad line " << viciiNTSC.nextWindowsContainRegularBadLine () << std::endl;
	result &= !viciiNTSC.currentWindowsContainRegularBadLine ();

	return (result ? 0 : 1);
}
