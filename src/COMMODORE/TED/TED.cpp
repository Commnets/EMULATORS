#include <COMMODORE/TED/TED.hpp>
#include <COMMODORE/TED/TEDSoundWrapper.hpp>
#include <F6500/IRQInterrupt.hpp>

// ---
const MCHEmul::RasterData COMMODORE::TED_PAL::_VRASTERDATA (260, 274, 4, 203, 245, 259, 259, 312, 4, 4);
/** Fetch windows and displayed pixels use different horizontal positions. \n
	The selected timing profile prepares matrix-plane addresses from H=435
	and graphics addresses from H=439; captures follow at H=439 and H=443. \n
	Retained cell transfers place the 320 active output dots at H=0..319.
	The presentation crop H=424..351 preserves 384 visible dots with 32-dot borders. \n
	These defaults remain replaceable through Configuration; PAL and NTSC
	share horizontal geometry and retain their independent vertical profiles. */
const MCHEmul::RasterData COMMODORE::TED_PAL::_HRASTERDATA 
	(360, 424, 0, 319, 351, 359, 359, 456, 8, 8);
const MCHEmul::RasterData COMMODORE::TED_NTSC::_VRASTERDATA (235, 249, 4, 203, 220, 234, 234, 262, 4, 4);
const MCHEmul::RasterData COMMODORE::TED_NTSC::_HRASTERDATA 
	(360, 424, 0, 319, 351, 359, 359, 456, 8, 8);
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
	  _externalFetchClock (false), _refreshClock (false),
	  _dmaState (DMAState::_IDLE), _dmaWindow (false),
	  _cpuTransaction (),
	  _cpuTimedWrites (),
	  _cpuRecovery (),
	  _pendingSoundMasterCycles (0),
	  _format (nullptr),
	  _cycleInRasterLine (1),
	  _inSingleClockMode (false),
	  _lastVBlankEntered (false),
	  _lastBadLineScrollY (-1), _newBadLineCondition (false), 
	  _tedGraphicInfo (),
	  _videoFetchState (),
	  _pendingVideoRead (),
	  _videoOutput (),
	  _debugCycleContext (),
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
	_externalFetchClock = _refreshClock = false;
	_dmaState = DMAState::_IDLE;
	_dmaWindow = false;

	publishRasterPosition ();

	_cpuTransaction = { };
	_cpuTimedWrites = { };
	_cpuRecovery = { };
	_pendingSoundMasterCycles = 0;

	_cycleInRasterLine = 1;

	_inSingleClockMode = singleClockModeForCurrentCycle ();

	_lastVBlankEntered = false;

	_lastBadLineScrollY = -1;
	_newBadLineCondition = false;

	_tedGraphicInfo = { };
	_videoFetchState = { };
	_pendingVideoRead = { };
	_videoOutput = { };
	latchVideoControls ();
	_debugCycleContext = { };
	_eventStatus.reset (_raster.visibleColumns ());

	return (true);
}

// ---
bool COMMODORE::TED::prepareCPUTimedWrites (MCHEmul::CPU* cpu,
	 const MCHEmul::PhysicalStorageSubsets& subsets)
{
	assert (cpu != nullptr && _TEDRegisters != nullptr);
	assert (_cpuTimedWrites._cpu == nullptr && _cpuTimedWrites._commands.empty ());

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

	_cpuTimedWrites._cpu = cpu;
	_cpuTimedWrites._subsets = subsets;

	return (true);
}

// ---
bool COMMODORE::TED::enableCPUTimedWrites ()
{
	if (_cpuTimedWrites._cpu == nullptr || _cpuTimedWrites._enabled)
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	// Include board registers that disable buffering in their constructors.
	for (const auto& subset : _cpuTimedWrites._subsets)
		subset.second -> setBufferMemorySetCommands (true);

	_cpuTransaction = { };

	// Preserve the existing completed half-pulse remainder on activation.
	_cpuRecovery = CPURecoveryState
		(_cpuTimedWrites._cpu -> clockCycles (), _cpuRecovery._halfCarry);

	_cpuTimedWrites._enabled = true;

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

	if (_cpuTimedWrites._enabled && !advanceToCPURead (dt -> _cpu))
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

	if (_cpuTimedWrites._enabled)
	{
		if (cpu != _cpuTimedWrites._cpu || _error != MCHEmul::_NOERROR)
		{
			_error = MCHEmul::_CHIP_ERROR;

			return (false);
		}

		if (_cpuTransaction._pending && !_cpuTransaction._writesExtracted)
		{
			// Only the first recovery follows semantic instruction execution.
			// Later calls consume CPU stop time and keep the same buffered writes.
			if (batchEnd - _cpuTransaction._startCycle !=
					_cpuTransaction._busData -> _numberCycles)
			{
				_error = MCHEmul::_CHIP_ERROR;

				return (false);
			}

			if (!extractPendingWrites ())
				return (false);

			_cpuTransaction._writesExtracted = true;
		}
	}
	else if (_cpuRecovery._nextCycle == 0)
	{
		_cpuRecovery._nextCycle = batchEnd;

		return (true);
	}

	const unsigned int batchStart = _cpuTransaction._pending
		? _cpuTransaction._startCycle : _cpuRecovery._nextCycle;

	// An advanced prefix already sampled the unchanged display-zone registers.
	if (_cpuTransaction._readAdvanceCycles == 0)
		_raster.reduceDisplayZone
			(!_TEDRegisters -> textDisplay25RowsActive (),
			 !_TEDRegisters -> textDisplay40ColumnsActive ());

	// Prefix recovery may already include waits that the stopped CPU has yet
	// to consume. The signed modular distance also admits a CPU-clock wrap.
	while ((int) (batchEnd - _cpuRecovery._nextCycle) > 0)
	{
		if (!simulateCPUCycle (cpu, batchStart, batchEnd))
			return (false);
	}

	if (_cpuTransaction._pending)
	{
		updateCPUStop (cpu);

		if (_cpuTransaction._logicalCycle ==
				_cpuTransaction._busData -> _numberCycles &&
			!DMARequested () &&
			_cpuRecovery._nextCycle == batchEnd)
		{
			if (_cpuTimedWrites._next != _cpuTimedWrites._commands.size ())
			{
				_error = MCHEmul::_CHIP_ERROR;

				return (false);
			}

			_cpuTimedWrites.clearCommands ();
			_cpuTransaction.finish ();
		}
	}

	if (_cpuRecovery._nextCycle == batchEnd && _TEDRegisters -> freezeActive ())
		requestIRQ (cpu, batchEnd);

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
void COMMODORE::TED::drawVisibleZone (unsigned int pixel, bool inScreen)
{
	unsigned short column, row;
	_raster.currentVisiblePosition (column, row);

	MCHEmul::ScreenMemory* screen = screenMemory ();

	if (!_videoFetchState._displayActive)
	{
		screen -> setPixel
			(column, row, _TEDRegisters -> borderColor ().asChar ());

		return;
	}

	screen -> setPixel (column, row, inScreen
		? videoPixelColor () : _TEDRegisters -> borderColor ().asChar ());

	// Preserve the diagnostic raster line, emitting its horizontal positions individually.
	if (_drawRasterInterruptPositions && !_drawOtherEvents)
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
void COMMODORE::TED::drawOtherEvents ()
{
	const EventsStatus::Marker& marker = _eventStatus._marker;

	if (!_drawOtherEvents || !marker._rowVisible)
		return;

	MCHEmul::ScreenMemory* screen = screenMemory ();

	_eventStatus.beginRow (marker._row);

	if ((marker._events & EventsStatus::Marker::_RASTER) != 0)
	{
		_eventStatus._rasterPending = true;
		_eventStatus._rasterLine = marker._rasterLine;
	}

	if (marker._visible && marker._emitted)
	{
		unsigned int color = 0;

		if ((marker._events & EventsStatus::Marker::_BADLINE) != 0)
			color = 132;

		if ((marker._events & EventsStatus::Marker::_IRQ) != 0)
			color = 128;

		if (color != 0)
		{
			// Both pixels have already been emitted; never cross a row boundary.
			const unsigned short firstColumn =
				marker._column == 0 ? 0 : marker._column - 1;

			for (size_t x = firstColumn; x <= marker._column; x++)
			{
				_eventStatus._marks [x] = (unsigned char) color;
				screen -> setPixel (x, marker._row, color);
			}
		}

		if ((size_t) marker._column + 1 == screen -> columns ())
			_eventStatus._rowFinished = true;
	}

	// An event can arrive in the horizontal blank after the visible row ended.
	if (marker._afterVisible)
		_eventStatus._rowFinished = true;

	if (_eventStatus._rowFinished && _eventStatus._rasterPending)
	{
		drawRasterIRQLine
			(_eventStatus._row, _eventStatus._rasterLine);

		_eventStatus._rasterPending = false;
	}

	if (_eventStatus._rowFinished &&
		marker._row >= 4 && screen -> rows () >= 5)
	{
		for (size_t i = 0; i < 5; i++)
		{
			if (_eventStatus._earlyRasterLines [i] == EventsStatus::_NOROW)
				continue;

			const std::string label = std::to_string
				((unsigned int) _eventStatus._earlyRasterLines [i]);
			const size_t width = label.size () * 6 + 2;
			const size_t x = i * 24;

			if (x + width <= screen -> columns ())
			{
				screen -> setBox (x, 0, width, 5, 130);
				screen -> setString (x, 0, label, 131);
			}

			_eventStatus._earlyRasterLines [i] = EventsStatus::_NOROW;
		}
	}
}

// ---
void COMMODORE::TED::drawRasterIRQLine
	(unsigned short row, unsigned short rasterLine)
{
	MCHEmul::ScreenMemory* screen = screenMemory ();

	// Preserve point markers drawn before this row completed.
	for (size_t x = 0; x < screen -> columns (); x++)
		screen -> setPixel
			(x, row, _eventStatus._marks [x] != 0
				? _eventStatus._marks [x] : 129);

	if (row < 5)
	{
		_eventStatus._earlyRasterLines [row] = rasterLine;

		return;
	}

	const std::string label = std::to_string ((unsigned int) rasterLine);
	const size_t width = label.size () * 6 + 2;

	if (width <= screen -> columns ())
	{
		// The five glyph rows lie entirely above the completed IRQ row.
		screen -> setBox (0, row - 5, width, 5, 130);
		screen -> setString (0, row - 5, label, 131);
	}
}

// ---
void COMMODORE::TED::clockVideoFetch
	(VideoFetchPhase phase)
{
	if (phase == VideoFetchPhase::_GRAPHICS)
	{
		// Advance the previously captured graphics before replacing it.
		if (_videoOutput._shiftActive)
			_videoOutput._current._graphics =
				_videoOutput._next._graphics;

		_videoOutput._next._graphics =
			captureVideoRead (0).value ();

		if (!_videoFetchState._window ||
			!_videoFetchState._displayActive)
			return;

		const unsigned int targets =
			_videoFetchState.captureTargets (_rasterPosition._videoLine);

		if (targets == 0)
			return;

		const MCHEmul::Address address =
			((targets & VideoFetchState::_ATTRIBUTES) != 0
				? _TEDRegisters -> attributeMemory ()
				: _TEDRegisters -> screenMemory ()) +
			(size_t) _videoFetchState._position;

		prepareVideoRead
			(VideoReadKind::_MATRIX,
			 address, _videoFetchState._bufferIndex);

		return;
	}

	if (_videoOutput._shiftActive)
	{
		// Snapshot the complete cell before advancing its metadata.
		_videoOutput._waiting = _videoOutput._current;

		_videoOutput._current._character =
			_videoOutput._next._character;
		_videoOutput._current._attribute =
			_videoOutput._next._attribute;
		_videoOutput._current._cursor =
			_videoOutput._next._cursor;
		_videoOutput._current._valid =
			_videoOutput._next._valid;
	}
	else
		// Outside the transfer window, retain metadata but supply zero graphics.
		_videoOutput._waiting._graphics = 0;

	const size_t index = _videoFetchState._bufferIndex;

	// Attribute output uses the retained byte preceding its possible replacement.
	const unsigned char previousAttribute =
		_tedGraphicInfo._colorData [index].value ();

	const unsigned int targets =
		(_videoFetchState._window &&
		 _videoFetchState._displayActive)
			? _videoFetchState.captureTargets (_rasterPosition._videoLine)
			: 0u;

	_tedGraphicInfo._VLMI = (unsigned short) index;
	captureVideoRead (targets);

	if (!_videoFetchState._window)
	{
		_videoOutput._next._character = 0;
		_videoOutput._next._attribute = 0;
		_videoOutput._next._cursor = false;
		_videoOutput._next._valid = false;

		return;
	}

	const COMMODORE::TEDRegisters::GraphicalInfo& gI =
		_TEDRegisters -> graphicalInfo ();

	_videoOutput._next._character =
		_tedGraphicInfo._screenCodeData [index].value ();
	_videoOutput._next._attribute = previousAttribute;
	_videoOutput._next._cursor =
		_tedGraphicInfo._VC == _TEDRegisters -> cursorPosition ();
	_videoOutput._next._valid = !_tedGraphicInfo._idleState;

	prepareGraphicsRead
		(index,
		 _tedGraphicInfo._screenCodeData [index],
		 _tedGraphicInfo._VC,
		 gI._currentCharacterRow,
		 _tedGraphicInfo._idleState);

	if (_videoFetchState._displayActive)
		_videoFetchState.advancePosition ();

	if (!_tedGraphicInfo._idleState)
		_tedGraphicInfo._VC = (_tedGraphicInfo._VC + 1) & 0x03ff;

	_videoFetchState.advanceBuffer ();
}

// ---
MCHEmul::UByte COMMODORE::TED::captureVideoRead
	(unsigned int targets)
{
	PendingVideoRead& pending = _pendingVideoRead;

	if (pending._kind == VideoReadKind::_NONE)
		return (MCHEmul::PhysicalStorage::_DEFAULTVALUE);

	MCHEmul::UByte result;

	if (pending._kind == VideoReadKind::_MATRIX)
	{
		// A prepared address is not a grant of the CPU bus phase. Discard this
		// opportunity without overwriting retained bytes or delaying the fetch.
		if (!DMAActive ())
		{
			pending._kind = VideoReadKind::_NONE;

			return (MCHEmul::PhysicalStorage::_DEFAULTVALUE);
		}

		// Both enabled destinations receive the same bus value.
		result = readRAMForVideo (pending._address);

		// Record actual transfers before ownership changes for the next phase.
		if (_drawOtherEvents && targets != 0)
			_eventStatus._marker._events |= EventsStatus::Marker::_BADLINE;

		if ((targets & VideoFetchState::_ATTRIBUTES) != 0)
		{
			_tedGraphicInfo._colorData [pending._index] = result;

			_IFDEBUG debugReadingVideoAttributes
				(pending._address, pending._index, result);
		}

		if ((targets & VideoFetchState::_MATRIX) != 0)
		{
			_tedGraphicInfo._screenCodeData [pending._index] = result;

			_IFDEBUG debugReadingVideoMatrix
				(pending._address, pending._index, result);
		}
	}
	else
	{
		result = readVideoGraphics
			(pending._address, pending._kind != VideoReadKind::_IDLE);

		_tedGraphicInfo._graphicData [pending._index] = result;

		// Report the selected operation before consuming its descriptor.
		_IFDEBUG debugReadingGraphics (pending, result);
	}

	pending._kind = VideoReadKind::_NONE;

	return (result);
}

// ---
void COMMODORE::TED::prepareGraphicsRead
	(size_t index, const MCHEmul::UByte& character,
	 unsigned short position, unsigned char row, bool idle)
{
	if (idle)
	{
		prepareVideoRead
			(VideoReadKind::_IDLE,
			 (_videoOutput._control1 & 0x40) != 0
				? _MEMORYPOSIDLE1 : _MEMORYPOSIDLE2,
			 index);

		return;
	}

	// Retain the complete address before a later capture can observe register changes.
	if ((_videoOutput._control1 & 0x20) == 0)
	{
		const bool reverse = (_videoOutput._control2 & 0x80) == 0;
		const size_t characterIndex = (size_t) character.value () &
			((_videoOutput._control1 & 0x40) != 0 ? 0x3f : (reverse ? 0x7f : 0xff));

		// FF13 remains programmed state; effective REVERSE selects the base alignment.
		const unsigned int base =
			(_TEDRegisters -> MCHEmul::PhysicalStorageSubset::readValue (0x13).value () & (reverse ? 0xfc : 0xf8)) << 8;

		prepareVideoRead
			(VideoReadKind::_CHARACTER,
			 MCHEmul::Address (2, base + (characterIndex << 3) + row), index);
	}
	else
	{
		prepareVideoRead
			(VideoReadKind::_BITMAP,
			 _TEDRegisters -> bitmapMemory () +
				(((size_t) (position & 0x03ff)) << 3) + row,
			 index);
	}
}

// ---
MCHEmul::UByte COMMODORE::TED::readVideoGraphics
	(const MCHEmul::Address& address, bool useGraphicsROMSelection)
{
	MCHEmul::Memory* memory = memoryRef ();
	const int previousView = memory -> activeView () -> id ();
	const bool previousTEDROM = ROMActiveToFetchCharData ();
	const bool graphicsROM = useGraphicsROMSelection
		? _TEDRegisters -> ROMActiveToFetchCharAndBitmap ()
		: previousTEDROM;

	if (previousView != _TEDView)
		memory -> setActiveView (_TEDView);

	if (previousTEDROM != graphicsROM)
		activeROMtoFetchCharData (graphicsROM);

	const MCHEmul::UByte result = memory -> value (address);

	// The temporary TED mapping must not escape this memory operation.
	if (previousTEDROM != graphicsROM)
		activeROMtoFetchCharData (previousTEDROM);

	if (previousView != _TEDView)
		memory -> setActiveView (previousView);

	return (result);
}

// ---
void COMMODORE::TED::clockVideoOutput
	(bool halfEnd, bool inScreen, unsigned char scrollX)
{
	if (_videoOutput._shiftActive || inScreen)
	{
		_videoOutput._pixel = (_videoOutput._pixel + 1) & 0x07;

		if (((_rasterPosition._horizontal + 1) & 0x07) ==
			_videoOutput._scrollX)
		{
			_videoOutput._output = _videoOutput._waiting;

			// Cursor inversion belongs to this load, not later mode changes.
			if (_videoOutput._output._cursor &&
				!_TEDRegisters -> flashCounterOn () &&
				_videoOutput._control1 == 0 &&
				(_videoOutput._control2 & 0x10) == 0)
				_videoOutput._output._graphics ^= 0xff;

			_videoOutput._doubleShift = false;
			_videoOutput._pixel = 0;

			_IFDEBUG debugVideoOutputLoad ();
		}
		else
		{
			// The pair phase advances even in hires; a mode write does not restart it.
			if (!_videoOutput.multicolor ())
				_videoOutput._output._graphics <<= 1;
			else if (_videoOutput._doubleShift)
				_videoOutput._output._graphics <<= 2;

			_videoOutput._doubleShift = !_videoOutput._doubleShift;
		}
	}
	else
	{
		// Keep metadata and pair phase while zeroing the disabled shift register.
		_videoOutput._output._graphics = 0;
		_videoOutput._pixel = 0;
	}

	if (_rasterPosition._horizontal == 311)
		_videoOutput._shiftActive = false;

	// A coincident CPU write does not alter the sample taken before that write.
	if (halfEnd)
		_videoOutput._scrollX = scrollX;
}

// ---
unsigned int COMMODORE::TED::videoPixelColor () const
{
	const VideoCell& cell = _videoOutput._output;

	if (!cell._valid)
		return (_TEDRegisters -> backgroundColor ().asChar ());

	// The effective mode interprets remaining bits; it never reconstructs consumed ones.
	switch ((_videoOutput._control1 >> 4) |
		((_videoOutput._control2 >> 4) & 0x01))
	{
		case 0: // Standard character mode.
		case 1: // Multicolor character mode, including hires cells.
			{
				if (_videoOutput.multicolor ())
				{
					const unsigned char pair = cell._graphics >> 6;
					return (pair == 3 ? cell._attribute & 0x77
						: _TEDRegisters -> backgroundColor (pair).asChar ());
				}

				bool foreground = (cell._graphics & 0x80) != 0;
				if ((_videoOutput._control2 & 0x90) == 0)
				{
					if ((cell._attribute & 0x80) != 0 &&
						_TEDRegisters -> flashCounterOn ())
						foreground = false;

					foreground ^= (cell._character & 0x80) != 0;
				}

				return (foreground ? cell._attribute & 0x7f
					: _TEDRegisters -> backgroundColor ().asChar ());
			}

		case 2: // Hires bitmap.
			{
				return ((cell._graphics & 0x80) != 0
					? (cell._character >> 4) | ((cell._attribute & 0x07) << 4)
					: (cell._character & 0x0f) | (cell._attribute & 0x70));
			}

		case 3: // Multicolor bitmap.
			{
				const unsigned char pair = cell._graphics >> 6;
				if (pair == 1)
					return ((cell._character >> 4) | ((cell._attribute & 0x07) << 4));
				if (pair == 2)
					return ((cell._character & 0x0f) | (cell._attribute & 0x70));

				return (_TEDRegisters -> backgroundColor (pair == 3 ? 1 : 0).asChar ());
			}

		case 4: // Extended-color character mode.
			{
				return ((cell._graphics & 0x80) != 0 ? cell._attribute & 0x7f
					: _TEDRegisters -> backgroundColor (cell._character >> 6).asChar ());
			}

		default: // Unsupported ECM combinations produce black in the selected profile.
			{
				return (0);
			}
	}
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

	_cpuTransaction._readAdvanceCycles = cycles;

	_IFDEBUG debugCPUReadAdvance (cpu, false);

	const unsigned int batchStart = _cpuTransaction._startCycle;
	const unsigned int batchEnd =
		batchStart + (unsigned int) busData._numberCycles;

	if (cycles != 0)
		_raster.reduceDisplayZone
			(!_TEDRegisters -> textDisplay25RowsActive (),
			 !_TEDRegisters -> textDisplay40ColumnsActive ());

	while (_cpuTransaction._logicalCycle < cycles)
	{
		if (!simulateCPUCycle (cpu, batchStart, batchEnd, false))
			return (false);

		if (_cpuRecovery._cycle._held)
		{
			// Recover the wait without consuming the pending logical read.
			if (!simulateCPUCycle (cpu, batchStart, batchEnd))
				return (false);

			// A frozen DMA window cannot expire through raster advancement.
			// Return to the scheduler instead of spinning inside a CPU callback.
			if (!_graphicsCycleActive && _TEDRegisters -> freezeActive ())
				break;

			continue;
		}

		// Leave the final permitted read suspended at the existing capture edge.
		if (_cpuTransaction._logicalCycle + 1 == cycles)
			break;

		if (!simulateCPUCycle (cpu, batchStart, batchEnd))
			return (false);
	}

	_IFDEBUG debugCPUReadAdvance (cpu, true);

	return (true);
}

// ---
void COMMODORE::TED::updateCPUStop (MCHEmul::CPU* cpu)
{
	// Recovery is at or ahead of the CPU here. The remaining logical bus
	// cycles plus that lead give the compensation without summing old waits.
	unsigned int remaining =
		(unsigned int) _cpuTransaction._busData -> _numberCycles -
			_cpuTransaction._logicalCycle + (_cpuRecovery._nextCycle - cpu -> clockCycles ());

	// Even a completed instruction must not start its next opcode while BA
	// is low. Reserve one scheduler tick; its actual wait is recovered once.
	if (_cpuTransaction._logicalCycle == _cpuTransaction._busData -> _numberCycles &&
		DMARequested ())
	{
		if (_cpuTransaction._waitCycles == 0)
			_cpuTransaction._firstStopPosition = _cpuRecovery._nextCycle - _cpuTransaction._startCycle;

		++remaining;
	}

	if (remaining == 0 ||
		(_cpuTransaction._stopRequested && cpu -> stopped () &&
		 cpu -> cyclesRemainStopped () == remaining))
		return;

	// setStop replaces its remaining counter. Never reload waits already
	// consumed by the CPU, and never request a stop from CPUAboutToExecute.
	cpu -> setStop
		(true, MCHEmul::InstructionDefined::_CYCLEALL,
		 _cpuTransaction._startCycle + _cpuTransaction._firstStopPosition, (int) remaining);

	if (!_cpuTransaction._stopRequested)
	{
		_cpuTransaction._stopRequested = true;

		_IFDEBUG debugCPUStop (cpu, remaining);
	}
}

// ---
bool COMMODORE::TED::simulateCPUCycle
	(MCHEmul::CPU* cpu,
	 unsigned int batchStart, unsigned int batchEnd,
	 bool completeCycle)
{
	COMMODORE::TED::CPUCycleState& cS = _cpuRecovery._cycle;

	if (!cS._accessPending)
	{
		selectConfiguration ();

		// Capture the clock decision once; later writes cannot change this cycle.
		cS = COMMODORE::TED::CPUCycleState
			(_cpuRecovery._nextCycle, _inSingleClockMode,
			 clockDivider (), _cpuRecovery._halfCarry);

		// Stop before the existing read-capture edge, leaving the bus write pending.
		if (!advanceCPUQuarters
			(cpu, cS.quarterDots () - 9, batchStart, batchEnd))
			return (false);

		cS._accessPending = true;
		cS._held = CPUAccessBlocked ();
	}

	if (!completeCycle)
		return (true);

	// A suspended cycle continues from its retained cursor and divider.
	if (!advanceCPUQuarters
		(cpu, cS.quarterDots (), batchStart, batchEnd))
		return (false);

	// Commit elapsed time only after the complete CPU cycle has been recovered.
	_cpuRecovery.commitElapsedTime ();

	publishRasterPosition ();

	if (_cpuTransaction._pending)
		_cpuTransaction.advance (cS._held, _cpuRecovery._nextCycle);

	cS._accessPending = false;
	++_cpuRecovery._nextCycle;

	return (true);
}

// ---
bool COMMODORE::TED::advanceCPUQuarters
	(MCHEmul::CPU* cpu, unsigned int target,
	 unsigned int batchStart, unsigned int batchEnd)
{
	COMMODORE::TED::CPUCycleState& cS = _cpuRecovery._cycle;

	// Callers stop before a read edge or at the CPU-cycle boundary.
	// These positions preserve the control/dot sequence across resumptions.
	while (cS._nextQuarter < target)
	{
		if (!processCPUQuarter (cpu, batchStart, batchEnd))
			return (false);

		// The half-pulse carry is a multiple of sixteen quarters, so the
		// low two cursor bits also identify the current slow-pulse edge.
		// Advance directly from control to dot, or from dot to control.
		cS._nextQuarter +=
			((cS._nextQuarter & 0x03) == 0) ? 3u : 1u;
	}

	return (true);
}

// ---
bool COMMODORE::TED::processCPUQuarter
	(MCHEmul::CPU* cpu,
	 unsigned int batchStart, unsigned int batchEnd)
{
	const COMMODORE::TED::CPUCycleState& cS = _cpuRecovery._cycle;
	const unsigned int phase = cS.phase ();

	// Capture before CPU writes and raster advancement. Phase processing is
	// sequential; callbacks must not recursively recover another TED phase.
	_debugCycleContext = deepDebugActive ()
		? debugCycleContext (cS, batchStart, batchEnd)
		: DebugCycleContext { };

	if ((phase & 0x03) == 0)
	{
		// Emission edges rearm after CPU writes, before sampling a new source.
		if (_drawOtherEvents && !_TEDRegisters -> launchIRQ ())
			_eventStatus._irqRequestMarked = false;

		if (phase == 0)
			_graphicsCycleActive = !_TEDRegisters -> freezeActive ();

		if (_graphicsCycleActive && !processRasterControl ())
			return (false);

		// A mismatch rearms the detector independently of IRQ acknowledgement.
		if (_rasterPosition._vertical != _TEDRegisters -> IRQRasterLineAt ())
			_rasterIRQMatched = false;

		if (phase == 0 && _graphicsCycleActive)
			beginTEDCycle ();

		return (true);
	}

	const unsigned short sampledLine = _rasterPosition._vertical;
	const bool matched = sampledLine == _TEDRegisters -> IRQRasterLineAt ();
	const bool cpuEnd =
		cS._nextQuarter + 1 == cS.quarterDots ();
	const bool slowEnd = phase == 31;
	const bool halfEnd = phase == 15 || slowEnd;

	if (_drawOtherEvents)
	{
		_eventStatus._marker = EventsStatus::Marker ();

		_eventStatus._marker._rowVisible = _raster.vData ().isInVisibleZone ();
		_eventStatus._marker._visible = _raster.isInVisibleZone ();

		if (_eventStatus._marker._rowVisible)
			_eventStatus._marker._row = _raster.vData ().currentVisiblePosition ();

		if (_eventStatus._marker._visible)
			_eventStatus._marker._column = _raster.hData ().currentVisiblePosition ();

		_eventStatus._marker._afterVisible =
			_graphicsCycleActive && _raster.hData ().isInLastBlankZone ();
	}

	unsigned char scrollX = 0;

	if (_graphicsCycleActive && halfEnd)
	{
		clockVideoFetch
			(slowEnd
				? VideoFetchPhase::_MATRIX
				: VideoFetchPhase::_GRAPHICS);

		if (slowEnd)
		{
			// Retain the preceding line condition before line and scroll updates.
			if (_verticalClockPending)
				_videoFetchState.latchNextLine (_rasterPosition._videoLine);

			_videoFetchState._scrollY =
				_TEDRegisters -> verticalScrollPosition ();
		}

		scrollX = _TEDRegisters -> horizontalScrollPosition ();
	}

	unsigned int writes = 0;

	if (cpuEnd && _cpuTimedWrites._enabled && !cS._held)
		if (!executePendingWriteAt (cpu, cS._cycle, writes))
			return (false);

	// Observe acknowledgement before a coincident source can become pending.
	if (_drawOtherEvents && !_TEDRegisters -> launchIRQ ())
		_eventStatus._irqRequestMarked = false;

	// Sample the operands preceding simultaneous CPU loads.
	// Acknowledgement wins over retaining a newly captured raster flag.
	const bool rasterRaised =
		sampleRasterIRQ (matched, (writes & _ACKRASTER) != 0);

	if (_drawOtherEvents && rasterRaised)
	{
		_eventStatus._marker._events |= EventsStatus::Marker::_RASTER;
		_eventStatus._marker._rasterLine = sampledLine;
	}

	// Capture marker coordinates before presentation advancement.
	// Normal video is rendered before any diagnostic overlay.
	if (_graphicsCycleActive)
	{
		clockBorders ();

		const bool inScreen = _videoOutput._horizontalOpen;

		if (_raster.isInVisibleZone ())
		{
			drawVisibleZone (phase >> 2, inScreen);
			if (_drawOtherEvents)
				_eventStatus._marker._emitted = true;
		}

		clockVideoOutput (halfEnd, inScreen, scrollX);

		_raster.moveDot ();
	}

	// Complete the current memory phase before changing ownership of the next.
	clockDMA (slowEnd);
	clockRasterDot (slowEnd, writes);

	_IFDEBUG
		if (rasterRaised)
			debugRasterEvent ("RasterIRQ", sampledLine);

	// A slow pulse can contain halves captured with different selected dividers.
	if (halfEnd)
		_pendingSoundMasterCycles += cS.halfMasterCycles ();

	if (slowEnd)
	{
		latchVideoControls ();

		if (_graphicsCycleActive)
			finishTEDCycle (cpu, cS._cycle);
		else
		{
			clockTEDCycle (false);

			_IFDEBUG debugFrozenTEDCycle ();
		}

		// The next CPU cycle uses the decision retained at this slow boundary.
		_inSingleClockMode = singleClockModeForCurrentCycle ();
	}

	if (!slowEnd || !_graphicsCycleActive)
	{
		if (rasterRaised || cpuEnd)
			if (requestIRQ (cpu, cS._cycle))
				_eventStatus._marker._events |= EventsStatus::Marker::_IRQ;

		if (_drawOtherEvents)
			drawOtherEvents ();
	}

	return (true);
}

// ---
void COMMODORE::TED::clockDMA
	(bool slowEnd)
{
	const DMAState previous = _dmaState;

	if (_graphicsCycleActive && slowEnd)
	{
		if (_rasterPosition._horizontal == 407)
			_dmaWindow = true;
		else if (_rasterPosition._horizontal == 295)
			_dmaWindow = false;
	}

	if (!_dmaWindow || !_videoFetchState._displayActive ||
		_videoFetchState.captureTargets (_rasterPosition._videoLine) == 0)
		_dmaState = DMAState::_IDLE;
	else if (_dmaState == DMAState::_IDLE)
		_dmaState = DMAState::_WARNING1;
	else if (slowEnd)
	{
		switch (_dmaState)
		{
			case DMAState::_WARNING1: _dmaState = DMAState::_WARNING2; break;
			case DMAState::_WARNING2: _dmaState = DMAState::_WARNING3; break;
			case DMAState::_WARNING3: _dmaState = DMAState::_ACTIVE; break;
			default: break;
		}
	}

	_IFDEBUG
		if (previous != _dmaState)
			debugRasterEvent (_dmaState == DMAState::_IDLE ? "DMABusReleased" :
					(_dmaState == DMAState::_ACTIVE ? "DMABusGranted" : "DMAWarning"),
				_rasterPosition._videoLine);
}

// ---
bool COMMODORE::TED::applyPresentationConfiguration ()
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
	_presentationConfiguration = _configuration;
	_lastVBlankEntered = false;
	_eventStatus.reset
		(_raster.visibleColumns (), _TEDRegisters -> launchIRQ ());

	_IFDEBUG debugRasterEvent ("RasterConfiguration", _rasterPosition._videoLine);
	notify (MCHEmul::Event (_VIDEOCONFIGURATIONCHANGED));

	return (true);
}

// ---
bool COMMODORE::TED::processRasterControl ()
{
	switch (_rasterPosition._horizontal)
	{
		case 296:
			{
				_videoFetchState._window = false;

				// Refresh keeps the CPU slow after external fetching ends.
				_externalFetchClock = false;
				_refreshClock = true;
			}

			break;

		case 336:
			{
				_refreshClock = false;
			}

			break;

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
					_TEDRegisters -> graphicalInfo ()._currentCharacterPosition = 0;

				_tedGraphicInfo._VC = 0;

				if (previousLine == 205)
					_videoFetchState._reload = 0;

				_rasterPosition._videoLine = _rasterPosition._vertical;
				_lastBadLineScrollY = -1;
				_newBadLineCondition = false;

				if (_videoFetchState._matrixLine &&
					_videoFetchState._displayActive &&
					_rasterPosition._videoLine < 204)
				{
					_tedGraphicInfo._idleState = false;
					_TEDRegisters -> graphicalInfo ()._currentCharacterRow = 0;
				}

				_IFDEBUG
					debugRasterEvent ("VideoLineLatch", previousLine);

				// Switch presentation at its new frame origin without resetting hardware counters.
				if (_configuration != _presentationConfiguration &&
					_rasterPosition._videoLine == _configuration -> _vertical.firstPosition () &&
					!applyPresentationConfiguration ())
					return (false);
			}

			break;

		case 400:
			{
				_externalFetchClock = true;
			}

			break;

		case 432:
			{
				_videoFetchState._window = true;

				if (_videoFetchState._displayActive)
					_videoFetchState._position = _videoFetchState._reload;

				_tedGraphicInfo._VC =
					_TEDRegisters -> graphicalInfo ()._currentCharacterPosition;
			}

			break;

		case 440:
			{
				// Opening is qualified; only the H=311 event closes retained transfers.
				if (_videoFetchState._displayActive)
					_videoOutput._shiftActive = true;

				// Preparation clears bytes, retaining cursor/validity metadata and
				// the independently running output cell, dot index and pair phase.
				_videoOutput._next._graphics = 0;
				_videoOutput._current._character =
					_videoOutput._current._attribute =
					_videoOutput._current._graphics = 0;
				_videoOutput._waiting._character =
					_videoOutput._waiting._attribute =
					_videoOutput._waiting._graphics = 0;
			}

			break;

		default:
			break;
	}

	return (true);
}

// ---
void COMMODORE::TED::clockRasterDot
	(bool slowEnd, unsigned int writes)
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
			debugRasterEvent ("HorizontalLoad", _rasterPosition._vertical);
	}
	else if (_graphicsCycleActive)
		_rasterPosition._horizontal =
			(_rasterPosition._horizontal == 455)
				? 0 : ((_rasterPosition._horizontal + 1) & 0x01ff);

	if (!slowEnd)
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
				debugRasterEvent ("VerticalClock", previousLine);
		}

		if (_flashClockPending &&
			_rasterPosition._videoLine == 205 &&
			(writes & _WRITEFLASH) == 0)
		{
			_TEDRegisters -> incrementFlashCounter ();

			_IFDEBUG
				debugRasterEvent ("FlashClock", _rasterPosition._videoLine);
		}
	}

	_verticalClockPending = _flashClockPending = false;
}

// ---
void COMMODORE::TED::beginTEDCycle ()
{
	_cycleInRasterLine = rasterCycle ();

	_IFDEBUG debugTEDCycle ();

	if (_rasterPosition._videoLine == 0 &&
		!_TEDRegisters -> blankEntireScreen ())
		_videoFetchState._displayActive = true;
	else if (_rasterPosition._videoLine == 204)
	{
		_videoFetchState._displayActive = false;
		_tedGraphicInfo._idleState = true;
	}

	if (detectNewBadLine ())
	{
		_newBadLineCondition = true;

		_IFDEBUG debugBadLine ();
	}

	if (_cycleInRasterLine != 57)
		return;

	COMMODORE::TEDRegisters::GraphicalInfo& gI =
		_TEDRegisters -> graphicalInfo ();

	// Retain the existing row-reload approximation.
	if (_videoFetchState._displayActive &&
		!_tedGraphicInfo._idleState &&
		gI._currentCharacterRow == 6)
		_videoFetchState._reload = _videoFetchState._position;

	if (gI._currentCharacterRow == 7)
	{
		gI._currentCharacterPosition =
			_tedGraphicInfo._VC & 0x03ff;

		if (!_newBadLineCondition)
			_tedGraphicInfo._idleState = true;
	}

	gI._currentCharacterRow =
		(gI._currentCharacterRow + 1) & 0x07;
}

// ---
void COMMODORE::TED::finishTEDCycle
	(MCHEmul::CPU* cpu, unsigned int requestCycle)
{
	clockTEDCycle (true);

	if (requestIRQ (cpu, requestCycle))
		_eventStatus._marker._events |= EventsStatus::Marker::_IRQ;

	_IFDEBUG debugTEDCycleEnd ();

	if (_drawOtherEvents)
		drawOtherEvents ();

	if (_raster.isInLastVBlankZone ())
	{
		if (!_lastVBlankEntered)
		{
			_lastVBlankEntered = true;

			_IFDEBUG debugRasterEvent
				("FrameReady", _rasterPosition._videoLine);

			if (_drawOtherEvents)
				_eventStatus.finishFrame ();

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
	assert (_cpuTimedWrites._commands.empty () && _cpuTimedWrites._next == 0);

	if (_cpuTimedWrites._cpu == nullptr ||
		_cpuTransaction._cpu != _cpuTimedWrites._cpu ||
		_cpuTransaction._busData == nullptr)
	{
		_error = MCHEmul::_CHIP_ERROR;

		return (false);
	}

	MCHEmul::Memory::configuration ().extractMemorySetCommandsBuffered
		(_cpuTimedWrites._subsets, _cpuTimedWrites._commands);

	// Matching every write is required before assigning bus-cycle coordinates.
	if (_cpuTimedWrites._commands.size () !=
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

	if (_cpuTimedWrites._next == _cpuTimedWrites._commands.size () ||
		cycle != pendingWriteCycle ())
		return (true);

	MCHEmul::SetMemoryCommand& command = _cpuTimedWrites._commands [_cpuTimedWrites._next];

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

	++_cpuTimedWrites._next;

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
std::string COMMODORE::TED::debugTimingAsString () const
{
	return ("CPUHalf=" + std::to_string (_debugCycleContext._cpuHalf) +
		",CPUQuarter=" + std::to_string (_debugCycleContext._cpuQuarter) +
		",CPUQuarterLength=" + std::to_string (_debugCycleContext._cpuQuarterLength) +
		",ClockDivider=" + std::to_string (_debugCycleContext._clockDivider) +
		",MasterCycles=" + std::to_string (_debugCycleContext._masterCycles) +
		",Reference=TEDPhaseEvent");
}

// ---
std::string COMMODORE::TED::debugClockAsString () const
{
	return ("SingleClock=" + std::to_string (_inSingleClockMode) +
		",ForcedSingleClock=" + std::to_string (_TEDRegisters -> singleClockModeForced ()) +
		",ExternalFetchClock=" + std::to_string (_externalFetchClock) +
		",RefreshClock=" + std::to_string (_refreshClock) +
		",Freeze=" + std::to_string (_TEDRegisters -> freezeActive ()) +
		",SelectedPAL=" + std::to_string (_configuration == &_configurations [0]) +
		",PresentationPAL=" + std::to_string (_presentationConfiguration == &_configurations [0]) +
		",PresentationPending=" + std::to_string (_configuration != _presentationConfiguration) +
		",SelectedDivider=" + std::to_string (_configuration -> _divider) +
		",GraphicsCycleActive=" + std::to_string (_graphicsCycleActive) +
		",VerticalWindow=" + std::to_string (_videoOutput._verticalOpen) +
		",HorizontalWindow=" + std::to_string (_videoOutput._horizontalOpen) +
		",EffectiveControl1=" + std::to_string (_videoOutput._control1) +
		",EffectiveControl2=" + std::to_string (_videoOutput._control2) +
		",DMAState=" + std::to_string ((unsigned int) _dmaState) +
		",DMAWindow=" + std::to_string (_dmaWindow) +
		",BA=" + std::to_string (!DMARequested ()) +
		",DMABusGranted=" + std::to_string (DMAActive ()));
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
std::string COMMODORE::TED::debugVideoOutputAsString () const
{
	const VideoCell& cell = _videoOutput._output;

	return ("Graphics=$" + MCHEmul::UByte (cell._graphics).asString
		(MCHEmul::UByte::OutputFormat::_HEXA, 2) +
		",Character=$" + MCHEmul::UByte (cell._character).asString
			(MCHEmul::UByte::OutputFormat::_HEXA, 2) +
		",Attribute=$" + MCHEmul::UByte (cell._attribute).asString
			(MCHEmul::UByte::OutputFormat::_HEXA, 2) +
		",CursorMatch=" + std::to_string (cell._cursor) +
		",Valid=" + std::to_string (cell._valid) +
		",PixelIndex=" + std::to_string (_videoOutput._pixel) +
		",DoubleShift=" + std::to_string (_videoOutput._doubleShift) +
		",ScrollX=" + std::to_string (_videoOutput._scrollX) +
		",Multicolor=" + std::to_string (_videoOutput.multicolor ()) +
		",FlashPhase=" + std::to_string (_TEDRegisters -> flashCounterOn ()) +
		",PixelColor=" + std::to_string (videoPixelColor ()));
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
	COMMODORE::TED::DebugCycleContext result;

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
void COMMODORE::TED::debugTEDCycle ()
{
	_deepDebugFile -> writeCompleteLine (className (), _debugCycleContext._cpuCycle, "Info Cycle",
		{ { "Timing",
			debugTimingAsString () + ",Snapshot=BeforeGraphics," +
			"CPUBatchStart=" + std::to_string (_debugCycleContext._cpuBatchStart) + "," +
			"CPUBatchEnd=" + std::to_string (_debugCycleContext._cpuBatchEnd) + "," +
			"CPUBatchIndex=" + std::to_string (_debugCycleContext._cpuBatchIndex) + "," +
			"CPUBatchCycles=" + std::to_string (_debugCycleContext._cpuBatchCycles) + "," +
			"ConversionSingleClock=" + std::to_string (_debugCycleContext._conversionSingleClock) + "," +
			"HalfCarryIn=" + std::to_string (_debugCycleContext._halfCarryIn) + "," +
			"HalfCarryOut=" + std::to_string (_debugCycleContext._halfCarryOut) },
		  { "Raster position", debugRasterAsString () },
		  { "Internal", debugInternalAsString () },
		  { "Display",
			"DisplayEnabled=" + std::to_string (!_TEDRegisters -> blankEntireScreen ()) + "," +
			"VideoActive=" + std::to_string (_videoFetchState._displayActive) + "," +
			"ScrollX=" + std::to_string (_TEDRegisters -> horizontalScrollPosition ()) + "," +
			"ScrollY=" + std::to_string (_TEDRegisters -> verticalScrollPosition ()) + "," +
			"Rows25=" + std::to_string (_TEDRegisters -> textDisplay25RowsActive ()) + "," +
			"Columns40=" + std::to_string (_TEDRegisters -> textDisplay40ColumnsActive ()) },
		  { "Badline",
			"Latched=" + std::to_string (_newBadLineCondition) + "," +
			"LastScrollY=" + std::to_string (_lastBadLineScrollY) + "," +
			"WithinBadlineRange=" + std::to_string
				(_rasterPosition._videoLine < 203) },
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
		 "TrailingWriteCycles=" + std::to_string (busData._trailingWriteCycles) + "," +
		 "LogicalCycle=" + std::to_string (_cpuTransaction._logicalCycle) + "," +
		 "WaitCycles=" + std::to_string (_cpuTransaction._waitCycles) + "," +
		 "WritesExtracted=" + std::to_string (_cpuTransaction._writesExtracted) + "," +
		 "StopRequested=" + std::to_string (_cpuTransaction._stopRequested));
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
			",RecoveryCPU=" + std::to_string (_cpuRecovery._nextCycle) },
		  { "Advance",
			"Phase=" + std::string (after ? "After" : "Before") +
			",PreparedCPUCycles=" + std::to_string (_cpuTransaction._readAdvanceCycles) +
			",NextQuarter=" + std::to_string (_cpuRecovery._cycle._nextQuarter) +
			",AccessPending=" + std::to_string
				(_cpuRecovery._cycle._accessPending) +
			",AccessHeld=" + std::to_string (_cpuRecovery._cycle._held) +
			",LogicalCycle=" + std::to_string (_cpuTransaction._logicalCycle) +
			",WaitCycles=" + std::to_string (_cpuTransaction._waitCycles) },
		  { "Raster position", debugRasterAsString () },
		  { "Clock",
			"HalfCarry=" +
				std::to_string (_cpuRecovery._halfCarry) },
		  { "Timers", debugTimersAsString () } });
}

// ---
void COMMODORE::TED::debugCPUStop
	(MCHEmul::CPU* cpu, unsigned int remaining) const
{
	_deepDebugFile -> writeCompleteLine
		(className (), cpu -> clockCycles (), "CPU DMA stop",
		{ { "Timing",
			"Reference=ObservedCPUClock,RecoveryCPU=" + std::to_string (_cpuRecovery._nextCycle) },
		  { "Bus cycles", debugCPUTransactionAsString () },
		  { "CPU stop",
			"FirstStopCPU=" + std::to_string
				(_cpuTransaction._startCycle + _cpuTransaction._firstStopPosition) +
			",RemainingCycles=" + std::to_string (remaining) },
		  { "Raster position", debugRasterAsString () },
		  { "Clock", debugClockAsString () } });
}

// ---
void COMMODORE::TED::debugBadLine ()
{
	_deepDebugFile -> writeCompleteLine (className (), _debugCycleContext._cpuCycle, "Badline detected",
		{ { "Timing", debugTimingAsString () },
		  { "Raster position", debugRasterAsString () },
		  { "Internal", debugInternalAsString () },
		  { "Badline", "Latched=" + std::to_string (_newBadLineCondition) + "," +
			"ScrollY=" + std::to_string (_videoFetchState._scrollY) + "," +
			"VideoActive=" + std::to_string (_videoFetchState._displayActive) + ",Cause=ScrollMatch" } });
}

// ---
void COMMODORE::TED::debugReadingVideoMatrix
	(const MCHEmul::Address& address, size_t index,
	 const MCHEmul::UByte& value)
{
	_deepDebugFile -> writeCompleteLine
		(className (), _debugCycleContext._cpuCycle, "Video matrix read",
		{ { "Timing", debugTimingAsString () },
		  { "Raster position", debugRasterAsString () },
		  { "Fetch", debugInternalAsString () },
		  { "Matrix",
			"Address=$" + MCHEmul::removeAll0
				(address.asString
					(MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) +
			",BufferIndex=" + std::to_string (index) +
			",Value=$" + value.asString
				(MCHEmul::UByte::OutputFormat::_HEXA, 2) +
			",Source=RAM" } });
}

// ---
void COMMODORE::TED::debugReadingVideoAttributes
	(const MCHEmul::Address& address, size_t index,
	 const MCHEmul::UByte& value)
{
	_deepDebugFile -> writeCompleteLine
		(className (), _debugCycleContext._cpuCycle, "Video attributes read",
		{ { "Timing", debugTimingAsString () },
		  { "Raster position", debugRasterAsString () },
		  { "Fetch", debugInternalAsString () },
		  { "Attribute",
			"Address=$" + MCHEmul::removeAll0
				(address.asString
					(MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) +
			",BufferIndex=" + std::to_string (index) +
			",Value=$" + value.asString
				(MCHEmul::UByte::OutputFormat::_HEXA, 2) +
			",Source=RAM" } });
}

// ---
void COMMODORE::TED::debugReadingGraphics
	(const PendingVideoRead& pending, const MCHEmul::UByte& value)
{
	const char* kind;

	switch (pending._kind)
	{
		case VideoReadKind::_CHARACTER:
			kind = "Character";
			break;

		case VideoReadKind::_BITMAP:
			kind = "Bitmap";
			break;

		case VideoReadKind::_IDLE:
			kind = "Idle";
			break;

		default:
			return;
	}

	_deepDebugFile -> writeCompleteLine
		(className (), _debugCycleContext._cpuCycle, "Graphics read",
		{ { "Timing", debugTimingAsString () },
		  { "Raster position", debugRasterAsString () },
		  { "Fetch", debugInternalAsString () },
		  { "Memory access",
			std::string ("Kind=") + kind +
			",Source=" +
				(pending._kind == VideoReadKind::_IDLE
					? "TEDView"
					: (_TEDRegisters -> ROMActiveToFetchCharAndBitmap ()
						? "ROM" : "RAM")) +
			",Address=$" + MCHEmul::removeAll0
				(pending._address.asString
					(MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) +
			",BufferIndex=" + std::to_string (pending._index) +
			",Value=$" + value.asString
				(MCHEmul::UByte::OutputFormat::_HEXA, 2) } });
}

// ---
void COMMODORE::TED::debugVideoOutputLoad () const
{
	_deepDebugFile -> writeCompleteLine
		(className (), _debugCycleContext._cpuCycle, "Video output loaded",
		{ { "Timing", debugTimingAsString () + ",Snapshot=AfterLoadBeforeControls" },
		  { "Raster position", debugRasterAsString () },
		  { "Internal", debugInternalAsString () },
		  { "Output", debugVideoOutputAsString () },
		  { "Clock", debugClockAsString () } });
}

// ---
void COMMODORE::TED::debugTEDCycleEnd ()
{
	_deepDebugFile -> writeCompleteLine (className (), _debugCycleContext._cpuCycle, "Info Cycle End",
		{ { "Timing", debugTimingAsString () + ",Snapshot=AfterGraphics" },
		  { "Raster position", debugRasterAsString () },
		  { "Internal", debugInternalAsString () },
		  { "Clock", debugClockAsString () },
		  { "Timers", debugTimersAsString () },
		  { "Interrupt", debugInterruptAsString () + "," +
			"CPURequestClockArgument=" + (_TEDRegisters -> reasonIRQCode () != 0
				? std::to_string (_debugCycleContext._irqRequestCycle) : std::string ("-")) } });
}

// ---
void COMMODORE::TED::debugFrozenTEDCycle () const
{
	_deepDebugFile -> writeCompleteLine
		(className (), _debugCycleContext._cpuCycle, "Frozen clock",
		{ { "Timing",
			debugTimingAsString () },
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
	_deepDebugFile -> writeCompleteLine
		(className (), cycle, "CPU write",
		{ { "Timing",
			"Reference=ScheduledCPUCycle,ObservedCPUClock=" +
				std::to_string (cpu -> clockCycles ()) +
			",BoundaryCPU=" + std::to_string (cycle + 1) +
			",CPUQuarterLength=" + std::to_string (_cpuRecovery._cycle.quarterDots ()) +
			",TEDPhase=" + std::to_string (_cpuRecovery._cycle.phase ()) +
			",PixelIndex=" + std::to_string (_cpuRecovery._cycle.phase () >> 2) +
			",RetainedDivider=" + std::to_string (_cpuRecovery._cycle._clockDivider) +
			",RetainedSingleClock=" + std::to_string (_cpuRecovery._cycle._singleClock) },
		  { "Transaction",
			"Type=" + std::string
				(_cpuTransaction._isInterrupt ? "Interrupt" : "Instruction") +
			",StartCPU=" + std::to_string (_cpuTransaction._startCycle) +
			",WriteIndex=" + std::to_string (_cpuTimedWrites._next) +
			",LogicalCycle=" + std::to_string (_cpuTransaction._logicalCycle) +
			",WaitCycles=" + std::to_string (_cpuTransaction._waitCycles) },
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
			",VideoActive=" + std::to_string (_videoFetchState._displayActive) +
			",Visible=" + std::to_string (_raster.isInVisibleZone ()) +
			",Screen=" + std::to_string (_raster.isInScreenZone ()) +
			"," + debugVideoOutputAsString () },
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
	(const char* event, unsigned short sourceLine) const
{
	_deepDebugFile -> writeCompleteLine
		(className (), _debugCycleContext._cpuCycle, "Raster event",
		{ { "Timing", debugTimingAsString () },
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
