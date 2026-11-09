#include <COMMODORE/TED/TED.hpp>
#include <COMMODORE/TED/TEDSoundWrapper.hpp>
#include <F6500/IRQInterrupt.hpp>

// ---
const MCHEmul::RasterData COMMODORE::TED_PAL::_VRASTERDATA (260, 274, 3, 202, 245, 259, 259, 312, 4, 4);
/** From the book TED (https://www.pagetable.com/docs/ted/TED%207360R0%20Preliminary%20Data%20Sheet.pdf) 
	it can me understood:
	There is an internal counter that goes from 0 to 455 (456 positions = dots).
	The HSYNC happens at the value 358 of this counter,
	The HANDSHAKING process with the processor happens between the values 400 and 432 (both included).
	The CHARACTER DATA is fetched and drawn between the values 432 and 304 (both included),
	what means that the counter reaches the limit and starts again at 0 at some point in the middle of the drawing,
	and what is 8 characters more that the ones drawn in the visible area (40 characters visible, 48 characters fetched).
	The REFRESH of the DMA memory happens between the values 304 and 344 (both included). \n
	The documentation also says that the single clock mode happens mandatory always for these actions and when 
	the processor is between the raster lines 0 and 204 (remeber that the first visible line happens at line 3
	but the first bad line is at line 0 instead).
	IN the rest of the lines (depending on PAL) the CPU can work at double speed except between the values 304 and 344
	where the refresh has to happen too always (is mandatory). 
	The HRASTERDATA is the same for PAL and for NTSC. */
const MCHEmul::RasterData COMMODORE::TED_PAL::_HRASTERDATA 
	(360, 400, 432, 295, 327, 359, 359, 456 /** For everyting to run, it has to be divisible by 8. */, 8, 8);
const MCHEmul::RasterData COMMODORE::TED_NTSC::_VRASTERDATA (235, 249, 3, 202, 220, 234, 234, 262, 4, 4);
const MCHEmul::RasterData COMMODORE::TED_NTSC::_HRASTERDATA 
	(360, 400, 432, 295, 327, 359, 359, 456 /** For everyting to run, it has to be divisible by 8. */, 8, 8);
const COMMODORE::TED::Configuration COMMODORE::TED::_PALCONFIGURATION
	(COMMODORE::TED_PAL::_VRASTERDATA, COMMODORE::TED_PAL::_HRASTERDATA,
	 COMMODORE::TED_PAL::_DIVIDERCPUCLOCK, COMMODORE::TED_PAL::_SCREENFREQUENCY);
const COMMODORE::TED::Configuration COMMODORE::TED::_NTSCCONFIGURATION
	(COMMODORE::TED_NTSC::_VRASTERDATA, COMMODORE::TED_NTSC::_HRASTERDATA,
	 COMMODORE::TED_NTSC::_DIVIDERCPUCLOCK, COMMODORE::TED_NTSC::_SCREENFREQUENCY);

// This two positions are fized...
const MCHEmul::Address COMMODORE::TED::_MEMORYPOSIDLE1 = MCHEmul::Address ({ 0xff, 0x39 }, false);
const MCHEmul::Address COMMODORE::TED::_MEMORYPOSIDLE2 = MCHEmul::Address ({ 0xff, 0x3f }, false);

// ---
COMMODORE::TED::SoundFunction::SoundFunction (MCHEmul::SoundLibWrapper* sW)
	: MCHEmul::SoundChip (_ID, 
		{ { "Name", "SoundFunction" },
		  { "Code", "Inside TED" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Year", "1984" } },
		sW),
	  _pendingSoundClockCycles (0),
	  _pendingSoundBatches ()
{ 
	// Take care that the sound emulation library was not null at all...
	// ...and also belonging to the right type..
	assert (dynamic_cast <COMMODORE::TEDSoundLibWrapper*> (soundWrapper ()) != nullptr);

	setClassName ("SoundFunction");
}

// ---
bool COMMODORE::TED::SoundFunction::initialize ()
{
	if (!MCHEmul::SoundChip::initialize ())
		return (false);

	_pendingSoundClockCycles = 0;
	_pendingSoundBatches.clear ();

	return (true);
}

// ---
void COMMODORE::TED::SoundFunction::addTEDCycle
	(unsigned int masterCycles)
{
	if (!_pendingSoundBatches.empty () &&
		_pendingSoundBatches.back ()._masterCycles == masterCycles)
		++_pendingSoundBatches.back ()._cycles;
	else
		_pendingSoundBatches.emplace_back (masterCycles);

	++_pendingSoundClockCycles;
}

// ---
void COMMODORE::TED::SoundFunction::setClockDivider
	(unsigned int divider)
{
	static_cast <COMMODORE::TEDSoundLibWrapper*>
		(soundWrapper ()) -> setClockDivider (divider);
}

// ---
MCHEmul::InfoStructure COMMODORE::TED::SoundFunction::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::SoundChip::getInfoStructure ());

	result.remove ("Memory"); // This info is not neccesary...
	result.add ("SoundLibWrapper",	std::move (soundWrapper () -> getInfoStructure ())); // To know which library is behing...

	return (result);
}

// ---
bool COMMODORE::TED::SoundFunction::simulatePendingCycles
	(MCHEmul::CPU* cpu, unsigned int referenceCycle,
	 bool beforeCPUWrite)
{
	unsigned int cyclesProcessed = 0;
	unsigned long long masterCyclesProcessed = 0;
	size_t samplesGenerated = 0;

	auto* wrapper =
		static_cast <COMMODORE::TEDSoundLibWrapper*> (soundWrapper ());

	// Each queued pulse keeps its elapsed time across later divider changes.
	for (const auto& batch : _pendingSoundBatches)
	{
		for (unsigned int i = 0; i < batch._cycles; i++)
		{
			_IFDEBUG
			{
				++cyclesProcessed;
				masterCyclesProcessed += batch._masterCycles;
			}

			MCHEmul::UBytes data;
			if (wrapper -> getDataForTEDCycle (cpu, batch._masterCycles, data))
			{
				_IFDEBUG samplesGenerated += data.size ();

				for (size_t j = 0; j < data.size (); j++)
				{
					char dt = data [j].value ();
					if (soundMemory () -> addSampleData (&dt, sizeof (char)))
						MCHEmul::SoundChip::notify (MCHEmul::Event (_SOUNDREADY));
				}
			}
		}
	}

	_IFDEBUG debugSoundBatch
		(cpu, referenceCycle, beforeCPUWrite,
		 cyclesProcessed, masterCyclesProcessed, samplesGenerated);

	_pendingSoundClockCycles = 0;
	_pendingSoundBatches.clear ();

	return (true);
}

// ---
void COMMODORE::TED::SoundFunction::debugSoundBatch
	(MCHEmul::CPU* cpu, unsigned int referenceCycle,
	 bool beforeCPUWrite, unsigned int cyclesProcessed,
	 unsigned long long masterCyclesProcessed,
	 size_t samplesGenerated) const
{
	_deepDebugFile -> writeCompleteLine
		(className (), referenceCycle, "Sound batch",
		{ { "Timing",
			std::string ("Reference=") +
				(beforeCPUWrite
					? "ScheduledCPUCycle" : "ObservedCPUClock") +
			",Cause=" +
				(beforeCPUWrite
					? "BeforeCPUWrite" : "ChipSimulation") +
			",ObservedCPUClock=" + std::to_string (cpu -> clockCycles ()) },
		  { "Sound timing",
			"ClockSource=TEDScheduler,Unit=SlowTEDCycle," +
			std::string ("PendingCyclesBefore=") +
				std::to_string (_pendingSoundClockCycles) + "," +
			"CyclesProcessed=" + std::to_string (cyclesProcessed) + "," +
			"MasterCyclesProcessed=" + std::to_string (masterCyclesProcessed) + "," +
			"MasterHz=" + std::to_string
				(static_cast <const COMMODORE::TEDSoundLibWrapper*>
					(soundWrapper ()) -> masterFrequency ()) + "," +
			"SamplesGenerated=" + std::to_string (samplesGenerated) } });
}

// ---
COMMODORE::TED::Configuration::Configuration
	(const MCHEmul::RasterData& vd, const MCHEmul::RasterData& hd,
	 unsigned short divider, unsigned short screenFrequency)
	: _vertical (vd), _horizontal (hd),
	  _divider (divider), _screenFrequency (screenFrequency),
	  _endOfScreen (vd.totalPositions () - 1),
	  _cyclesPerRasterLine (hd.totalPositions () >> 3)
{
	// The scheduler retains the TED's fixed horizontal decodes and nine-bit vertical counter.
	assert (hd.totalPositions () == 456 && hd.firstPosition () == 360 &&
		hd.retracePosition () == 359 && vd.totalPositions () <= 512);
}

// ---
COMMODORE::TED::TED (int intId, bool initialPAL,
		const Configuration& pal, const Configuration& ntsc,
		int vV, MCHEmul::SoundLibWrapper* sW, const MCHEmul::Attributes& attrs)
	: MCHEmul::GraphicalChip (_ID, attrs),
	  _interruptId (intId),
	  _configurations { pal, ntsc },
	  _initialConfiguration (&_configurations [initialPAL ? 0 : 1]),
	  _T1 (1, COMMODORE::TEDTimer::RunMode::_FROMRELOADVALUE),
	  _T2 (2, COMMODORE::TEDTimer::RunMode::_FROMFFFF),
	  _T3 (3, COMMODORE::TEDTimer::RunMode::_FROMFFFF),
	  _soundFunction (new COMMODORE::TED::SoundFunction (sW)),
	  _TEDRegisters (nullptr), 
	  _TEDView (vV),
	  _raster (_initialConfiguration -> _vertical, _initialConfiguration -> _horizontal, 8 /** step. */),
	  _drawRasterInterruptPositions (false),
	  _drawOtherEvents (false),
	  _configuration (_initialConfiguration),
	  _presentationConfiguration (_initialConfiguration),
	  _rasterPosition (),
	  _verticalClockPending (false), _flashClockPending (false),
	  _rasterIRQMatched (false),
	  _graphicsCycleActive (false),
	  _cpuTransaction (),
	  _timedWriteCPU (nullptr),
	  _timedWriteSubsets (),
	  _pendingWrites (),
	  _nextPendingWrite (0),
	  _cpuTimedWritesEnabled (false), _cpuTransactionPending (false),
	  _cpuCycleState (), _cpuReadAdvanceCycles (0),
	  _pendingPacingCycles (0),
	  _pendingSoundMasterCycles (0),
	  _lastCPUCycles (0), _pendingHalfCyclesFromLastExecution (0),
	  _format (nullptr),
	  _cycleInRasterLine (1),
	  _videoActive (true),
	  _inSingleClockMode (false),
	  _lastVBlankEntered (false),
	  _lastBadLineScrollY (-1), _newBadLineCondition (false), 
	  _tedGraphicInfo (),
	  _drawResult (),
	  _eventStatus ()
{
	assert (pal._divider == 20 && ntsc._divider == 16);

	setClassName ("TED");

	_format = SDL_AllocFormat (SDL_PIXELFORMAT_ARGB8888);
}

// ---
COMMODORE::TED::~TED ()
{
	/**
		delete (_soundFunction);
		It is not invoked, because the sound function is
		managed as a different chip from the computer on regards simulation.
		That chip is owned by the computer and then destroyed by it. */

	SDL_FreeFormat (_format);
}

// ---
double COMMODORE::TED::nominalFrameFrequency () const
{
	return ((double) static_cast <const COMMODORE::TEDSoundLibWrapper*>
		(_soundFunction -> soundWrapper ()) -> masterFrequency () /
		((double) _configuration -> _divider * _configuration -> _cyclesPerRasterLine *
		 (_configuration -> _endOfScreen + 1)));
}

// ---
bool COMMODORE::TED::initialize ()
{
	_configuration = _presentationConfiguration = _initialConfiguration;
	_raster.configure (_configuration -> _vertical, _configuration -> _horizontal,
		_configuration -> _vertical.firstPosition (), _configuration -> _horizontal.firstPosition ());

	if (!MCHEmul::GraphicalChip::initialize ())
		return (false);

	assert (memoryRef () != nullptr);

	// Gets the memory block dedicated to the TED
	if (!(_TEDRegisters = 
		dynamic_cast <COMMODORE::TEDRegisters*> (memoryRef () -> subset (COMMODORE::TEDRegisters::_TEDREGS_SUBSET))))
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	_raster.reduceDisplayZone (false, false);

	_T1.initialize ();
	_T2.initialize ();
	_T3.initialize ();

	// The sound function is initialized independly...
	assert (_soundFunction -> soundWrapper () != nullptr);

	_TEDRegisters -> lookAtTimers (&_T1, &_T2, &_T3);
	_TEDRegisters -> lookAtSoundLibWrapper (_soundFunction -> soundWrapper ());
	_TEDRegisters -> linkToRasterPosition (&_rasterPosition);
	_TEDRegisters -> initialize ();

	// Emulator startup configuration for the selected board, not measured power-on state.
	if (_initialConfiguration == &_configurations [1])
		_TEDRegisters -> setValue (0x07, MCHEmul::UByte (0xc8));
	_soundFunction -> setClockDivider (clockDivider ());

	// At H=360 the presentation row has advanced, but the hardware vertical
	// increment is still pending at H=392. Seed the preceding line once.
	_rasterPosition = COMMODORE::TEDRegisters::RasterPosition ();
	_rasterPosition._vertical = _raster.previousLine ();
	_rasterPosition._horizontal = _raster.currentColumn ();
	_rasterPosition._videoLine = _rasterPosition._vertical;

	_verticalClockPending = _flashClockPending = false;
	_rasterIRQMatched =
		_rasterPosition._vertical == _TEDRegisters -> IRQRasterLineAt ();
	_graphicsCycleActive = false;

	publishRasterPosition ();

	_cpuTransaction = { };

	_timedWriteCPU = nullptr;
	_timedWriteSubsets.clear ();
	_pendingWrites.clear ();
	_nextPendingWrite = 0;
	_cpuTimedWritesEnabled = _cpuTransactionPending = false;
	_cpuCycleState = { };
	_cpuReadAdvanceCycles = 0;
	_pendingPacingCycles = 0;
	_pendingSoundMasterCycles = 0;

	_lastCPUCycles = _pendingHalfCyclesFromLastExecution = 0;
	
	_cycleInRasterLine = 1;

	_videoActive = true;

	_inSingleClockMode = singleClockModeForCurrentCycle ();

	_lastVBlankEntered = false;

	_lastBadLineScrollY = -1;
	_newBadLineCondition = false;

	_tedGraphicInfo = { };
	_drawResult = { };
	_eventStatus = { };

	return (true);
}

// ---
bool COMMODORE::TED::prepareCPUTimedWrites (MCHEmul::CPU* cpu,
	 const MCHEmul::PhysicalStorageSubsets& subsets)
{
	assert (cpu != nullptr && _TEDRegisters != nullptr);
	assert (_timedWriteCPU == nullptr && _pendingWrites.empty ());

	const MCHEmul::PhysicalStorageSubsets::const_iterator registers =
		subsets.find (_TEDRegisters -> id ());

	if (cpu -> ticksCounter () != nullptr ||
		cpu -> memoryRef () != memoryRef () ||
		registers == subsets.end () ||
		registers -> second != _TEDRegisters)
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	for (const auto& subset : subsets)
	{
		if (subset.second == nullptr ||
			subset.first != subset.second -> id () ||
			subset.second -> type () != MCHEmul::PhysicalStorage::Type::_RAM ||
			memoryRef () -> subset (subset.first) != subset.second)
		{
			_error = MCHEmul::_INIT_ERROR;

			return (false);
		}
	}

	_timedWriteCPU = cpu;
	_timedWriteSubsets = subsets;

	return (true);
}

// ---
bool COMMODORE::TED::enableCPUTimedWrites ()
{
	if (_timedWriteCPU == nullptr || _cpuTimedWritesEnabled)
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	// Include board registers that disable buffering in their constructors.
	for (const auto& subset : _timedWriteSubsets)
		subset.second -> setBufferMemorySetCommands (true);

	_lastCPUCycles = _timedWriteCPU -> clockCycles ();
	_cpuTransaction = { };
	_cpuTransactionPending = false;
	_cpuCycleState = { };
	_cpuReadAdvanceCycles = 0;
	_pendingPacingCycles = 0;
	_cpuTimedWritesEnabled = true;

	return (true);
}

// ---
void COMMODORE::TED::CPUAboutToExecute
	(const MCHEmul::InstructionContextEventData* dt)
{
	assert (dt != nullptr);

	const MCHEmul::InstructionDefined* instruction =
		static_cast <const MCHEmul::InstructionDefined*> (dt -> _instruction);
	// The 7501 predicts conditional alternatives before notifying execution.
	const size_t cycleStructure =
		(size_t) (dt -> _clockCycles - instruction -> clockCycles ());
	assert (cycleStructure < instruction -> busCycleDatas ().size ());

	const MCHEmul::BusCycleData& busData =
		instruction -> busCycleData (cycleStructure);
	assert (busData._numberCycles == dt -> _clockCycles);

	if (!prepareCPUTransaction (dt -> _cpu, busData, false))
		return;

	_IFDEBUG
	{
		_TEDRegisters -> linkToDebugCPU (dt -> _cpu);
		debugCPUAboutToExecute (dt);
	}

	if (_cpuTimedWritesEnabled && !advanceToCPURead (dt -> _cpu))
		_error = MCHEmul::_CHIP_ERROR;
}

// ---
void COMMODORE::TED::CPUAboutToExecute
	(const MCHEmul::InterruptContextEventData* dt)
{
	assert (dt != nullptr);

	const MCHEmul::BusCycleData& busData =
		dt -> _interrupt -> busCycleData ();

	if (!prepareCPUTransaction (dt -> _cpu, busData, true))
		return;

	_IFDEBUG
	{
		_TEDRegisters -> linkToDebugCPU (dt -> _cpu);
		debugCPUAboutToExecute (dt);
	}
}

// ---
bool COMMODORE::TED::simulate (MCHEmul::CPU* cpu)
{
	const unsigned int batchEnd = cpu -> clockCycles ();

	if (_cpuTimedWritesEnabled)
	{
		if (cpu != _timedWriteCPU || _error != MCHEmul::_NOERROR)
		{
			_error = MCHEmul::_CHIP_ERROR;

			return (false);
		}

		if (_cpuTransactionPending)
		{
			const unsigned int completedAdvanceCycles =
				(_cpuReadAdvanceCycles == 0)
					? 0 : _cpuReadAdvanceCycles - 1;
			const bool suspended =
				_cpuCycleState._accessPending;

			if (batchEnd - _cpuTransaction._startCycle !=
					_cpuTransaction._busData -> _numberCycles ||
				_lastCPUCycles !=
					_cpuTransaction._startCycle + completedAdvanceCycles ||
				suspended != (_cpuReadAdvanceCycles != 0))
			{
				_error = MCHEmul::_CHIP_ERROR;

				return (false);
			}

			if (!extractPendingWrites ())
				return (false);
		}
	}
	else if (_lastCPUCycles == 0)
	{
		_lastCPUCycles = batchEnd;

		return (true);
	}

	const unsigned int batchStart = _cpuTransactionPending
		? _cpuTransaction._startCycle : _lastCPUCycles;

	// An advanced prefix already sampled the unchanged display-zone registers.
	if (_cpuReadAdvanceCycles == 0)
		_raster.reduceDisplayZone
			(!_TEDRegisters -> textDisplay25RowsActive (),
			 !_TEDRegisters -> textDisplay40ColumnsActive ());

	while (_lastCPUCycles != batchEnd)
	{
		if (!simulateCPUCycle (cpu, batchStart, batchEnd))
			return (false);
	}

	if (_cpuTransactionPending)
	{
		if (_nextPendingWrite != _pendingWrites.size ())
		{
			_error = MCHEmul::_CHIP_ERROR;

			return (false);
		}

		_pendingWrites.clear ();
		_nextPendingWrite = 0;
		_cpuTransactionPending = false;
		_cpuReadAdvanceCycles = 0;
	}

	if (_TEDRegisters -> freezeActive ())
	{
		if (requestIRQ (cpu, batchEnd))
		{
			COMMODORE::TED::EventMarker marker;

			marker._events = EventMarker::_IRQ;
			marker._visible = _raster.isInVisibleZone ();

			if (marker._visible)
				_raster.currentVisiblePosition (marker._column, marker._row);

			drawOtherEvents (marker);
		}
	}

	return (true);
}

// ---
MCHEmul::InfoStructure COMMODORE::TED::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::GraphicalChip::getInfoStructure ());

	result.remove ("Memory"); // This info is not neccesary...
	result.add ("TEDRegisters",		std::move (_TEDRegisters -> getInfoStructure ()));
	result.add ("Raster",			std::move (_raster.getInfoStructure ()));
	result.add ("Sound",			std::move (_soundFunction -> getInfoStructure ()));
	result.add ("TEDTimer1",		std::move (_T1.getInfoStructure ()));
	result.add ("TEDTimer2",		std::move (_T2.getInfoStructure ()));
	result.add ("TEDTimer3",		std::move (_T3.getInfoStructure ()));

	// Expose live execution state separately from the published raster copy.
	MCHEmul::InfoStructure execution;

	execution.add ("COUNTERX",			_rasterPosition._horizontal);
	execution.add ("COUNTERY",			_rasterPosition._vertical);
	execution.add ("VIDEOLINE",			_rasterPosition._videoLine);
	execution.add ("RASTERCYCLE",			rasterCycle ());
	execution.add ("SELECTEDPAL",			_configuration == &_configurations [0]);
	execution.add ("PRESENTATIONPAL",
		_presentationConfiguration == &_configurations [0]);
	execution.add ("PRESENTATIONPENDING",
		_configuration != _presentationConfiguration);
	execution.add ("CLOCKDIVIDER",		_configuration -> _divider);
	execution.add ("SINGLECLOCK",			_inSingleClockMode);
	execution.add ("GRAPHICSCYCLEACTIVE",	_graphicsCycleActive);
	execution.add ("VERTICALCLOCKPENDING",	_verticalClockPending);
	execution.add ("FLASHCLOCKPENDING",	_flashClockPending);
	execution.add ("HORIZONTALWRITEPENDING",
		_rasterPosition._horizontalWritePending);
	execution.add ("VC",					_tedGraphicInfo._VC);

	result.add ("Execution", std::move (execution));

	return (result);
}

// ---
MCHEmul::UBytes COMMODORE::TED::screenMemorySnapShot (MCHEmul::CPU* cpu) const
{
	// Use the same RAM source as the live matrix fetch, including machine aliases.
	MCHEmul::UBytes result (std::vector <MCHEmul::UByte> (0x03e8));
	for (size_t i = 0; i < result.size (); i++)
		result [i] = readRAMForVideo (_TEDRegisters -> screenMemory () + i);

	return (result);
}

// ---
MCHEmul::UBytes COMMODORE::TED::attributeMemorySnapShot (MCHEmul::CPU* cpu) const
{
	// Attributes are RAM bytes even when CPU or graphical ROM overlays are selected.
	MCHEmul::UBytes result (std::vector <MCHEmul::UByte> (0x03e8));
	for (size_t i = 0; i < result.size (); i++)
		result [i] = readRAMForVideo (_TEDRegisters -> attributeMemory () + i);

	return (result);
}

// ---
MCHEmul::UBytes COMMODORE::TED::bitmapMemorySnapShot (MCHEmul::CPU* cpu)
{ 
	MCHEmul::Memory* memory = cpu -> memoryRef ();
	const int previousView = memory -> activeView () -> id ();
	const bool previousTEDROM = ROMActiveToFetchCharData ();
	const bool graphicsROM = _TEDRegisters -> ROMActiveToFetchCharAndBitmap ();

	if (previousView != _TEDView)
		memory -> setActiveView (_TEDView);

	// Use the graphical source throughout this inspection, then restore the mapping.
	if (previousTEDROM != graphicsROM)
		activeROMtoFetchCharData (graphicsROM);

	MCHEmul::UBytes result = memory -> values
		(_TEDRegisters -> bitmapMemory (), 0x1f40 /** 8000 positions = 40 x 25 x 8. */); 

	if (previousTEDROM != graphicsROM)
		activeROMtoFetchCharData (previousTEDROM);

	if (previousView != _TEDView)
		memory -> setActiveView (previousView);

	return (result);
}

// ---
MCHEmul::Strings COMMODORE::TED::charsDrawSnapshot (MCHEmul::CPU* cpu,
	const std::vector <size_t>& chrs)
{
	// Lambda function to draw the content of a byte in a "monocolor way".
	// That is: one bit, one pixel, one color...
	auto textByteMonocolor = [](const MCHEmul::UByte& u) -> std::string 
		{ 
			std::string result;
			for (size_t l = 0; l < 8; l++)
				result += ((u.value () & (1 << (7 - l))) != 0x00) ? "#" : " ";
			return (result);
		};

	// Lambda function to draw the content of a byte in "multicolor way".
	// That is: a par of bit, two pixels, one color...
	auto textByteMultiColor = [](const MCHEmul::UByte& u) -> std::string
		{
			std::string result;

			for (size_t l = 0; l < 8; l += 2)
			{ 
				switch ((u.value () & (0x03 << (6 - l))) >> (6 - l))
				{
					case 0x00: result += "  "; break;
					case 0x01: result += "OO"; break;
					case 0x02: result += "XX"; break;
					case 0x03: result += "##"; break;
					// This situation doesn't happen...
					default: break;
				}
			}

			return (result);
		};

	MCHEmul::Memory* memory = cpu -> memoryRef ();
	const int previousView = memory -> activeView () -> id ();
	const bool previousTEDROM = ROMActiveToFetchCharData ();
	const bool graphicsROM = _TEDRegisters -> ROMActiveToFetchCharAndBitmap ();

	if (previousView != _TEDView)
		memory -> setActiveView (_TEDView);

	// Use the graphical source throughout this inspection, then restore the mapping.
	if (previousTEDROM != graphicsROM)
		activeROMtoFetchCharData (graphicsROM);

	MCHEmul::Strings result;
	for (size_t i = 0; i < 256; i++)
	{
		if (!chrs.empty () && 
			std::find (chrs.begin (), chrs.end (), i) == chrs.end ())
			continue;

		MCHEmul::Address chrAdd = _TEDRegisters -> charDataMemory () + (i << 3);
		std::string dt = std::to_string (i) + "---\n$" +
			MCHEmul::removeAll0 (chrAdd.asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) + "\n";
		MCHEmul::UBytes chrDt = memory -> values (chrAdd, 0x08);
		for (size_t j = 0; j < 8; j++) // 8 lines per character...
		{
			if (j != 0)
				dt += "\n";

			// In multicolor, depending on the bit 3 of the attribute byte
			// in the color matrix, the character can be draw as monocolor or as multicolor...
			// The rest of the modes are draw as monocolor, including the extended multicolor char...
			dt += textByteMonocolor (chrDt [j]);
			if (_TEDRegisters -> graphicMulticolorTextModeActive ())
				dt += " | " /** Separated by a symbol. */ + textByteMultiColor (chrDt [j]);
			dt += "|$" + chrDt [j].asString (MCHEmul::UByte::OutputFormat::_HEXA, 2);
		}

		result.emplace_back (std::move (dt));
	}

	result.emplace_back ("---");

	if (previousTEDROM != graphicsROM)
		activeROMtoFetchCharData (previousTEDROM);

	if (previousView != _TEDView)
		memory -> setActiveView (previousView);

	return (result);
}

// ---
MCHEmul::ScreenMemory* COMMODORE::TED::createScreenMemory ()
{
	static struct RGBStr { unsigned int R,G,B; } _COLORS [128] =
		{ 
			// Black
			{ 0x00,0x00,0x00 },{ 0x00,0x00,0x00 },{ 0x00,0x00,0x00 },{ 0x00,0x00,0x00 },
			{ 0x00,0x00,0x00 },{ 0x00,0x00,0x00 },{ 0x00,0x00,0x00 },{ 0x00,0x00,0x00 },
			// White
			{ 0x33,0x33,0x33 },{ 0x42,0x42,0x42 },{ 0x4C,0x4C,0x4C },{ 0x5E,0x5E,0x5E },
			{ 0x84,0x84,0x84 },{ 0xAA,0xAA,0xAA },{ 0xC3,0xC3,0xC3 },{ 0xFF,0xFF,0xFF },
			// Red
			{ 0x6C,0x1B,0x1C },{ 0x87,0x34,0x34 },{ 0x9D,0x49,0x49 },{ 0xB0,0x5D,0x5E },
			{ 0xDC,0x89,0x8A },{ 0xF5,0xAA,0xAA },{ 0xFF,0xC4,0xC4 },{ 0xFF,0xFF,0xFF },
			// Cyan
			{ 0x00,0x4B,0x4A },{ 0x14,0x66,0x65 },{ 0x26,0x7A,0x79 },{ 0x3A,0x8E,0x8D },
			{ 0x66,0xBA,0xB9 },{ 0x80,0xD4,0xD3 },{ 0x9A,0xEA,0xE9 },{ 0xFF,0xFF,0xFF },
			// Purple
			{ 0x61,0x0F,0x71 },{ 0x7E,0x2B,0x87 },{ 0x94,0x40,0x9D },{ 0xA8,0x56,0xB0 },
			{ 0xD2,0x80,0xD9 },{ 0xEC,0x9A,0xF3 },{ 0xFF,0xB4,0xFF },{ 0xFF,0xFF,0xFF },
			// Green
			{ 0x00,0x57,0x01 },{ 0x12,0x72,0x15 },{ 0x26,0x87,0x2B },{ 0x3B,0x9C,0x41 },
			{ 0x66,0xC6,0x6B },{ 0x80,0xE1,0x85 },{ 0x9A,0xF8,0xA0 },{ 0xFF,0xFF,0xFF },
			// Blue
			{ 0x26,0x26,0x98 },{ 0x3F,0x41,0xB0 },{ 0x54,0x56,0xC6 },{ 0x69,0x6B,0xDB },
			{ 0x92,0x95,0xFF },{ 0xAB,0xAF,0xFF },{ 0xC6,0xC9,0xFF },{ 0xFF,0xFF,0xFF },
			// Yellow
			{ 0x40,0x40,0x00 },{ 0x5A,0x5A,0x00 },{ 0x70,0x70,0x00 },{ 0x85,0x85,0x00 },
			{ 0xAE,0xAE,0x25 },{ 0xC8,0xC8,0x3F },{ 0xE0,0xE0,0x5A },{ 0xFF,0xFF,0xFF },
			// Orange
			{ 0x60,0x29,0x00 },{ 0x78,0x40,0x00 },{ 0x8B,0x55,0x1A },{ 0xA0,0x6A,0x30 },
			{ 0xCA,0x92,0x58 },{ 0xE0,0xAB,0x71 },{ 0xF7,0xC5,0x8B },{ 0xFF,0xFF,0xFF },
			// Brown
			{ 0x52,0x34,0x00 },{ 0x68,0x4A,0x00 },{ 0x7A,0x5E,0x16 },{ 0x8F,0x74,0x2B },
			{ 0xBA,0x9D,0x53 },{ 0xD0,0xB6,0x6C },{ 0xE6,0xD0,0x87 },{ 0xFF,0xFF,0xFF },
			// Yellow - Green
			{ 0x24,0x4E,0x00 },{ 0x3B,0x68,0x00 },{ 0x52,0x7B,0x14 },{ 0x67,0x91,0x28 },
			{ 0x90,0xBC,0x51 },{ 0xAA,0xD5,0x6A },{ 0xC3,0xEC,0x84 },{ 0xFF,0xFF,0xFF },
			// Pink
			{ 0x6D,0x14,0x38 },{ 0x87,0x2F,0x50 },{ 0x9D,0x44,0x65 },{ 0xB0,0x5A,0x79 },
			{ 0xDC,0x85,0xA2 },{ 0xF5,0x9E,0xBA },{ 0xFF,0xB8,0xD0 },{ 0xFF,0xFF,0xFF },
			// Blue - Green
			{ 0x00,0x51,0x31 },{ 0x16,0x6B,0x47 },{ 0x2B,0x80,0x5C },{ 0x41,0x95,0x71 },
			{ 0x6B,0xBF,0x9A },{ 0x85,0xD9,0xB4 },{ 0xA0,0xF0,0xCF },{ 0xFF,0xFF,0xFF },
			// Light Blue
			{ 0x10,0x35,0x86 },{ 0x2A,0x4F,0x9D },{ 0x40,0x64,0xB3 },{ 0x56,0x78,0xC8 },
			{ 0x7F,0xA2,0xF2 },{ 0x99,0xBC,0xFF },{ 0xB4,0xD2,0xFF },{ 0xFF,0xFF,0xFF },
			// Dark Blue
			{ 0x33,0x1F,0x9B },{ 0x4B,0x3A,0xB0 },{ 0x60,0x4F,0xC6 },{ 0x76,0x64,0xDB },
			{ 0x9D,0x8C,0xFF },{ 0xB6,0xA6,0xFF },{ 0xD0,0xC1,0xFF },{ 0xFF,0xFF,0xFF },
			// Light Green
			{ 0x13,0x54,0x00 },{ 0x22,0x63,0x00 },{ 0x2D,0x6E,0x00 },{ 0x3E,0x7F,0x07 },
			{ 0x64,0xA5,0x2D },{ 0x8A,0xCB,0x53 },{ 0xA4,0xE5,0x6D },{ 0xDF,0xFF,0xA8 }
		};

	// From a structure based on positions, to a structure based on luminance...
	unsigned int* cP = new unsigned int [128 + 5];
	for (size_t i = 0; i < 16; i++)
		for (size_t j = 0; j < 8; j++)
			cP [i + (j << 4)] = SDL_MapRGBA (_format, 
				_COLORS [(i << 3) + j].R, _COLORS [(i << 3) + j].G, _COLORS [(i << 3) + j].B, 0xe0);

	// More colors just to reflect events...
	cP [128] = SDL_MapRGBA (_format, 0X00, 0Xf5, 0xff, 0xff); // light Cyan
	cP [129] = SDL_MapRGBA (_format, 0Xfc, 0Xe7, 0x00, 0xff); // light Yellow
	cP [130] = SDL_MapRGBA (_format, 0Xff, 0X6d, 0x28, 0xff); // light Orange
	cP [131] = SDL_MapRGBA (_format, 0Xea, 0X04, 0x7e, 0xff); // light Purple
	cP [132] = SDL_MapRGBA (_format, 0X3e, 0Xc7, 0x0b, 0xff); // light Green

	return (new MCHEmul::ScreenMemory (numberColumns (), numberRows (), cP));
}

// ---
void COMMODORE::TED::treatRasterCycle (const DebugCycleContext& dC)
{
	COMMODORE::TEDRegisters::GraphicalInfo& gI =
		_TEDRegisters -> graphicalInfo ();

	switch (_cycleInRasterLine)
	{
		case 1:
			{
				_tedGraphicInfo._VC = gI._currentCharacterPosition;
				_tedGraphicInfo._VLMI = 0;
			}

			break;

		case 57:
			{
				if (gI._currentCharacterRow == 7)
				{
					gI._currentCharacterPosition =
						_tedGraphicInfo._VC & 0x03ff;

					// Reload the position before returning to idle.
					if (!_newBadLineCondition)
						_tedGraphicInfo._idleState = true;
				}

				gI._currentCharacterRow =
					(gI._currentCharacterRow + 1) & 0x07;
			}

			break;

		default:
			break;
	}

	if (_cycleInRasterLine >= 9 && _cycleInRasterLine <= 48)
	{
		// Re-entering the window must not advance beyond the 40-byte buffers.
		_tedGraphicInfo._VLMI = _cycleInRasterLine - 9;

		// Matrix and attribute fetches retain the existing badline condition.
		if (_newBadLineCondition)
		{
			readVideoMatrixAndColorRAM ();

			_IFDEBUG debugReadingVideoMatrix (dC);
		}

		readGraphicalInfo ();

		_IFDEBUG debugReadingGraphics (dC);

		if (!_tedGraphicInfo._idleState)
			_tedGraphicInfo._VC = (_tedGraphicInfo._VC + 1) & 0x03ff;
	}
}

// ---
void COMMODORE::TED::drawVisibleZone (unsigned int pixel)
{
	unsigned short column, row;
	_raster.currentVisiblePosition (column, row);

	MCHEmul::ScreenMemory* screen = screenMemory ();
	unsigned int color = _TEDRegisters -> borderColor ().asChar ();

	if (!_videoActive)
	{
		screen -> setPixel (column, row, color);

		return;
	}

	if (_raster.isInScreenZone ())
	{
		const unsigned int code = _drawResult._foregroundColorData [pixel];

		if (code == DrawResult::_TRANSPARENT)
			color = _drawResult._invalid
				? 0x00 : _TEDRegisters -> backgroundColor ().asChar ();
		else if ((code & DrawResult::_BACKGROUNDREFERENCE) != 0)
			color = _TEDRegisters ->
				backgroundColor ((size_t) (code & 0x03u)).asChar ();
		else
			color = code;
	}

	screen -> setPixel (column, row, color);

	// Preserve the diagnostic raster line, emitting its horizontal positions individually.
	if (_drawRasterInterruptPositions)
	{
		const unsigned short line =
			_raster.lineInVisibleZone (_TEDRegisters -> IRQRasterLineAt ());

		if (line <= _raster.vData ().lastVisiblePosition ())
		{
			screen -> setPixel (column, line, 130);

			const unsigned short initialColumn = (column >> 3) << 3;

			// Write the label after the block so its remaining dots do not overwrite it.
			if (pixel == 7 &&
				initialColumn >= 8 && initialColumn < 40 && line > 6)
				screen -> setString
					(0, line - 6,
					 std::to_string
						((unsigned int) _TEDRegisters -> IRQRasterLineAt ()) +
					 " " + std::to_string ((unsigned int) line), 131);
		}
	}
}

// ---
void COMMODORE::TED::drawOtherEvents (const EventMarker& marker)
{
	if (!_drawOtherEvents || !marker._visible || marker._events == 0)
		return;

	unsigned int color;

	// A combined request takes priority over its raster source and badline.
	if ((marker._events & EventMarker::_IRQ) != 0)
		color = 128;
	else if ((marker._events & EventMarker::_RASTER) != 0)
		color = 129;
	else
		color = 132;

	// End the marker at the event position: do not overwrite a future pixel.
	const unsigned short firstColumn =
		marker._column == 0 ? 0 : marker._column - 1;

	screenMemory () -> setHorizontalLine
		(firstColumn, marker._row,
		 marker._column == 0 ? 1 : 2, color);
}

// ---
COMMODORE::TED::DrawResult COMMODORE::TED::drawGraphics (const COMMODORE::TED::DrawContext& dC)
{
	// To draw is only valid in the graphical state...
	if (_tedGraphicInfo._idleState)
		return (COMMODORE::TED::DrawResult ());

	// The "display" column being involved...
	// In cb, the SCROLLX is involved, so it could be negative! starting from -7, 
	// When e.g. the raster is at the very first "screen "dislay" column, 
	// there are no reductions is the screen (display == screen) and SCROLLX = 0x00
	int cb = (int) dC._RCA - (int) dC._ICD - (int) dC._SC;
	// To draw just only when there is a possibility of at least draw a pixel!
	if (cb <= -8)
		return (COMMODORE::TED::DrawResult ());

	// At this point rc positive for sure, and cb could be negative...
	// Never invoke the methods within the swith case statements direcly
	// a crash might be generated...

	COMMODORE::TED::DrawResult result;
	switch (_TEDRegisters -> graphicModeActive ())
	{
		case COMMODORE::TEDRegisters::GraphicMode::_CHARMODE:
			{
				// Calculates whether the position of the cursor is or not visible...
				// ...as this is the only mode where the cursor can be drawn...
				unsigned short cp = _TEDRegisters -> cursorPosition (); // This is number between 0 and 1000...
				int cpy = ((((int) cp / 0x28)) << 3) + (int) _raster.vData ().firstDisplayPosition () 
							+ ((int) _TEDRegisters -> verticalScrollPosition () - 3);
				int cpx = (((cp % 0x28)) << 3) - dC._SC; // Not needed to add the border, but it can be negative because the scroll...
				bool dCA = cpx >= cb && cpx <= (cb + 7); // Is the first part of the cursor visible?
				bool dCB = (cpx + 7 + dC._SC) >= cb && (cpx + 7 + dC._SC) <= (cb + 7); // or the last one?
				int dCI = 0, dCN = 8; // From what position to draw, and how many pixles should be drawn...
				if (dCA) { dCI = dC._SC; dCN = 8 - dCI; }
				else if (dCB) { dCI = 0; dCN = dC._SC; }

				result = std::move (drawMonoColorChar (cb, 
					(dC._RR >= cpy && dC._RR < (cpy + 8)) && (dCA || dCB), dCI, dCN));
			}

			break;

		case COMMODORE::TEDRegisters::GraphicMode::_MULTICOLORCHARMODE:
			{
				result = std::move (drawMultiColorChar (cb));
			}

			break;

		case COMMODORE::TEDRegisters::GraphicMode::_EXTENDEDBACKGROUNDMODE:
			{
				result = std::move (drawMultiColorExtendedChar (cb));
			}

			break;

		case COMMODORE::TEDRegisters::GraphicMode::_BITMAPMODE:
			{
				result = std::move (drawMonoColorBitMap (cb));
			}

			break;

		case COMMODORE::TEDRegisters::GraphicMode::_MULTICOLORBITMAPMODE:
			{
				result = std::move (drawMultiColorBitMap (cb));
			}

			break;

		case COMMODORE::TEDRegisters::GraphicMode::_INVALIDTEXMODE:
			{
				// Like multicolor char mode, but invalid...
				result = std::move (drawMultiColorChar (cb, result._invalid = true));
			}

			break;

		case COMMODORE::TEDRegisters::GraphicMode::_INVALIDBITMAPMODE1:
			{
				// Like moncocolor bitmap mode, but invalid...
				result = std::move (drawMonoColorBitMap (cb, result._invalid = true));
			}
			break;

		case COMMODORE::TEDRegisters::GraphicMode::_INVALIDBITMAPMODE2:
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
COMMODORE::TED::DrawResult COMMODORE::TED::drawMonoColorChar
	(int cb, bool c, int dCI, int dCN)
{
	COMMODORE::TED::DrawResult result;

	// Attribute blinking and cursor visibility share the same FLASH phase.
	const bool flashOn = _TEDRegisters -> flashCounterOn ();

	for (int i = 0; i < 8; i++)
	{
		int pp = cb + i;
		if (pp < 0)
			continue;
		if (pp >= 320)
			break;

		size_t iBy = ((size_t) pp) >> 3;
		size_t iBt = 7 - (((size_t) pp) % 8);

		bool dP = _tedGraphicInfo._graphicData [iBy].bit (iBt);

		if (_tedGraphicInfo._screenCodeData [iBy].bit (7) &&
			_TEDRegisters -> reverseVideoActive ())
			dP = !dP;

		if (_tedGraphicInfo._colorData [iBy].bit (7) && flashOn)
			dP = false;

		if (c && !flashOn && i >= dCI && i < (dCI + dCN))
			dP = !dP;

		result._foregroundColorData [i] = dP
			? (unsigned int) (_tedGraphicInfo._colorData [iBy].value () & 0x7f)
			: DrawResult::_BACKGROUNDREFERENCE;
	}

	return (result);
}

// ---
COMMODORE::TED::DrawResult COMMODORE::TED::drawMultiColorChar (int cb, bool inv)
{
	COMMODORE::TED::DrawResult result;

	const bool reverseVideo = _TEDRegisters -> reverseVideoActive ();

	for (size_t i = 0; i < 8; i++)
	{
		int pp = cb + (int) i;
		if (pp < -1)
			continue;
		if (pp >= 320)
			break;

		size_t iBy = (pp >= 0) ? ((size_t) pp >> 3) : 0;

		if (_tedGraphicInfo._colorData [iBy].bit (3))
		{
			// Derive the pair from the source position, independently of output alignment.
			// This also handles a character-mode change inside the eight-pixel block.
			size_t iBt = (pp >= 0)
				? 3 - (((size_t) pp & 0x07) >> 1) : 3;
			unsigned char cs =
				(_tedGraphicInfo._graphicData [iBy].value () >> (iBt << 1)) & 0x03;

			if (_tedGraphicInfo._screenCodeData [iBy].bit (7) &&
				reverseVideo)
				cs = 0x03 - cs;

			if (inv)
				cs = 0x00;

			result._foregroundColorData [i] = (cs == 0x03)
				? (_tedGraphicInfo._colorData [iBy].value () & 0b01110111)
				: (DrawResult::_BACKGROUNDREFERENCE | (unsigned int) cs);
		}
		else
		{
			if (pp < 0)
				continue;

			size_t iBt = 7 - ((size_t) pp & 0x07);
			bool dP = _tedGraphicInfo._graphicData [iBy].bit (iBt);

			if (_tedGraphicInfo._screenCodeData [iBy].bit (7) &&
				reverseVideo)
				dP = !dP;

			result._foregroundColorData [i] = inv
				? 0x00
				: dP
					? (unsigned int) (_tedGraphicInfo._colorData [iBy].value () & 0x7f)
					: DrawResult::_BACKGROUNDREFERENCE;
		}
	}

	return (result);
}

// ---
COMMODORE::TED::DrawResult COMMODORE::TED::drawMultiColorExtendedChar (int cb)
{
	// The mode is quite similar to the standard text mode, 
	// with the difference the 0 pixels (background) can have different background colors...

	COMMODORE::TED::DrawResult result;

	for (int i = 0; i < 8 /** To paint always 8 pixels */; i++)
	{
		int pp = cb + i;
		if (pp < 0)
			continue;
		if (pp >= 320)
			break;

		// Calculate the byte and bit where the pixel info is...
		size_t iBy = ((size_t) pp) >> 3 /** To determine the byte. */;
		size_t iBt = 7 - (((size_t) pp) % 8); /** From MSB to LSB. */

		// Status of the pixel...
		// There is no neither blinking nor hardware cursor possibility in this mode either...
		// Pixel must be on or off?
		// In this mode there is no reverse mode or blinking...
		bool dP = _tedGraphicInfo._graphicData [iBy].bit (iBt);
		// The color of the pixel will depend on whether it is on or off...
		// ...and also on the 2 MSB bites of the screen code byte...
		// In the case of cs == 0x00 it will be redudant as it is already background, but just in case...
		unsigned int cs = ((_tedGraphicInfo._screenCodeData [iBy].value () & 0xc0) >> 6) & 0x03; // 0, 1, 2, or 3
		unsigned int fc = 
			dP 
				? (_tedGraphicInfo._colorData [iBy].value ()) 
				: (DrawResult::_BACKGROUNDREFERENCE | cs);
		// ...and draws it!!
		result._foregroundColorData [i] = fc;
	}

	return (result);
}

// ---
COMMODORE::TED::DrawResult COMMODORE::TED::drawMonoColorBitMap (int cb, bool inv)
{
	COMMODORE::TED::DrawResult result;

	for (int i = 0; i < 8 /** To paint always 8 pixels. */; i++)
	{
		int pp = cb + i;
		if (pp < 0)
			continue;
		if (pp >= 320)
			break;

		size_t iBy = ((size_t) pp) >> 3; // To determine the byte...
		size_t iBt = 7 - (((size_t) pp) % 8); // From MSB to LSB...
		bool bS = _tedGraphicInfo._graphicData [iBy].bit (iBt);

		// Determines the color as a function of the screen code and the attribute byte...
		unsigned int fc =
			inv
				? 0x00 // When invalid, all pixels are black...
				: bS
					? (((_tedGraphicInfo._screenCodeData[iBy].value() & 0xf0) >> 4) // Moved to color info...
						| ((_tedGraphicInfo._colorData[iBy].value() & 0x07) << 4))  // Moved to luminance info...
					: ((_tedGraphicInfo._screenCodeData[iBy].value() & 0x0f)		// Idem...
						| (_tedGraphicInfo._colorData[iBy].value() & 0x70));

		// ...and draws it!!
		result._foregroundColorData [i] = fc;
	}

	return (result);
}

// ---
COMMODORE::TED::DrawResult COMMODORE::TED::drawMultiColorBitMap (int cb, bool inv)
{
	COMMODORE::TED::DrawResult result;

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
			cs = (_tedGraphicInfo._graphicData [iBy].value () >> (iBt << 1)) & 0x03; // 0, 1, 2 or 3
		}
		// This is the case when pp == -1...
		else
			cs = (_tedGraphicInfo._graphicData [0].value () >> 6) & 0x03; // 0, 1, 2 or 3

		// Determines the color...
		unsigned fc = // The value 0x00 is not tested....
				inv
					? 0x00 // When invalid all pixels are black...
					: (cs == 0x00) // In the case of cs == 0x00 it will be redudant as it is already background, but just in case...
						? DrawResult::_BACKGROUNDREFERENCE
						: ((cs == 0x01) // The color is the defined in the video matrix (color) & color data (luminance)...
							? (((_tedGraphicInfo._screenCodeData [iBy].value () & 0xf0 /** bits 4, 5, 6 & 7. */) >> 4) /** to color position. */
								// There is a mistake in the TED documentation, it shows bits 4, 5 & 6 instead of bits 0, 1 & 2...
								| ((_tedGraphicInfo._colorData [iBy].value () & 0x07 /** bits 0, 1 & 2. */) << 4))
							: ((cs == 0x02) // The color is defined in the video matrix (color) & color data (luminance)...
								? ((_tedGraphicInfo._screenCodeData [iBy].value () & 0x0f /** bits 0, 1, 2 & 3 */)
									| (_tedGraphicInfo._colorData [iBy].value () & 0x70 /** bits 4, 5 & 6. */) /** to luminance position. */)
								: (DrawResult::_BACKGROUNDREFERENCE | 0x01u)));

		// ...and draws it!!
		result._foregroundColorData [i] = fc;
		result._foregroundColorData [i + 1] = fc;
	}

	return (result);
}

// ---
bool COMMODORE::TED::advanceToCPURead (MCHEmul::CPU* cpu)
{
	const MCHEmul::BusCycleData& busData =
		*_cpuTransaction._busData;

	unsigned int cycles = 0;

	if (busData._lastReadCycle != MCHEmul::BusCycleData::_NOCYCLE)
		cycles = (unsigned int) (busData._lastReadCycle + 1);

	// CPU execution has not produced buffered writes yet.
	if (busData._numberWriteCycles != 0 &&
		cycles > busData._firstWriteCycle)
		cycles = (unsigned int) (busData._firstWriteCycle);

	_cpuReadAdvanceCycles = cycles;

	_IFDEBUG debugCPUReadAdvance (cpu, false);

	const unsigned int batchStart = _cpuTransaction._startCycle;
	const unsigned int batchEnd =
		batchStart + (unsigned int) busData._numberCycles;

	if (cycles != 0)
		_raster.reduceDisplayZone
			(!_TEDRegisters -> textDisplay25RowsActive (),
			 !_TEDRegisters -> textDisplay40ColumnsActive ());

	for (unsigned int i = 0; i < cycles; ++i)
	{
		// Leave the final CPU cycle suspended immediately before its access.
		if (!simulateCPUCycle
			(cpu, batchStart, batchEnd, i + 1 != cycles))
			return (false);
	}

	_IFDEBUG debugCPUReadAdvance (cpu, true);

	return (true);
}

// ---
bool COMMODORE::TED::simulateCPUCycle
	(MCHEmul::CPU* cpu,
	 unsigned int batchStart, unsigned int batchEnd,
	 bool completeCycle)
{
	if (!_cpuCycleState._accessPending)
	{
		selectConfiguration ();

		// Capture the clock decision once; later writes cannot change this cycle.
		_cpuCycleState = COMMODORE::TED::CPUCycleState
			(_lastCPUCycles, _inSingleClockMode,
			 clockDivider (), _pendingHalfCyclesFromLastExecution);

		// Stop before the existing read-capture edge, leaving the bus write pending.
		if (!advanceCPUQuarters
			(cpu, _cpuCycleState.quarterDots () - 9,
			 batchStart, batchEnd))
			return (false);

		_cpuCycleState._accessPending = true;
	}

	if (!completeCycle)
		return (true);

	// A suspended cycle continues from its retained cursor and divider.
	if (!advanceCPUQuarters
		(cpu, _cpuCycleState.quarterDots (), batchStart, batchEnd))
		return (false);

	// Commit elapsed time only after the complete CPU cycle has been recovered.
	_pendingHalfCyclesFromLastExecution =
		(_cpuCycleState._halfCarryIn +
		 _cpuCycleState.halfTEDCycles ()) & 0x01;
	_pendingPacingCycles += _cpuCycleState.masterCycles ();

	publishRasterPosition ();

	_cpuCycleState._accessPending = false;
	++_lastCPUCycles;

	return (true);
}

// ---
bool COMMODORE::TED::advanceCPUQuarters
	(MCHEmul::CPU* cpu, unsigned int target,
	 unsigned int batchStart, unsigned int batchEnd)
{
	while (_cpuCycleState._nextQuarter < target)
	{
		const unsigned int part = _cpuCycleState.phase () & 0x03;

		if (part == 1 || part == 2)
		{
			// No counter, register or graphics event occurs on these edges.
			unsigned int count = 3 - part;
			if (count > target - _cpuCycleState._nextQuarter)
				count = target - _cpuCycleState._nextQuarter;

			_cpuCycleState._nextQuarter += count;

			continue;
		}

		if (!processCPUQuarter (cpu, batchStart, batchEnd))
			return (false);

		++_cpuCycleState._nextQuarter;
	}

	return (true);
}

// ---
bool COMMODORE::TED::processCPUQuarter
	(MCHEmul::CPU* cpu,
	 unsigned int batchStart, unsigned int batchEnd)
{
	const COMMODORE::TED::CPUCycleState& cS = _cpuCycleState;
	const unsigned int phase = cS.phase ();

	COMMODORE::TED::DebugCycleContext dC = { };

	_IFDEBUG
		dC = debugCycleContext (cS, batchStart, batchEnd);

	// Rearm the diagnostic marker when the combined request disappears.
	if (_drawOtherEvents && !_TEDRegisters -> launchIRQ ())
		_eventStatus._irqRequestMarked = false;

	if ((phase & 0x03) == 0)
	{
		if (phase == 0)
			_graphicsCycleActive = !_TEDRegisters -> freezeActive ();

		if (_graphicsCycleActive && !processRasterControl (dC))
			return (false);

		// A mismatch rearms the detector independently of IRQ acknowledgement.
		if (_rasterPosition._vertical != _TEDRegisters -> IRQRasterLineAt ())
			_rasterIRQMatched = false;

		if (phase == 0 && _graphicsCycleActive)
			beginTEDCycle (dC);

		return (true);
	}

	const unsigned short sampledLine = _rasterPosition._vertical;
	const bool matched = sampledLine == _TEDRegisters -> IRQRasterLineAt ();
	const bool cpuEnd =
		cS._nextQuarter + 1 == cS.quarterDots ();

	unsigned int writes = 0;

	if (cpuEnd && _cpuTimedWritesEnabled)
		if (!executePendingWriteAt (cpu, cS._cycle, writes))
			return (false);

	// Observe acknowledgement before a coincident source can become pending.
	if (_drawOtherEvents && !_TEDRegisters -> launchIRQ ())
		_eventStatus._irqRequestMarked = false;

	// Sample the operands preceding simultaneous CPU loads.
	// Acknowledgement wins over retaining a newly captured raster flag.
	const bool rasterRaised =
		sampleRasterIRQ (matched, (writes & _ACKRASTER) != 0);

	COMMODORE::TED::EventMarker marker;

	if (_drawOtherEvents)
	{
		marker._visible = _raster.isInVisibleZone ();
		if (marker._visible)
			_raster.currentVisiblePosition (marker._column, marker._row);

		if (_eventStatus._badLinePending)
			marker._events |= EventMarker::_BADLINE;

		if (rasterRaised)
			marker._events |= EventMarker::_RASTER;

		_eventStatus._badLinePending = false;
	}

	// Capture marker coordinates before presentation advancement.
	// Normal video is rendered before any diagnostic overlay.
	if (_graphicsCycleActive)
	{
		if (_raster.isInVisibleZone ())
			drawVisibleZone (phase >> 2);

		_raster.moveDot ();
	}

	clockRasterDot (phase, writes, dC);

	_IFDEBUG
		if (rasterRaised)
			debugRasterEvent (dC, "RasterIRQ", sampledLine);

	// A slow pulse can contain halves captured with different selected dividers.
	if (phase == 15 || phase == 31)
		_pendingSoundMasterCycles += cS.halfMasterCycles ();

	if (phase == 31)
	{
		if (_graphicsCycleActive)
			finishTEDCycle (cpu, cS._cycle, dC, marker);
		else
		{
			clockTEDCycle (false);

			_IFDEBUG debugFrozenTEDCycle (dC);
		}

		// The next CPU cycle uses the decision retained at this slow boundary.
		_inSingleClockMode = singleClockModeForCurrentCycle ();
	}

	if (phase != 31 || !_graphicsCycleActive)
	{
		if (rasterRaised || cpuEnd)
			if (requestIRQ (cpu, cS._cycle))
				marker._events |= EventMarker::_IRQ;

		if (_drawOtherEvents)
			drawOtherEvents (marker);
	}

	return (true);
}

// ---
void COMMODORE::TED::selectConfiguration ()
{
	const Configuration* configuration = &_configurations [_TEDRegisters -> PALActive () ? 0 : 1];
	if (_configuration == configuration)
		return;

	_configuration = configuration;
	_soundFunction -> setClockDivider (_configuration -> _divider);
	notify (MCHEmul::Event (_VIDEOCONFIGURATIONCHANGED));
}

// ---
bool COMMODORE::TED::applyPresentationConfiguration (const DebugCycleContext& dC)
{
	const MCHEmul::Raster previousRaster = _raster;
	_raster.configure (_configuration -> _vertical, _configuration -> _horizontal,
		_rasterPosition._videoLine, _rasterPosition._horizontal);
	_raster.reduceDisplayZone (!_TEDRegisters -> textDisplay25RowsActive (),
		!_TEDRegisters -> textDisplay40ColumnsActive ());

	// Allocate before discarding the old buffer so failure leaves its geometry usable.
	MCHEmul::ScreenMemory* screenMemory = createScreenMemory ();
	if (screenMemory == nullptr)
	{
		_raster = previousRaster;
		_error = MCHEmul::_CHIP_ERROR;
		return (false);
	}

	for (size_t row = 0; row < screenMemory -> rows (); row++)
		screenMemory -> setHorizontalLine (0, row, screenMemory -> columns (), 0);

	delete (_screenMemory);
	_screenMemory = screenMemory;
	_drawResult = { };
	_presentationConfiguration = _configuration;
	_lastVBlankEntered = false;
	_eventStatus._badLinePending = false;

	_IFDEBUG debugRasterEvent (dC, "RasterConfiguration", _rasterPosition._videoLine);
	notify (MCHEmul::Event (_VIDEOCONFIGURATIONCHANGED));

	return (true);
}

// ---
bool COMMODORE::TED::processRasterControl (const DebugCycleContext& dC)
{
	switch (_rasterPosition._horizontal)
	{
		case 348:
			{
				_flashClockPending = true;
			}

			break;

		case 384:
			{
				_verticalClockPending = true;
			}

			break;

		case 392:
			{
				const unsigned short previousLine = _rasterPosition._videoLine;

				// The previous capture qualifies this reset independently
				// of a coincident vertical-counter load.
				if (previousLine == _configuration -> _endOfScreen)
				{
					_TEDRegisters -> graphicalInfo ()._currentCharacterPosition = 0;
					_tedGraphicInfo._VC = 0;
				}

				_rasterPosition._videoLine = _rasterPosition._vertical;
				_lastBadLineScrollY = -1;
				_newBadLineCondition = false;

				_IFDEBUG
					debugRasterEvent (dC, "VideoLineLatch", previousLine);

				// Switch presentation at its new frame origin without resetting hardware counters.
				if (_configuration != _presentationConfiguration &&
					_rasterPosition._videoLine == _configuration -> _vertical.firstPosition () &&
					!applyPresentationConfiguration (dC))
					return (false);
			}

			break;

		default:
			break;
	}

	return (true);
}

// ---
void COMMODORE::TED::clockRasterDot
	(unsigned int phase, unsigned int writes, const DebugCycleContext& dC)
{
	if (_rasterPosition._horizontalWritePending &&
		(writes & _WRITEHORIZONTAL) == 0)
	{
		// The low phase advances naturally at the delayed load edge.
		_rasterPosition._horizontal =
			((_rasterPosition._horizontal + 1) & 0x0007) |
			(((_rasterPosition._horizontalWriteValue ^ 0xff) & 0xfc) << 1);
		_rasterPosition._horizontalWritePending = false;

		_IFDEBUG
			debugRasterEvent (dC, "HorizontalLoad", _rasterPosition._vertical);
	}
	else if (_graphicsCycleActive)
		_rasterPosition._horizontal =
			(_rasterPosition._horizontal == 455)
				? 0 : ((_rasterPosition._horizontal + 1) & 0x01ff);

	if (phase != 31)
		return;

	if (_graphicsCycleActive)
	{
		if (_verticalClockPending && (writes & _WRITEVERTICAL) == 0)
		{
			const unsigned short previousLine = _rasterPosition._vertical;

			_rasterPosition._vertical =
				(previousLine == _configuration -> _endOfScreen)
					? 0 : ((previousLine + 1) & 0x01ff);

			_IFDEBUG
				debugRasterEvent (dC, "VerticalClock", previousLine);
		}

		if (_flashClockPending &&
			_rasterPosition._videoLine == 205 &&
			(writes & _WRITEFLASH) == 0)
		{
			_TEDRegisters -> incrementFlashCounter ();

			_IFDEBUG
				debugRasterEvent (dC, "FlashClock", _rasterPosition._videoLine);
		}
	}

	_verticalClockPending = _flashClockPending = false;
}

// ---
void COMMODORE::TED::beginTEDCycle (const DebugCycleContext& dC)
{
	_cycleInRasterLine = rasterCycle ();

	_IFDEBUG debugTEDCycle (dC);

	_videoActive = (_rasterPosition._videoLine == _FIRSTBADLINE)
		? !_TEDRegisters -> blankEntireScreen () : _videoActive;

	if (detectNewBadLine ())
	{
		_newBadLineCondition = true;
		_tedGraphicInfo._idleState = false;
		_TEDRegisters -> graphicalInfo ()._currentCharacterRow = 0;
		// Keep only a visible event awaiting the first following emission edge.
		_eventStatus._badLinePending =
			_drawOtherEvents && _raster.isInVisibleZone ();

		_IFDEBUG debugBadLine (dC);
	}

	treatRasterCycle (dC);

	// The decoded graphics remain valid until the next slow-pulse preparation.
	_drawResult = { };

	if (_videoActive && _raster.isInVisibleZone ())
	{
		unsigned short column, row;
		_raster.currentVisiblePosition (column, row);

		const COMMODORE::TED::DrawContext drawContext =
		{
			/** _ICD */ _raster.hData ().firstDisplayPosition (),
			/** _SC  */ _TEDRegisters -> horizontalScrollPosition (),
			/** _RCA */ (unsigned short) ((column >> 3) << 3),
			/** _RR  */ row };

		_drawResult = drawGraphics (drawContext);
	}

	_IFDEBUG debugPreparedGraphics (dC);
}

// ---
void COMMODORE::TED::finishTEDCycle
	(MCHEmul::CPU* cpu, unsigned int requestCycle,
	 const DebugCycleContext& dC, EventMarker& marker)
{
	clockTEDCycle (true);

	if (requestIRQ (cpu, requestCycle))
		marker._events |= EventMarker::_IRQ;

	_IFDEBUG debugTEDCycleEnd (dC);

	if (_drawOtherEvents)
		drawOtherEvents (marker);

	if (_raster.isInLastVBlankZone ())
	{
		if (!_lastVBlankEntered)
		{
			_lastVBlankEntered = true;

			_IFDEBUG debugRasterEvent
				(dC, "FrameReady", _rasterPosition._videoLine);

			notify (MCHEmul::Event (_GRAPHICSREADY));
		}
	}
	else
		_lastVBlankEntered = false;
}

// ---
bool COMMODORE::TED::requestIRQ (MCHEmul::CPU* cpu, unsigned int cycle)
{
	const int reason = (int) _TEDRegisters -> reasonIRQCode ();

	if (reason == 0)
	{
		if (_drawOtherEvents)
			_eventStatus._irqRequestMarked = false;

		return (false);
	}

	cpu -> requestInterrupt (F6500::IRQInterrupt::_ID, cycle, this, reason);

	if (!_drawOtherEvents || _eventStatus._irqRequestMarked)
		return (false);

	_eventStatus._irqRequestMarked = true;

	return (true);
}

// ---
bool COMMODORE::TED::extractPendingWrites ()
{
	assert (_pendingWrites.empty () && _nextPendingWrite == 0);

	if (_timedWriteCPU == nullptr ||
		_cpuTransaction._cpu != _timedWriteCPU ||
		_cpuTransaction._busData == nullptr)
	{
		_error = MCHEmul::_CHIP_ERROR;

		return (false);
	}

	MCHEmul::Memory::configuration ().extractMemorySetCommandsBuffered
		(_timedWriteSubsets, _pendingWrites);

	// Matching every write is required before assigning bus-cycle coordinates.
	if (_pendingWrites.size () !=
		_cpuTransaction._busData -> _numberWriteCycles)
	{
		_error = MCHEmul::_CHIP_ERROR;

		return (false);
	}

	return (true);
}

// ---
bool COMMODORE::TED::executePendingWriteAt
	(MCHEmul::CPU* cpu, unsigned int cycle, unsigned int& rasterWrites)
{
	rasterWrites = 0;

	if (_nextPendingWrite == _pendingWrites.size () ||
		cycle != pendingWriteCycle ())
		return (true);

	MCHEmul::SetMemoryCommand& command = _pendingWrites [_nextPendingWrite];

	const bool tedWrite = command.subset () == _TEDRegisters;
	const size_t position = tedWrite ? command.position () % 0x20 : 0;

	// Queued audio pulses retain the parameters that preceded this bus write.
	if (tedWrite &&
		(position == 0x07 || (position >= 0x0e && position <= 0x12)))
	{
		if (!_soundFunction -> simulateBeforeCPUWrite (cpu, cycle))
		{
			_error = MCHEmul::_CHIP_ERROR;

			return (false);
		}
	}

	if (tedWrite)
	{
		switch (position)
		{
			case 0x09: rasterWrites = command.value ().bit (1) ? _ACKRASTER : 0; break;
			case 0x1c:
			case 0x1d: rasterWrites = _WRITEVERTICAL; break;
			case 0x1e: rasterWrites = _WRITEHORIZONTAL; break;
			case 0x1f: rasterWrites = _WRITEFLASH; break;
			default: break;
		}
	}

	_IFDEBUG debugCPUWrite (cpu, cycle, command, false);

	command.execute ();
	applyCPUWriteEffects (command);

	if (tedWrite && position == 0x07)
		selectConfiguration ();

	if (tedWrite && (position == 0x06 || position == 0x07))
		_raster.reduceDisplayZone
			(!_TEDRegisters -> textDisplay25RowsActive (),
			 !_TEDRegisters -> textDisplay40ColumnsActive ());

	_IFDEBUG debugCPUWrite (cpu, cycle, command, true);

	++_nextPendingWrite;

	return (true);
}

//-----
// ---
std::string COMMODORE::TED::debugRasterAsString () const
{
	return ("Column=" + std::to_string (_raster.currentColumn ()) +
		",ColumnBase0=" + std::to_string (_raster.currentColumnAtBase0 ()) +
		",Row=" + std::to_string (_raster.currentLine ()) +
		",RowBase0=" + std::to_string (_raster.currentLineAtBase0 ()) +
		",Cycle=" + std::to_string (_cycleInRasterLine) +
		",CounterColumn=" + std::to_string (_rasterPosition._horizontal) +
		",CounterRow=" + std::to_string (_rasterPosition._vertical) +
		",VideoLine=" + std::to_string (_rasterPosition._videoLine) +
		",CounterCycle=" + std::to_string (rasterCycle ()));
}

// ---
std::string COMMODORE::TED::debugTimingAsString (const DebugCycleContext& dC) const
{
	return ("CPUHalf=" + std::to_string (dC._cpuHalf) +
		",CPUQuarter=" + std::to_string (dC._cpuQuarter) +
		",CPUQuarterLength=" + std::to_string (dC._cpuQuarterLength) +
		",ClockDivider=" + std::to_string (dC._clockDivider) +
		",MasterCycles=" + std::to_string (dC._masterCycles) +
		",Reference=TEDPhaseEvent");
}

// ---
std::string COMMODORE::TED::debugClockAsString () const
{
	return ("SingleClock=" + std::to_string (_inSingleClockMode) +
		",ForcedSingleClock=" + std::to_string (_TEDRegisters -> singleClockModeForced ()) +
		",Freeze=" + std::to_string (_TEDRegisters -> freezeActive ()) +
		",SelectedPAL=" + std::to_string (_configuration == &_configurations [0]) +
		",PresentationPAL=" + std::to_string (_presentationConfiguration == &_configurations [0]) +
		",PresentationPending=" + std::to_string (_configuration != _presentationConfiguration) +
		",SelectedDivider=" + std::to_string (_configuration -> _divider) +
		",GraphicsCycleActive=" + std::to_string (_graphicsCycleActive));
}

// ---
std::string COMMODORE::TED::debugInternalAsString () const
{
	return ("VCReload=" + std::to_string
		(_TEDRegisters -> graphicalInfo ()._currentCharacterPosition) + "," +
		"VC=" + std::to_string (_tedGraphicInfo._VC) + "," +
		"RC=" + std::to_string (_TEDRegisters -> graphicalInfo ()._currentCharacterRow) + "," +
		"VLMI=" + std::to_string (_tedGraphicInfo._VLMI) + "," +
		"Idle=" + std::to_string (_tedGraphicInfo._idleState));
}

// ---
std::string COMMODORE::TED::debugTimersAsString () const
{
	std::string result;
	const COMMODORE::TEDTimer* timers [3] = { &_T1, &_T2, &_T3 };
	for (size_t i = 0; i < 3; i++)
	{
		if (i != 0)
			result += ",";
		const std::string prefix = "T" + std::to_string (i + 1);
		result += prefix + "Counter=" + std::to_string (timers [i] -> currentValue ()) + "," +
			prefix + "Reload=" + std::to_string (timers [i] -> reloadValue ()) + "," +
			prefix + "Counting=" + std::to_string (timers [i] -> _counting) + "," +
			prefix + "Pending=" + std::to_string (timers [i] -> interruptRequested ()) + "," +
			prefix + "Enabled=" + std::to_string (timers [i] -> interruptEnabled ()) + "," +
			prefix + "ReachedZero=" + std::to_string (timers [i] -> reaches0 ());
	}

	return (result);
}

// ---
COMMODORE::TED::DebugCycleContext COMMODORE::TED::debugCycleContext
	(const CPUCycleState& cS,
	 unsigned int batchStart, unsigned int batchEnd) const
{
	COMMODORE::TED::DebugCycleContext result = { };

	result._cpuQuarterLength = cS.quarterDots ();
	result._clockDivider = cS._clockDivider;
	result._masterCycles = cS.masterCycles ();
	result._cpuCycle =
		cS._cycle + ((cS._nextQuarter + 1) / result._cpuQuarterLength);
	result._cpuQuarter =
		(cS._nextQuarter + 1) % result._cpuQuarterLength;
	result._cpuHalf =
		(result._cpuQuarter << 1) / result._cpuQuarterLength;
	result._cpuBatchStart = batchStart;
	result._cpuBatchEnd = batchEnd;
	result._cpuBatchIndex = cS._cycle - batchStart;
	result._cpuBatchCycles = batchEnd - batchStart;
	result._halfCarryIn = cS._halfCarryIn;
	result._halfCarryOut =
		(cS._halfCarryIn + cS.halfTEDCycles ()) & 0x01;
	result._conversionSingleClock = cS._singleClock;
	result._irqRequestCycle = cS._cycle;

	return (result);
}

// ---
void COMMODORE::TED::debugTEDCycle (const DebugCycleContext& dC)
{
	_deepDebugFile -> writeCompleteLine (className (), dC._cpuCycle, "Info Cycle",
		{ { "Timing",
			debugTimingAsString (dC) + ",Snapshot=BeforeGraphics," +
			"CPUBatchStart=" + std::to_string (dC._cpuBatchStart) + "," +
			"CPUBatchEnd=" + std::to_string (dC._cpuBatchEnd) + "," +
			"CPUBatchIndex=" + std::to_string (dC._cpuBatchIndex) + "," +
			"CPUBatchCycles=" + std::to_string (dC._cpuBatchCycles) + "," +
			"ConversionSingleClock=" + std::to_string (dC._conversionSingleClock) + "," +
			"HalfCarryIn=" + std::to_string (dC._halfCarryIn) + "," +
			"HalfCarryOut=" + std::to_string (dC._halfCarryOut) },
		  { "Raster position", debugRasterAsString () },
		  { "Internal", debugInternalAsString () },
		  { "Display",
			"DisplayEnabled=" + std::to_string (!_TEDRegisters -> blankEntireScreen ()) + "," +
			"VideoActive=" + std::to_string (_videoActive) + "," +
			"ScrollX=" + std::to_string (_TEDRegisters -> horizontalScrollPosition ()) + "," +
			"ScrollY=" + std::to_string (_TEDRegisters -> verticalScrollPosition ()) + "," +
			"Rows25=" + std::to_string (_TEDRegisters -> textDisplay25RowsActive ()) + "," +
			"Columns40=" + std::to_string (_TEDRegisters -> textDisplay40ColumnsActive ()) },
		  { "Badline",
			"Latched=" + std::to_string (_newBadLineCondition) + "," +
			"LastScrollY=" + std::to_string (_lastBadLineScrollY) + "," +
			"WithinBadlineRange=" + std::to_string
				(_rasterPosition._videoLine >= _FIRSTBADLINE && _rasterPosition._videoLine <= _LASTBADLINE) },
		  { "Clock", debugClockAsString () },
		  { "Graphics mode", "Mode=" + std::to_string ((int) _TEDRegisters -> graphicModeActive ()) },
		  { "Memory",
			"Screen=$" + MCHEmul::removeAll0 (_TEDRegisters -> screenMemory ().asString
				(MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) + "," +
			"Attributes=$" + MCHEmul::removeAll0 (_TEDRegisters -> attributeMemory ().asString
				(MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) + "," +
			"Characters=$" + MCHEmul::removeAll0 (_TEDRegisters -> charDataMemory ().asString
				(MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) + "," +
			"Bitmap=$" + MCHEmul::removeAll0 (_TEDRegisters -> bitmapMemory ().asString
				(MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) + "," +
			"CPUROMActive=" + std::to_string (_TEDRegisters -> ROMMemoryConfigurationActive ()) + "," +
			"GraphicsROMSelected=" + std::to_string (_TEDRegisters -> ROMActiveToFetchCharAndBitmap ()) },
		  { "Timers", debugTimersAsString () },
		  { "Interrupt", debugInterruptAsString () } });
}

// ---
void COMMODORE::TED::debugCPUAboutToExecute
	(const MCHEmul::InstructionContextEventData* dt)
{
	_deepDebugFile -> writeCompleteLine
		(className (), dt -> _cpu -> clockCycles (),
		 "CPU instruction transaction",
		{ { "Transaction",
			"Type=Instruction," +
			std::string ("Address=$") +
				MCHEmul::removeAll0 (dt -> _address.asString
					(MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) + "," +
			"Code=" + std::to_string (dt -> _instruction -> code ()) + "," +
			"Cycles=" + std::to_string (dt -> _clockCycles) },
		  { "Bus cycles", debugCPUTransactionAsString () },
		  { "Raster origin",
			"Row=" + std::to_string (_raster.currentLine ()) + "," +
			"Cycle=" + std::to_string (_cycleInRasterLine) },
		  { "Clock",
			"SingleClock=" + std::to_string (_inSingleClockMode) } });
}

// ---
void COMMODORE::TED::debugCPUAboutToExecute
	(const MCHEmul::InterruptContextEventData* dt)
{
	_deepDebugFile -> writeCompleteLine
		(className (), _cpuTransaction._startCycle,
		 "CPU interrupt transaction",
		{ { "Transaction",
			"Type=Interrupt,Id=" +
				std::to_string (dt -> _interrupt -> id ()) },
		  { "Bus cycles", debugCPUTransactionAsString () },
		  { "Raster origin",
			"Row=" + std::to_string (_raster.currentLine ()) + "," +
			"Cycle=" + std::to_string (_cycleInRasterLine) } });
}

// ---
std::string COMMODORE::TED::debugCPUTransactionAsString () const
{
	const MCHEmul::BusCycleData& busData = *_cpuTransaction._busData;

	return
		("StartCPU=" + std::to_string (_cpuTransaction._startCycle) + "," +
		 "IntrinsicCycles=" + std::to_string (busData._numberCycles) + "," +
		 "ReadCycles=" + std::to_string (busData._numberReadCycles) + "," +
		 "WriteCycles=" + std::to_string (busData._numberWriteCycles) + "," +
		 "FirstWriteCycle=" +
			(busData._firstWriteCycle == MCHEmul::BusCycleData::_NOCYCLE
				? std::string ("-")
				: std::to_string (busData._firstWriteCycle)) + "," +
		 "LastReadCycle=" +
			(busData._lastReadCycle == MCHEmul::BusCycleData::_NOCYCLE
				? std::string ("-")
				: std::to_string (busData._lastReadCycle)) + "," +
		 "TrailingWriteCycles=" + std::to_string (busData._trailingWriteCycles));
}

// ---
void COMMODORE::TED::debugCPUReadAdvance
	(MCHEmul::CPU* cpu, bool after) const
{
	_deepDebugFile -> writeCompleteLine
		(className (), cpu -> clockCycles (), "CPU read advance",
		{ { "Timing",
			"Reference=ObservedCPUClock,StartCPU=" +
				std::to_string (_cpuTransaction._startCycle) +
			",RecoveryCPU=" + std::to_string (_lastCPUCycles) },
		  { "Advance",
			"Phase=" + std::string (after ? "After" : "Before") +
			",PreparedCPUCycles=" + std::to_string (_cpuReadAdvanceCycles) +
			",NextQuarter=" + std::to_string (_cpuCycleState._nextQuarter) +
			",AccessPending=" + std::to_string
				(_cpuCycleState._accessPending) },
		  { "Raster position", debugRasterAsString () },
		  { "Clock",
			"HalfCarry=" +
				std::to_string (_pendingHalfCyclesFromLastExecution) },
		  { "Timers", debugTimersAsString () } });
}

// ---
void COMMODORE::TED::debugBadLine (const DebugCycleContext& dC)
{
	_deepDebugFile -> writeCompleteLine (className (), dC._cpuCycle, "Badline detected",
		{ { "Timing", debugTimingAsString (dC) },
		  { "Raster position", debugRasterAsString () },
		  { "Internal", debugInternalAsString () },
		  { "Badline", "Latched=" + std::to_string (_newBadLineCondition) + "," +
			"ScrollY=" + std::to_string (_TEDRegisters -> verticalScrollPosition ()) + "," +
			"VideoActive=" + std::to_string (_videoActive) + ",Cause=ScrollMatch" } });
}

// ---
void COMMODORE::TED::debugReadingVideoMatrix (const DebugCycleContext& dC)
{
	const size_t vc = (size_t) _tedGraphicInfo._VC & 0x03ff;
	_deepDebugFile -> writeCompleteLine (className (), dC._cpuCycle, "Video matrix read",
		{ { "Timing", debugTimingAsString (dC) },
		  { "Raster position", debugRasterAsString () },
		  { "Fetch", debugInternalAsString () },
		  { "Matrix", "Address=$" + MCHEmul::removeAll0
			((_TEDRegisters -> screenMemory () + vc).asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) + "," +
			"Value=$" + _tedGraphicInfo._lastScreenCodeDataRead.asString (MCHEmul::UByte::OutputFormat::_HEXA, 2) + ",Source=RAM" },
		  { "Attribute", "Address=$" + MCHEmul::removeAll0
			((_TEDRegisters -> attributeMemory () + vc).asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) + "," +
			"Value=$" + _tedGraphicInfo._lastColorDataRead.asString (MCHEmul::UByte::OutputFormat::_HEXA, 2) + ",Source=RAM" } });
}

// ---
void COMMODORE::TED::debugReadingGraphics (const DebugCycleContext& dC)
{
	MCHEmul::Address address = _TEDRegisters -> graphicExtendedColorTextModeActive ()
		? _MEMORYPOSIDLE1 : _MEMORYPOSIDLE2;
	std::string kind = "Idle", source = "TEDView", characterData;
	if (!_tedGraphicInfo._idleState)
	{
		source = _TEDRegisters -> ROMActiveToFetchCharAndBitmap () ? "ROM" : "RAM";
		if (_TEDRegisters -> textMode ())
		{
			const unsigned char mask = _TEDRegisters -> characterIndexMask ();
			const unsigned char code = _tedGraphicInfo._screenCodeData [_tedGraphicInfo._VLMI].value ();
			kind = "Character";
			address = _TEDRegisters -> charDataMemory () + ((size_t) (code & mask) << 3) +
				_TEDRegisters -> graphicalInfo ()._currentCharacterRow;
			characterData = ",CharacterCode=" + std::to_string (code) + "," +
				"CharacterMask=" + std::to_string (mask) + ",CharacterIndex=" + std::to_string (code & mask);
		}
		else
		{
			kind = "Bitmap";
			address = _TEDRegisters -> bitmapMemory () + ((size_t) (_tedGraphicInfo._VC & 0x03ff) << 3) +
				_TEDRegisters -> graphicalInfo ()._currentCharacterRow;
		}
	}

	_deepDebugFile -> writeCompleteLine (className (), dC._cpuCycle, "Graphics read",
		{ { "Timing", debugTimingAsString (dC) },
		  { "Raster position", debugRasterAsString () },
		  { "Fetch", debugInternalAsString () },
		  { "Memory access", "Kind=" + kind + ",Source=" + source + ",Address=$" +
			MCHEmul::removeAll0 (address.asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) + ",Value=$" +
			_tedGraphicInfo._lastGraphicDataRead.asString (MCHEmul::UByte::OutputFormat::_HEXA, 2) + characterData } });
}

// ---
void COMMODORE::TED::debugPreparedGraphics (const DebugCycleContext& dC) const
{
	std::string output;
	for (size_t i = 0; i < 8; i++)
	{
		if (i != 0)
			output += ",";

		// Preserve background references: their palette values are resolved at emission.
		const unsigned int code = _drawResult._foregroundColorData [i];
		output += "P" + std::to_string (i) + "=" +
			(code == DrawResult::_TRANSPARENT
				? "T" : ((code & DrawResult::_BACKGROUNDREFERENCE) != 0)
					? "B" + std::to_string (code & 0x03u) : "C" + std::to_string (code));
	}

	_deepDebugFile -> writeCompleteLine
		(className (), dC._cpuCycle, "Graphics prepared",
		{ { "Timing", debugTimingAsString (dC) + ",Snapshot=AfterPreparation" },
		  { "Raster position", debugRasterAsString () },
		  { "Internal", debugInternalAsString () },
		  { "Preparation",
			"Decoded=" + std::to_string (_videoActive && _raster.isInVisibleZone ()) +
			",Mode=" + std::to_string ((int) _TEDRegisters -> graphicModeActive ()) +
			",ScrollX=" + std::to_string (_TEDRegisters -> horizontalScrollPosition ()) +
			",FlashPhase=" + std::to_string (_TEDRegisters -> flashCounterOn ()) },
		  { "Prepared output", output + ",Invalid=" + std::to_string (_drawResult._invalid) },
		  { "Clock", debugClockAsString () } });
}

// ---
void COMMODORE::TED::debugTEDCycleEnd (const DebugCycleContext& dC)
{
	_deepDebugFile -> writeCompleteLine (className (), dC._cpuCycle, "Info Cycle End",
		{ { "Timing", debugTimingAsString (dC) + ",Snapshot=AfterGraphics" },
		  { "Raster position", debugRasterAsString () },
		  { "Internal", debugInternalAsString () },
		  { "Clock", debugClockAsString () },
		  { "Timers", debugTimersAsString () },
		  { "Interrupt", debugInterruptAsString () + "," +
			"CPURequestClockArgument=" + (_TEDRegisters -> reasonIRQCode () != 0
				? std::to_string (dC._irqRequestCycle) : std::string ("-")) } });
}

// ---
void COMMODORE::TED::debugFrozenTEDCycle
	(const DebugCycleContext& dC) const
{
	_deepDebugFile -> writeCompleteLine
		(className (), dC._cpuCycle, "Frozen clock",
		{ { "Timing",
			debugTimingAsString (dC) },
		  { "Raster position", debugRasterAsString () },
		  { "Clock distribution",
			"RasterEnabled=0,TimersEnabled=0,SoundEnabled=1" },
		  { "Interrupt", debugInterruptAsString () } });
}

// ---
void COMMODORE::TED::debugCPUWrite
	(MCHEmul::CPU* cpu, unsigned int cycle,
	 const MCHEmul::SetMemoryCommand& command, bool after) const
{
	const unsigned int code =
		_drawResult._foregroundColorData [_cpuCycleState.phase () >> 2];
	const std::string preparedCode = code == DrawResult::_TRANSPARENT
		? "T" : ((code & DrawResult::_BACKGROUNDREFERENCE) != 0)
			? "B" + std::to_string (code & 0x03u) : "C" + std::to_string (code);

	_deepDebugFile -> writeCompleteLine
		(className (), cycle, "CPU write",
		{ { "Timing",
			"Reference=ScheduledCPUCycle,ObservedCPUClock=" +
				std::to_string (cpu -> clockCycles ()) +
			",BoundaryCPU=" + std::to_string (cycle + 1) +
			",CPUQuarterLength=" + std::to_string (_cpuCycleState.quarterDots ()) +
			",TEDPhase=" + std::to_string (_cpuCycleState.phase ()) +
			",PixelIndex=" + std::to_string (_cpuCycleState.phase () >> 2) +
			",RetainedDivider=" + std::to_string (_cpuCycleState._clockDivider) +
			",RetainedSingleClock=" + std::to_string (_cpuCycleState._singleClock) },
		  { "Transaction",
			"Type=" + std::string
				(_cpuTransaction._isInterrupt ? "Interrupt" : "Instruction") +
			",StartCPU=" + std::to_string (_cpuTransaction._startCycle) +
			",WriteIndex=" + std::to_string (_nextPendingWrite) },
		  { "Memory access",
			"Phase=" + std::string (after ? "After" : "Before") +
			",Subset=" + std::to_string (command.subset () -> id ()) +
			",Address=$" + MCHEmul::removeAll0
				((command.subset () -> initialAddress () + command.position ()).
					asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) +
			",Value=$" + command.value ().asString
				(MCHEmul::UByte::OutputFormat::_HEXA, 2) },
		  { "Raster position", debugRasterAsString () },
		  { "Clock", debugClockAsString () },
		  { "Output",
			"GraphicsCycleActive=" + std::to_string (_graphicsCycleActive) +
			",VideoActive=" + std::to_string (_videoActive) +
			",Visible=" + std::to_string (_raster.isInVisibleZone ()) +
			",Screen=" + std::to_string (_raster.isInScreenZone ()) +
			",PixelIndex=" + std::to_string (_cpuCycleState.phase () >> 2) +
			",PreparedCode=" + preparedCode +
			",Invalid=" + std::to_string (_drawResult._invalid) },
		  { "Palette",
			"Background0=" + std::to_string (_TEDRegisters -> backgroundColor (0).asChar ()) +
			",Background1=" + std::to_string (_TEDRegisters -> backgroundColor (1).asChar ()) +
			",Background2=" + std::to_string (_TEDRegisters -> backgroundColor (2).asChar ()) +
			",Background3=" + std::to_string (_TEDRegisters -> backgroundColor (3).asChar ()) +
			",Border=" + std::to_string (_TEDRegisters -> borderColor ().asChar ()) },
		  { "Interrupt", debugInterruptAsString () } });
}

// ---
void COMMODORE::TED::debugRasterEvent
	(const DebugCycleContext& dC,
	 const char* event, unsigned short sourceLine) const
{
	_deepDebugFile -> writeCompleteLine
		(className (), dC._cpuCycle, "Raster event",
		{ { "Timing", debugTimingAsString (dC) },
		  { "Event",
			std::string ("Kind=") + event +
			",SourceLine=" + std::to_string (sourceLine) },
		  { "Raster position", debugRasterAsString () },
		  { "Clock", debugClockAsString () },
		  { "Pending",
			"VerticalClock=" + std::to_string (_verticalClockPending) +
			",FlashClock=" + std::to_string (_flashClockPending) +
			",HorizontalWrite=" +
				std::to_string (_rasterPosition._horizontalWritePending) },
		  { "Interrupt", debugInterruptAsString () } });
}
//-----

// ---
COMMODORE::TED_PAL::TED_PAL (int intId, int vV, MCHEmul::SoundLibWrapper* wS,
	const TED::Configuration& pal, const TED::Configuration& ntsc)
	: COMMODORE::TED (intId, true, pal, ntsc, vV, wS,
		{ { "Name", "TED" },
		  { "Code", "7360/8360 for PAL" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Year", "1984" } })
{
	// Nothing else to do...
}

// ---
COMMODORE::TED_NTSC::TED_NTSC (int intId, int vV, MCHEmul::SoundLibWrapper* wS,
	const TED::Configuration& pal, const TED::Configuration& ntsc)
	: COMMODORE::TED (intId, false, pal, ntsc, vV, wS,
		{ { "Name", "TED" },
		  { "Code", "7360/8360 for NTSC" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Year", "1984" } })
{
	// Nothing else to do...
}
