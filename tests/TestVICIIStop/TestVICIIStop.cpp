#include <iostream>
#include <string>
#include <vector>
#include "VICIIGraphicReference.hpp"

#define SDL_MAIN_HANDLED
#include <CORE/Memory.hpp>
#include <COMMODORE/VICII/VICII.hpp>
#include <COMMODORE/VICII/VICIIRegisters.hpp>

/** Read observation stays in the test RAM, not in VIC-II production. */
class VICIITestRAM final : public MCHEmul::PhysicalStorageSubset
{
	public:
	VICIITestRAM (MCHEmul::PhysicalStorage* storage)
		: MCHEmul::PhysicalStorageSubset (0, storage, 0, MCHEmul::Address (2, 0), 0x10000)
							{ _reads.reserve (4); }
	void beginReadCapture ();
	void endReadCapture ()
							{ _capture = false; }
	const std::vector <unsigned short>& reads () const
							{ return (_reads); }

	private:
	virtual const MCHEmul::UByte& readValue (size_t position) const override;
	bool _capture = false;
	mutable std::vector <unsigned short> _reads;
};

// ---
void VICIITestRAM::beginReadCapture ()
{
	_reads.clear ();
	_capture = true;
}

// ---
const MCHEmul::UByte& VICIITestRAM::readValue (size_t position) const
{
	if (_capture)
		_reads.push_back ((unsigned short) (position));
	return (MCHEmul::PhysicalStorageSubset::readValue (position));
}

/** Plain 64 KiB memory used to exercise the real VIC-II fetch path. */
class VICIITestMemory final : public MCHEmul::Memory
{
	public:
	VICIITestMemory ()
		: MCHEmul::Memory (0, basicContent ())
							{ }

	private:
	virtual MCHEmul::Stack* lookForStack () override
							{ return (nullptr); }
	virtual MCHEmul::MemoryView* lookForCPUView () override
							{ return (view (0)); }

	static MCHEmul::Memory::Content basicContent ();
};

// ---
MCHEmul::Memory::Content VICIITestMemory::basicContent ()
{
	MCHEmul::PhysicalStorage* ram = new MCHEmul::PhysicalStorage
		(0, MCHEmul::PhysicalStorage::Type::_RAM, 0x10000);
	MCHEmul::PhysicalStorageSubset* ramSubset = new VICIITestRAM (ram);
	MCHEmul::PhysicalStorage* registerStorage = new MCHEmul::PhysicalStorage
		(1, MCHEmul::PhysicalStorage::Type::_RAM, 0x0400);
	COMMODORE::VICIIRegisters* registers = new COMMODORE::VICIIRegisters
		(registerStorage, 0, MCHEmul::Address ({ 0x00, 0xd0 }, false), 0x0400);
	MCHEmul::MemoryView* view = new MCHEmul::MemoryView
		(0, MCHEmul::PhysicalStorageSubsets ({ { 0, ramSubset } }));

	MCHEmul::Memory::Content result;
	result._physicalStorages = MCHEmul::PhysicalStorages
		({ { 0, ram }, { 1, registerStorage } });
	result._subsets = MCHEmul::PhysicalStorageSubsets
		({ { 0, ramSubset }, { COMMODORE::VICIIRegisters::_VICREGS_SUBSET, registers } });
	result._views = MCHEmul::MemoryViews ({ { 0, view } });

	return (result);
}

/** One independently specified result from Bauer's BA/AEC timing rules. */
struct ExpectedPrediction final
{
	int _firstStopCycle;
	int _instructionEffectCycle;
	int _firstNormalCycle;
	unsigned int _cyclesToStop;
	std::vector <unsigned int> _writeEffectPositions;
};

/** One current-behavior checkpoint and its independently derived VIC-II target. */
struct BadLineScenario final
{
	std::string _name;
	unsigned short _startCycle;
	bool _startedFromIdle;
	bool _sequenceActive;
	unsigned short _targetFirstCAccessCycle;
	unsigned short _targetFirstValidCAccessCycle;
	unsigned short _targetFirstValidGAccessCycle;
};

/** Timing summary observed without performing a VIC-II memory access. */
struct BadLineScenarioObservation final
{
	unsigned short _firstBACycle;
	unsigned short _firstAECCycle;
	unsigned short _firstReportedCAccessCycle;
	unsigned short _firstEffectiveCAccessCycle;
	unsigned short _firstValidCAccessCycle;
};

/** One buffered $d011 write and the VIC-II state that must remain visible
	during the cycle in which the CPU performs it. */
struct D011WritePhaseScenario final
{
	std::string _name;
	unsigned short _writeCycle;
	unsigned short _rasterLine;
	bool _startedFromIdle;
	bool _conditionActiveBeforeWrite;
	bool _writtenConditionActive;
	unsigned short _targetFirstBACycle;
	unsigned short _targetFirstCAccessCycle;
	unsigned char _targetBAPrefetchCycles;
};

/** One late $d011 write and its independently observed c/g-access pipeline. */
struct LateBadLineRegisterScenario final
{
	std::string _name;
	unsigned short _writeCycle;
	unsigned short _rasterLine;
	bool _startedFromIdle;
	unsigned short _initialVC;
	unsigned short _initialVLMI;
	unsigned short _initialGAccessIndex;
	unsigned short _targetFirstCAccessCycle;
	unsigned short _targetFirstCAccessIndex;
	unsigned short _targetCAccessCount;
	unsigned short _targetInvalidCAccessCount;
	unsigned short _targetVCAfterFirstGAccess;
	unsigned short _targetGAccessIndexAfterFirstGAccess;
	unsigned short _targetFirstValidCAccessCycle;
	unsigned short _targetFirstValidGAccessCycle;
	unsigned short _targetFirstValidGAccessIndex;
};

/** One independently specified late-bad-line graphics-output result. */
struct LateBadLineOutputScenario final
{
	std::string _name;
	unsigned short _startCycle;
	bool _startedFromIdle;
	unsigned char _invalidColorData;
	unsigned short _targetFirstInvalidGAccessIndex;
	unsigned short _targetFirstInvalidOutputCycle;
	unsigned short _targetFirstValidGAccessIndex;
	unsigned short _targetFirstValidOutputCycle;
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
		  _testMemory (),
		  _colorStorage (1, MCHEmul::PhysicalStorage::Type::_RAM, 0x0400),
		  _colorRAM
			(1, &_colorStorage, 0,
			 MCHEmul::Address ({ 0x00, 0xd8 }, false), 0x0400),
		  _registerStorage (0, MCHEmul::PhysicalStorage::Type::_RAM, 0x0400),
		  _registers
			(&_registerStorage, 0,
			 MCHEmul::Address ({ 0x00, 0xd0 }, false), 0x0400)
	{
		assert (_testMemory.initialize ());
		_colorRAM.initialize ();
		this -> setMemoryRef (&_testMemory);
		this -> setColorRAM
			(&_colorRAM, MCHEmul::Address ({ 0x00, 0xd8 }, false));
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

	/** Verifies the PAL $d012 read boundary used by 10000Members and the
		one-cycle BEQ $00 stabilizer that follows the comparison. */
	bool test10000MembersRasterReadStabilizer ()
	{
		if (!this -> initialize ())
		{
			std::cout << "10000Members $d012 stabilizer | ERROR initializing VIC-II"
				<< std::endl;

			return (false);
		}

		const MCHEmul::CycleStructure cmpAbsolute
			(4, MCHEmul::CPUCycle::_READ);
		const MCHEmul::BusCycleData busData (cmpAbsolute);
		// The final read is three cycles after the opcode fetch, not four.
		// Starts 60/61 straddle the raster boundary and stabilize at 51:4.
		const unsigned short starts [] = { 59, 60, 61 };
		const unsigned char expectedRaster [] = { 0x32, 0x32, 0x33 };
		const unsigned int expectedBranchCycles [] = { 3, 3, 2 };
		const unsigned short expectedContinuationCycles [] = { 3, 4, 4 };
		bool result = true;
		while (this -> _raster.currentLine () != 50)
			this -> _raster.vData ().next ();

		for (size_t i = 0; i < 3; i++)
		{
			this -> _cycleInRasterLine = starts [i];
			this -> _vicGraphicInfo._ROW = 50;
			this -> selectCPUStopWindowsForCurrentAndNextLine ();
			this -> prepareCPUStopPrediction
				(&cmpAbsolute, &busData, 4, 1000 + (unsigned int) i * 4);

			const unsigned char raster = this -> _VICIIRegisters -> value
				(MCHEmul::Address ({ 0x12, 0xd0 }, false)).value ();
			const unsigned int branchCycles = raster == 0x32 ? 3 : 2;
			const unsigned int continuationPosition =
				(unsigned int) (starts [i] - 1) + 4 + branchCycles;
			const unsigned short continuationLine =
				(unsigned short) (50 + continuationPosition /
				 this -> cyclesPerRasterLine ());
			const unsigned short continuationCycle =
				(unsigned short) (continuationPosition %
				 this -> cyclesPerRasterLine () + 1);
			const bool scenarioResult =
				raster == expectedRaster [i] &&
				branchCycles == expectedBranchCycles [i] &&
				continuationLine == 51 &&
				continuationCycle == expectedContinuationCycles [i];
			result &= scenarioResult;

			std::cout << "10000Members $d012 stabilizer | start 50:"
				<< starts [i] << " | read $" << std::hex
				<< (unsigned int) raster << std::dec
				<< " | BEQ " << branchCycles << " cycles"
				<< " | continuation " << continuationLine << ':'
				<< continuationCycle << " | expected read $" << std::hex
				<< (unsigned int) expectedRaster [i] << std::dec
				<< " / BEQ " << expectedBranchCycles [i]
				<< " / continuation 51:" << expectedContinuationCycles [i] << " | "
				<< (scenarioResult ? "OK" : "ERROR") << std::endl;

			this -> _pendingCPUTransaction.reset ();
			this -> _pendingCPUStopPrediction = CPUStopPrediction ();
		}

		return (result);
	}

	/** Verifies that output capture preserves the state on both sides of a
		visual register write and reuses the initial state when no write applies. */
	bool testOutputStateCapture ()
	{
		const unsigned char previousDisplayActiveSpritesMask =
			this -> _displayActiveSpritesMask;
		this -> _displayActiveSpritesMask = 0x07;

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
		this -> _displayActiveSpritesMask = previousDisplayActiveSpritesMask;
		std::cout << "VIC-II output-state capture | "
			<< (result ? "OK" : "ERROR") << std::endl;

		return (result);
	}

	/** Verifies that a visual register write performed during phi2 is projected
		using the physical raster position rather than the beginning of the
		aligned ScreenMemory slice. */
	bool testAlignedPhi2OutputWritePhase ()
	{
		if (!MCHEmul::GraphicalChip::initialize ())
		{
			std::cout << "VIC-II aligned phi2 output write | ERROR initializing screen"
				<< std::endl;

			return (false);
		}

		const unsigned short rasterColumn = 100, alignedColumn = 96;
		const size_t firstPostWriteColumn = (size_t) rasterColumn + 4;
		MCHEmul::ScreenMemory* screen = this -> screenMemory ();

		// Obtain the palette values through ScreenMemory itself. The test checks
		// output placement, not the SDL representation of the C64 colors.
		screen -> setPixel (0, 0, 1);
		screen -> setPixel (1, 0, 2);
		const unsigned int beforeColor = screen -> frameData () [0];
		const unsigned int afterColor = screen -> frameData () [1];

		DrawContext backgroundCurrent
			(24, rasterColumn, alignedColumn, 0);
		backgroundCurrent._beforeCPUWrite._backgroundColors [0] = 1;
		backgroundCurrent._afterCPUWrite =
			backgroundCurrent._beforeCPUWrite;
		backgroundCurrent._afterCPUWrite._backgroundColors [0] = 2;
		backgroundCurrent._registerEffect._applied = true;
		backgroundCurrent._registerEffect._affectsOutput = true;
		backgroundCurrent._registerEffect._registerPosition = 0x21;
		backgroundCurrent._registerEffect._previousValue =
			MCHEmul::UByte (0x01);
		backgroundCurrent._registerEffect._newValue =
			MCHEmul::UByte (0x02);

		DrawContext backgroundNext
			(24, rasterColumn + 8, alignedColumn + 8, 0);
		backgroundNext._beforeCPUWrite =
			backgroundCurrent._afterCPUWrite;

		this -> drawOutputStateLine
			(backgroundCurrent, alignedColumn, 0, 8, false);
		this -> drawOutputStateLine
			(backgroundNext, alignedColumn + 8, 0, 8, false);

		bool backgroundResult =
			&backgroundCurrent.outputStateAtPixel (0) ==
				&backgroundCurrent._beforeCPUWrite &&
			&backgroundCurrent.outputStateAtPixel (7) ==
				&backgroundCurrent._beforeCPUWrite;
		size_t firstBackgroundFailure = alignedColumn + 16;
		for (size_t column = alignedColumn;
			column < alignedColumn + 16; column++)
		{
			const unsigned int expectedColor =
				column < firstPostWriteColumn
					? beforeColor : afterColor;
			if (screen -> frameData () [column] != expectedColor)
			{
				if (firstBackgroundFailure == alignedColumn + 16)
					firstBackgroundFailure = column;
				backgroundResult = false;
			}
		}

		std::cout << "VIC-II aligned phi2 background write"
			<< " | RC/RCA " << rasterColumn << '/' << alignedColumn
			<< " | boundary " << firstPostWriteColumn;
		if (!backgroundResult)
			std::cout << " | first mismatch " << firstBackgroundFailure;
		std::cout << " | " << (backgroundResult ? "OK" : "ERROR")
			<< std::endl;

		// A $d016 write whose physical effect begins exactly at column 104
		// must be equivalent to changing XSCROLL between the two aligned slices.
		const unsigned char previousDrawingSpritesMask =
			this -> _drawingSpritesMask;
		this -> _drawingSpritesMask = 0;

		DrawContext scrollWriteCurrent
			(24, rasterColumn, alignedColumn, 1);
		scrollWriteCurrent._beforeCPUWrite._graphicMode =
			COMMODORE::VICIIRegisters::GraphicMode::_CHARMODE;
		scrollWriteCurrent._beforeCPUWrite._horizontalScroll = 6;
		scrollWriteCurrent._beforeCPUWrite._backgroundColors [0] = 1;
		scrollWriteCurrent._afterCPUWrite =
			scrollWriteCurrent._beforeCPUWrite;
		scrollWriteCurrent._afterCPUWrite._horizontalScroll = 4;
		scrollWriteCurrent._registerEffect._applied = true;
		scrollWriteCurrent._registerEffect._affectsOutput = true;
		scrollWriteCurrent._registerEffect._registerPosition = 0x16;
		scrollWriteCurrent._registerEffect._previousValue =
			MCHEmul::UByte (0x0e);
		scrollWriteCurrent._registerEffect._newValue =
			MCHEmul::UByte (0x0c);

		DrawContext scrollWriteNext
			(24, rasterColumn + 8, alignedColumn + 8, 1);
		scrollWriteNext._beforeCPUWrite =
			scrollWriteCurrent._afterCPUWrite;

		this -> resetGraphicAccessCountersForCurrentLine ();
		this -> _vicGraphicInfo._graphicData [0] =
			MCHEmul::UByte (0xa5);
		this -> _vicGraphicInfo._screenCodeDrawData [0] =
			MCHEmul::UByte (0x41);
		this -> _vicGraphicInfo._colorDrawData [0] =
			MCHEmul::UByte (0x05);
		this -> stageGraphicOutputData ();
		this -> commitGraphicOutputData ();

		this -> _vicGraphicInfo._graphicData [0] =
			MCHEmul::UByte (0x3c);
		this -> _vicGraphicInfo._screenCodeDrawData [0] =
			MCHEmul::UByte (0x42);
		this -> _vicGraphicInfo._colorDrawData [0] =
			MCHEmul::UByte (0x06);
		this -> stageGraphicOutputData ();

		this -> drawVisibleZone (nullptr, scrollWriteCurrent);
		this -> commitGraphicOutputData ();
		this -> drawVisibleZone (nullptr, scrollWriteNext);

		DrawContext scrollReferenceCurrent
			(24, rasterColumn, alignedColumn, 2);
		scrollReferenceCurrent._beforeCPUWrite =
			scrollWriteCurrent._beforeCPUWrite;

		DrawContext scrollReferenceNext
			(24, rasterColumn + 8, alignedColumn + 8, 2);
		scrollReferenceNext._beforeCPUWrite =
			scrollWriteCurrent._afterCPUWrite;

		this -> resetGraphicAccessCountersForCurrentLine ();
		this -> _vicGraphicInfo._graphicData [0] =
			MCHEmul::UByte (0xa5);
		this -> _vicGraphicInfo._screenCodeDrawData [0] =
			MCHEmul::UByte (0x41);
		this -> _vicGraphicInfo._colorDrawData [0] =
			MCHEmul::UByte (0x05);
		this -> stageGraphicOutputData ();
		this -> commitGraphicOutputData ();

		this -> _vicGraphicInfo._graphicData [0] =
			MCHEmul::UByte (0x3c);
		this -> _vicGraphicInfo._screenCodeDrawData [0] =
			MCHEmul::UByte (0x42);
		this -> _vicGraphicInfo._colorDrawData [0] =
			MCHEmul::UByte (0x06);
		this -> stageGraphicOutputData ();

		this -> drawVisibleZone (nullptr, scrollReferenceCurrent);
		this -> commitGraphicOutputData ();
		this -> drawVisibleZone (nullptr, scrollReferenceNext);

		bool scrollResult = true;
		size_t firstScrollFailure = alignedColumn + 16;
		const size_t screenColumns = screen -> columns ();
		for (size_t column = alignedColumn;
			column < alignedColumn + 16; column++)
		{
			if (screen -> frameData () [screenColumns + column] !=
				screen -> frameData () [(screenColumns << 1) + column])
			{
				if (firstScrollFailure == alignedColumn + 16)
					firstScrollFailure = column;
				scrollResult = false;
			}
		}

		std::cout << "VIC-II aligned phi2 XSCROLL write"
			<< " | RC/RCA " << rasterColumn << '/' << alignedColumn
			<< " | boundary " << firstPostWriteColumn;
		if (!scrollResult)
			std::cout << " | first mismatch " << firstScrollFailure;
		std::cout << " | " << (scrollResult ? "OK" : "ERROR")
			<< std::endl;

		this -> _drawingSpritesMask = previousDrawingSpritesMask;
		this -> resetGraphicAccessCountersForCurrentLine ();

		return (backgroundResult && scrollResult);
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
		// The horizontal comparator uses the cached masks maintained by the
		// production sprite sequencer, so the isolated test must seed them too.
		this -> _displayActiveSpritesMask = 0x0f;
		this -> _drawingSpritesMask = 0;

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
		this -> _displayActiveSpritesMask =
			this -> _drawingSpritesMask = 0;
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
		this -> _badLineConditionActive = true;
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
		this -> _badLineCAccessStartedFromIdle = true;
		this -> _cycleInRasterLine = 35;
		const bool beforeLateCAccess = !this -> isBadLineCAccessCycle ();
		this -> _cycleInRasterLine = 36;
		const bool firstLateCAccess = this -> isBadLineCAccessCycle ();
		this -> _badLineCAccessStartedFromIdle = false;
		this -> _cycleInRasterLine = 36;
		const bool firstActiveLateCAccess = this -> isBadLineCAccessCycle ();

		this -> _badLineCAccessStartedFromIdle = true;
		this -> _badLineCAccessStartCycle = 22;
		this -> _cycleInRasterLine = 21;
		const bool letsScrollItBeforeCAccess = !this -> isBadLineCAccessCycle ();
		this -> _cycleInRasterLine = 22;
		const bool letsScrollItFirstCAccess = this -> isBadLineCAccessCycle ();

		this -> _badLineCAccessStartCycle = 44;
		this -> _cycleInRasterLine = 43;
		const bool membersBeforeCAccess = !this -> isBadLineCAccessCycle ();
		this -> _cycleInRasterLine = 44;
		const bool membersFirstCAccess = this -> isBadLineCAccessCycle ();
		const bool result =
			beforePipeline && firstCAccess && firstSharedCycle &&
			lastSharedCycle && lastGAccess &&
			beforeLateCAccess && firstLateCAccess &&
			firstActiveLateCAccess &&
			letsScrollItBeforeCAccess && letsScrollItFirstCAccess &&
			membersBeforeCAccess && membersFirstCAccess;

		this -> _badLineConditionActive = false;
		this -> _badLineCAccessActive = false;
		this -> _badLineCAccessStartedFromIdle = false;
		this -> _badLineCAccessAllowedThisLine = false;
		this -> _badLineCAccessStartCycle = 0;
		this -> _cycleInRasterLine = 1;

		std::cout << "Staggered c/g access windows | "
			<< (result ? "OK" : "ERROR") << std::endl;

		return (result);
	}

	/** Verifies late-badline scheduling against the independently derived VIC-II
		target, including the first attempted and first valid c-accesses. */
	bool testBadLineScenarioMatrix ()
	{
		const BadLineScenario scenarios [] =
		{
			{ "start 12", 12, false, true, 15, 15, 16 },
			{ "start 13", 13, true,  true, 15, 16, 17 },
			{ "start 14", 14, true,  true, 15, 17, 18 },
			{ "start 15 idle", 15, true,  true, 15, 18, 19 },
			{ "Taboo 16 display", 16, false, true, 16, 19, 20 },
			{ "10000Members 18 idle", 18, true, true, 18, 21, 22 },
			{ "10000Members 19 idle", 19, true, true, 19, 22, 23 },
			{ "LetsScrollitB 22 idle", 22, true, true, 22, 25, 26 },
			{ "LetsScrollitB 34 display", 34, false, true, 34, 37, 38 },
			{ "LetsScrollitB 35 display", 35, false, true, 35, 38, 39 },
			{ "Nameless 44 idle", 44, true, true, 44, 47, 48 },
			{ "10000Members 45 idle", 45, true, true, 45, 48, 49 },
			{ "Taboo 48 display", 48, false, true, 48, 51, 52 },
			{ "Nameless 48 idle", 48, true, true, 48, 51, 52 },
			{ "LetsScrollitA 50 idle", 50, true, true, 50, 53, 54 },
			{ "late start 53 idle", 53, true, true, 53, 0, 0 },
			{ "LetsScrollitB 54 display", 54, false, true, 54, 0, 0 },
			{ "start 54 idle", 54, true, true, 54, 0, 0 },
			{ "start 55", 55, true, false, 0, 0, 0 },
			{ "start 57", 57, true, false, 0, 0, 0 }
		};
		bool result = true;

		for (const auto& scenario : scenarios)
		{
			this -> _badLineConditionActive = scenario._sequenceActive;
			this -> _badLineCAccessActive = scenario._sequenceActive;
			this -> _badLineCAccessStartedFromIdle = scenario._startedFromIdle;
			this -> _badLineCAccessAllowedThisLine = scenario._sequenceActive;
			this -> _badLineCAccessStartCycle = scenario._sequenceActive
				? scenario._startCycle : 0;
			this -> _cycleInRasterLine = scenario._startCycle;
			this -> _currentSpriteDMAMask = 0;
			this -> _nextSpriteDMAMask = 0;
			this -> _BAPrefetchCycles = this -> _BA_PREFETCH_RESET_VALUE;

			BadLineScenarioObservation observation = { 0, 0, 0, 0, 0 };
			this -> selectCPUStopWindowsForCurrentAndNextLine ();
			this -> actualizeCPUStopWindowsAfterBadLineChange ();
			if (scenario._sequenceActive)
			{
				CPUStopWindow activeWindow;
				if (CPUStopWindowAt
					((CPURasterCycle) scenario._startCycle,
					 *this -> _currentCPUStopWindows,
					 *this -> _nextCPUStopWindows, activeWindow))
				{
					observation._firstBACycle =
						(unsigned short) activeWindow._firstBACycle;
					observation._firstAECCycle =
						(unsigned short) activeWindow._firstAECCycle;
				}
			}

			observation._firstReportedCAccessCycle = firstBadLineCAccessCycle ();
			for (this -> _cycleInRasterLine = 1;
				 this -> _cycleInRasterLine <= 54;
				 this -> _cycleInRasterLine++)
			{
				this -> updateBAPrefetchStateAtCurrentCycle ();
				if (isBadLineCAccessCycle ())
				{
					if (observation._firstEffectiveCAccessCycle == 0)
						observation._firstEffectiveCAccessCycle = _cycleInRasterLine;

					if (observation._firstValidCAccessCycle == 0 &&
						cAccessDataValidAtCurrentCycle ())
						observation._firstValidCAccessCycle = _cycleInRasterLine;
				}
			}

			const unsigned short expectedFirstBA = scenario._sequenceActive
				? scenario._startCycle : 0;
			const unsigned short expectedFirstAEC = scenario._sequenceActive
				? (unsigned short) (scenario._startCycle + 3) : 0;
			const bool scenarioResult =
				observation._firstBACycle == expectedFirstBA &&
				observation._firstAECCycle == expectedFirstAEC &&
				observation._firstReportedCAccessCycle ==
					 scenario._targetFirstCAccessCycle &&
				observation._firstEffectiveCAccessCycle ==
					 scenario._targetFirstCAccessCycle &&
				observation._firstValidCAccessCycle ==
					 scenario._targetFirstValidCAccessCycle;
			result &= scenarioResult;

			std::cout << "Bad-line scenario " << scenario._name
				<< " | BA/AEC "
				<< observation._firstBACycle << "/"
				<< observation._firstAECCycle
				<< " | first C reported/effective "
				<< observation._firstReportedCAccessCycle << "/"
				<< observation._firstEffectiveCAccessCycle
				<< " | first valid C "
				<< observation._firstValidCAccessCycle
				<< " | target first/valid C/G "
				<< scenario._targetFirstCAccessCycle << "/"
				<< scenario._targetFirstValidCAccessCycle << "/"
				<< scenario._targetFirstValidGAccessCycle
				<< " | " << (scenarioResult ? "OK" : "ERROR") << std::endl;
		}

		this -> _badLineConditionActive = false;
		this -> _badLineCAccessActive = false;
		this -> _badLineCAccessStartedFromIdle = false;
		this -> _badLineCAccessAllowedThisLine = false;
		this -> _badLineCAccessStartCycle = 0;
		this -> _BAPrefetchCycles = this -> _BA_PREFETCH_RESET_VALUE;
		this -> _cycleInRasterLine = 1;
		this -> selectCPUStopWindowsForCurrentAndNextLine ();
		this -> actualizeCPUStopWindowsAfterBadLineChange ();

		return (result);
	}

	/** Verifies the production cycle order with a real buffered $d011 write. \n
		Bad-line evaluation, BA and both VIC-II memory phases must retain the
		pre-write YSCROLL throughout that cycle. */
	bool testD011WriteBadLinePhaseOrder ()
	{
		const D011WritePhaseScenario scenarios [] =
		{
			{ "early cycle 11 idle",
			  11, 155, true, false, true, 0, 0, 4 },
			{ "regular start cycle 12 idle",
			  12, 156, true, false, true, 0, 0, 4 },
			{ "early cycle 13 idle",
			  13, 157, true, false, true, 0, 0, 4 },
			{ "early cycle 14 idle",
			  14, 158, true, false, true, 0, 0, 4 },
			{ "first g-access cycle 15 idle",
			  15, 159, true, false, true, 0, 0, 4 },
			{ "LetsScrollitB cycle 21 idle",
			  21, 155, true, false, true, 0, 0, 4 },
			{ "cancel cycle 26 display",
			  26, 155, false, true, false, 21, 21, 0 },
			{ "restart cycle 30 display",
			  30, 155, false, false, true, 0, 0, 4 },
			{ "LetsScrollitA cycle 31 idle",
			  31, 53, true, false, true, 0, 0, 4 },
			{ "cycle 34 display",
			  34, 120, false, false, true, 0, 0, 4 },
			{ "10000Members cycle 45 idle",
			  45, 51, true, false, true, 0, 0, 4 },
			{ "cycle 54 idle",
			  54, 167, true, false, true, 0, 0, 4 },
			{ "cycle 55 idle",
			  55, 167, true, false, true, 0, 0, 4 }
		};
		const MCHEmul::CycleStructure cycleStructure =
			{ MCHEmul::CPUCycle::_READ, MCHEmul::CPUCycle::_READ,
			  MCHEmul::CPUCycle::_READ, MCHEmul::CPUCycle::_WRITE };
		const MCHEmul::BusCycleData busData (cycleStructure);
		bool result = true;

		if (this -> screenMemory () == nullptr &&
			!MCHEmul::GraphicalChip::initialize ())
		{
			std::cout << "$d011 bad-line write phase | ERROR initializing screen"
				<< std::endl;

			return (false);
		}

		for (const auto& scenario : scenarios)
		{
			_registers.initialize ();
			this -> _raster.initialize ();
			while (this -> _raster.currentLine () != scenario._rasterLine)
				this -> _raster.vData ().next ();
			this -> resetBadLineStateForNewRasterLine ();
			this -> resetGraphicAccessCountersForCurrentLine ();
			this -> _DENSeenAtLine30 = true;
			this -> _vicGraphicInfo._ROW = scenario._rasterLine;
			this -> _vicGraphicInfo._RC = scenario._startedFromIdle ? 7 : 3;
			this -> _currentSpriteDMAMask = 0;
			this -> _nextSpriteDMAMask = 0;
			this -> _spriteDMAStateMask = 0;
			this -> _BAPrefetchCycles = scenario._conditionActiveBeforeWrite
				? 0 : this -> _BA_PREFETCH_RESET_VALUE;
			this -> _cycleInRasterLine = scenario._writeCycle;
			if (scenario._startedFromIdle &&
				!scenario._conditionActiveBeforeWrite)
				this -> enterIdleState ();
			else
				this -> enterScreenState ();

			const unsigned char targetYScroll =
				(unsigned char) (scenario._rasterLine & 0x07);
			const unsigned char initialYScroll =
				scenario._conditionActiveBeforeWrite
					? targetYScroll
					: (unsigned char) ((targetYScroll + 1) & 0x07);
			const unsigned char writtenYScroll =
				scenario._writtenConditionActive
					? targetYScroll
					: (unsigned char) ((targetYScroll + 1) & 0x07);
			_registers.setRegister
				(0x11, MCHEmul::UByte ((unsigned char) (0x18 | initialYScroll)));
			_registers.setRegister (0x15, MCHEmul::UByte::_0);
			_registers.setRegister (0x1a, MCHEmul::UByte::_0);
			if (scenario._conditionActiveBeforeWrite)
			{
				this -> _badLineConditionActive = true;
				this -> _badLineAlreadyDetectedThisLine = true;
				this -> _badLineBAAlreadyRequested = true;
				this -> _badLineBARequestCycle = 21;
				this -> _badLineCAccessActive = true;
				this -> _badLineCAccessStartedFromIdle = false;
				this -> _badLineCAccessAllowedThisLine = true;
				this -> _badLineCAccessStartCycle = 21;
			}
			this -> selectCPUStopWindowsForCurrentAndNextLine ();
			if (scenario._conditionActiveBeforeWrite)
				this -> actualizeCPUStopWindowsAfterBadLineChange ();

			const unsigned int writeClock = 1000 + scenario._writeCycle;
			this -> _pendingCPUTransaction._cycleStructure = &cycleStructure;
			this -> _pendingCPUTransaction._busCycleData = &busData;
			this -> _pendingCPUTransaction._clockCycles = 4;
			this -> _pendingCPUTransaction._startCycle =
				(CPURasterCycle) scenario._writeCycle - 3;
			this -> _pendingCPUTransaction._startCPUCycle = writeClock - 3;
			this -> _pendingCPUStopPrediction = CPUStopPrediction ();
			this -> _pendingCPUStopPrediction._valid = true;
			this -> _pendingCPUStopPrediction._positionsToWriteEffects [0] = 3;
			this -> _pendingRegisterWrites.clear ();
			this -> _pendingRegisterWrites.emplace_back
				(&_registers, 0x11, MCHEmul::UByte
					((unsigned char) (0x18 | writtenYScroll)));

			this -> simulateRasterCycle (nullptr, writeClock, 0);

			const bool scenarioResult =
				_registers.verticalScrollPosition () == writtenYScroll &&
				this -> _badLineConditionActive ==
					scenario._conditionActiveBeforeWrite &&
				this -> _badLineBARequestCycle ==
					scenario._targetFirstBACycle &&
				this -> firstBadLineCAccessCycle () ==
					scenario._targetFirstCAccessCycle &&
				this -> _BAPrefetchCycles ==
					scenario._targetBAPrefetchCycles &&
				this -> _badLineCAccessStartedFromIdle ==
					(scenario._startedFromIdle &&
					 scenario._conditionActiveBeforeWrite) &&
				this -> idleStateActive () ==
					(scenario._startedFromIdle &&
					 !scenario._conditionActiveBeforeWrite);
			result &= scenarioResult;

			std::cout << "$d011 bad-line write phase " << scenario._name
				<< " | condition " << this -> _badLineConditionActive
				<< '/' << scenario._conditionActiveBeforeWrite
				<< " | BA/C/prefetch "
				<< this -> _badLineBARequestCycle << '/'
				<< this -> firstBadLineCAccessCycle () << '/'
				<< (unsigned int) this -> _BAPrefetchCycles
				<< " | target " << scenario._targetFirstBACycle << '/'
				<< scenario._targetFirstCAccessCycle << '/'
				<< (unsigned int) scenario._targetBAPrefetchCycles
				<< " | " << (scenarioResult ? "OK" : "ERROR") << std::endl;

			this -> _pendingCPUTransaction.reset ();
			this -> _pendingCPUStopPrediction = CPUStopPrediction ();
			this -> _pendingRegisterWrites.clear ();
		}

		_registers.initialize ();
		this -> _raster.initialize ();
		// Reset the fixture's cached row with its raster before projecting BA.
		this -> _vicGraphicInfo._ROW = this -> _raster.currentLine ();
		this -> resetBadLineStateForNewRasterLine ();
		this -> enterIdleState ();
		this -> resetGraphicAccessCountersForCurrentLine ();
		this -> _BAPrefetchCycles = this -> _BA_PREFETCH_RESET_VALUE;
		this -> _cycleInRasterLine = 1;
		this -> selectCPUStopWindowsForCurrentAndNextLine ();

		return (result);
	}

	/** Verifies the complete late-$d011-write data path without performing a
		memory access: all VIC-II activity precedes the write, while recognition and
		the first c-access occur in the following cycle. It also checks the
		three invalid c-accesses while BA propagates to AEC, and the matching
		video-matrix entry consumed by each subsequent g-access. */
	bool testLateBadLineRegisterWritePipeline ()
	{
		const LateBadLineRegisterScenario scenarios [] =
		{
			{ "LetsScrollitB cycle 21 idle", 21, 155, true,
			  320, 0, 5, 22, 0, 33, 3, 320, 7, 25, 26, 10 },
			{ "LetsScrollitA cycle 31 idle", 31, 53, true,
			  40, 0, 15, 32, 0, 23, 3, 40, 17, 35, 36, 20 },
			{ "cycle 34 display", 34, 120, false,
			  219, 19, 18, 35, 21, 20, 3, 221, 20, 38, 39, 23 },
			{ "10000Members cycle 45 idle", 45, 51, true,
			  0, 0, 29, 46, 0, 9, 3, 0, 31, 49, 50, 34 },
			{ "cycle 53 idle", 53, 167, true,
			  384, 0, 37, 54, 0, 1, 1, 384, 39, 0, 0, 0 }
		};
		bool result = true;

		for (const auto& scenario : scenarios)
		{
			_registers.initialize ();
			this -> _raster.initialize ();
			while (this -> _raster.currentLine () != scenario._rasterLine)
				this -> _raster.vData ().next ();
			this -> resetBadLineStateForNewRasterLine ();
			this -> resetGraphicAccessCountersForCurrentLine ();
			this -> _DENSeenAtLine30 = true;
			this -> _vicGraphicInfo._ROW = this -> _raster.currentLine ();
			this -> _vicGraphicInfo._VCBASE = scenario._initialVC;
			this -> _vicGraphicInfo._VC = scenario._initialVC;
			this -> _vicGraphicInfo._VLMI = scenario._initialVLMI;
			this -> _vicGraphicInfo._GAccessIndex =
				scenario._initialGAccessIndex;
			this -> _vicGraphicInfo._RC = 0;
			this -> _currentSpriteDMAMask = 0;
			this -> _nextSpriteDMAMask = 0;
			this -> _BAPrefetchCycles = this -> _BA_PREFETCH_RESET_VALUE;
			if (scenario._startedFromIdle)
				this -> enterIdleState ();
			else
				this -> enterScreenState ();

			const unsigned char targetYScroll =
				(unsigned char) (scenario._rasterLine & 0x07);
			const unsigned char previousYScroll =
				(unsigned char) ((targetYScroll + 1) & 0x07);
			_registers.setRegister
				(0x11, MCHEmul::UByte ((unsigned char) (0x18 | previousYScroll)));
			this -> selectCPUStopWindowsForCurrentAndNextLine ();

			const bool conditionBeforeWrite = this -> _badLineConditionActive;
			bool conditionImmediatelyAfterWrite = false;

			bool videoMatrixDataValid [40] = { false };
			unsigned short firstConditionCycle = 0;
			unsigned short firstBACycle = 0;
			unsigned short firstAECCycle = 0;
			unsigned short firstCAccessCycle = 0;
			unsigned short lastCAccessCycle = 0;
			unsigned short cAccessCount = 0;
			unsigned short invalidCAccessCount = 0;
			unsigned short firstCAccessIndex = 0;
			unsigned short vcAfterFirstGAccess = 0;
			unsigned short gAccessIndexAfterFirstGAccess = 0;
			unsigned short firstValidCAccessCycle = 0;
			unsigned short firstValidGAccessCycle = 0;
			unsigned short firstValidGAccessIndex = 0;
			unsigned char prefetchAtFirstCAccess =
				this -> _BA_PREFETCH_RESET_VALUE;
			bool displayDeferredUntilAfterFirstCAccess = true;

			for (this -> _cycleInRasterLine = scenario._writeCycle;
				 this -> _cycleInRasterLine <= 55;
				 this -> _cycleInRasterLine++)
			{
				const bool idleAtCycleStart = this -> idleStateActive ();
				this -> treatBadLineStateAtCurrentCycle ();
				if (firstConditionCycle == 0 && this -> _badLineConditionActive)
					firstConditionCycle = this -> _cycleInRasterLine;
				this -> updateBAPrefetchStateAtCurrentCycle ();

				CPUStopWindow activeWindow;
				if (firstBACycle == 0 && CPUStopWindowAt
					((CPURasterCycle) this -> _cycleInRasterLine,
					 *this -> _currentCPUStopWindows,
					 *this -> _nextCPUStopWindows, activeWindow))
				{
					firstBACycle = this -> _cycleInRasterLine;
					firstAECCycle = (unsigned short) activeWindow._firstAECCycle;
				}

				// The g-access consumes the previously prepared entry before the
				// c-access of the same VIC-II cycle selects the following one.
				if (this -> isGraphicAccessCycle ())
				{
					if (this -> screenStateActive ())
					{
						const size_t matrixIndex = this -> videoMatrixLineIndex ();
						if (firstValidGAccessCycle == 0 &&
							matrixIndex < 40 && videoMatrixDataValid [matrixIndex])
						{
							firstValidGAccessCycle = this -> _cycleInRasterLine;
							firstValidGAccessIndex =
								this -> _vicGraphicInfo._GAccessIndex;
						}
					}

					this -> advanceGraphicAccessCounters ();
				}

				if (this -> isBadLineCAccessCycle ())
				{
					const size_t matrixIndex = this -> videoMatrixLineIndex ();
					const bool valid = this -> cAccessDataValidAtCurrentCycle ();
					lastCAccessCycle = this -> _cycleInRasterLine;
					cAccessCount++;
					if (!valid)
						invalidCAccessCount++;
					if (firstCAccessCycle == 0)
					{
						firstCAccessCycle = this -> _cycleInRasterLine;
						firstCAccessIndex = (unsigned short) matrixIndex;
						vcAfterFirstGAccess = this -> _vicGraphicInfo._VC;
						gAccessIndexAfterFirstGAccess =
							this -> _vicGraphicInfo._GAccessIndex;
						prefetchAtFirstCAccess = this -> _BAPrefetchCycles;
						if (scenario._startedFromIdle)
							displayDeferredUntilAfterFirstCAccess &=
								idleAtCycleStart && this -> idleStateActive ();
					}

					if (matrixIndex < 40)
						videoMatrixDataValid [matrixIndex] = valid;
					if (firstValidCAccessCycle == 0 && valid)
						firstValidCAccessCycle = this -> _cycleInRasterLine;
				}

				this -> finishBadLineBusCycle ();
				if (scenario._startedFromIdle &&
					this -> _cycleInRasterLine == scenario._targetFirstCAccessCycle)
					displayDeferredUntilAfterFirstCAccess &=
						this -> screenStateActive ();

				// The CPU write is the final bus effect of its cycle. The VIC-II
				// observes the new YSCROLL when processing the following cycle.
				if (this -> _cycleInRasterLine == scenario._writeCycle)
				{
					_registers.setRegister
						(0x11, MCHEmul::UByte
							((unsigned char) (0x18 | targetYScroll)));
					conditionImmediatelyAfterWrite =
						this -> _badLineConditionActive;
				}
			}

			const bool scenarioResult =
				!conditionBeforeWrite && !conditionImmediatelyAfterWrite &&
				firstConditionCycle == scenario._targetFirstCAccessCycle &&
				firstBACycle == scenario._targetFirstCAccessCycle &&
				firstAECCycle ==
					(unsigned short) (scenario._targetFirstCAccessCycle + 3) &&
				firstCAccessCycle == scenario._targetFirstCAccessCycle &&
				firstCAccessIndex == scenario._targetFirstCAccessIndex &&
				lastCAccessCycle == 54 &&
				cAccessCount == scenario._targetCAccessCount &&
				invalidCAccessCount == scenario._targetInvalidCAccessCount &&
				vcAfterFirstGAccess == scenario._targetVCAfterFirstGAccess &&
				gAccessIndexAfterFirstGAccess ==
					scenario._targetGAccessIndexAfterFirstGAccess &&
				prefetchAtFirstCAccess ==
					(unsigned char) (this -> _BA_PREFETCH_RESET_VALUE - 1) &&
				firstValidCAccessCycle ==
					scenario._targetFirstValidCAccessCycle &&
				firstValidGAccessCycle ==
					scenario._targetFirstValidGAccessCycle &&
				firstValidGAccessIndex ==
					scenario._targetFirstValidGAccessIndex &&
				displayDeferredUntilAfterFirstCAccess;
			result &= scenarioResult;

			std::cout << "Late $d011 bad-line pipeline " << scenario._name
				<< " | condition/BA/AEC "
				<< firstConditionCycle << "/" << firstBACycle << "/"
				<< firstAECCycle
				<< " | C first/last/index/count/invalid "
				<< firstCAccessCycle << "/" << lastCAccessCycle << "/"
				<< firstCAccessIndex << "/" << cAccessCount << "/"
				<< invalidCAccessCount
				<< " | VC/G-index " << vcAfterFirstGAccess << "/"
				<< gAccessIndexAfterFirstGAccess
				<< " | valid C/G/G-index "
				<< firstValidCAccessCycle << "/" << firstValidGAccessCycle
				<< "/" << firstValidGAccessIndex
				<< " | " << (scenarioResult ? "OK" : "ERROR") << std::endl;
		}

		_registers.initialize ();
		this -> _raster.initialize ();
		this -> resetBadLineStateForNewRasterLine ();
		this -> _DENSeenAtLine30 = false;
		this -> _vicGraphicInfo._ROW = this -> _raster.currentLine ();
		this -> _vicGraphicInfo._VCBASE = 0;
		this -> _vicGraphicInfo._VC = 0;
		this -> _vicGraphicInfo._RC = 0;
		this -> enterIdleState ();
		this -> resetGraphicAccessCountersForCurrentLine ();
		this -> _BAPrefetchCycles = this -> _BA_PREFETCH_RESET_VALUE;
		this -> _cycleInRasterLine = 1;
		this -> selectCPUStopWindowsForCurrentAndNextLine ();

		return (result);
	}

	/** Verifies that a late Bad Line Condition cancels and restarts BA and
		c-access activity on each instantaneous transition. */
	bool testBadLineConditionTransitions ()
	{
		this -> enterIdleState ();
		this -> _badLineConditionActive = true;
		this -> _cycleInRasterLine = 22;
		this -> _currentSpriteDMAMask = 0;
		this -> _nextSpriteDMAMask = 0;
		this -> updateBadLineCAccessStateAtCurrentCycle (true);
		this -> actualizeCPUStopWindowsAfterBadLineChange ();
		this -> finishBadLineBusCycle ();

		this -> _badLineConditionActive = false;
		this -> _cycleInRasterLine = 26;
		this -> updateBadLineCAccessStateAtCurrentCycle (false);
		this -> actualizeCPUStopWindowsAfterBadLineChange ();
		CPUStopWindow activeWindow;
		const bool lateIntervalCancelled =
			!this -> _badLineCAccessActive &&
			!isBadLineCAccessCycle () &&
			!CPUStopWindowAt
				(26, *this -> _currentCPUStopWindows,
				 *this -> _nextCPUStopWindows, activeWindow);

		this -> _badLineConditionActive = true;
		this -> _cycleInRasterLine = 30;
		this -> updateBadLineCAccessStateAtCurrentCycle (false);
		this -> actualizeCPUStopWindowsAfterBadLineChange ();
		const bool reactivationStartsNewInterval =
			this -> _badLineCAccessActive &&
			firstBadLineCAccessCycle () == 30 &&
			isBadLineCAccessCycle () &&
			CPUStopWindowAt
				(30, *this -> _currentCPUStopWindows,
				 *this -> _nextCPUStopWindows, activeWindow) &&
			activeWindow._firstBACycle == 30 &&
			activeWindow._firstAECCycle == 33 &&
			activeWindow._lastCycle == 54;

		this -> _badLineConditionActive = false;
		this -> _badLineCAccessActive = true;
		this -> _badLineCAccessStartedFromIdle = true;
		this -> _badLineCAccessAllowedThisLine = false;
		this -> _badLineCAccessStartCycle = 13;
		this -> _cycleInRasterLine = 14;
		this -> treatGraphicFetchStartCycle ();
		const bool earlySequenceCancelledAt14 =
			!this -> _badLineCAccessActive &&
			!this -> _badLineCAccessAllowedThisLine &&
			firstBadLineCAccessCycle () == 0;
		const bool result = lateIntervalCancelled &&
			reactivationStartsNewInterval && earlySequenceCancelledAt14;

		this -> _badLineConditionActive = false;
		this -> _badLineCAccessActive = false;
		this -> _badLineCAccessStartedFromIdle = false;
		this -> _badLineCAccessAllowedThisLine = false;
		this -> _badLineCAccessStartCycle = 0;
		this -> _cycleInRasterLine = 1;
		this -> actualizeCPUStopWindowsAfterBadLineChange ();

		std::cout << "Bad-line condition transitions | late cancellation "
			<< lateIntervalCancelled << ", reactivation "
			<< reactivationStartsNewInterval << ", early cancellation "
			<< earlySequenceCancelledAt14 << " | "
			<< (result ? "OK" : "ERROR") << std::endl;

		return (result);
	}

	/** Verifies that the BA lead/prefetch state follows the merged bus signal,
		including source handoff, a real BA-high gap and raster-line continuity. */
	bool testSharedBAPrefetchState ()
	{
		CPUStopWindows noWindows;
		CPUStopWindows continuous =
		{
			CPUStopWindow (30, 33, 33, false, 0x01),
			CPUStopWindow (34, 37, 54, true, 0x00)
		};
		this -> mergeCPUStopWindows (continuous);
		this -> _currentCPUStopWindows = &continuous;
		this -> _nextCPUStopWindows = &noWindows;
		this -> _BAPrefetchCycles = this -> _BA_PREFETCH_RESET_VALUE;

		this -> _cycleInRasterLine = 29;
		this -> updateBAPrefetchStateAtCurrentCycle ();
		bool continuousResult = this -> _BAPrefetchCycles == 4;
		this -> _cycleInRasterLine = 30;
		this -> updateBAPrefetchStateAtCurrentCycle ();
		continuousResult &= this -> _BAPrefetchCycles == 3;
		this -> _cycleInRasterLine = 31;
		this -> updateBAPrefetchStateAtCurrentCycle ();
		continuousResult &= this -> _BAPrefetchCycles == 2;
		this -> _cycleInRasterLine = 32;
		this -> updateBAPrefetchStateAtCurrentCycle ();
		continuousResult &= this -> _BAPrefetchCycles == 1;
		this -> _cycleInRasterLine = 33;
		this -> updateBAPrefetchStateAtCurrentCycle ();
		continuousResult &= this -> _BAPrefetchCycles == 0;
		this -> _cycleInRasterLine = 34;
		this -> updateBAPrefetchStateAtCurrentCycle ();
		continuousResult &= this -> cAccessDataValidAtCurrentCycle ();

		CPUStopWindows separated =
		{
			CPUStopWindow (30, 33, 32, false, 0x01),
			CPUStopWindow (34, 37, 54, true, 0x00)
		};
		this -> mergeCPUStopWindows (separated);
		this -> _currentCPUStopWindows = &separated;
		this -> _BAPrefetchCycles = this -> _BA_PREFETCH_RESET_VALUE;
		for (this -> _cycleInRasterLine = 30;
			 this -> _cycleInRasterLine <= 32;
			 this -> _cycleInRasterLine++)
			this -> updateBAPrefetchStateAtCurrentCycle ();
		this -> _cycleInRasterLine = 33;
		this -> updateBAPrefetchStateAtCurrentCycle ();
		const bool gapReloadedState = this -> _BAPrefetchCycles == 4;
		for (this -> _cycleInRasterLine = 34;
			 this -> _cycleInRasterLine <= 36;
			 this -> _cycleInRasterLine++)
			this -> updateBAPrefetchStateAtCurrentCycle ();
		const bool separatedInvalidState =
			this -> _BAPrefetchCycles == 1 &&
			!this -> cAccessDataValidAtCurrentCycle ();
		this -> _cycleInRasterLine = 37;
		this -> updateBAPrefetchStateAtCurrentCycle ();
		const bool separatedValidState =
			this -> cAccessDataValidAtCurrentCycle ();

		CPUStopWindows tail =
		{
			CPUStopWindow (61, 64, 63, false, 0x02)
		};
		CPUStopWindows head =
		{
			CPUStopWindow (1, 4, 2, false, 0x02)
		};
		this -> _currentCPUStopWindows = &tail;
		this -> _BAPrefetchCycles = this -> _BA_PREFETCH_RESET_VALUE;
		for (this -> _cycleInRasterLine = 61;
			 this -> _cycleInRasterLine <= 63;
			 this -> _cycleInRasterLine++)
			this -> updateBAPrefetchStateAtCurrentCycle ();
		this -> _currentCPUStopWindows = &head;
		this -> _cycleInRasterLine = 1;
		this -> updateBAPrefetchStateAtCurrentCycle ();
		const bool rasterContinuityResult =
			this -> cAccessDataValidAtCurrentCycle ();

		const bool result = continuousResult && gapReloadedState &&
			separatedInvalidState && separatedValidState &&
			rasterContinuityResult;

		this -> _BAPrefetchCycles = this -> _BA_PREFETCH_RESET_VALUE;
		this -> _cycleInRasterLine = 1;
		this -> selectCPUStopWindowsForCurrentAndNextLine ();

		std::cout << "Shared BA prefetch state | continuous "
			<< continuousResult << ", gap " << gapReloadedState
			<< ", separated invalid/valid " << separatedInvalidState
			<< "/" << separatedValidState << ", raster continuity "
			<< rasterContinuityResult << " | "
			<< (result ? "OK" : "ERROR") << std::endl;

		return (result);
	}

	/** Verifies that adjacent sprite and badline BA windows retain one continuous
		AEC history, while a real BA-high gap keeps both intervals independent. */
	bool testBadLineSpriteWindowCompositionBaseline ()
	{
		CPUStopWindows continuous =
		{
			CPUStopWindow (30, 33, 33, false, 0x01),
			CPUStopWindow (34, 37, 54, true, 0x00)
		};
		this -> mergeCPUStopWindows (continuous);
		const bool continuousResult =
			continuous.size () == 1 &&
			continuous [0]._firstBACycle == 30 &&
			continuous [0]._firstAECCycle == 33 &&
			continuous [0]._lastCycle == 54 &&
			continuous [0]._badLineSource &&
			continuous [0]._spriteSourceMask == 0x01;

		CPUStopWindows separated =
		{
			CPUStopWindow (30, 33, 32, false, 0x01),
			CPUStopWindow (34, 37, 54, true, 0x00)
		};
		this -> mergeCPUStopWindows (separated);
		const bool separatedResult =
			separated.size () == 2 &&
			separated [0]._firstBACycle == 30 &&
			separated [1]._firstBACycle == 34;
		const bool result = continuousResult && separatedResult;

		std::cout << "Bad-line/sprite window composition baseline | continuous "
			<< continuousResult << ", separated " << separatedResult << " | "
			<< (result ? "OK" : "ERROR") << std::endl;

		return (result);
	}

	/** Verifies that a late bad-line interval starting from idle requests BA and
		attempts its first c-access when recognized, while AEC becomes effective
		after the complete three-cycle BA warning. */
	bool testLateBadLineCPUStopWindow ()
	{
		this -> _badLineConditionActive = true;
		this -> _badLineCAccessActive = true;
		this -> _badLineCAccessStartedFromIdle = true;
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
			this -> firstBadLineCAccessCycle () == 36;

		this -> _badLineCAccessStartCycle = 41;
		this -> _cycleInRasterLine = 41;
		this -> actualizeCPUStopWindowsAfterBadLineChange ();

		const bool lateResult = windows.size () == 1 &&
			windows [0]._firstBACycle == 41 &&
			windows [0]._firstAECCycle == 44 &&
			windows [0]._lastCycle == 54 &&
			this -> _badLineBARequestCycle == 41 &&
			this -> firstBadLineCAccessCycle () == 41;

		this -> _badLineCAccessStartCycle = 54;
		this -> _cycleInRasterLine = 54;
		this -> actualizeCPUStopWindowsAfterBadLineChange ();

		const bool lastCycleResult = windows.size () == 1 &&
			windows [0]._firstBACycle == 54 &&
			windows [0]._firstAECCycle == 57 &&
			windows [0]._lastCycle == 54 &&
			this -> _badLineBARequestCycle == 54 &&
			this -> firstBadLineCAccessCycle () == 54;

		const bool result = middleResult && lateResult && lastCycleResult;

		this -> _badLineConditionActive = false;
		this -> _badLineCAccessActive = false;
		this -> _badLineCAccessStartedFromIdle = false;
		this -> _badLineCAccessStartCycle = 0;
		this -> _cycleInRasterLine = 1;
		this -> actualizeCPUStopWindowsAfterBadLineChange ();

		std::cout << "Late bad-line BA/AEC window | "
			<< (result ? "OK" : "ERROR") << std::endl;

		return (result);
	}

	/** Verifies the complete output-cycle delay of every g-access and that
		XSCROLL reloads the preceding cycle's complete data/code/color triplet. */
	bool testGraphicOutputSequencer ()
	{
		this -> resetGraphicAccessCountersForCurrentLine ();
		bool result = true;
		// Cycle 16 draws screen-relative pixels -8..-1. Its g-access must become
		// available in cycle 17, whose output slice starts at visible pixel 0.
		for (unsigned short cycle = 16; cycle <= 56; cycle++)
		{
			if (cycle <= 55)
			{
				this -> _vicGraphicInfo._GAccessIndex = cycle - 16;
				this -> _vicGraphicInfo._graphicData [cycle - 16] =
					MCHEmul::UByte ((unsigned char) (cycle - 16));
				this -> _vicGraphicInfo._screenCodeDrawData [cycle - 16] =
					MCHEmul::UByte ((unsigned char) (0x80 + cycle - 16));
				this -> _vicGraphicInfo._colorDrawData [cycle - 16] =
					MCHEmul::UByte ((unsigned char) (0x40 + cycle - 16));
				this -> stageGraphicOutputData ();
			}

			for (size_t i = 0; i < 8; i++)
			{
				const bool active = this -> prepareGraphicOutputPixel (0, i);
				result &= active == (cycle >= 17);
				if (active)
					result &= this -> _vicGraphicInfo._graphicOutput._data.value () ==
						(unsigned char) (cycle - 17) &&
						this -> _vicGraphicInfo._graphicOutput._screenCode.value () ==
							(unsigned char) (0x80 + cycle - 17) &&
						this -> _vicGraphicInfo._graphicOutput._colorData.value () ==
							(unsigned char) (0x40 + cycle - 17) &&
						this -> _vicGraphicInfo._graphicOutput._pixel == (unsigned char) i;
				this -> advanceGraphicOutputPixel ();
			}

			this -> commitGraphicOutputData ();
		}

		// With XSCROLL=4, one active byte spans two slices while the current
		// g-access waits until the end of the slice before replacing the input latch.
		this -> resetGraphicAccessCountersForCurrentLine ();
		this -> _vicGraphicInfo._graphicData [0] = MCHEmul::UByte (0xaa);
		this -> _vicGraphicInfo._screenCodeDrawData [0] = MCHEmul::UByte (0xa1);
		this -> _vicGraphicInfo._colorDrawData [0] = MCHEmul::UByte (0x01);
		this -> stageGraphicOutputData ();
		for (size_t i = 0; i < 8; i++)
		{
			result &= !this -> prepareGraphicOutputPixel (4, i);
			this -> advanceGraphicOutputPixel ();
		}
		this -> commitGraphicOutputData ();

		this -> _vicGraphicInfo._GAccessIndex = 1;
		this -> _vicGraphicInfo._graphicData [1] = MCHEmul::UByte (0x55);
		this -> _vicGraphicInfo._screenCodeDrawData [1] = MCHEmul::UByte (0xb2);
		this -> _vicGraphicInfo._colorDrawData [1] = MCHEmul::UByte (0x02);
		this -> stageGraphicOutputData ();
		for (size_t i = 0; i < 8; i++)
		{
			const bool active = this -> prepareGraphicOutputPixel (4, i);
			result &= active == (i >= 4);
			if (active)
				result &= this -> _vicGraphicInfo._graphicOutput._data.value () == 0xaa &&
					this -> _vicGraphicInfo._graphicOutput._screenCode.value () == 0xa1 &&
					this -> _vicGraphicInfo._graphicOutput._colorData.value () == 0x01 &&
					this -> _vicGraphicInfo._graphicOutput._pixel == (unsigned char) (i - 4);
			this -> advanceGraphicOutputPixel ();
		}
		this -> commitGraphicOutputData ();

		this -> _vicGraphicInfo._GAccessIndex = 2;
		this -> _vicGraphicInfo._graphicData [2] = MCHEmul::UByte (0x33);
		this -> _vicGraphicInfo._screenCodeDrawData [2] = MCHEmul::UByte (0xc3);
		this -> _vicGraphicInfo._colorDrawData [2] = MCHEmul::UByte (0x03);
		this -> stageGraphicOutputData ();
		for (size_t i = 0; i < 8; i++)
		{
			result &= this -> prepareGraphicOutputPixel (4, i);
			result &= this -> _vicGraphicInfo._graphicOutput._data.value () ==
				(i < 4 ? 0xaa : 0x55);
			result &= this -> _vicGraphicInfo._graphicOutput._screenCode.value () ==
				(i < 4 ? 0xa1 : 0xb2);
			result &= this -> _vicGraphicInfo._graphicOutput._colorData.value () ==
				(i < 4 ? 0x01 : 0x02);
			result &= this -> _vicGraphicInfo._graphicOutput._pixel ==
				(unsigned char) (i < 4 ? i + 4 : i - 4);
			this -> advanceGraphicOutputPixel ();
		}
		this -> commitGraphicOutputData ();

		// A visual write can move the XSCROLL comparator between the two
		// four-pixel spans. Reload exactly once when the new comparator is still
		// ahead, and retain the pending latch when it has already been missed.
		this -> resetGraphicAccessCountersForCurrentLine ();
		this -> _vicGraphicInfo._graphicData [0] = MCHEmul::UByte (0x11);
		this -> _vicGraphicInfo._screenCodeDrawData [0] = MCHEmul::UByte (0xa1);
		this -> _vicGraphicInfo._colorDrawData [0] = MCHEmul::UByte (0x01);
		this -> stageGraphicOutputData ();
		this -> commitGraphicOutputData ();
		for (size_t i = 0; i < 4; i++)
		{
			result &= !this -> prepareGraphicOutputPixel (6, i);
			this -> advanceGraphicOutputPixel ();
		}
		for (size_t i = 4; i < 8; i++)
		{
			const bool active = this -> prepareGraphicOutputPixel (4, i);
			result &= active;
			if (active)
				result &= this -> _vicGraphicInfo._graphicOutput._data.value () == 0x11 &&
					this -> _vicGraphicInfo._graphicOutput._screenCode.value () == 0xa1 &&
					this -> _vicGraphicInfo._graphicOutput._colorData.value () == 0x01 &&
					this -> _vicGraphicInfo._graphicOutput._pixel == (unsigned char) (i - 4);
			this -> advanceGraphicOutputPixel ();
		}
		result &= !this -> _vicGraphicInfo._graphicOutput._pending;

		this -> resetGraphicAccessCountersForCurrentLine ();
		this -> _vicGraphicInfo._graphicData [0] = MCHEmul::UByte (0x22);
		this -> _vicGraphicInfo._screenCodeDrawData [0] = MCHEmul::UByte (0xb2);
		this -> _vicGraphicInfo._colorDrawData [0] = MCHEmul::UByte (0x02);
		this -> stageGraphicOutputData ();
		this -> commitGraphicOutputData ();
		for (size_t i = 0; i < 4; i++)
		{
			result &= !this -> prepareGraphicOutputPixel (6, i);
			this -> advanceGraphicOutputPixel ();
		}
		for (size_t i = 4; i < 8; i++)
		{
			result &= !this -> prepareGraphicOutputPixel (2, i);
			this -> advanceGraphicOutputPixel ();
		}
		result &= this -> _vicGraphicInfo._graphicOutput._pending &&
			!this -> _vicGraphicInfo._graphicOutput._active;

		this -> resetGraphicAccessCountersForCurrentLine ();
		this -> _vicGraphicInfo._graphicData [0] = MCHEmul::UByte (0x33);
		this -> _vicGraphicInfo._screenCodeDrawData [0] = MCHEmul::UByte (0xc3);
		this -> _vicGraphicInfo._colorDrawData [0] = MCHEmul::UByte (0x03);
		this -> stageGraphicOutputData ();
		this -> commitGraphicOutputData ();
		for (size_t i = 0; i < 4; i++)
		{
			const bool active = this -> prepareGraphicOutputPixel (2, i);
			result &= active == (i >= 2);
			this -> advanceGraphicOutputPixel ();
		}
		for (size_t i = 4; i < 8; i++)
		{
			result &= this -> prepareGraphicOutputPixel (6, i);
			result &= this -> _vicGraphicInfo._graphicOutput._data.value () == 0x33 &&
				this -> _vicGraphicInfo._graphicOutput._screenCode.value () == 0xc3 &&
				this -> _vicGraphicInfo._graphicOutput._colorData.value () == 0x03 &&
				this -> _vicGraphicInfo._graphicOutput._pixel == (unsigned char) (i - 2);
			this -> advanceGraphicOutputPixel ();
		}
		result &= !this -> _vicGraphicInfo._graphicOutput._pending;

		std::cout << "VIC-II graphics output sequencer | "
			<< (result ? "OK" : "ERROR") << std::endl;

		return (result);
	}

	/** Verifies that the matrix entries fetched by late bad lines traverse the
		g-access, staged, pending and output latches without an added character
		delay, for every possible XSCROLL comparator position. */
	bool testLateBadLineGraphicOutputPipeline ()
	{
		const LateBadLineOutputScenario scenarios [] =
		{
			{ "control cycle 18 idle", 18, true, 0x05, 3, 20, 6, 23 },
			{ "cycle 22 idle", 22, true, 0x09, 7, 24, 10, 27 },
			{ "cycle 35 display", 35, false, 0x09, 20, 37, 23, 40 },
			{ "control cycle 44 idle", 44, true, 0x06, 29, 46, 32, 49 },
			{ "cycle 54 idle", 54, true, 0x09, 39, 56, 0, 0 }
		};
		bool result = true;

		for (const LateBadLineOutputScenario& scenario : scenarios)
		{
			bool scenarioResult = true;
			for (unsigned char xScroll = 0; xScroll < 8; xScroll++)
			{
				this -> resetGraphicAccessCountersForCurrentLine ();
				if (scenario._startedFromIdle)
					this -> enterIdleState ();
				else
					this -> enterScreenState ();

				this -> _badLineConditionActive = false;
				this -> _badLineCAccessAllowedThisLine = false;
				this -> _badLineCAccessActive = false;
				this -> _badLineCAccessStartedFromIdle = false;
				this -> _badLineCAccessStartCycle = 0;

				unsigned char expectedGraphicData [64] = { 0 };
				unsigned char expectedScreenCode [64] = { 0 };
				unsigned char expectedColorData [64] = { 0 };
				for (size_t i = 0; i < 40; i++)
				{
					this -> _vicGraphicInfo._screenCodeData [i] =
						MCHEmul::UByte ((unsigned char) (0x20 + i));
					this -> _vicGraphicInfo._colorData [i] =
						MCHEmul::UByte ((unsigned char) (i & 0x0f));
				}

				unsigned short firstInvalidGAccessIndex = 0;
				unsigned short firstValidGAccessIndex = 0;
				unsigned short firstInvalidOutputCycle = 0;
				unsigned short firstValidOutputCycle = 0;
				unsigned char firstInvalidOutputPixel = 0;
				unsigned char firstValidOutputPixel = 0;

				for (unsigned short cycle = 16; cycle <= 56; cycle++)
				{
					this -> _cycleInRasterLine = cycle;
					if (cycle == scenario._startCycle)
					{
						this -> _badLineConditionActive = true;
						this -> _badLineCAccessAllowedThisLine = true;
						this -> _badLineCAccessActive = true;
						this -> _badLineCAccessStartedFromIdle =
							scenario._startedFromIdle;
						this -> _badLineCAccessStartCycle = cycle;
					}

					if (cycle <= 55)
					{
						const size_t gAccessIndex = this -> graphicAccessIndex ();
						MCHEmul::UByte graphicData;
						MCHEmul::UByte screenCode;
						MCHEmul::UByte colorData;
						if (this -> idleStateActive ())
						{
							graphicData = MCHEmul::UByte
								((unsigned char) (0x10 + gAccessIndex));
							screenCode = MCHEmul::UByte::_0;
							colorData = MCHEmul::UByte::_0;
						}
						else
						{
							const size_t vMLI = this -> videoMatrixLineIndex ();
							screenCode = this -> _vicGraphicInfo._screenCodeData [vMLI];
							colorData = this -> _vicGraphicInfo._colorData [vMLI];
							graphicData = MCHEmul::UByte ((unsigned char)
								(screenCode.value () ^ colorData.value () ^ 0x5a));
						}

						this -> _vicGraphicInfo._graphicData [gAccessIndex] = graphicData;
						this -> _vicGraphicInfo._screenCodeDrawData [gAccessIndex] =
							screenCode;
						this -> _vicGraphicInfo._colorDrawData [gAccessIndex] = colorData;
						expectedGraphicData [cycle] = graphicData.value ();
						expectedScreenCode [cycle] = screenCode.value ();
						expectedColorData [cycle] = colorData.value ();

						this -> stageGraphicOutputData ();
						const auto& output = this -> _vicGraphicInfo._graphicOutput;
						scenarioResult &= output._staged &&
							output._stagedData == graphicData &&
							output._stagedScreenCode == screenCode &&
							output._stagedColorData == colorData;

						if (screenCode.value () == 0xff &&
							firstInvalidGAccessIndex == 0)
							firstInvalidGAccessIndex = (unsigned short) gAccessIndex;
						else if ((screenCode.value () & 0xc0) == 0x80 &&
							firstValidGAccessIndex == 0)
							firstValidGAccessIndex = (unsigned short) gAccessIndex;

						this -> advanceGraphicAccessCounters ();
					}

					const bool cAccess = cycle >= scenario._startCycle && cycle <= 54;
					scenarioResult &= this -> isBadLineCAccessCycle () == cAccess;
					if (cAccess)
					{
						const size_t vMLI = this -> videoMatrixLineIndex ();
						if (cycle < scenario._startCycle + 3)
						{
							this -> _vicGraphicInfo._screenCodeData [vMLI] =
								MCHEmul::UByte::_FF;
							this -> _vicGraphicInfo._colorData [vMLI] =
								MCHEmul::UByte (scenario._invalidColorData);
						}
						else
						{
							this -> _vicGraphicInfo._screenCodeData [vMLI] =
								MCHEmul::UByte ((unsigned char) (0x80 | vMLI));
							this -> _vicGraphicInfo._colorData [vMLI] =
								MCHEmul::UByte ((unsigned char) (0x08 | (vMLI & 0x07)));
						}
					}

					this -> finishBadLineBusCycle ();
					for (size_t pixel = 0; pixel < 8; pixel++)
					{
						const bool active = this -> prepareGraphicOutputPixel
							(xScroll, pixel);
						unsigned short sourceCycle = 0;
						unsigned char sourcePixel = 0;
						if (pixel >= xScroll && cycle >= 17)
						{
							sourceCycle = cycle - 1;
							sourcePixel = (unsigned char) (pixel - xScroll);
						}
						else if (pixel < xScroll && cycle >= 18)
						{
							sourceCycle = cycle - 2;
							sourcePixel = (unsigned char) (8 - xScroll + pixel);
						}

						const bool expectedActive = sourceCycle >= 16 && sourceCycle <= 55;
						scenarioResult &= active == expectedActive;
						if (active && expectedActive)
						{
							const auto& output = this -> _vicGraphicInfo._graphicOutput;
							scenarioResult &= output._data.value () ==
								expectedGraphicData [sourceCycle] &&
								output._screenCode.value () ==
									expectedScreenCode [sourceCycle] &&
								output._colorData.value () ==
									expectedColorData [sourceCycle] &&
								output._pixel == sourcePixel;

							if (output._screenCode.value () == 0xff &&
								firstInvalidOutputCycle == 0)
							{
								firstInvalidOutputCycle = cycle;
								firstInvalidOutputPixel = (unsigned char) pixel;
							}
							else if ((output._screenCode.value () & 0xc0) == 0x80 &&
								firstValidOutputCycle == 0)
							{
								firstValidOutputCycle = cycle;
								firstValidOutputPixel = (unsigned char) pixel;
							}
						}
						this -> advanceGraphicOutputPixel ();
					}
					this -> commitGraphicOutputData ();
				}

				const bool runResult =
					firstInvalidGAccessIndex ==
						scenario._targetFirstInvalidGAccessIndex &&
					firstInvalidOutputCycle ==
						scenario._targetFirstInvalidOutputCycle &&
					firstInvalidOutputPixel == xScroll &&
					firstValidGAccessIndex ==
						scenario._targetFirstValidGAccessIndex &&
					firstValidOutputCycle ==
						scenario._targetFirstValidOutputCycle &&
					(scenario._targetFirstValidOutputCycle == 0 ||
					 firstValidOutputPixel == xScroll);
				scenarioResult &= runResult;
				if (!runResult)
					std::cout << "Late bad-line graphics output " << scenario._name
						<< " | XSCROLL " << (unsigned int) xScroll
						<< " | invalid G/output/pixel " << firstInvalidGAccessIndex
						<< '/' << firstInvalidOutputCycle << '/'
						<< (unsigned int) firstInvalidOutputPixel
						<< " | valid G/output/pixel " << firstValidGAccessIndex
						<< '/' << firstValidOutputCycle << '/'
						<< (unsigned int) firstValidOutputPixel << std::endl;
			}

			std::cout << "Late bad-line graphics output " << scenario._name
				<< " | XSCROLL 0..7 | "
				<< (scenarioResult ? "OK" : "ERROR") << std::endl;
			result &= scenarioResult;
		}

		this -> _badLineConditionActive = false;
		this -> _badLineCAccessAllowedThisLine = false;
		this -> _badLineCAccessActive = false;
		this -> _badLineCAccessStartedFromIdle = false;
		this -> _badLineCAccessStartCycle = 0;
		this -> enterIdleState ();
		this -> resetGraphicAccessCountersForCurrentLine ();

		return (result);
	}

	/** Verifies the same late-bad-line output sequence through the production
		matrix/color and graphics memory-access methods, using deterministic RAM. */
	bool testLateBadLineRealFetchPipeline ()
	{
		const LateBadLineRegisterScenario scenarios [] =
		{
			{ "LetsScrollitB cycle 21 idle", 21, 155, true,
			  320, 0, 5, 22, 0, 33, 3, 320, 7, 25, 26, 10 },
			{ "LetsScrollitA cycle 31 idle", 31, 53, true,
			  40, 0, 15, 32, 0, 23, 3, 40, 17, 35, 36, 20 },
			{ "cycle 34 display", 34, 120, false,
			  219, 19, 18, 35, 21, 20, 3, 221, 20, 38, 39, 23 },
			{ "10000Members cycle 45 idle", 45, 51, true,
			  0, 0, 29, 46, 0, 9, 3, 0, 31, 49, 50, 34 },
			{ "cycle 53 idle", 53, 167, true,
			  384, 0, 37, 54, 0, 1, 1, 384, 39, 0, 0, 0 }
		};
		bool result = true;

		// Each matrix position and character row has a predictable independent
		// signature. An incorrect VC, VMLI, character code or RC changes the byte.
		for (size_t vC = 0; vC < 0x0400; vC++)
		{
			_testMemory.set
				(MCHEmul::Address (2, (unsigned int) (0x0400 + vC)),
				 MCHEmul::UByte ((unsigned char) (0x80 | (vC & 0x3f))), true);
			_colorRAM.put
				(MCHEmul::Address (2, (unsigned int) (0xd800 + vC)),
				 MCHEmul::UByte ((unsigned char) (0x08 | (vC & 0x07))));
		}
		for (size_t offset = 0; offset < 0x0800; offset++)
			_testMemory.set
				(MCHEmul::Address (2, (unsigned int) (0x1000 + offset)),
				 MCHEmul::UByte ((unsigned char) ((0x1000 + offset) & 0xff)), true);
		_testMemory.set
			(MCHEmul::Address (2, 0x3fff), MCHEmul::UByte (0xee), true);

		for (const LateBadLineRegisterScenario& scenario : scenarios)
		{
			bool scenarioResult = true;
			for (unsigned char xScroll = 0; xScroll < 8; xScroll++)
			{
				_registers.initialize ();
				_registers.setBank (0);
				_registers.setRegister (0x18, MCHEmul::UByte (0x15));
				this -> _raster.initialize ();
				while (this -> _raster.currentLine () != scenario._rasterLine)
					this -> _raster.vData ().next ();

				this -> resetBadLineStateForNewRasterLine ();
				this -> resetGraphicAccessCountersForCurrentLine ();
				this -> _DENSeenAtLine30 = true;
				this -> _vicGraphicInfo._ROW = this -> _raster.currentLine ();
				this -> _vicGraphicInfo._VCBASE = scenario._initialVC;
				this -> _vicGraphicInfo._VC = scenario._initialVC;
				this -> _vicGraphicInfo._VLMI = scenario._initialVLMI;
				this -> _vicGraphicInfo._GAccessIndex =
					scenario._initialGAccessIndex;
				this -> _vicGraphicInfo._RC =
					scenario._startedFromIdle ? 7 : 3;
				this -> _currentSpriteDMAMask = 0;
				this -> _nextSpriteDMAMask = 0;
				this -> _BAPrefetchCycles = this -> _BA_PREFETCH_RESET_VALUE;
				this -> _cpuOpcodeLowNibble = 0x09;
				if (scenario._startedFromIdle)
					this -> enterIdleState ();
				else
					this -> enterScreenState ();

				for (size_t i = 0; i < 40; i++)
				{
					this -> _vicGraphicInfo._screenCodeData [i] =
						MCHEmul::UByte ((unsigned char) (0x20 + i));
					this -> _vicGraphicInfo._colorData [i] =
						MCHEmul::UByte ((unsigned char) (i & 0x07));
				}

				const unsigned char targetYScroll =
					(unsigned char) (scenario._rasterLine & 0x07);
				_registers.setRegister
					(0x11, MCHEmul::UByte ((unsigned char)
						(0x18 | ((targetYScroll + 1) & 0x07))));
				this -> selectCPUStopWindowsForCurrentAndNextLine ();

				const bool conditionBeforeWrite = this -> _badLineConditionActive;

				const unsigned short targetFirstInvalidGAccessCycle =
					(unsigned short) (scenario._targetFirstCAccessCycle + 1);
				const unsigned short targetFirstInvalidOutputCycle =
					(unsigned short) (targetFirstInvalidGAccessCycle + 1);
				const unsigned short targetFirstValidOutputCycle =
					scenario._targetFirstValidGAccessCycle == 0 ? 0 :
						(unsigned short) (scenario._targetFirstValidGAccessCycle + 1);
				const unsigned char rc = this -> _vicGraphicInfo._RC;
				const unsigned char targetInvalidGraphicData =
					(unsigned char) ((0x1000 + (0xff << 3) + rc) & 0xff);
				const unsigned short targetFirstValidVC =
					scenario._targetFirstValidCAccessCycle == 0 ? 0 :
						(unsigned short) (scenario._targetVCAfterFirstGAccess + 3);
				const unsigned char targetValidScreenCode =
					(unsigned char) (0x80 | (targetFirstValidVC & 0x3f));
				const unsigned char targetValidColorData =
					(unsigned char) (0x08 | (targetFirstValidVC & 0x07));
				const unsigned char targetValidGraphicData =
					(unsigned char) ((0x1000 +
						(targetValidScreenCode << 3) + rc) & 0xff);

				unsigned short firstCAccessCycle = 0, firstValidCAccessCycle = 0;
				unsigned short firstInvalidGAccessCycle = 0,
					firstInvalidGAccessIndex = 0;
				unsigned short firstValidGAccessCycle = 0,
					firstValidGAccessIndex = 0;
				unsigned short firstInvalidOutputCycle = 0,
					firstValidOutputCycle = 0;
				unsigned char firstInvalidOutputPixel = 0,
					firstValidOutputPixel = 0;
				bool firstCAccessDataCorrect = false,
					firstInvalidGAccessDataCorrect = false,
					firstInvalidOutputDataCorrect = false;
				bool firstValidCAccessDataCorrect =
					scenario._targetFirstValidCAccessCycle == 0;
				bool firstValidGAccessDataCorrect =
					scenario._targetFirstValidGAccessCycle == 0;
				bool firstValidOutputDataCorrect =
					targetFirstValidOutputCycle == 0;

				for (this -> _cycleInRasterLine = scenario._writeCycle;
					 this -> _cycleInRasterLine <= 56;
					 this -> _cycleInRasterLine++)
				{
					this -> treatBadLineStateAtCurrentCycle ();
					this -> updateBAPrefetchStateAtCurrentCycle ();
					const bool cAccess = this -> isBadLineCAccessCycle ();
					const bool validCAccess =
						cAccess && this -> cAccessDataValidAtCurrentCycle ();
					const bool gAccess = this -> isGraphicAccessCycle ();
					const size_t gAccessIndex = gAccess
						? this -> graphicAccessIndex () : 0;

					this -> treatGraphicAccessCycle ();
					if (gAccess)
					{
						const unsigned char screenCode = this ->
							_vicGraphicInfo._screenCodeDrawData [gAccessIndex].value ();
						const unsigned char colorData = this ->
							_vicGraphicInfo._colorDrawData [gAccessIndex].value ();
						const unsigned char graphicData = this ->
							_vicGraphicInfo._graphicData [gAccessIndex].value ();
						if (screenCode == 0xff && firstInvalidGAccessCycle == 0)
						{
							firstInvalidGAccessCycle = this -> _cycleInRasterLine;
							firstInvalidGAccessIndex = (unsigned short) gAccessIndex;
							firstInvalidGAccessDataCorrect = colorData == 0x09 &&
								graphicData == targetInvalidGraphicData;
						}
						else if ((screenCode & 0xc0) == 0x80 &&
							firstValidGAccessCycle == 0)
						{
							firstValidGAccessCycle = this -> _cycleInRasterLine;
							firstValidGAccessIndex = (unsigned short) gAccessIndex;
							firstValidGAccessDataCorrect =
								screenCode == targetValidScreenCode &&
								colorData == targetValidColorData &&
								graphicData == targetValidGraphicData;
						}
					}

					if (cAccess && firstCAccessCycle == 0)
					{
						firstCAccessCycle = this -> _cycleInRasterLine;
						firstCAccessDataCorrect =
							this -> _vicGraphicInfo._lastScreenCodeDataRead.value () == 0xff &&
							this -> _vicGraphicInfo._lastColorDataRead.value () == 0x09;
					}
					if (validCAccess && firstValidCAccessCycle == 0)
					{
						firstValidCAccessCycle = this -> _cycleInRasterLine;
						firstValidCAccessDataCorrect =
							this -> _vicGraphicInfo._lastScreenCodeDataRead.value () ==
								targetValidScreenCode &&
							this -> _vicGraphicInfo._lastColorDataRead.value () ==
								targetValidColorData;
					}

					this -> finishBadLineBusCycle ();
					if (this -> _cycleInRasterLine == scenario._writeCycle)
						_registers.setRegister
							(0x11, MCHEmul::UByte
								((unsigned char) (0x18 | targetYScroll)));

					for (size_t pixel = 0; pixel < 8; pixel++)
					{
						if (this -> prepareGraphicOutputPixel (xScroll, pixel))
						{
							const auto& output = this -> _vicGraphicInfo._graphicOutput;
							if (output._screenCode.value () == 0xff &&
								firstInvalidOutputCycle == 0)
							{
								firstInvalidOutputCycle = this -> _cycleInRasterLine;
								firstInvalidOutputPixel = (unsigned char) pixel;
								firstInvalidOutputDataCorrect =
									output._colorData.value () == 0x09 &&
									output._data.value () == targetInvalidGraphicData;
							}
							else if ((output._screenCode.value () & 0xc0) == 0x80 &&
								firstValidOutputCycle == 0)
							{
								firstValidOutputCycle = this -> _cycleInRasterLine;
								firstValidOutputPixel = (unsigned char) pixel;
								firstValidOutputDataCorrect =
									output._screenCode.value () == targetValidScreenCode &&
									output._colorData.value () == targetValidColorData &&
									output._data.value () == targetValidGraphicData;
							}
						}
						this -> advanceGraphicOutputPixel ();
					}
					this -> commitGraphicOutputData ();
				}

				const bool runResult =
					!conditionBeforeWrite &&
					firstCAccessCycle == scenario._targetFirstCAccessCycle &&
					firstCAccessDataCorrect &&
					firstInvalidGAccessCycle == targetFirstInvalidGAccessCycle &&
					firstInvalidGAccessIndex ==
						scenario._targetGAccessIndexAfterFirstGAccess &&
					firstInvalidGAccessDataCorrect &&
					firstInvalidOutputCycle == targetFirstInvalidOutputCycle &&
					firstInvalidOutputPixel == xScroll &&
					firstInvalidOutputDataCorrect &&
					firstValidCAccessCycle == scenario._targetFirstValidCAccessCycle &&
					firstValidCAccessDataCorrect &&
					firstValidGAccessCycle == scenario._targetFirstValidGAccessCycle &&
					firstValidGAccessIndex == scenario._targetFirstValidGAccessIndex &&
					firstValidGAccessDataCorrect &&
					firstValidOutputCycle == targetFirstValidOutputCycle &&
					(targetFirstValidOutputCycle == 0 ||
					 firstValidOutputPixel == xScroll) &&
					firstValidOutputDataCorrect;
				scenarioResult &= runResult;
				if (!runResult)
					std::cout << "Late bad-line real fetch " << scenario._name
						<< " | XSCROLL " << (unsigned int) xScroll
						<< " | C " << firstCAccessCycle << '/'
						<< firstValidCAccessCycle
						<< " | invalid G/output " << firstInvalidGAccessCycle
						<< ':' << firstInvalidGAccessIndex << '/'
						<< firstInvalidOutputCycle << ':'
						<< (unsigned int) firstInvalidOutputPixel
						<< " | valid G/output " << firstValidGAccessCycle
						<< ':' << firstValidGAccessIndex << '/'
						<< firstValidOutputCycle << ':'
						<< (unsigned int) firstValidOutputPixel
						<< " | data " << firstCAccessDataCorrect << '/'
						<< firstInvalidGAccessDataCorrect << '/'
						<< firstInvalidOutputDataCorrect << '/'
						<< firstValidCAccessDataCorrect << '/'
						<< firstValidGAccessDataCorrect << '/'
						<< firstValidOutputDataCorrect << std::endl;
			}

			std::cout << "Late bad-line real fetch " << scenario._name
				<< " | XSCROLL 0..7 | "
				<< (scenarioResult ? "OK" : "ERROR") << std::endl;
			result &= scenarioResult;
		}

		this -> resetBadLineStateForNewRasterLine ();
		this -> enterIdleState ();
		this -> resetGraphicAccessCountersForCurrentLine ();
		this -> _cycleInRasterLine = 1;
		this -> selectCPUStopWindowsForCurrentAndNextLine ();

		return (result);
	}

	/** Internal consistency check: expected bytes come from production buffers
		and the current sequencer delay is assumed. This is not an external oracle. */
	bool testLateBadLineRenderedPixelPipeline ()
	{
		struct PixelScenario final
		{
			std::string _name;
			unsigned short _writeCycle, _rasterLine, _initialVC;
			unsigned char _initialRC;
			bool _startedFromIdle;
		};

		const PixelScenario scenarios [] =
		{
			{ "cycle 17 idle", 17, 154, 320, 7, true },
			{ "LetsScrollitB cycle 21 idle", 21, 155, 320, 7, true },
			{ "LetsScrollitA cycle 31 idle", 31, 53, 40, 7, true },
			{ "cycle 34 display", 34, 120, 200, 3, false },
			{ "cycle 43 idle", 43, 166, 384, 7, true },
			{ "10000Members cycle 45 idle", 45, 51, 0, 7, true },
			{ "cycle 53 idle", 53, 167, 384, 7, true },
			{ "cycle 54 idle", 54, 167, 384, 7, true }
		};
		bool result = true;

		// The byte at every character-row address has an independently predictable
		// bit pattern; matrix and color data remain distinct for every entry.
		for (size_t vC = 0; vC < 0x0400; vC++)
		{
			_testMemory.set
				(MCHEmul::Address (2, (unsigned int) (0x0400 + vC)),
				 MCHEmul::UByte ((unsigned char) (0x80 | (vC & 0x3f))), true);
			_colorRAM.put
				(MCHEmul::Address (2, (unsigned int) (0xd800 + vC)),
				 MCHEmul::UByte ((unsigned char) (0x08 | (vC & 0x07))));
		}
		for (size_t offset = 0; offset < 0x0800; offset++)
			_testMemory.set
				(MCHEmul::Address (2, (unsigned int) (0x1000 + offset)),
				 MCHEmul::UByte ((unsigned char)
					((offset ^ (offset >> 3) ^ 0xa5) & 0xff)), true);
		_testMemory.set
			(MCHEmul::Address (2, 0x3fff), MCHEmul::UByte (0x3c), true);

		for (const PixelScenario& scenario : scenarios)
		{
			bool scenarioResult = true;
			for (unsigned char xScroll = 0; xScroll < 8; xScroll++)
			{
				bool runResult = true;
				unsigned short firstFailureCycle = 0;
				size_t firstFailurePixel = 0;
				unsigned int firstActual = 0, firstExpected = 0;
				_registers.initialize ();
				_registers.setBank (0);
				_registers.setRegister (0x18, MCHEmul::UByte (0x15));
				this -> _raster.initialize ();
				while (this -> _raster.currentLine () != scenario._rasterLine)
					this -> _raster.vData ().next ();

				this -> resetBadLineStateForNewRasterLine ();
				this -> resetGraphicAccessCountersForCurrentLine ();
				this -> _DENSeenAtLine30 = true;
				this -> _vicGraphicInfo._ROW = this -> _raster.currentLine ();
				this -> _vicGraphicInfo._VCBASE = scenario._initialVC;
				this -> _vicGraphicInfo._VC = scenario._initialVC;
				this -> _vicGraphicInfo._VLMI = 0;
				this -> _vicGraphicInfo._GAccessIndex = 0;
				this -> _vicGraphicInfo._RC = scenario._initialRC;
				this -> _currentSpriteDMAMask = 0;
				this -> _nextSpriteDMAMask = 0;
				this -> _BAPrefetchCycles = this -> _BA_PREFETCH_RESET_VALUE;
				this -> _cpuOpcodeLowNibble = 0x09;
				if (scenario._startedFromIdle)
					this -> enterIdleState ();
				else
					this -> enterScreenState ();

				for (size_t i = 0; i < 40; i++)
				{
					this -> _vicGraphicInfo._screenCodeData [i] =
						MCHEmul::UByte ((unsigned char) (0x20 + i));
					this -> _vicGraphicInfo._colorData [i] =
						MCHEmul::UByte ((unsigned char) (i & 0x07));
				}

				const unsigned char targetYScroll =
					(unsigned char) (scenario._rasterLine & 0x07);
				_registers.setRegister
					(0x11, MCHEmul::UByte ((unsigned char)
						(0x18 | ((targetYScroll + 1) & 0x07))));
				this -> selectCPUStopWindowsForCurrentAndNextLine ();

				unsigned char expectedGraphicData [64] = { 0 };
				unsigned char expectedColorData [64] = { 0 };
				for (unsigned short cycle = 16; cycle <= 56; cycle++)
				{
					this -> _cycleInRasterLine = cycle;
					this -> treatBadLineStateAtCurrentCycle ();
					this -> updateBAPrefetchStateAtCurrentCycle ();
					const bool gAccess = this -> isGraphicAccessCycle ();
					const size_t gAccessIndex = gAccess
						? this -> graphicAccessIndex () : 0;
					this -> treatGraphicAccessCycle ();
					this -> finishBadLineBusCycle ();
					if (gAccess)
					{
						expectedGraphicData [cycle] = this ->
							_vicGraphicInfo._graphicData [gAccessIndex].value ();
						expectedColorData [cycle] = this ->
							_vicGraphicInfo._colorDrawData [gAccessIndex].value ();
					}

					// The CPU write occurs after all VIC-II memory activity of this
					// cycle and becomes visible to bad-line detection in the next one.
					if (cycle == scenario._writeCycle)
						_registers.setRegister
							(0x11, MCHEmul::UByte
								((unsigned char) (0x18 | targetYScroll)));

					const unsigned short rasterColumn =
						(unsigned short) ((cycle - 16) << 3);
					DrawContext dC (8, rasterColumn, rasterColumn, 0);
					dC._beforeCPUWrite._graphicMode =
						COMMODORE::VICIIRegisters::GraphicMode::_CHARMODE;
					dC._beforeCPUWrite._horizontalScroll = xScroll;
					dC._beforeCPUWrite._idleState = this -> idleStateActive ();
					DrawResult drawResult;
					this -> drawGraphics
						(dC, dC._beforeCPUWrite, 0, 8, drawResult);

					unsigned char expectedMask = 0;
					for (size_t pixel = 0; pixel < 8; pixel++)
					{
						unsigned short sourceCycle = 0;
						unsigned char sourcePixel = 0;
						if (pixel >= xScroll && cycle >= 17)
						{
							sourceCycle = cycle - 1;
							sourcePixel = (unsigned char) (pixel - xScroll);
						}
						else if (pixel < xScroll && cycle >= 18)
						{
							sourceCycle = cycle - 2;
							sourcePixel = (unsigned char) (8 - xScroll + pixel);
						}

						const int displayPixel =
							((int) cycle - 17) * 8 + (int) pixel;
						const bool foreground =
							sourceCycle >= 16 && sourceCycle <= 55 &&
							displayPixel >= 0 && displayPixel < 320 &&
							MCHEmul::UByte (expectedGraphicData [sourceCycle]).bit
								(7 - sourcePixel);
						if (foreground)
							expectedMask |= (unsigned char) (0x80 >> pixel);

						const unsigned int expectedForegroundColor = foreground
							? (dC._beforeCPUWrite._idleState
								? 0 : expectedColorData [sourceCycle] & 0x0f)
							: MCHEmul::_U0;
						const bool pixelResult =
							drawResult._foregroundColorData [pixel] ==
								expectedForegroundColor &&
							drawResult._backgroundColorData [pixel] == MCHEmul::_U0;
						if (!pixelResult && firstFailureCycle == 0)
						{
							firstFailureCycle = cycle;
							firstFailurePixel = pixel;
							firstActual =
								drawResult._foregroundColorData [pixel];
							firstExpected = expectedForegroundColor;
						}
						runResult &= pixelResult;
					}
					const bool maskResult =
						drawResult._collisionGraphicData.value () == expectedMask;
					if (!maskResult && firstFailureCycle == 0)
					{
						firstFailureCycle = cycle;
						firstActual = drawResult._collisionGraphicData.value ();
						firstExpected = expectedMask;
					}
					runResult &= maskResult;
					this -> commitGraphicOutputData ();
				}

				if (!runResult)
					std::cout << "Late bad-line rendered pixels " << scenario._name
						<< " | XSCROLL " << (unsigned int) xScroll
						<< " | first cycle/pixel " << firstFailureCycle << '/'
						<< firstFailurePixel << " | actual/expected "
						<< firstActual << '/' << firstExpected
						<< " | ERROR" << std::endl;
				scenarioResult &= runResult;
			}

			std::cout << "Late bad-line rendered pixels " << scenario._name
				<< " | XSCROLL 0..7 | "
				<< (scenarioResult ? "OK" : "ERROR") << std::endl;
			result &= scenarioResult;
		}

		// A phi2 XSCROLL write reloads a pending character only when the new
		// comparator is still ahead of the already emitted four-pixel span.
		this -> resetGraphicAccessCountersForCurrentLine ();
		this -> _vicGraphicInfo._graphicData [0] = MCHEmul::UByte (0xa5);
		this -> _vicGraphicInfo._screenCodeDrawData [0] = MCHEmul::UByte (0x41);
		this -> _vicGraphicInfo._colorDrawData [0] = MCHEmul::UByte (0x05);
		this -> stageGraphicOutputData ();
		this -> commitGraphicOutputData ();
		DrawContext comparatorAhead (0, 0, 0, 0);
		comparatorAhead._beforeCPUWrite._graphicMode =
			COMMODORE::VICIIRegisters::GraphicMode::_CHARMODE;
		comparatorAhead._beforeCPUWrite._horizontalScroll = 6;
		comparatorAhead._afterCPUWrite = comparatorAhead._beforeCPUWrite;
		comparatorAhead._afterCPUWrite._horizontalScroll = 4;
		comparatorAhead._registerEffect._applied = true;
		comparatorAhead._registerEffect._affectsOutput = true;
		DrawResult comparatorAheadResult;
		this -> drawGraphics
			(comparatorAhead, comparatorAhead._beforeCPUWrite,
			 0, DrawContext::_FIRSTPIXELAFTERCPUWRITE, comparatorAheadResult);
		this -> drawGraphics
			(comparatorAhead, comparatorAhead._afterCPUWrite,
			 DrawContext::_FIRSTPIXELAFTERCPUWRITE, 8, comparatorAheadResult);
		bool splitResult =
			comparatorAheadResult._collisionGraphicData.value () == 0x0a &&
			comparatorAheadResult._foregroundColorData [4] == 0x05 &&
			comparatorAheadResult._foregroundColorData [5] == MCHEmul::_U0 &&
			comparatorAheadResult._foregroundColorData [6] == 0x05 &&
			comparatorAheadResult._foregroundColorData [7] == MCHEmul::_U0 &&
			!this -> _vicGraphicInfo._graphicOutput._pending &&
			this -> _vicGraphicInfo._graphicOutput._active &&
			this -> _vicGraphicInfo._graphicOutput._pixel == 4;

		this -> resetGraphicAccessCountersForCurrentLine ();
		this -> _vicGraphicInfo._graphicData [0] = MCHEmul::UByte (0x5a);
		this -> _vicGraphicInfo._screenCodeDrawData [0] = MCHEmul::UByte (0x42);
		this -> _vicGraphicInfo._colorDrawData [0] = MCHEmul::UByte (0x06);
		this -> stageGraphicOutputData ();
		this -> commitGraphicOutputData ();
		DrawContext comparatorMissed (0, 0, 0, 0);
		comparatorMissed._beforeCPUWrite._graphicMode =
			COMMODORE::VICIIRegisters::GraphicMode::_CHARMODE;
		comparatorMissed._beforeCPUWrite._horizontalScroll = 6;
		comparatorMissed._afterCPUWrite = comparatorMissed._beforeCPUWrite;
		comparatorMissed._afterCPUWrite._horizontalScroll = 2;
		comparatorMissed._registerEffect._applied = true;
		comparatorMissed._registerEffect._affectsOutput = true;
		DrawResult comparatorMissedResult;
		this -> drawGraphics
			(comparatorMissed, comparatorMissed._beforeCPUWrite,
			 0, DrawContext::_FIRSTPIXELAFTERCPUWRITE, comparatorMissedResult);
		this -> drawGraphics
			(comparatorMissed, comparatorMissed._afterCPUWrite,
			 DrawContext::_FIRSTPIXELAFTERCPUWRITE, 8, comparatorMissedResult);
		splitResult &=
			comparatorMissedResult._collisionGraphicData == MCHEmul::UByte::_0 &&
			this -> _vicGraphicInfo._graphicOutput._pending &&
			!this -> _vicGraphicInfo._graphicOutput._active;

		std::cout << "VIC-II phi2 XSCROLL rendered pixels | "
			<< (splitResult ? "OK" : "ERROR") << std::endl;
		result &= splitResult;

		this -> resetBadLineStateForNewRasterLine ();
		this -> enterIdleState ();
		this -> resetGraphicAccessCountersForCurrentLine ();
		this -> _cycleInRasterLine = 1;
		this -> selectCPUStopWindowsForCurrentAndNextLine ();

		return (result);
	}

	/** Compares actual raster-aligned output with an independent PAL reference. */
	bool testReferenceNormalLineAlignment ()
	{
		if (!VICIIGraphicReference::testIndependentAnchors ())
			return (false);
		bool result = true;
		for (unsigned char scroll = 0; scroll < 8; scroll++)
		{
			result &= compareGraphicReference
				({ "normal display", 120, 200, 3, false, 0x19,
					(unsigned char) (8 | scroll), {} });
			result &= compareGraphicReference
				({ "regular bad line", 120, 200, 3, false, 0x18,
					(unsigned char) (8 | scroll), {} });
		}
		return (result);
	}

	/** Replays the D011 timing of logged cases with controlled monochrome data,
		not the entire demo or its original graphics mode. */
	bool testReferenceLateBadLineOutput ()
	{
		if (!VICIIGraphicReference::testIndependentAnchors ())
			return (false);
		const GraphicReferenceScenario scenarios [] =
		{
			{ "cycle 17 idle", 154, 320, 7, true, 0x1b, 8,
				{ { 17, 0x11, 0x1a } } },
			{ "LetsScrollitB timing", 155, 320, 7, true, 0x1c, 8,
				{ { 21, 0x11, 0x1b } } },
			{ "LetsScrollitA timing", 53, 40, 7, true, 0x1e, 8,
				{ { 31, 0x11, 0x1d } } },
			{ "cycle 34 display", 120, 200, 3, false, 0x19, 8,
				{ { 34, 0x11, 0x18 } } },
			{ "10000Members timing (not mode 7)", 51, 0, 7, true, 0x1c, 8,
				{ { 45, 0x11, 0x1b } } },
			{ "cycle 53 idle", 167, 384, 7, true, 0x18, 8,
				{ { 53, 0x11, 0x1f } } },
			{ "cycle 54 idle", 167, 384, 7, true, 0x18, 8,
				{ { 54, 0x11, 0x1f } } }
		};
		bool result = true;
		for (const GraphicReferenceScenario& scenario : scenarios)
			for (unsigned char scroll = 0; scroll < 8; scroll++)
			{
				GraphicReferenceScenario stimulus = scenario;
				stimulus._initialD016 = (unsigned char) (8 | scroll);
				result &= compareGraphicReference (stimulus);
			}
		return (result);
	}

	/** Separates hblank writes from mid-content XSCROLL transitions. */
	bool testReferenceXScrollSampling ()
	{
		if (!VICIIGraphicReference::testIndependentAnchors ())
			return (false);
		bool result = true;
		const unsigned short cycles [] = { 10, 25, 54, 60 };
		const unsigned char transitions [3][2] = { { 6, 4 }, { 6, 2 }, { 2, 6 } };
		for (unsigned short cycle : cycles)
			for (const auto& transition : transitions)
				result &= compareGraphicReference
					({ "XSCROLL write cycle " + std::to_string (cycle) + " " +
						std::to_string (transition [0]) + "->" +
						std::to_string (transition [1]), 120, 200, 3, false,
						0x19, (unsigned char) (8 | transition [0]),
						{ { cycle, 0x16, (unsigned char) (8 | transition [1]) } } });
		return (result);
	}

	/** Separates text decoding from the real final border-composition path. */
	bool testReferenceMulticolorAndBorders ()
	{
		if (!VICIIGraphicReference::testMulticolorAndBorderAnchors ())
			return (false);
		bool result = true;
		const unsigned short cycles [] = { 10, 25, 54, 60 };
		const unsigned char transitions [3][2] = { { 6, 4 }, { 6, 2 }, { 2, 6 } };
		for (unsigned char columns = 0; columns < 2; columns++)
		{
			const unsigned char control = (unsigned char) (0x10 | (columns != 0 ? 8 : 0));
			for (unsigned char scroll = 0; scroll < 8; scroll++)
			{
				for (unsigned char colorMode = 0; colorMode < 3; colorMode++)
				{
					GraphicReferenceScenario scenario { "MCM normal color mode " +
						std::to_string (colorMode), 120, 200, 3, false, 0x19,
						(unsigned char) (control | scroll), {} };
					configureMulticolorReferenceScenario (scenario, colorMode);
					result &= compareGraphicReference (scenario);
				}
				GraphicReferenceScenario regular { "MCM regular bad line", 120, 200,
					3, false, 0x18, (unsigned char) (control | scroll), {} };
				configureMulticolorReferenceScenario (regular, 1);
				result &= compareGraphicReference (regular);
				GraphicReferenceScenario lateB { "LetsScrollitB timing MCM", 155, 320,
					7, true, 0x1c, (unsigned char) (control | scroll),
					{ { 21, 0x11, 0x1b } } };
				configureMulticolorReferenceScenario (lateB, 2);
				result &= compareGraphicReference (lateB);
				GraphicReferenceScenario lateA { "LetsScrollitA timing MCM", 53, 40,
					7, true, 0x1e, (unsigned char) (control | scroll),
					{ { 31, 0x11, 0x1d } } };
				configureMulticolorReferenceScenario (lateA, 2);
				result &= compareGraphicReference (lateA);
			}
			for (unsigned short cycle : cycles)
				for (const auto& transition : transitions)
				{
					GraphicReferenceScenario scenario { "MCM XSCROLL cycle " +
						std::to_string (cycle) + " " + std::to_string (transition [0]) +
						"->" + std::to_string (transition [1]), 120, 200, 3, false, 0x19,
						(unsigned char) (control | transition [0]),
						{ { cycle, 0x16, (unsigned char) (control | transition [1]) } } };
					configureMulticolorReferenceScenario (scenario, 2);
					result &= compareGraphicReference (scenario);
				}
		}
		return (result);
	}

	bool testReferenceD021ColorWrites ()
	{
		if (!VICIIGraphicReference::testColorResolutionAnchors ())
			return (false);
		bool result = true;
		const unsigned short writeCycles [] = { 10, 25, 34, 54 };
		for (unsigned char columns = 0; columns < 2; columns++)
			for (unsigned char scroll = 0; scroll < 8; scroll++)
				for (unsigned short writeCycle : writeCycles)
				{
					GraphicReferenceScenario scenario { "isolated D021 write cycle " +
						std::to_string (writeCycle), 120, 200, 3, false, 0x19,
						(unsigned char) (0x10 | (columns != 0 ? 8 : 0) | scroll),
						{ { writeCycle, 0x21, 13 } } };
					// A zero multicolor glyph selects only D021. Neither late matrix
					// fetches nor foreground classification can explain a discrepancy.
					scenario._backgroundColors = { { 3, 6, 14 } };
					scenario._capturedMemory = scenario._matrixSnapshot = true;
					scenario._initialMatrix.fill (0x20);
					scenario._initialColors.fill (8);
					scenario._memory = { { 0x1103, 0 } };
					scenario._compareComposition = true;
					scenario._compareForeground = false;
					result &= compareGraphicReference (scenario);
				}
		return (result);
	}

	/** Verifies the 10000Members graphics-source phase at $652f/$6537/$653f. \n
		Equal bitmap bytes cannot hide a wrong source. */
	bool test10000MembersGraphicSourcePhase ()
	{
		bool result = true;
		for (unsigned char scroll = 0; scroll < 8; scroll++)
		{
			GraphicReferenceScenario scenario
				{ "10000Members graphic source phase", 98, 165, 7, false,
				  0x3b, (unsigned char) (0x10 | scroll), {} };
			scenario._bank = 1;
			scenario._initialD018 = 0x08;
			scenario._capturedMemory = scenario._matrixSnapshot = true;
			scenario._bitmapReference = scenario._compareSourceIdentity = true;
			scenario._backgroundColors = { { 2, 5, 7 } };
			for (unsigned short index = 0; index < 40; index++)
			{
				scenario._initialMatrix [index] = (unsigned char) (0x20 + index);
				scenario._initialColors [index] = (unsigned char) (8 | (index & 7));
				const unsigned short address = (unsigned short)
					(0x6000 + ((165 + index) << 3) + 7);
				const unsigned char data = index == 0 ? 0x96 : index == 1 ? 0x69 :
					index == 2 ? 0x3c : (unsigned char) (0x81 + index * 37);
				scenario._memory.push_back ({ address, data });
			}
			result &= compareGraphicSourceReference (scenario);
		}
		return (result);
	}

	/** Reproduces the late line-51 badline and follows its persistent state to
		the 38-column right edge rendered on line 98. */
	bool test10000MembersLateBadLineRightEdge ()
	{
		const unsigned char scrolls [4] = { 5, 3, 1, 7 };
		const unsigned short d011WriteCycles [4] = { 48, 48, 48, 47 };
		bool result = true;
		for (unsigned short frame = 0; frame < 4; frame++)
		{
			GraphicReferenceScenario scenario
				{ "10000Members late badline right edge", 51, 0, 7, true,
				  0x7c, (unsigned char) (0x10 | scrolls [frame]),
				  { { d011WriteCycles [frame], 0x11, 0x7b } } };
			scenario._bank = 1;
			scenario._initialD018 = 0x08;
			scenario._capturedMemory = scenario._matrixSnapshot = true;
			scenario._bitmapReference = scenario._compareSourceIdentity = true;
			scenario._backgroundColors = { { 2, 5, 7 } };
			scenario._borderColor = 14;
			scenario._memory.reserve (0x2400);
			scenario._colorMemory.reserve (0x0400);
			for (unsigned short index = 0; index < 40; index++)
			{
				scenario._initialMatrix [index] = (unsigned char) (0x20 + index);
				scenario._initialColors [index] = (unsigned char) (8 | (index & 7));
			}
			for (unsigned int address = 0x4000; address < 0x4400; address++)
				scenario._memory.push_back
					({ (unsigned short) (address),
					   (unsigned char) (0x40 | ((address - 0x4000) & 0x3f)) });
			for (unsigned int address = 0x6000; address < 0x8000; address++)
				scenario._memory.push_back
					({ (unsigned short) (address),
					   (unsigned char) (0x81 + ((address - 0x6000) * 37)) });
			for (unsigned short vc = 0; vc < 1024; vc++)
				scenario._colorMemory.push_back
					({ vc, (unsigned char) (8 | (vc & 7)) });

			VICIIGraphicReference reference;
			reference.initialize (scenario);
			initializeGraphicReferenceStimulus (scenario);
			VICIIGraphicReference::Pixels actualSources {};
			std::array <bool, 504> actualBorders {};
			std::array <unsigned short, 256> sourceCycleByScreenCode {},
				sourceGraphicIndexByScreenCode {}, sourceMatrixIndexByScreenCode {};

			for (unsigned short line = 51; line <= 98; line++)
			{
				std::vector <GraphicReferenceWrite> writes;
				if (line == 51)
					writes = scenario._writes;
				else if (line == 58)
					writes = { { 59, 0x11, 0x3b } };
				if (line != 51)
					reference.beginNextLine (line, writes);
				if (line == 98)
				{
					sourceCycleByScreenCode = {};
					sourceGraphicIndexByScreenCode.fill (0xffff);
					sourceMatrixIndexByScreenCode.fill (0xffff);
				}

				for (unsigned short cycle = 1; cycle <= 63; cycle++)
				{
					reference.executeCycle (cycle);
					this -> _cycleInRasterLine = cycle;
					this -> treatBadLineStateAtCurrentCycle ();
					if (cycle == 14)
						this -> treatGraphicFetchStartCycle ();
					this -> updateBAPrefetchStateAtCurrentCycle ();
					const bool gAccess = this -> isGraphicAccessCycle ();
					const size_t gIndex = gAccess ? this -> graphicAccessIndex () : 0;
					const unsigned short matrixIndex = gAccess && !this -> idleStateActive ()
						? this -> _vicGraphicInfo._VLMI : 0xffff;
					this -> treatGraphicAccessCycle ();
					if (line == 98 && gAccess)
					{
						sourceCycleByScreenCode
							[this -> _vicGraphicInfo._screenCodeDrawData [gIndex].value ()] = cycle;
						sourceGraphicIndexByScreenCode
							[this -> _vicGraphicInfo._screenCodeDrawData [gIndex].value ()] =
								(unsigned short) gIndex;
						sourceMatrixIndexByScreenCode
							[this -> _vicGraphicInfo._screenCodeDrawData [gIndex].value ()] = matrixIndex;
					}
					this -> finishBadLineBusCycle ();
					if (cycle == 58)
						this -> treatGraphicRowEndCycle ();

					const bool visible = this -> _raster.isInVisibleZone ();
					unsigned short cv = 0, rv = 0;
					if (visible)
						this -> _raster.currentVisiblePosition (cv, rv);
					DrawContext context (this -> _raster.hData ().firstDisplayPosition (),
						cv, (unsigned short) ((cv >> 3) << 3), rv);
					if (visible)
					{
						this -> captureOutputState (context._beforeCPUWrite);
						this -> actualizeMainBorderStatus (context, 0, 4);
					}
					for (const GraphicReferenceWrite& write : writes)
						if (write._cycle == cycle)
							_registers.setRegister
								(write._register, MCHEmul::UByte (write._value));
					if (visible)
					{
						this -> captureOutputState (context._afterCPUWrite);
						this -> actualizeMainBorderStatus (context, 4, 8);
						const unsigned short x = context._RCA;
						for (size_t pixel = 0; pixel < 8; pixel++)
						{
							const bool active = this -> prepareGraphicOutputPixel
								(context.outputStateAtPixel (pixel)._horizontalScroll, pixel);
							if (line == 98)
							{
								GraphicReferencePixel& output = actualSources [x + pixel];
								output._sampled = true;
								actualBorders [x + pixel] = context.mainBorderAtPixel (pixel);
								if (active)
								{
									auto& graphicOutput =
										this -> _vicGraphicInfo._graphicOutput;
									output._screenCode =
										graphicOutput._screenCode.value ();
									output._sourceCycle =
										sourceCycleByScreenCode [output._screenCode];
									output._graphicIndex =
										sourceGraphicIndexByScreenCode [output._screenCode];
									output._matrixIndex =
										sourceMatrixIndexByScreenCode [output._screenCode];
									const unsigned char pair = (unsigned char)
										((graphicOutput._data.value () >>
										 (2 * (3 - (graphicOutput._pixel >> 1)))) & 0x03);
									output._foreground = (pair & 0x02) != 0;
									output._color = pair == 0
										? (unsigned char) context.outputStateAtPixel
											(pixel)._backgroundColors [0]
										: (pair == 1
											? (unsigned char) (graphicOutput._screenCode.value () >> 4)
											: (pair == 2
												? (unsigned char) (graphicOutput._screenCode.value () & 0x0f)
												: (unsigned char) (graphicOutput._colorData.value () & 0x0f)));
								}
							}
							this -> advanceGraphicOutputPixel ();
						}
					}
					this -> commitGraphicOutputData ();
					this -> advanceRasterPosition ();
				}
			}

			const VICIIGraphicReference::Pixels& expected = reference.outputPixels ();
			bool coverage = true, borderMatch = true, sourceMatch = true,
				pixelMatch = true, phaseSensitive = false;
			unsigned short firstDifferentX = 0;
			for (unsigned short x = 320; x < 360; x++)
			{
				const bool sampled = actualSources [x]._sampled && expected [x]._sampled;
				const bool border = sampled && actualBorders [x] == expected [x]._border;
				const bool source = !sampled || expected [x]._border ||
					(actualSources [x]._screenCode == expected [x]._screenCode &&
					 actualSources [x]._sourceCycle == expected [x]._sourceCycle &&
					 actualSources [x]._graphicIndex == expected [x]._graphicIndex &&
					 actualSources [x]._matrixIndex == expected [x]._matrixIndex);
				const bool pixel = !sampled || expected [x]._border ||
					(actualSources [x]._foreground == expected [x]._foreground &&
					 actualSources [x]._color == expected [x]._color);
				if ((!sampled || !border || !source || !pixel) && firstDifferentX == 0)
					firstDifferentX = x;
				coverage &= sampled;
				borderMatch &= border;
				sourceMatch &= source;
				pixelMatch &= pixel;
				if (x >= 328 && !expected [x]._border && expected [x - 8]._sampled)
					phaseSensitive |= expected [x]._screenCode != expected [x - 8]._screenCode ||
						expected [x]._sourceCycle != expected [x - 8]._sourceCycle;
			}
			const bool counters = this -> _vicGraphicInfo._VCBASE ==
				reference.videoCounterBase ();
			const bool frameResult = coverage && borderMatch && sourceMatch && pixelMatch &&
				phaseSensitive && counters && !reference.memoryIncomplete ();
			std::cout << "10000Members late badline right edge | XSCROLL "
				<< (unsigned int) scrolls [frame] << " | D011 write "
				<< d011WriteCycles [frame] << " | VCBASE actual/reference "
				<< this -> _vicGraphicInfo._VCBASE << '/' << reference.videoCounterBase ()
				<< " | coverage " << (coverage ? "OK" : "ERROR")
				<< " | border " << (borderMatch ? "OK" : "ERROR")
				<< " | sources " << (sourceMatch ? "OK" : "ERROR")
				<< " | pixels " << (pixelMatch ? "OK" : "ERROR")
				<< " | phase mutant " << (phaseSensitive ? "REJECTED" : "INEFFECTIVE")
				<< " | RAM " << (!reference.memoryIncomplete () ? "COMPLETE" : "INCOMPLETE");
			if (!frameResult && firstDifferentX != 0)
				std::cout << " | first X " << firstDifferentX
					<< " | border actual/reference " << actualBorders [firstDifferentX]
					<< '/' << expected [firstDifferentX]._border
					<< " | source cycle/code actual "
					<< actualSources [firstDifferentX]._sourceCycle << '/'
					<< (unsigned int) actualSources [firstDifferentX]._screenCode
					<< " reference " << expected [firstDifferentX]._sourceCycle << '/'
					<< (unsigned int) expected [firstDifferentX]._screenCode
					<< " | G/M actual " << actualSources [firstDifferentX]._graphicIndex
					<< '/' << actualSources [firstDifferentX]._matrixIndex
					<< " reference " << expected [firstDifferentX]._graphicIndex
					<< '/' << expected [firstDifferentX]._matrixIndex
					<< " | foreground/color actual "
					<< actualSources [firstDifferentX]._foreground << '/'
					<< (unsigned int) actualSources [firstDifferentX]._color
					<< " reference " << expected [firstDifferentX]._foreground << '/'
					<< (unsigned int) expected [firstDifferentX]._color;
			std::cout << std::endl;
			result &= frameResult;
		}

		this -> resetBadLineStateForNewRasterLine ();
		this -> enterIdleState ();
		this -> resetGraphicAccessCountersForCurrentLine ();
		this -> _cycleInRasterLine = 1;
		this -> selectCPUStopWindowsForCurrentAndNextLine ();
		return (result);
	}

	/** Replays the complete right-edge fine/coarse-scroll sequence observed in
		10000MEMBERS1.log and compares final framebuffer pixels against VICE's
		independent dmli plus pipe0/pipe1 composition. */
	bool test10000MembersRightEdgeCompositionSequence ()
	{
		const unsigned short videoCounterBases [18] =
			{ 166, 166, 166, 167, 167, 167, 167, 168, 168,
			  168, 168, 169, 169, 169, 170, 170, 170, 171 };
		const unsigned char scrolls [18] =
			{ 5, 3, 1, 7, 5, 3, 1, 7, 4, 2, 0, 5, 3, 0, 6, 3, 1, 6 };
		const unsigned char incomingData [6] =
			{ 0x00, 0x3a, 0xaa, 0xff, 0xaa, 0xac };
		const unsigned char incomingScreen [6] =
			{ 0x00, 0x05, 0x05, 0x0b, 0x05, 0x05 };
		const unsigned char incomingColor [6] =
			{ 0x00, 0x0b, 0x0b, 0x05, 0x0b, 0x0b };
		const unsigned char activeColor [6] =
			{ 0x00, 0x00, 0x00, 0x0b, 0x05, 0x05 };
		bool result = true, phaseMutantRejected = false;

		for (unsigned short frame = 0; frame < 18; frame++)
		{
			const unsigned short vcbase = videoCounterBases [frame];
			const unsigned short group = vcbase - videoCounterBases [0];
			GraphicReferenceScenario scenario
				{ "10000Members captured right-edge composition", 98, vcbase, 7, false,
				  0x3b, (unsigned char) (0x10 | scrolls [frame]), {} };
			scenario._bank = 1;
			scenario._initialD018 = 0x08;
			scenario._capturedMemory = scenario._matrixSnapshot = true;
			scenario._bitmapReference = scenario._compareComposition = true;
			scenario._backgroundColors = { { 2, 5, 7 } };
			scenario._borderColor = 14;
			scenario._memory.reserve (40);
			for (unsigned short index = 0; index < 40; index++)
			{
				scenario._initialMatrix [index] =
					(unsigned char) (0x40 | (index & 0x3f));
				scenario._initialColors [index] =
					(unsigned char) (8 | (index & 7));
				const unsigned short address = (unsigned short)
					(0x6000 + ((vcbase + index) << 3) + 7);
				scenario._memory.push_back
					({ address, (unsigned char) (0x81 + index * 37) });
			}

			// The two latches reaching ScreenMemory X=344 are copied literally from
			// the log. Neighbouring cells remain unique so an eight-dot phase
			// error cannot pass because two bitmap bytes happen to be equal.
			scenario._initialMatrix [37] = 0;
			scenario._initialColors [37] = activeColor [group];
			scenario._initialMatrix [38] = incomingScreen [group];
			scenario._initialColors [38] = incomingColor [group];
			scenario._memory [37]._value = 0;
			scenario._memory [38]._value = incomingData [group];

			VICIIGraphicReference reference;
			reference.initialize (scenario);
			for (unsigned short cycle = 1; cycle <= 63; cycle++)
				reference.executeCycle (cycle);
			bool frameRejectsMutant = false;
			for (unsigned short x = 344; x < 351; x++)
			{
				const GraphicReferencePixel& expected = reference.outputPixels () [x];
				const GraphicReferencePixel& shifted = reference.outputPixels () [x - 8];
				frameRejectsMutant |= expected._sampled && shifted._sampled &&
					(expected._border != shifted._border ||
					 expected._foreground != shifted._foreground ||
					 expected._composedColor != shifted._composedColor ||
					 expected._screenCode != shifted._screenCode);
			}
			phaseMutantRejected |= frameRejectsMutant;
			const bool frameResult = compareGraphicReference (scenario);
			std::cout << "10000Members right-edge sequence | frame " << frame + 1
				<< " | VCBASE " << vcbase
				<< " | XSCROLL " << (unsigned int) scrolls [frame]
				<< " | eight-dot mutant "
				<< (frameRejectsMutant ? "REJECTED" : "NOT OBSERVABLE")
				<< " | " << (frameResult ? "OK" : "ERROR") << std::endl;
			result &= frameResult;
		}

		std::cout << "10000Members right-edge sequence summary"
			<< " | frames 18"
			<< " | eight-dot mutant "
			<< (phaseMutantRejected ? "REJECTED" : "INEFFECTIVE")
			<< " | " << (result && phaseMutantRejected ? "OK" : "ERROR")
			<< std::endl;
		return (result && phaseMutantRejected);
	}

	unsigned short currentRasterLine () const
							{ return (this -> _raster.currentLine ()); }

	bool testLetsScrollitALineReplay ()
							{ return (compareDemoLineReplay (false)); }
	bool testLetsScrollitBLineReplay ()
							{ return (compareDemoLineReplay (true)); }
	bool test10000MembersLineReplay ()
	{
		bool result = true;
		for (unsigned short frame = 0; frame < 3; ++frame)
			result &= compareDemoLineReplay (VICIIGraphicReference::membersLineReplay (frame));
		return (result);
	}

	private:
	void configureMulticolorReferenceScenario
		(GraphicReferenceScenario& scenario, unsigned char colorMode)
	{
		scenario._initialColorMode = colorMode;
		scenario._backgroundColors = { { 2, 5, 7 } };
		scenario._borderColor = 14;
		scenario._compareComposition = true;
	}

	/** Compares the source identity at every physical pixel. \n
		The production renderer and its coordinates remain untouched. */
	bool compareGraphicSourceReference (const GraphicReferenceScenario& scenario)
	{
		VICIIGraphicReference reference;
		reference.initialize (scenario);
		initializeGraphicReferenceStimulus (scenario);
		VICIIGraphicReference::Pixels actualSources {};
		std::array <unsigned short, 256> sourceCycleByScreenCode {},
			sourceGraphicIndexByScreenCode {}, sourceMatrixIndexByScreenCode {};
		sourceGraphicIndexByScreenCode.fill (0xffff);
		sourceMatrixIndexByScreenCode.fill (0xffff);

		for (unsigned short cycle = 1; cycle <= 63; cycle++)
		{
			reference.executeCycle (cycle);
			this -> _cycleInRasterLine = cycle;
			this -> treatBadLineStateAtCurrentCycle ();
			if (cycle == 14)
				this -> treatGraphicFetchStartCycle ();
			this -> updateBAPrefetchStateAtCurrentCycle ();
			const bool gAccess = this -> isGraphicAccessCycle ();
			const size_t gIndex = gAccess ? this -> graphicAccessIndex () : 0;
			const unsigned short matrixIndex = gAccess && !this -> idleStateActive ()
				? this -> _vicGraphicInfo._VLMI : 0xffff;
			this -> treatGraphicAccessCycle ();
			if (gAccess)
			{
				sourceCycleByScreenCode
					[this -> _vicGraphicInfo._screenCodeDrawData [gIndex].value ()] = cycle;
				sourceGraphicIndexByScreenCode
					[this -> _vicGraphicInfo._screenCodeDrawData [gIndex].value ()] =
						(unsigned short) gIndex;
				sourceMatrixIndexByScreenCode
					[this -> _vicGraphicInfo._screenCodeDrawData [gIndex].value ()] = matrixIndex;
			}
			this -> finishBadLineBusCycle ();
			if (cycle == 58)
				this -> treatGraphicRowEndCycle ();

			const bool visible = this -> _raster.isInVisibleZone ();
			unsigned short cv = 0, rv = 0;
			if (visible)
				this -> _raster.currentVisiblePosition (cv, rv);
			if (visible)
			{
				const unsigned short x = (unsigned short) ((cv >> 3) << 3);
				for (size_t pixel = 0; pixel < 8; pixel++)
				{
					GraphicReferencePixel& output = actualSources [x + pixel];
					output._sampled = true;
					if (this -> prepareGraphicOutputPixel
						((unsigned char) (scenario._initialD016 & 7), pixel))
					{
						output._screenCode =
							this -> _vicGraphicInfo._graphicOutput._screenCode.value ();
						output._sourceCycle = sourceCycleByScreenCode [output._screenCode];
						output._graphicIndex =
							sourceGraphicIndexByScreenCode [output._screenCode];
						output._matrixIndex =
							sourceMatrixIndexByScreenCode [output._screenCode];
					}
					this -> advanceGraphicOutputPixel ();
				}
			}
			this -> commitGraphicOutputData ();
			this -> _raster.hData ().add (8);
		}

		const VICIIGraphicReference::Pixels& expected = reference.outputPixels ();
		VICIIGraphicReference::Pixels observed = expected;
		bool coverageComplete = true;
		for (unsigned short x = 32; x < 352; x++)
			coverageComplete &= actualSources [x]._sampled;
		for (unsigned short x = 0; x < observed.size (); x++)
			if (actualSources [x]._sampled)
			{
				observed [x]._sampled = true;
				observed [x]._screenCode = actualSources [x]._screenCode;
				observed [x]._sourceCycle = actualSources [x]._sourceCycle;
				observed [x]._graphicIndex = actualSources [x]._graphicIndex;
				observed [x]._matrixIndex = actualSources [x]._matrixIndex;
			}
		unsigned short firstDifferentX = 0;
		const bool sourcesMatch = coverageComplete && VICIIGraphicReference::pixelsMatch
			(observed, expected, firstDifferentX, false, true);

		// These mutants prove that the comparator rejects both the current-cycle
		// source and a whole-character displacement in visible coordinates.
		VICIIGraphicReference::Pixels onePhase = expected, shifted = expected;
		for (unsigned short x = 32; x < 344; x++)
		{
			onePhase [x]._screenCode = expected [x + 8]._screenCode;
			onePhase [x]._sourceCycle = expected [x + 8]._sourceCycle;
			onePhase [x]._graphicIndex = expected [x + 8]._graphicIndex;
			onePhase [x]._matrixIndex = expected [x + 8]._matrixIndex;
		}
		for (unsigned short x = 40; x < 352; x++)
		{
			shifted [x]._screenCode = expected [x - 8]._screenCode;
			shifted [x]._sourceCycle = expected [x - 8]._sourceCycle;
			shifted [x]._graphicIndex = expected [x - 8]._graphicIndex;
			shifted [x]._matrixIndex = expected [x - 8]._matrixIndex;
		}
		unsigned short mutantDifference = 0;
		const bool mutantsRejected =
			!VICIIGraphicReference::pixelsMatch
				(onePhase, expected, mutantDifference, false, true) &&
			!VICIIGraphicReference::pixelsMatch
				(shifted, expected, mutantDifference, false, true);

		std::cout << "10000Members graphics source phase | XSCROLL "
			<< (unsigned int) (scenario._initialD016 & 7)
			<< " | coverage " << (coverageComplete ? "OK" : "ERROR")
			<< " | sources " << (sourcesMatch ? "OK" : "ERROR")
			<< " | mutants " << (mutantsRejected ? "REJECTED" : "ERROR");
		if (!sourcesMatch)
			std::cout << " | first X " << firstDifferentX
				<< " | actual cycle/code " << observed [firstDifferentX]._sourceCycle << '/'
				<< (unsigned int) observed [firstDifferentX]._screenCode
				<< " expected " << expected [firstDifferentX]._sourceCycle << '/'
				<< (unsigned int) expected [firstDifferentX]._screenCode;
		std::cout << std::endl;

		this -> resetBadLineStateForNewRasterLine ();
		this -> enterIdleState ();
		this -> resetGraphicAccessCountersForCurrentLine ();
		this -> _cycleInRasterLine = 1;
		this -> selectCPUStopWindowsForCurrentAndNextLine ();
		return (sourcesMatch && mutantsRejected);
	}

	/** Only stimulus generation is shared. The expected fetch/display state
		is owned by VICIIGraphicReference and never reads production buffers. */
	void initializeGraphicReferenceStimulus (const GraphicReferenceScenario& scenario)
	{
		// Only these RAM regions are reachable in this restricted fixture.
		for (unsigned int address = 0x0400; address < 0x0800; address++)
			_testMemory.set (MCHEmul::Address (2, address),
				MCHEmul::UByte (VICIIGraphicReference::memoryByte
					((unsigned short) (address))), true);
		for (unsigned int address = 0x1000; address < 0x1800; address++)
			_testMemory.set (MCHEmul::Address (2, address),
				MCHEmul::UByte (VICIIGraphicReference::memoryByte
					((unsigned short) (address))), true);
		_testMemory.set (MCHEmul::Address (2, 0x3fff),
			MCHEmul::UByte (VICIIGraphicReference::memoryByte (0x3fff)), true);
		for (unsigned short vc = 0; vc < 1024; vc++)
			_colorRAM.put (MCHEmul::Address (2, 0xd800 + vc),
				MCHEmul::UByte (VICIIGraphicReference::colorByte (vc)));
		if (scenario._capturedMemory)
		{
			// Poison uncaptured addresses. A wrong fetch must not accidentally
			// obtain a plausible zero from the synthetic memory used by old tests.
			for (unsigned int address = 0; address < 0x10000; address++)
				_testMemory.set (MCHEmul::Address (2, address), MCHEmul::UByte (0x5a), true);
			for (const GraphicReferenceMemoryByte& byte : scenario._memory)
				_testMemory.set (MCHEmul::Address (2, byte._address), MCHEmul::UByte (byte._value), true);
			for (unsigned short vc = 0; vc < 1024; vc++)
				_colorRAM.put (MCHEmul::Address (2, 0xd800 + vc), MCHEmul::UByte (0x0d));
			for (const GraphicReferenceMemoryByte& byte : scenario._colorMemory)
				_colorRAM.put (MCHEmul::Address (2, 0xd800 + byte._address), MCHEmul::UByte (byte._value));
		}

		_registers.initialize ();
		_registers.setBank (scenario._bank);
		_registers.setRegister (0x18, MCHEmul::UByte (scenario._initialD018));
		_registers.setRegister (0x11, MCHEmul::UByte (scenario._initialD011));
		_registers.setRegister (0x16, MCHEmul::UByte (scenario._initialD016));
		_registers.setRegister (0x20, MCHEmul::UByte (scenario._borderColor));
		for (unsigned char i = 0; i < 3; i++)
			_registers.setRegister (0x21 + i, MCHEmul::UByte (scenario._backgroundColors [i]));
		this -> _raster.initialize ();
		while (this -> _raster.currentLine () != scenario._rasterLine)
			this -> _raster.vData ().next ();
		this -> _raster.hData ().reset ();
		this -> resetBadLineStateForNewRasterLine ();
		this -> resetGraphicAccessCountersForCurrentLine ();
		this -> _DENSeenAtLine30 = true;
		this -> _vicGraphicInfo._ROW = scenario._rasterLine;
		this -> _vicGraphicInfo._VCBASE = this -> _vicGraphicInfo._VC =
			scenario._initialVC;
		this -> _vicGraphicInfo._RC = scenario._initialRC;
		this -> _vicGraphicInfo._ffVBorder = false;
		this -> _vicGraphicInfo._ffMBorder = true;
		this -> _currentSpriteDMAMask = this -> _nextSpriteDMAMask = 0;
		this -> _displayActiveSpritesMask = this -> _drawingSpritesMask = 0;
		this -> _BAPrefetchCycles = 4;
		this -> _cpuOpcodeLowNibble = scenario._initialCpuNibble;
		if (scenario._initialIdle)
			this -> enterIdleState ();
		else
			this -> enterScreenState ();
		for (unsigned short i = 0; i < 40; i++)
		{
			this -> _vicGraphicInfo._screenCodeData [i] = MCHEmul::UByte
				(scenario._matrixSnapshot ? scenario._initialMatrix [i] :
					VICIIGraphicReference::initialScreenCode (i));
			this -> _vicGraphicInfo._colorData [i] = MCHEmul::UByte
				(scenario._matrixSnapshot ? scenario._initialColors [i] :
					VICIIGraphicReference::stimulusInitialColor (scenario, i));
		}
		this -> _cycleInRasterLine = 1;
		this -> selectCPUStopWindowsForCurrentAndNextLine ();
	}

	bool compareGraphicReference (const GraphicReferenceScenario& scenario)
	{
		VICIIGraphicReference reference;
		reference.initialize (scenario);
		initializeGraphicReferenceStimulus (scenario);
		VICIIGraphicReference::Pixels actualPixels {};
		VICIIGraphicReference::Fetches actualFetches {};

		for (unsigned short cycle = 1; cycle <= 63; cycle++)
		{
			reference.executeCycle (cycle);
			this -> _cycleInRasterLine = cycle;
			// Exercise the real bus and drawing helpers in their current production
			// order. This is a component harness, not CPU/IRQ/sprite integration.
			this -> treatBadLineStateAtCurrentCycle ();
			if (cycle == 14)
				this -> treatGraphicFetchStartCycle ();
			this -> updateBAPrefetchStateAtCurrentCycle ();
			GraphicReferenceFetch& observation = actualFetches [cycle];
			observation._gAccess = this -> isGraphicAccessCycle ();
			observation._cAccess = this -> isBadLineCAccessCycle ();
			observation._invalidC = observation._cAccess &&
				!this -> cAccessDataValidAtCurrentCycle ();
			CPUStopWindow window;
			observation._baLow = this -> CPUStopWindowAt
				((CPURasterCycle) cycle, *this -> _currentCPUStopWindows,
				 *this -> _nextCPUStopWindows, window);
			const size_t gIndex = observation._gAccess
				? this -> graphicAccessIndex () : 0;
			this -> treatGraphicAccessCycle ();
			if (observation._gAccess)
			{
				observation._gData = this -> _vicGraphicInfo._graphicData [gIndex].value ();
				observation._gScreen = this -> _vicGraphicInfo._screenCodeDrawData [gIndex].value ();
				observation._gColor = this -> _vicGraphicInfo._colorDrawData [gIndex].value ();
			}
			if (observation._cAccess)
			{
				observation._cIndex = this -> _vicGraphicInfo._VLMI;
				observation._cScreen = this -> _vicGraphicInfo._screenCodeData
					[observation._cIndex].value ();
				observation._cColor = this -> _vicGraphicInfo._colorData
					[observation._cIndex].value ();
			}
			this -> finishBadLineBusCycle ();
			if (cycle == 58)
				this -> treatGraphicRowEndCycle ();
			observation._vc = this -> _vicGraphicInfo._VC;
			observation._vlmi = this -> _vicGraphicInfo._VLMI;
			observation._prefetch = this -> _BAPrefetchCycles;

			// Obtain the same real coordinates as simulateRasterCycle(), not a
			// synthetic (cycle-16)*8 context that presupposes the output delay.
			const bool visible = this -> _raster.isInVisibleZone ();
			unsigned short cv = 0, rv = 0;
			if (visible)
				this -> _raster.currentVisiblePosition (cv, rv);
			DrawContext context (this -> _raster.hData ().firstDisplayPosition (),
				cv, (unsigned short) ((cv >> 3) << 3), rv);
			if (visible)
				this -> captureOutputState (context._beforeCPUWrite);
			for (const GraphicReferenceWrite& write : scenario._writes)
				if (write._cycle == cycle)
					_registers.setRegister (write._register, MCHEmul::UByte (write._value));
			if (visible)
			{
				this -> captureOutputState (context._afterCPUWrite);
				// PAL's actual RC-RCA is four dots: phi2 starts beyond this aligned
				// eight-dot group. The next group captures the changed register state.
				assert (context.firstOutputPixelAfterCPUWrite () == 8);
				DrawResult drawn;
				this -> drawGraphics (context, context._beforeCPUWrite, 0, 8, drawn);
				const unsigned short x = context._RCA;
				for (unsigned short pixel = 0; pixel < 8; pixel++)
				{
					GraphicReferencePixel& output = actualPixels [x + pixel];
					output._sampled = true;
					output._foreground = drawn._collisionGraphicData.bit (7 - pixel);
					// Color and priority are independent: MC pair 01 uses D022 but
					// remains background. Hires-in-MCM also needs both result arrays.
					output._color = (unsigned char)
						(drawn._foregroundColorData [pixel] != MCHEmul::_U0
							? drawn._foregroundColorData [pixel]
							: drawn._backgroundColorData [pixel] != MCHEmul::_U0
								? drawn._backgroundColorData [pixel] : context._beforeCPUWrite._backgroundColors [0]);
				}
			}
			this -> commitGraphicOutputData ();
			this -> _raster.hData ().add (8);
		}

		bool fetchesMatch = true;
		unsigned short firstFetchDifference = 0;
		for (unsigned short cycle = 1; cycle <= 63; cycle++)
		{
			const GraphicReferenceFetch& a = actualFetches [cycle];
			const GraphicReferenceFetch& e = reference.fetches () [cycle];
			const bool match = a._gAccess == e._gAccess && a._cAccess == e._cAccess &&
				a._baLow == e._baLow && a._prefetch == e._prefetch &&
				a._vc == e._vc && a._vlmi == e._vlmi &&
				(!e._gAccess || (a._gData == e._gData &&
					a._gScreen == e._gScreen && a._gColor == e._gColor)) &&
				(!e._cAccess || (a._invalidC == e._invalidC &&
					a._cIndex == e._cIndex && a._cScreen == e._cScreen &&
					a._cColor == e._cColor));
			if (!match && firstFetchDifference == 0)
				firstFetchDifference = cycle;
			fetchesMatch &= match;
		}
		unsigned short firstDifferentX = 0;
		const bool outputMatch = VICIIGraphicReference::pixelsMatch
			(actualPixels, reference.outputPixels (), firstDifferentX, scenario._compareForeground);
		std::cout << "PAL independent text | " << scenario._name
			<< " | MCM " << ((scenario._initialD016 & 0x10) != 0)
			<< " | columns " << ((scenario._initialD016 & 8) != 0 ? 40 : 38)
			<< " | XSCROLL " << (unsigned int) (scenario._initialD016 & 7)
			<< " | fetches " << (fetchesMatch ? "OK" : "ERROR")
			<< " | pixels " << (outputMatch ? "OK" : "ERROR");
		if (!fetchesMatch)
		{
			const GraphicReferenceFetch& a = actualFetches [firstFetchDifference];
			const GraphicReferenceFetch& e = reference.fetches () [firstFetchDifference];
			std::cout << " | first fetch cycle " << firstFetchDifference
				<< " | VC/VLMI actual " << a._vc << '/' << a._vlmi
				<< " expected " << e._vc << '/' << e._vlmi
				<< " | g data/code/color actual " << (unsigned int) a._gData << '/'
				<< (unsigned int) a._gScreen << '/' << (unsigned int) a._gColor
				<< " expected " << (unsigned int) e._gData << '/'
				<< (unsigned int) e._gScreen << '/' << (unsigned int) e._gColor
				<< " | c index/code/color/invalid actual " << a._cIndex << '/'
				<< (unsigned int) a._cScreen << '/' << (unsigned int) a._cColor << '/'
				<< a._invalidC << " expected " << e._cIndex << '/'
				<< (unsigned int) e._cScreen << '/' << (unsigned int) e._cColor << '/'
				<< e._invalidC << " | BA/prefetch actual " << a._baLow << '/'
				<< (unsigned int) a._prefetch << " expected " << e._baLow << '/'
				<< (unsigned int) e._prefetch;
		}
		if (!outputMatch)
		{
			const GraphicReferencePixel& a = actualPixels [firstDifferentX];
			const GraphicReferencePixel& e = reference.outputPixels () [firstDifferentX];
			std::cout << " | first raster line/X " << scenario._rasterLine << '/'
				<< firstDifferentX << " | foreground actual/expected "
				<< a._foreground << '/' << e._foreground << " | color actual/expected "
				<< (unsigned int) a._color << '/' << (unsigned int) e._color
				<< " | reference source g-cycle/code " << e._sourceCycle << '/'
				<< (unsigned int) e._screenCode;
		}
		std::cout << std::endl;
		this -> resetBadLineStateForNewRasterLine ();
		this -> enterIdleState ();
		this -> resetGraphicAccessCountersForCurrentLine ();
		this -> _cycleInRasterLine = 1;
		this -> selectCPUStopWindowsForCurrentAndNextLine ();
		const bool compositionMatch = !scenario._compareComposition ||
			compareGraphicReferenceComposition (scenario, reference);
		return (fetchesMatch && outputMatch && compositionMatch);
	}

	bool compareGraphicReferenceComposition
		(const GraphicReferenceScenario& scenario, const VICIIGraphicReference& reference)
	{
		if (this -> screenMemory () == nullptr && !MCHEmul::GraphicalChip::initialize ())
		{
			std::cout << "PAL reference composition | ERROR initializing screen" << std::endl;
			return (false);
		}
		initializeGraphicReferenceStimulus (scenario);
		MCHEmul::ScreenMemory* screen = this -> screenMemory ();
		std::array <unsigned int, 16> palette {};
		for (unsigned char color = 0; color < 16; color++)
		{
			screen -> setPixel (color, 0, color);
			palette [color] = screen -> frameData () [color];
		}
		std::array <bool, 504> sampled {}, borders {};
		std::array <unsigned int, 504> actual {};
		for (unsigned short cycle = 1; cycle <= 63; cycle++)
		{
			this -> _cycleInRasterLine = cycle;
			this -> treatBadLineStateAtCurrentCycle ();
			if (cycle == 14)
				this -> treatGraphicFetchStartCycle ();
			this -> updateBAPrefetchStateAtCurrentCycle ();
			this -> treatGraphicAccessCycle ();
			this -> finishBadLineBusCycle ();
			if (cycle == 58)
				this -> treatGraphicRowEndCycle ();
			const bool visible = this -> _raster.isInVisibleZone ();
			unsigned short cv = 0, rv = 0;
			if (visible)
				this -> _raster.currentVisiblePosition (cv, rv);
			DrawContext context (this -> _raster.hData ().firstDisplayPosition (),
				cv, (unsigned short) ((cv >> 3) << 3), rv);
			if (visible)
			{
				this -> captureOutputState (context._beforeCPUWrite);
				this -> actualizeMainBorderStatus (context, 0, 4);
			}
			for (const GraphicReferenceWrite& write : scenario._writes)
				if (write._cycle == cycle)
					_registers.setRegister (write._register, MCHEmul::UByte (write._value));
			if (visible)
			{
				this -> captureOutputState (context._afterCPUWrite);
				this -> actualizeMainBorderStatus (context, 4, 8);
				// Exercise the real full/mixed/empty-border rendering paths, not
				// an overlay assembled from the test's graphic comparison.
				this -> drawVisibleZone (nullptr, context);
				const unsigned short x = context._RCA;
				for (unsigned short pixel = 0; pixel < 8; pixel++)
				{
					sampled [x + pixel] = true;
					borders [x + pixel] = context.mainBorderAtPixel (pixel);
					actual [x + pixel] = screen -> frameData ()
						[(size_t) rv * screen -> columns () + context._RCA + pixel];
				}
			}
			this -> commitGraphicOutputData ();
			this -> _raster.hData ().add (8);
		}
		bool result = true;
		unsigned short firstDifferentX = 0;
		for (unsigned short x = 24; x < 360; x++)
		{
			const GraphicReferencePixel& expected = reference.outputPixels () [x];
			const bool match = sampled [x] && expected._sampled &&
				borders [x] == expected._border && actual [x] == palette [expected._composedColor];
			if (!match && result)
				firstDifferentX = x;
			result &= match;
		}
		std::cout << "PAL final composition | " << scenario._name
			<< " | columns " << ((scenario._initialD016 & 8) != 0 ? 40 : 38)
			<< " | XSCROLL " << (unsigned int) (scenario._initialD016 & 7)
			<< " | " << (result ? "OK" : "ERROR");
		if (!result)
		{
			const GraphicReferencePixel& expected = reference.outputPixels () [firstDifferentX];
			std::cout << " | first raster line/X " << scenario._rasterLine << '/' << firstDifferentX
				<< " | border actual/expected " << borders [firstDifferentX] << '/' << expected._border
				<< " | packed color actual/expected " << actual [firstDifferentX] << '/'
				<< palette [expected._composedColor] << " | expected C64 color "
				<< (unsigned int) expected._composedColor
				<< " | reference source g-cycle/code " << expected._sourceCycle << '/'
				<< (unsigned int) expected._screenCode;
		}
		std::cout << std::endl;
		this -> resetBadLineStateForNewRasterLine ();
		this -> enterIdleState ();
		this -> resetGraphicAccessCountersForCurrentLine ();
		this -> _cycleInRasterLine = 1;
		this -> selectCPUStopWindowsForCurrentAndNextLine ();
		return (result);
	}

	std::vector <GraphicReferenceWrite> replayWritesForLine
		(const GraphicReplayFixture& fixture, unsigned short line) const
	{
		std::vector <GraphicReferenceWrite> writes;
		for (const GraphicReplayCycle& cycle : fixture._cycles)
			if (cycle._line == line)
				writes.insert (writes.end (), cycle._writes.begin (), cycle._writes.end ());
		return (writes);
	}

	bool replayFetchesMatch (const GraphicReferenceFetch& actual,
		const GraphicReferenceFetch& expected) const
	{
		return (actual._gAccess == expected._gAccess && actual._cAccess == expected._cAccess &&
			actual._baLow == expected._baLow && actual._prefetch == expected._prefetch &&
			actual._vc == expected._vc && actual._vlmi == expected._vlmi &&
			(!expected._gAccess || (actual._gAddress == expected._gAddress &&
				actual._gData == expected._gData && actual._gScreen == expected._gScreen &&
				actual._gColor == expected._gColor)) &&
			(!expected._cAccess || (actual._invalidC == expected._invalidC &&
				actual._cIndex == expected._cIndex && actual._cScreen == expected._cScreen &&
				actual._cColor == expected._cColor)));
	}

	bool compareDemoLineReplay (bool demoB)
							{ return (compareDemoLineReplay (VICIIGraphicReference::demoLineReplay (demoB))); }

	bool compareDemoLineReplay (GraphicReplayFixture fixture)
	{
		fixture._scenario._writes = replayWritesForLine (fixture, fixture._scenario._rasterLine);
		if (this -> screenMemory () == nullptr && !MCHEmul::GraphicalChip::initialize ())
			return (false);
		MCHEmul::ScreenMemory* screen = this -> screenMemory ();
		std::array <unsigned int, 16> palette {};
		for (unsigned char color = 0; color < 16; color++)
		{
			screen -> setPixel (color, 0, color);
			palette [color] = screen -> frameData () [color];
		}
		VICIITestRAM* ram = static_cast <VICIITestRAM*> (_testMemory.subset (0));
		bool result = true;
		bool bitmapShiftRejected = false;
		unsigned int bitmapEdgeSamples = 0;
		std::cout << "PAL demo replay | " << fixture._scenario._name
			<< " | capture " << (fixture._capturedBackgrounds && fixture._capturedBorder ? "COMPLETE" : "INCOMPLETE")
			<< " | backgrounds " << (fixture._capturedBackgrounds ? "captured" : "controlled 0/0/0")
			<< " | border controlled 0 | foreground NOT compared" << std::endl;

		// Two executions avoid advancing the graphic sequencer twice: first
		// observe underlying text, then exercise actual final composition.
		for (unsigned char composition = 0; composition < 2; composition++)
		{
			VICIIGraphicReference reference;
			reference.initialize (fixture._scenario);
			initializeGraphicReferenceStimulus (fixture._scenario);
			for (size_t sprite = 0; sprite < 8; sprite++)
				this -> _vicSpriteInfo [sprite] = typename VICIIType::VICSpriteInfo ();
			this -> _spriteDMAStateMask = 0;
			this -> _pendingCPUTransaction = typename VICIIType::PendingCPUTransaction ();
			std::array <bool, 504> sampled {}, borders {};
			std::array <unsigned int, 504> colors {};
			bool logMatch = true, busMatch = true, knownMemory = true;
			unsigned int firstBusDifference = 0, firstLogDifference = 0;
			for (const GraphicReplayCycle& captured : fixture._cycles)
			{
				if (captured._cycle == 1 && captured._line != fixture._scenario._rasterLine)
				{
					reference.beginNextLine (captured._line, replayWritesForLine (fixture, captured._line));
					sampled = {};
					borders = {};
					colors = {};
					logMatch = busMatch = knownMemory = true;
					firstBusDifference = firstLogDifference = 0;
				}
				// Raster advances through the real new-line housekeeping. Matrix,
				// VCBASE, RC and output state are never restored from each snapshot.
				if (this -> _cycleInRasterLine != captured._cycle ||
					this -> _vicGraphicInfo._ROW != captured._line)
				{
					std::cout << "PAL demo replay | ERROR raster at absolute " << captured._absoluteCycle << std::endl;
					return (false);
				}
				reference.setCpuNibble (captured._cpuNibble);
				reference.executeCycle (captured._cycle);
				this -> _cpuOpcodeLowNibble = captured._cpuNibble;
				this -> treatBadLineStateAtCurrentCycle ();
				if (captured._cycle == 14)
					this -> treatGraphicFetchStartCycle ();
				this -> updateBAPrefetchStateAtCurrentCycle ();
				GraphicReferenceFetch actual;
				actual._gAccess = this -> isGraphicAccessCycle ();
				actual._cAccess = this -> isBadLineCAccessCycle ();
				actual._invalidC = actual._cAccess && !this -> cAccessDataValidAtCurrentCycle ();
				CPUStopWindow window;
				actual._baLow = this -> CPUStopWindowAt ((CPURasterCycle) captured._cycle,
					*this -> _currentCPUStopWindows, *this -> _nextCPUStopWindows, window);
				const size_t gIndex = actual._gAccess ? this -> graphicAccessIndex () : 0;
				ram -> beginReadCapture ();
				this -> treatGraphicAccessCycle ();
				ram -> endReadCapture ();
				// Actual addresses come from RAM's readValue observer. No duplicate
				// production address formula or second graphics access is required.
				for (unsigned short address : ram -> reads ())
				{
					bool known = false;
					for (const GraphicReferenceMemoryByte& byte : fixture._scenario._memory)
						known |= byte._address == address;
					knownMemory &= known;
					if (!known)
						std::cout << "PAL demo replay | INCOMPLETE RAM | absolute " << captured._absoluteCycle
							<< " | address " << address << std::endl;
				}
				if (actual._gAccess)
				{
					actual._gAddress = ram -> reads ().empty () ? 0 : ram -> reads () [0];
					actual._gData = this -> _vicGraphicInfo._graphicData [gIndex].value ();
					actual._gScreen = this -> _vicGraphicInfo._screenCodeDrawData [gIndex].value ();
					actual._gColor = this -> _vicGraphicInfo._colorDrawData [gIndex].value ();
				}
				if (actual._cAccess)
				{
					actual._cIndex = this -> _vicGraphicInfo._VLMI;
					actual._cScreen = this -> _vicGraphicInfo._screenCodeData [actual._cIndex].value ();
					actual._cColor = this -> _vicGraphicInfo._colorData [actual._cIndex].value ();
				}
				this -> finishBadLineBusCycle ();
				if (captured._cycle == 58)
					this -> treatGraphicRowEndCycle ();
				actual._vc = this -> _vicGraphicInfo._VC;
				actual._vlmi = this -> _vicGraphicInfo._VLMI;
				actual._prefetch = this -> _BAPrefetchCycles;
				const GraphicReferenceFetch& expected = reference.fetches () [captured._cycle];
				const bool bus = replayFetchesMatch (actual, expected);
				const bool logged = actual._gAccess == captured._gAccess && actual._cAccess == captured._cAccess &&
					(!captured._gAccess || (actual._gAddress == captured._gAddress && actual._gData == captured._gData &&
						actual._gScreen == captured._gScreen && actual._gColor == captured._gColor)) &&
					(!captured._cAccess || (actual._cIndex == captured._cIndex && actual._cScreen == captured._cScreen &&
						actual._cColor == captured._cColor && actual._invalidC == captured._invalidC));
				if ((!bus && firstBusDifference == 0) || (!logged && firstLogDifference == 0))
				{
					if (composition == 0)
						std::cout << "PAL demo replay divergence | absolute " << captured._absoluteCycle
							<< " | line/cycle " << captured._line << '/' << captured._cycle
							<< " | reference/log " << bus << '/' << logged
							<< " | g address/data/code/color actual " << actual._gAddress << '/'
							<< (unsigned int) actual._gData << '/' << (unsigned int) actual._gScreen << '/'
							<< (unsigned int) actual._gColor << " reference " << expected._gAddress << '/'
							<< (unsigned int) expected._gData << '/' << (unsigned int) expected._gScreen << '/'
							<< (unsigned int) expected._gColor << " log " << captured._gAddress << '/'
							<< (unsigned int) captured._gData << '/' << (unsigned int) captured._gScreen << '/'
							<< (unsigned int) captured._gColor << " | VC/VLMI actual/reference "
							<< actual._vc << '/' << actual._vlmi << ' ' << expected._vc << '/' << expected._vlmi
							<< " | c code/color actual/reference/log " << (unsigned int) actual._cScreen << '/'
							<< (unsigned int) actual._cColor << ' ' << (unsigned int) expected._cScreen << '/'
							<< (unsigned int) expected._cColor << ' ' << (unsigned int) captured._cScreen << '/'
							<< (unsigned int) captured._cColor << std::endl;
					if (!bus && firstBusDifference == 0)
						firstBusDifference = captured._absoluteCycle;
					if (!logged && firstLogDifference == 0)
						firstLogDifference = captured._absoluteCycle;
				}
				busMatch &= bus;
				logMatch &= logged;
				const bool visible = this -> _raster.isInVisibleZone ();
				unsigned short cv = 0, rv = 0;
				if (visible)
					this -> _raster.currentVisiblePosition (cv, rv);
				DrawContext context (this -> _raster.hData ().firstDisplayPosition (),
					cv, (unsigned short) ((cv >> 3) << 3), rv);
				if (visible)
				{
					this -> captureOutputState (context._beforeCPUWrite);
					if (composition != 0)
						this -> actualizeMainBorderStatus (context, 0, 4);
				}
				for (const GraphicReferenceWrite& write : captured._writes)
				{
					_registers.setRegister (write._register, MCHEmul::UByte (write._value));
				}
				if (visible)
				{
					this -> captureOutputState (context._afterCPUWrite);
					DrawResult drawn;
					if (composition != 0)
					{
						this -> actualizeMainBorderStatus (context, 4, 8);
						this -> drawVisibleZone (nullptr, context);
					}
					else
						this -> drawGraphics (context, context._beforeCPUWrite, 0, 8, drawn);
					const unsigned short x = context._RCA;
					for (unsigned short pixel = 0; pixel < 8; pixel++)
					{
						sampled [x + pixel] = true;
						borders [x + pixel] = context.mainBorderAtPixel (pixel);
						colors [x + pixel] = composition != 0 ? screen -> frameData ()
							[(size_t) rv * screen -> columns () + context._RCA + pixel] :
							(unsigned int) (drawn._foregroundColorData [pixel] != MCHEmul::_U0
								? drawn._foregroundColorData [pixel] : drawn._backgroundColorData [pixel] != MCHEmul::_U0
									? drawn._backgroundColorData [pixel] : context._beforeCPUWrite._backgroundColors [0]);
					}
				}
				this -> commitGraphicOutputData ();
				this -> advanceRasterPosition ();
				if (captured._cycle == 63)
				{
					bool output = true;
					unsigned short firstX = 0;
					for (unsigned short x = composition != 0 ? 24 : 32; x < (composition != 0 ? 360 : 352); x++)
					{
						const GraphicReferencePixel& pixel = reference.outputPixels () [x];
						if (fixture._scenario._bitmapReference && composition == 0)
						{
							const unsigned short edge = (fixture._scenario._initialD016 & 8) != 0 ? 344 : 335;
							if (x >= edge && x < edge + 8 && sampled [x] && pixel._sampled && pixel._color != 0)
								bitmapEdgeSamples++;
							const GraphicReferencePixel& shifted = reference.outputPixels () [x - 8];
							bitmapShiftRejected |= sampled [x] && shifted._sampled && colors [x] != shifted._color;
						}
						const bool match = sampled [x] && pixel._sampled && (composition != 0
							? borders [x] == pixel._border && colors [x] == palette [pixel._composedColor]
							: colors [x] == pixel._color);
						if (!match && output)
							firstX = x;
						output &= match;
					}
					const bool counters = this -> _vicGraphicInfo._VCBASE == reference.videoCounterBase ();
					std::cout << "PAL demo replay | " << fixture._scenario._name << " | line " << captured._line
						<< " | " << (composition != 0 ? "composition" : "graphics color")
						<< " | log bus " << (logMatch ? "OK" : "ERROR")
						<< " | independent bus " << (busMatch ? "OK" : "ERROR")
						<< " | output " << (output ? "OK" : "ERROR")
						<< " | VCBASE actual/reference " << this -> _vicGraphicInfo._VCBASE << '/'
						<< reference.videoCounterBase () << (counters ? " OK" : " ERROR")
						<< " | RAM " << (knownMemory && !reference.memoryIncomplete () ? "COMPLETE" : "INCOMPLETE");
					if (!output)
						std::cout << " | first X " << firstX << " actual " << colors [firstX]
							<< " expected color " << (unsigned int) reference.outputPixels () [firstX]._color
							<< " source cycle/code " << reference.outputPixels () [firstX]._sourceCycle << '/'
							<< (unsigned int) reference.outputPixels () [firstX]._screenCode;
					std::cout << std::endl;
					result &= logMatch && busMatch && output && counters && knownMemory && !reference.memoryIncomplete ();
				}
			}
		}
		if (fixture._scenario._bitmapReference)
		{
			std::cout << "PAL bitmap replay coverage | " << fixture._scenario._name
				<< " | nonzero visible right-edge samples " << bitmapEdgeSamples
				<< " | shifted-eight-pixel reference rejected " << (bitmapShiftRejected ? "YES" : "NO") << std::endl;
			result &= bitmapEdgeSamples != 0 && bitmapShiftRejected;
		}
		this -> resetBadLineStateForNewRasterLine ();
		this -> enterIdleState ();
		this -> resetGraphicAccessCountersForCurrentLine ();
		return (result);
	}

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

	VICIITestMemory _testMemory;
	MCHEmul::PhysicalStorage _colorStorage;
	MCHEmul::PhysicalStorageSubset _colorRAM;
	MCHEmul::PhysicalStorage _registerStorage;
	COMMODORE::VICIIRegisters _registers;
};

int main (int argc, char* argv [])
{
	std::cout << "TestVICIIStop" << std::endl;
	if (argc == 2 && std::string (argv [1]) == "--members-line-replay")
	{
		TestVICII <COMMODORE::VICII_PAL> membersVICII;
		bool membersResult = VICIIGraphicReference::testBitmapAndInvalidModeAnchors ();
		membersResult &= membersVICII.test10000MembersLineReplay ();
		return (membersResult ? 0 : 1);
	}
	if (argc == 2 && std::string (argv [1]) == "--members-graphic-phase")
	{
		TestVICII <COMMODORE::VICII_PAL> membersVICII;
		return (membersVICII.test10000MembersGraphicSourcePhase () ? 0 : 1);
	}
	if (argc == 2 && std::string (argv [1]) == "--members-late-edge")
	{
		TestVICII <COMMODORE::VICII_PAL> membersVICII;
		return (membersVICII.test10000MembersLateBadLineRightEdge () ? 0 : 1);
	}
	if (argc == 2 && std::string (argv [1]) == "--members-edge-composition")
	{
		TestVICII <COMMODORE::VICII_PAL> membersVICII;
		return (membersVICII.test10000MembersRightEdgeCompositionSequence () ? 0 : 1);
	}
	if (argc == 2 && std::string (argv [1]) == "--members-raster-stabilizer")
	{
		TestVICII <COMMODORE::VICII_PAL> stabilizerVICII;
		return (stabilizerVICII.test10000MembersRasterReadStabilizer () ? 0 : 1);
	}
	if (argc == 2 && std::string (argv [1]) == "--color-reference")
	{
		TestVICII <COMMODORE::VICII_PAL> colorVICII;
		return (colorVICII.testReferenceD021ColorWrites () ? 0 : 1);
	}
	if (argc == 2 && std::string (argv [1]) == "--demo-line-replay")
	{
		TestVICII <COMMODORE::VICII_PAL> replayVICII;
		bool replayResult = VICIIGraphicReference::testColorResolutionAnchors ();
		replayResult &= replayVICII.testLetsScrollitALineReplay ();
		replayResult &= replayVICII.testLetsScrollitBLineReplay ();
		return (replayResult ? 0 : 1);
	}
	if (argc == 2 && std::string (argv [1]) == "--graphic-reference")
	{
		TestVICII <COMMODORE::VICII_PAL> referenceVICII;
		bool referenceResult = referenceVICII.testReferenceNormalLineAlignment ();
		referenceResult &= referenceVICII.testReferenceLateBadLineOutput ();
		referenceResult &= referenceVICII.testReferenceXScrollSampling ();
		referenceResult &= referenceVICII.testReferenceMulticolorAndBorders ();
		referenceResult &= referenceVICII.testReferenceD021ColorWrites ();
		referenceResult &= referenceVICII.testLetsScrollitALineReplay ();
		referenceResult &= referenceVICII.testLetsScrollitBLineReplay ();
		return (referenceResult ? 0 : 1);
	}
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
	TestVICII <COMMODORE::VICII_PAL> stabilizerVICII;
	result &= stabilizerVICII.test10000MembersRasterReadStabilizer ();
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
	result &= vicii.testAlignedPhi2OutputWritePhase ();
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
	result &= vicii.testBadLineScenarioMatrix ();
	result &= vicii.testD011WriteBadLinePhaseOrder ();
	result &= vicii.testLateBadLineRegisterWritePipeline ();
	result &= vicii.testBadLineConditionTransitions ();
	result &= vicii.testSharedBAPrefetchState ();
	result &= vicii.testBadLineSpriteWindowCompositionBaseline ();
	result &= vicii.testLateBadLineCPUStopWindow ();
	result &= vicii.testGraphicOutputSequencer ();
	result &= vicii.test10000MembersGraphicSourcePhase ();
	result &= vicii.test10000MembersLateBadLineRightEdge ();
	result &= vicii.test10000MembersRightEdgeCompositionSequence ();
	result &= vicii.testLateBadLineGraphicOutputPipeline ();
	result &= vicii.testLateBadLineRealFetchPipeline ();
	result &= vicii.testLateBadLineRenderedPixelPipeline ();
	result &= vicii.testReferenceNormalLineAlignment ();
	result &= vicii.testReferenceLateBadLineOutput ();
	result &= vicii.testReferenceXScrollSampling ();
	result &= vicii.testReferenceMulticolorAndBorders ();
	result &= vicii.testReferenceD021ColorWrites ();
	result &= vicii.testLetsScrollitALineReplay ();
	result &= vicii.testLetsScrollitBLineReplay ();
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
