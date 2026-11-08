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
	  _pendingSoundClockCycles (0)
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

	return (true);
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
	size_t samplesGenerated = 0;

	if (soundWrapper () != nullptr)
	{
		for (unsigned int i = _pendingSoundClockCycles; i > 0; i--)
		{
			_IFDEBUG ++cyclesProcessed;

			// The number of bytes that can come from the wrapper might not be fixed...
			MCHEmul::UBytes data;
			if (soundWrapper () -> getData (cpu, data))
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

	// Record the pending count before clearing the consumed batch.
	_IFDEBUG debugSoundBatch
		(cpu, referenceCycle, beforeCPUWrite,
		 cyclesProcessed, samplesGenerated);

	_pendingSoundClockCycles = 0;

	return (true);
}

// ---
void COMMODORE::TED::SoundFunction::debugSoundBatch
	(MCHEmul::CPU* cpu, unsigned int referenceCycle,
	 bool beforeCPUWrite, unsigned int cyclesProcessed,
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
			"SamplesGenerated=" + std::to_string (samplesGenerated) } });
}

// ---
COMMODORE::TED::TED (int intId, unsigned short clkcpum, unsigned short sfq, 
		const MCHEmul::RasterData& vd, const MCHEmul::RasterData& hd,
		int vV, MCHEmul::SoundLibWrapper* sW, const MCHEmul::Attributes& attrs)
	: MCHEmul::GraphicalChip (_ID, attrs),
	  _interruptId (intId),
	  _timesFasterThanCPU (clkcpum),
	  _screenfrequency (sfq),
	  _T1 (1, COMMODORE::TEDTimer::RunMode::_FROMRELOADVALUE),
	  _T2 (2, COMMODORE::TEDTimer::RunMode::_FROMFFFF),
	  _T3 (3, COMMODORE::TEDTimer::RunMode::_FROMFFFF),
	  _soundFunction (new COMMODORE::TED::SoundFunction (sW)),
	  _TEDRegisters (nullptr), 
	  _TEDView (vV),
	  _cyclesPerRasterLine (57),
	  _raster (vd, hd, 8 /** step. */),
	  _drawRasterInterruptPositions (false),
	  _drawOtherEvents (false),
	  _timesFrameDrawn (0),
	  _cpuTransaction (),
	  _timedWriteCPU (nullptr),
	  _timedWriteSubsets (),
	  _pendingWrites (),
	  _nextPendingWrite (0),
	  _cpuTimedWritesEnabled (false), _cpuTransactionPending (false),
	  _cpuCycleRecovery (), _cpuReadAdvanceCycles (0),
	  _pendingPacingCycles (0),
	  _lastCPUCycles (0), _pendingHalfCyclesFromLastExecution (0),
	  _format (nullptr),
	  _cycleInRasterLine (1),
	  _videoActive (true),
	  _inSingleClockMode (false),
	  _lastVBlankEntered (false),
	  _lastBadLineScrollY (-1), _newBadLineCondition (false), 
	  _tedGraphicInfo (),
	  _eventStatus { 0 }
{
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
bool COMMODORE::TED::initialize ()
{
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

	_raster.initialize ();

	_T1.initialize ();
	_T2.initialize ();
	_T3.initialize ();

	// The sound function is initialized independly...
	assert (_soundFunction -> soundWrapper () != nullptr);

	_TEDRegisters -> lookAtTimers (&_T1, &_T2, &_T3);
	_TEDRegisters -> lookAtSoundLibWrapper (_soundFunction -> soundWrapper ());
	_TEDRegisters -> linkToRaster (&_raster);
	_TEDRegisters -> initialize ();

	_timesFrameDrawn = 0;

	_cpuTransaction = { };

	_timedWriteCPU = nullptr;
	_timedWriteSubsets.clear ();
	_pendingWrites.clear ();
	_nextPendingWrite = 0;
	_cpuTimedWritesEnabled = _cpuTransactionPending = false;
	_cpuCycleRecovery = { };
	_cpuReadAdvanceCycles = 0;
	_pendingPacingCycles = 0;

	_lastCPUCycles = _pendingHalfCyclesFromLastExecution = 0;
	
	_cycleInRasterLine = 1;

	_videoActive = true;

	_inSingleClockMode = false;

	_lastVBlankEntered = false;

	_lastBadLineScrollY = -1;
	_newBadLineCondition = false;

	_tedGraphicInfo = { };
	_eventStatus = { 0 };

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
	_cpuCycleRecovery = { };
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
				_cpuCycleRecovery._phase == CPUCyclePhase::_BEFOREACCESS;

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

	_inSingleClockMode = singleClockModeForCurrentCycle ();

	if (_TEDRegisters -> freezeActive ())
	{
		const int reason = (int) _TEDRegisters -> reasonIRQCode ();
		if (reason != 0)
			cpu -> requestInterrupt
				(F6500::IRQInterrupt::_ID, batchEnd, this, reason);
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
MCHEmul::UBytes COMMODORE::TED::bitmapMemorySnapShot (MCHEmul::CPU* cpu) const
{ 
	int aVID = cpu -> memoryRef () -> activeView () -> id ();
	if (aVID != _TEDView)
		cpu -> memoryRef () -> setActiveView (_TEDView);

	MCHEmul::UBytes result = cpu -> memoryRef () -> values 
		(_TEDRegisters -> bitmapMemory (), 0x1f40 /** 8000 positions = 40 x 25 x 8. */); 

	cpu -> memoryRef () -> setActiveView (aVID);

	return (result);
}

// ---
MCHEmul::Strings COMMODORE::TED::charsDrawSnapshot (MCHEmul::CPU* cpu,
	const std::vector <size_t>& chrs) const
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

	int aVID = cpu -> memoryRef () -> activeView () -> id ();
	if (aVID != _TEDView)
		cpu -> memoryRef () -> setActiveView (_TEDView);

	MCHEmul::Strings result;
	for (size_t i = 0; i < 256; i++)
	{
		if (!chrs.empty () && 
			std::find (chrs.begin (), chrs.end (), i) == chrs.end ())
			continue;

		MCHEmul::Address chrAdd = _TEDRegisters -> charDataMemory () + (i << 3);
		std::string dt = std::to_string (i) + "---\n$" +
			MCHEmul::removeAll0 (chrAdd.asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) + "\n";
		MCHEmul::UBytes chrDt = cpu -> memoryRef () -> values (chrAdd, 0x08);
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

	cpu -> memoryRef () -> setActiveView (aVID);

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

				if (_newBadLineCondition)
					gI._currentCharacterRow = 0;
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
		_tedGraphicInfo._VLMI++;
	}
}

// ---
void COMMODORE::TED::drawVisibleZone (MCHEmul::CPU* cpu)
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
				_TEDRegisters -> borderColor ().asChar ());

		return;
	}

	// In other case...
	// Everyting is the color of the background initially...
	// ..and it will be covered with the foreground (border) later if needed..
	// This is how the TED works...
	screenMemory () -> setHorizontalLine ((size_t) cav, (size_t) rv,
		(cav + 8) > _raster.visibleColumns () ? (_raster.visibleColumns () - cav) : 8, 
			_TEDRegisters -> backgroundColor ().asChar ());

	// Now the information is drawn,...
	// ...and also the collisions detected at the same time
	drawGraphicsAndMoveToScreen (
		{
			/** _ICD */ _raster.hData ().firstDisplayPosition (),		// DISLAY: The original...
			/** _SC	 */ _TEDRegisters -> horizontalScrollPosition (),	// From 0 - 7 
			/** _RC	 */ cv,												// Where the horizontal raster is (not adjusted to 8) inside the window
			/** _RCA */ cav,											// Where the horizontal raster is (adjusted to 8) inside the window
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
			_TEDRegisters -> borderColor ().asChar ());
	}

	// If there were requested to draw the position where the Raster Interrupt is generated...
	// All draw are replace by a line with a color different of the background!
	// The line is only drawn if that position where within the visible zone!
	if (_drawRasterInterruptPositions)
	{
		unsigned short lrt = 
			_raster.lineInVisibleZone (_TEDRegisters -> IRQRasterLineAt ());
		if (lrt <= _raster.vData ().lastVisiblePosition ())
		{
			screenMemory () -> setHorizontalLine ((size_t) cav, (size_t) lrt,
				(cav + 8) > _raster.visibleColumns () ? (_raster.visibleColumns () - cav) : 8, 130);
			if (cav >= 8 && cav < 40 && lrt > 6)
				screenMemory () -> setString ((size_t) 0, (size_t) (lrt - 6), 
					std::to_string ((unsigned int) _TEDRegisters -> IRQRasterLineAt ()) + " " +
					std::to_string ((unsigned int) lrt), 131);
		}
	}

	// Draw the enabled TED diagnostic overlays...
	if (_drawOtherEvents)
		drawOtherEvents ();
}

// ---
void COMMODORE::TED::drawGraphicsAndMoveToScreen (const COMMODORE::TED::DrawContext& dC)
{
	// This varible keeps info about the text/graphics:
	// Whether the 8 pixels to draw are foreground or background...
	// ...and the color of the ones that are not finally background!
	// ...for subsequent composition with the border.
	COMMODORE::TED::DrawResult colGraphics = drawGraphics (dC);

	// The the graphical info is moved to the screen...
	drawResultToScreen (colGraphics, dC);
}

// ---
void COMMODORE::TED::drawOtherEvents ()
{
	// Draw the bad line event...
	// From the moment the condition is identified and a full line!
	if (_eventStatus._badLine != std::numeric_limits <unsigned short>::max ())
	{ 
		if (_raster.vData ().currentVisiblePosition () == _eventStatus._badLine)
			screenMemory () -> setHorizontalLine 
				(_raster.hData ().currentVisiblePosition (), _eventStatus._badLine, 2, 128); // in points and draw in auxiliar color...
		else
			_eventStatus._badLine = std::numeric_limits <unsigned short>::max ();
	}
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
COMMODORE::TED::DrawResult COMMODORE::TED::drawMonoColorChar (int cb, bool c, int dCI, int dCN)
{
	COMMODORE::TED::DrawResult result;

	for (int i = 0; i < 8 /** To paint always 8 pixels */; i++)
	{
		int pp = cb + i;
		if (pp < 0)
			continue;
		if (pp >= 320)
			break;

		// Calculate the byte and bit where the pixel info is...
		size_t iBy = ((size_t) pp) >> 3; // To determine the byte...
		size_t iBt = 7 - (((size_t) pp) % 8); // From MSB to LSB...

		// The status of the pixels can vary a lot!
		// First: Pixel must be on or off?
		bool dP = _tedGraphicInfo._graphicData [iBy].bit (iBt);
		// If the reverse video is active, the pixels is inverted...
		if (_tedGraphicInfo._screenCodeData [iBy].bit (7) &&
			_TEDRegisters -> reverseVideoActive ()) dP = !dP;
		// Second: Blinks or not?: The bit of the byte read defines whether the color blinks or not...
		if (_tedGraphicInfo._colorData [iBy].bit (7) && 
			_TEDRegisters -> flashCounterOn ()) dP = false;
		// Thirs: Is the cursor active on it?:
		// ...if the cursor is over the pixels to draw and the cursor hardware is on, changes the status of the pixel...
		// Hardware status only happen in the MonoColorChar mode...
		if (c && _tedGraphicInfo.cursorHardwareStatus () && 
			i >= dCI && i < (dCI + dCN)) dP = !dP;

		// ...and finally draws it...
		// In the case of dP == false, it is redundant at it is already background...
		result._foregroundColorData [i] = dP 
			? (unsigned int) (_tedGraphicInfo._colorData [iBy].value () & 0x7f /** Without the bit 7 (tha define blinking mode or not) */)
			: (unsigned int) (_TEDRegisters -> backgroundColor ().asChar ());

		// When 0, it is background...
		// Not necessary to the color of the pixels as it will be always the basic background color,
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
				: _TEDRegisters -> backgroundColor ((size_t) cs).asChar ();
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
					: (unsigned int) (_TEDRegisters -> backgroundColor ().asChar ());
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
				: _TEDRegisters -> backgroundColor (cs).asChar ();
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
						? _TEDRegisters -> backgroundColor ().asChar ()
						: ((cs == 0x01) // The color is the defined in the video matrix (color) & color data (luminance)...
							? (((_tedGraphicInfo._screenCodeData [iBy].value () & 0xf0 /** bits 4, 5, 6 & 7. */) >> 4) /** to color position. */
								// There is a mistake in the TED documentation, it shows bits 4, 5 & 6 instead of bits 0, 1 & 2...
								| ((_tedGraphicInfo._colorData [iBy].value () & 0x07 /** bits 0, 1 & 2. */) << 4))
							: ((cs == 0x02) // The color is defined in the video matrix (color) & color data (luminance)...
								? ((_tedGraphicInfo._screenCodeData [iBy].value () & 0x0f /** bits 0, 1, 2 & 3 */)
									| (_tedGraphicInfo._colorData [iBy].value () & 0x70 /** bits 4, 5 & 6. */) /** to luminance position. */)
								: (_TEDRegisters -> backgroundColor (0x01).asChar ())));

		// ...and draws it!!
		result._foregroundColorData [i] = fc;
		result._foregroundColorData [i + 1] = fc;
	}

	return (result);
}

// ---
void COMMODORE::TED::drawResultToScreen (const COMMODORE::TED::DrawResult& cT, const COMMODORE::TED::DrawContext& dC)
{
	// The eight pixels to draw...
	for (size_t i = 0; i < 8; i++)
	{
		size_t pos = (size_t) dC._RCA + i;

		// If the graphic mode was invalid...
		if (cT._invalid)
			// the pixel will be always black by default...
			screenMemory () -> setPixel (pos, (size_t) dC._RR, 0x00 /** black. */);

		// ...and the foreground ones finally...
		if (cT._foregroundColorData [i] != ~0)
			screenMemory () -> setPixel (pos, (size_t) dC._RR, cT._foregroundColorData [i]);
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
	if (_cpuCycleRecovery._phase != CPUCyclePhase::_BEFOREACCESS)
		beginCPUCycle (cpu, batchStart, batchEnd);

	if (!completeCycle)
		return (true);

	if (!finishCPUCycle (cpu, batchStart, batchEnd))
		return (false);

	// The recovery cursor advances only after the CPU cycle is complete.
	++_lastCPUCycles;

	return (true);
}

// ---
void COMMODORE::TED::beginCPUCycle
	(MCHEmul::CPU* cpu,
	 unsigned int batchStart, unsigned int batchEnd)
{
	_cpuCycleRecovery._context = COMMODORE::TED::CPUCycleContext
		(_lastCPUCycles, singleClockModeForCurrentCycle ());
	_cpuCycleRecovery._halfCarryIn =
		_pendingHalfCyclesFromLastExecution;
	_cpuCycleRecovery._graphicsStarted = false;

	const COMMODORE::TED::CPUCycleContext& cC =
		_cpuCycleRecovery._context;
	const unsigned int halfTEDCycles =
		_cpuCycleRecovery._halfCarryIn + cC.halfTEDCycles ();

	const bool pulse = halfTEDCycles >= 2;
	const bool pulseBeforeAccess =
		cC._singleClock && _cpuCycleRecovery._halfCarryIn != 0;

	_pendingHalfCyclesFromLastExecution = halfTEDCycles & 0x01;
	_inSingleClockMode = cC._singleClock;

	COMMODORE::TED::DebugCycleContext dC = { };

	if (pulse)
	{
		_IFDEBUG
			dC = debugCycleContext
				(cC, batchStart, batchEnd,
				 _cpuCycleRecovery._halfCarryIn,
				 _pendingHalfCyclesFromLastExecution);
	}

	if (pulseBeforeAccess)
	{
		// This pulse precedes the CPU access and must not run again on resume.
		if (_TEDRegisters -> freezeActive ())
		{
			clockTEDCycle (false);

			_IFDEBUG debugFrozenTEDCycle (dC);
		}
		else
		{
			beginTEDCycle (dC);
			finishTEDCycle (cpu, cC._cycle, dC);
		}
	}
	else if (pulse && !_TEDRegisters -> freezeActive ())
	{
		beginTEDCycle (dC);
		_cpuCycleRecovery._graphicsStarted = true;
	}

	_cpuCycleRecovery._phase = CPUCyclePhase::_BEFOREACCESS;
}

// ---
bool COMMODORE::TED::finishCPUCycle
	(MCHEmul::CPU* cpu,
	 unsigned int batchStart, unsigned int batchEnd)
{
	const COMMODORE::TED::CPUCycleContext& cC =
		_cpuCycleRecovery._context;
	const unsigned int halfTEDCycles =
		_cpuCycleRecovery._halfCarryIn + cC.halfTEDCycles ();

	const bool pulse = halfTEDCycles >= 2;
	const bool pulseBeforeAccess =
		cC._singleClock && _cpuCycleRecovery._halfCarryIn != 0;

	if (_cpuTimedWritesEnabled &&
		!executePendingWriteAt (cpu, cC._cycle))
		return (false);

	if (pulse && !pulseBeforeAccess)
	{
		COMMODORE::TED::DebugCycleContext dC = { };

		_IFDEBUG
			dC = debugCycleContext
				(cC, batchStart, batchEnd,
				 _cpuCycleRecovery._halfCarryIn,
				 _pendingHalfCyclesFromLastExecution);

		// Retain the gating decision made before the CPU access.
		// A FREEZE write affects subsequent pulses.
		if (_cpuCycleRecovery._graphicsStarted)
			finishTEDCycle (cpu, cC._cycle, dC);
		else
		{
			clockTEDCycle (false);

			_IFDEBUG debugFrozenTEDCycle (dC);
		}
	}

	_pendingPacingCycles += cC.halfTEDCycles ();

	_cpuCycleRecovery._phase = CPUCyclePhase::_COMPLETE;

	return (true);
}

// ---
void COMMODORE::TED::beginTEDCycle (const DebugCycleContext& dC)
{
	_IFDEBUG debugTEDCycle (dC);

	// Preserve the current display-enable sampling during the first badline.
	_videoActive = (_raster.currentLine () == _FIRSTBADLINE)
		? !_TEDRegisters -> blankEntireScreen () : _videoActive;

	if (detectNewBadLine ())
	{
		_newBadLineCondition = true;
		_tedGraphicInfo._idleState = false;
		_eventStatus._badLine = _raster.vData ().currentVisiblePosition ();

		_IFDEBUG debugBadLine (dC);
	}

	// Select the clock before graphics processing, at the existing raster position.
	_inSingleClockMode = singleClockModeForCurrentCycle ();

	treatRasterCycle (dC);
}

// ---
void COMMODORE::TED::finishTEDCycle
	(MCHEmul::CPU* cpu, unsigned int requestCycle, const DebugCycleContext& dC)
{
	COMMODORE::TEDRegisters::GraphicalInfo& gI = _TEDRegisters -> graphicalInfo ();

	// Draw using the current raster position, before advancing it.
	if (_raster.isInVisibleZone ())
		drawVisibleZone (cpu);

	++_cycleInRasterLine;

	if (_raster.moveCycles (1))
	{
		_cycleInRasterLine = 1;
		_lastBadLineScrollY = -1;
		_newBadLineCondition = false;

		if (_raster.currentLine () == 0)
		{
			gI._currentCharacterPosition = 0;
			gI._currentCharacterRow = 0;
			_tedGraphicInfo._VC = 0;

			_TEDRegisters -> incrementFlashCounter ();

			if (++_timesFrameDrawn >= (_screenfrequency >> 1))
			{
				_timesFrameDrawn = 0;
				_tedGraphicInfo.changeCursorHardwareStatus ();
			}
		}
		else
			_tedGraphicInfo._VC = gI._currentCharacterPosition;

		if (_raster.currentLine () == _TEDRegisters -> IRQRasterLineAt ())
			_TEDRegisters -> activateRasterIRQ ();
	}

	_TEDRegisters -> setCurrentRasterPosition (_raster.currentLine (), _raster.currentColumn ());

	// Timers and sound receive the same completed slow-clock pulse.
	clockTEDCycle (true);

	const int reason = (int) _TEDRegisters -> reasonIRQCode ();
	if (reason != 0)
		cpu -> requestInterrupt (F6500::IRQInterrupt::_ID, requestCycle, this, reason);

	_IFDEBUG debugTEDCycleEnd (dC);

	// Presentation follows the raster transition that completed this pulse.
	if (_raster.isInLastVBlankZone ())
	{
		if (!_lastVBlankEntered)
		{
			_lastVBlankEntered = true;

			notify (MCHEmul::Event (_GRAPHICSREADY));
		}
	}
	else
		_lastVBlankEntered = false;
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
	(MCHEmul::CPU* cpu, unsigned int cycle)
{
	if (_nextPendingWrite == _pendingWrites.size () ||
		cycle != pendingWriteCycle ())
		return (true);

	MCHEmul::SetMemoryCommand& command =
		_pendingWrites [_nextPendingWrite];

	if (command.subset () == _TEDRegisters)
	{
		const size_t position = command.position () % 0x20;

		// Queued pulses must use the audio parameters that preceded this write.
		if (position >= 0x0e && position <= 0x12)
		{
			if (!_soundFunction -> simulateBeforeCPUWrite (cpu, cycle))
			{
				_error = MCHEmul::_CHIP_ERROR;

				return (false);
			}
		}
	}

	_IFDEBUG debugCPUWrite (cpu, cycle, command, false);

	command.execute ();
	applyCPUWriteEffects (command);

	if (command.subset () == _TEDRegisters)
	{
		const size_t position = command.position () % 0x20;

		if (position == 0x06 || position == 0x07)
			_raster.reduceDisplayZone
				(!_TEDRegisters -> textDisplay25RowsActive (),
				 !_TEDRegisters -> textDisplay40ColumnsActive ());
	}

	_IFDEBUG debugCPUWrite (cpu, cycle, command, true);

	++_nextPendingWrite;

	// A write can enable a pending source even without completing a TED pulse.
	const int reason = (int) _TEDRegisters -> reasonIRQCode ();
	if (reason != 0)
		cpu -> requestInterrupt
			(F6500::IRQInterrupt::_ID, cycle, this, reason);

	return (true);
}

//-----
// ---
std::string COMMODORE::TED::debugRasterAsString () const
{
	return ("Column=" + std::to_string (_raster.currentColumn ()) + "," +
		"ColumnBase0=" + std::to_string (_raster.currentColumnAtBase0 ()) + "," +
		"Row=" + std::to_string (_raster.currentLine ()) + "," +
		"RowBase0=" + std::to_string (_raster.currentLineAtBase0 ()) + "," +
		"Cycle=" + std::to_string (_cycleInRasterLine));
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
	(const CPUCycleContext& cC,
	 unsigned int batchStart, unsigned int batchEnd,
	 unsigned int halfCarryIn, unsigned int halfCarryOut) const
{
	COMMODORE::TED::DebugCycleContext result = { };

	result._cpuBatchStart = batchStart;
	result._cpuBatchEnd = batchEnd;
	result._cpuBatchIndex = cC._cycle - batchStart;
	result._cpuBatchCycles = batchEnd - batchStart;
	result._halfCarryIn = halfCarryIn;
	result._halfCarryOut = halfCarryOut;
	result._conversionSingleClock = cC._singleClock;
	result._irqRequestCycle = cC._cycle;

	// With a carried half, a slow CPU cycle completes the TED cycle halfway through.
	if (cC._singleClock && halfCarryIn != 0)
	{
		result._cpuCycle = cC._cycle;
		result._cpuHalf = 1;
	}
	else
		result._cpuCycle = cC._cycle + 1;

	return (result);
}

// ---
void COMMODORE::TED::debugTEDCycle (const DebugCycleContext& dC)
{
	_deepDebugFile -> writeCompleteLine (className (), dC._cpuCycle, "Info Cycle",
		{ { "Timing",
			"CPUHalf=" + std::to_string (dC._cpuHalf) + ",Reference=TEDCycleCompletion,Snapshot=BeforeCycle," +
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
				(_raster.currentLine () >= _FIRSTBADLINE && _raster.currentLine () <= _LASTBADLINE) },
		  { "Clock",
			"SingleClock=" + std::to_string (_inSingleClockMode) + "," +
			"ForcedSingleClock=" + std::to_string (_TEDRegisters -> singleClockModeForced ()) + "," +
			"Freeze=" + std::to_string (_TEDRegisters -> freezeActive ()) },
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
			",AccessPending=" + std::to_string
				(_cpuCycleRecovery._phase == CPUCyclePhase::_BEFOREACCESS) },
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
		{ { "Timing", "CPUHalf=" + std::to_string (dC._cpuHalf) + ",Reference=TEDCycleCompletion" },
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
		{ { "Timing", "CPUHalf=" + std::to_string (dC._cpuHalf) + ",Reference=TEDCycleCompletion" },
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
			const unsigned char mask = _TEDRegisters -> graphicExtendedColorTextModeActive ()
				? 0x3f : (_TEDRegisters -> reverseVideoActive () ? 0x7f : 0xff);
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
		{ { "Timing", "CPUHalf=" + std::to_string (dC._cpuHalf) + ",Reference=TEDCycleCompletion" },
		  { "Raster position", debugRasterAsString () },
		  { "Fetch", debugInternalAsString () },
		  { "Memory access", "Kind=" + kind + ",Source=" + source + ",Address=$" +
			MCHEmul::removeAll0 (address.asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) + ",Value=$" +
			_tedGraphicInfo._lastGraphicDataRead.asString (MCHEmul::UByte::OutputFormat::_HEXA, 2) + characterData } });
}

// ---
void COMMODORE::TED::debugTEDCycleEnd (const DebugCycleContext& dC)
{
	_deepDebugFile -> writeCompleteLine (className (), dC._cpuCycle, "Info Cycle End",
		{ { "Timing", "CPUHalf=" + std::to_string (dC._cpuHalf) + ",Reference=TEDCycleCompletion,Snapshot=AfterCycle" },
		  { "Raster position", debugRasterAsString () },
		  { "Internal", debugInternalAsString () },
		  { "Clock", "SingleClock=" + std::to_string (_inSingleClockMode) + "," +
			"ForcedSingleClock=" + std::to_string (_TEDRegisters -> singleClockModeForced ()) + "," +
			"Freeze=" + std::to_string (_TEDRegisters -> freezeActive ()) },
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
			"CPUHalf=" + std::to_string (dC._cpuHalf) +
			",Reference=TEDCycleCompletion" },
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
				std::to_string (cpu -> clockCycles ()) },
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
		  { "Clock",
			"Freeze=" + std::to_string (_TEDRegisters -> freezeActive ()) +
			",ForcedSingleClock=" +
				std::to_string (_TEDRegisters -> singleClockModeForced ()) },
		  { "Interrupt", debugInterruptAsString () } });
}
//-----

// ---
COMMODORE::TED_PAL::TED_PAL (int intId, unsigned short clkcpum, int vV, MCHEmul::SoundLibWrapper* wS)
	: COMMODORE::TED (intId, clkcpum, _SCREENFREQUENCY,
		 _VRASTERDATA, _HRASTERDATA, vV, wS,
		{ { "Name", "TED" },
		  { "Code", "7360/8360 for PAL" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Year", "1984" } })
{
	// Nothing else to do...
}

// ---
COMMODORE::TED_NTSC::TED_NTSC (int intId, unsigned short clkcpum, int vV, MCHEmul::SoundLibWrapper* wS)
	: COMMODORE::TED (intId, clkcpum, _SCREENFREQUENCY,
		 _VRASTERDATA, _HRASTERDATA, vV, wS,
		{ { "Name", "TED" },
		  { "Code", "7360/8360 for NTSC" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Year", "1984" } })
{
	// Nothing else to do...
}
