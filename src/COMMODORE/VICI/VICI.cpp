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
	HORIZONTAL RASTER: \n
	------------------ \n
	In one side: \n
	The VICI is connected to a clock at 4,433,618 Hz. \n
	It divides this clock by 4 to obtain approximately 1,108,404.5 CPU cycles per second. \n
	The original visible-width estimate was 233 native pixels. \n
	The existing inclusive interval 40..506 actually contains 467 internal positions, or 233.5 native pixels. \n
	Preserve that width while aligning its position with the current output timing. \n
	\n
	In the other side: \n
	Every line takes 71 CPU cycles and each cycle represents 4 native pixels. \n
	The emulator doubles the horizontal representation, giving 8 internal positions per cycle. \n
	The complete line therefore contains 71 * 8 = 568 positions, numbered 0..567. \n
	A horizontal-origin comparison at cycle N produces its first character pixels at N+6 in the current model. \n
	Those six cycles add 6 * 8 = 48 internal positions before the first output. \n
	The old visible interval did not include that output delay in its alignment. \n
	For the BASIC origin 12 and 22 columns observed in VIC20.LOG, output starts at (12 + 6) * 8 = 144. \n
	The text occupies 22 * 16 = 352 internal positions. \n
	With the old visible origin 40, its margins would be 144 - 40 = 104 and 467 - 104 - 352 = 11. \n
	Move the visible interval by the same 48 positions: 40 + 48 = 88 and 506 + 48 = 554. \n
	The margins then become 144 - 88 = 56 and 467 - 56 - 352 = 59. \n
	\n
	Finally: \n
	The left non-visible interval contains 88 positions, and the right one contains 567 - 554 = 13. \n
	568 - (88 HIDDEN LEFT + 56 LEFT MARGIN + 352 TEXT + 59 RIGHT MARGIN) = 13 HIDDEN RIGHT. \n
	The visible width remains 554 - 88 + 1 = 467 internal positions. \n
	\n
=>	As character acquisition and output are controlled by internal state, the display bounds cover the visible interval. \n
	Only the viewport alignment changes; line duration, origin comparisons and acquisition delays are preserved. \n
	This aligns the viewport with the current output model, rather than specifying measured analog blanking. \n
	So: 0,88,88,554,554,567. Total 568. */
const MCHEmul::RasterData COMMODORE::VICI_PAL::_VRASTERDATA	(0, 28, 28, 311, 311, 311, 311, 312, 0, 0);
const MCHEmul::RasterData COMMODORE::VICI_PAL::_HRASTERDATA	(0, 88, 88, 554, 554, 567, 567, 568, 0, 0);
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
	HORIZONTAL RASTER: \n
	------------------ \n
	In one side: \n
	The VICI is connected to a clock at 14,318,181 Hz. \n
	It divides this clock by 14 to obtain approximately 1,022,727 CPU cycles per second. \n
	The visible width is 210 native pixels, represented by 420 internal positions. \n
	The existing inclusive interval 8..427 contains those 420 positions. \n
	Preserve that width while applying the same output-delay alignment as PAL. \n
	\n
	In the other side: \n
	Every line takes 65 CPU cycles and each cycle represents 4 native pixels. \n
	The emulator doubles the horizontal representation, giving 8 internal positions per cycle. \n
	The complete line therefore contains 65 * 8 = 520 positions, numbered 0..519. \n
	A horizontal-origin comparison at cycle N produces its first character pixels at N+6 in the current model. \n
	Those six cycles add 6 * 8 = 48 internal positions before the first output. \n
	Move the visible interval by those 48 positions: 8 + 48 = 56 and 427 + 48 = 475. \n
	For example, origin 5 and 22 columns produce their first output at (5 + 6) * 8 = 88. \n
	The text occupies 22 * 16 = 352 positions, leaving margins of 88 - 56 = 32 and 420 - 32 - 352 = 36. \n
	Origin 5 is a calculation example; an NTSC execution and hardware comparison remain pending. \n
	\n
	Finally: \n
	The left non-visible interval contains 56 positions, and the right one contains 519 - 475 = 44. \n
	520 - (56 HIDDEN LEFT + 32 LEFT MARGIN + 352 TEXT + 36 RIGHT MARGIN) = 44 HIDDEN RIGHT. \n
	The visible width remains 475 - 56 + 1 = 420 internal positions. \n
	\n
=>	As character acquisition and output are controlled by internal state, the display bounds cover the visible interval. \n
	Only the viewport alignment changes; line duration, origin comparisons and acquisition delays are preserved. \n
	This is the NTSC counterpart of the PAL alignment, not a measured analog blanking specification. \n
	So: 0,56,56,475,475,519. Total 520. */
const MCHEmul::RasterData COMMODORE::VICI_NTSC::_VRASTERDATA (0, 28, 28, 260, 260, 260, 260, 261, 0, 0);
const MCHEmul::RasterData COMMODORE::VICI_NTSC::_HRASTERDATA (0, 56, 56, 475, 475, 519, 519, 520, 0, 0);

// ---
COMMODORE::VICI::SoundFunction::SoundFunction (MCHEmul::SoundLibWrapper* sW)
	: MCHEmul::SoundChip (_ID, 
		{ { "Name", "SoundFunction" },
		  { "Code", "Inside 6560 (NTSC), 6561/6561E/6561-101 (PAL)" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Year", "1977" } },
		sW),
	  _lastCPUCycles (0),
	  _clockSynchronized (false)
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
	_clockSynchronized = false;

	return (true);
}

// ---
bool COMMODORE::VICI::SoundFunction::simulateUntil (MCHEmul::CPU* cpu, unsigned int cycle)
{
	assert (cpu != nullptr);
	if (!_clockSynchronized)
	{
		synchronizeClock (cycle);

		return (true);
	}

	// Reject a future target or a request to replay time already completed.
	if (cycle - _lastCPUCycles > cpu -> clockCycles () - _lastCPUCycles)
	{
		_error = MCHEmul::_CHIP_ERROR;

		return (false);
	}

	for (unsigned int remaining = cycle - _lastCPUCycles; remaining > 0; --remaining)
	{
		if (soundWrapper () != nullptr)
		{
			_IFDEBUG debugVICISoundCycle (cpu, cpu -> clockCycles () - _lastCPUCycles);

			MCHEmul::UBytes data;
			if (soundWrapper () -> getData (cpu, data))
				for (size_t j = 0; j < data.size (); ++j)
				{
					char value = (char) data [j].value ();
					if (soundMemory () -> addSampleData (&value, sizeof (char)))
						MCHEmul::SoundChip::notify (MCHEmul::Event (_SOUNDREADY));
				}
		}

		++_lastCPUCycles;
	}

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
	  _videoMemory (nullptr),
	  _videoBusData (MCHEmul::UByte (0xff)),
	  _videoBusColor (MCHEmul::UByte (0x0f)),
	  _lastCPUCycles (0),
	  _format (nullptr),
	  _cycleInRasterLine (0),
	  _lastVBlankEntered (false),
	  _scrX1 (0), _scrY1 (0), _scrX2 (0), _scrY2 (0),
	  _lightPenFrameLatched (false), _lightPenButtonPressed (false),
	  _timedWriteCPU (nullptr),
	  _timedWriteSubsets (),
	  _writeBusCycleData (nullptr),
	  _writeStartCycle (0),
	  _pendingWrites (),
	  _cyclesSimulatedInAdvance (0),
	  _advancedCyclePending (false)
{
	assert (_cyclesPerRasterLine >= 65);

	setClassName ("VICI");

	_format = SDL_AllocFormat (SDL_PIXELFORMAT_ARGB8888);
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

	_videoMemory = memoryRef () -> view (_VICIView);
	if (_videoMemory == nullptr)
	{
		MCHEmul::GraphicalChip::_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	// Deterministic initial values; these do not specify physical power-on levels.
	_videoBusData = MCHEmul::UByte (0xff);
	_videoBusColor = MCHEmul::UByte (0x0f);

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
	_timedWriteCPU = nullptr;
	_timedWriteSubsets.clear ();
	_writeBusCycleData = nullptr;
	_writeStartCycle = 0;
	_pendingWrites.clear ();
	_cyclesSimulatedInAdvance = 0;
	_advancedCyclePending = false;
	
	_cycleInRasterLine = 1;

	_lastVBlankEntered = false;

	_scrX1 = _scrY1 = _scrX2 = _scrY2 = 0;

	_lightPenFrameLatched = false;
	_lightPenButtonPressed = false;

	_vicGraphicInfo.initialize (_VICIRegisters);

	return (true);
}

// ---
bool COMMODORE::VICI::enableCPUTimedWrites
	(MCHEmul::CPU* cpu, const MCHEmul::PhysicalStorageSubsets& subsets)
{
	assert (cpu != nullptr && _VICIRegisters != nullptr);
	assert (_timedWriteCPU == nullptr && _pendingWrites.empty ());
	const MCHEmul::PhysicalStorageSubsets::const_iterator registers =
		subsets.find (_VICIRegisters -> id ());

	if (cpu -> ticksCounter () != nullptr || registers == subsets.end () ||
		registers -> second != _VICIRegisters)
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	for (const auto& subset : subsets)
	{
		if (subset.second == nullptr ||
			!MCHEmul::Memory::configuration ().bufferMemorySetCommands (subset.second))
		{
			_error = MCHEmul::_INIT_ERROR;

			return (false);
		}
	}

	_timedWriteCPU = cpu;
	_timedWriteSubsets = subsets;
	_lastCPUCycles = cpu -> clockCycles ();
	_soundFunction -> synchronizeClock (_lastCPUCycles);

	return (true);
}

// ---
void COMMODORE::VICI::CPUAboutToExecute
	(const MCHEmul::InstructionContextEventData* dt)
{
	assert (dt != nullptr);

	if (_timedWriteCPU == nullptr || _error != MCHEmul::_NOERROR)
		return;

	const MCHEmul::InstructionDefined* instruction =
		static_cast <const MCHEmul::InstructionDefined*> (dt -> _instruction);
	const size_t cycleStructure =
		(size_t) (dt -> _clockCycles - instruction -> clockCycles ());
	const MCHEmul::BusCycleData& busData =
		instruction -> busCycleData (cycleStructure);

	if (!prepareCPUTransaction (dt -> _cpu, &busData))
		return;

	unsigned int cycles = 0;
	if (busData._lastReadCycle != MCHEmul::BusCycleData::_NOCYCLE)
		cycles = (unsigned int) (busData._lastReadCycle + 1);
	// Buffered writes do not exist yet: stop before the first write cycle.
	if (busData._numberWriteCycles != 0 &&
		cycles > busData._firstWriteCycle)
		cycles = (unsigned int) (busData._firstWriteCycle);

	_IFDEBUG debugCPUAboutToExecute (dt, cycles);

	if (!simulate (dt -> _cpu, cycles))
		_error = MCHEmul::_CHIP_ERROR;
}

// ---
void COMMODORE::VICI::CPUAboutToExecute (const MCHEmul::InterruptContextEventData* dt)
{
	assert (dt != nullptr);
	if (_timedWriteCPU == nullptr || _error != MCHEmul::_NOERROR)
		return;

	prepareCPUTransaction (dt -> _cpu, &dt -> _interrupt -> busCycleData ());
}

// ---
bool COMMODORE::VICI::simulate (MCHEmul::CPU* cpu)
{
	assert (cpu != nullptr);

	const unsigned int currentCPUCycles = cpu -> clockCycles ();

	if (_timedWriteCPU != nullptr)
	{
		if (cpu != _timedWriteCPU || _error != MCHEmul::_NOERROR ||
			cpu -> ticksCounter () != nullptr)
		{
			_error = MCHEmul::_CHIP_ERROR;

			return (false);
		}

		if (_writeBusCycleData != nullptr)
		{
			const unsigned int elapsedCycles =
				currentCPUCycles - _writeStartCycle;

			// This mode requires complete transactions and cannot replay time.
			if (elapsedCycles != _writeBusCycleData -> _numberCycles ||
				elapsedCycles < _cyclesSimulatedInAdvance ||
				_lastCPUCycles !=
					_writeStartCycle + _cyclesSimulatedInAdvance)
			{
				_error = MCHEmul::_CHIP_ERROR;

				return (false);
			}
		}
		else
			assert (_cyclesSimulatedInAdvance == 0 &&
				!_advancedCyclePending);

		if (!extractPendingWrites ())
			return (false);

		if (_advancedCyclePending)
		{
			// Its video phase ran before the CPU performed the register read.
			if (!finishRasterCycle (cpu, _lastCPUCycles - 1))
				return (false);

			_advancedCyclePending = false;
		}
	}
	else
	if (_lastCPUCycles == 0)
	{
		// Preserve direct-access component operation without a connected scheduler.
		_lastCPUCycles = currentCPUCycles;

		return (true);
	}

	for (unsigned int remaining = currentCPUCycles - _lastCPUCycles;
		remaining > 0; --remaining)
	{
		if (!simulateRasterCycle (cpu, _lastCPUCycles, true))
			return (false);

		++_lastCPUCycles;
	}

	if (!_pendingWrites.empty ())
	{
		_error = MCHEmul::_CHIP_ERROR;

		return (false);
	}

	_writeBusCycleData = nullptr;
	_cyclesSimulatedInAdvance = 0;
	_advancedCyclePending = false;

	return (true);
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
MCHEmul::UBytes COMMODORE::VICI::screenMemorySnapShot (MCHEmul::CPU* cpu) const
{
	std::vector <MCHEmul::UByte> result;
	result.reserve (0x01fa);

	for (size_t i = 0; i < 0x01fa; ++i)
		result.emplace_back
			(_videoMemory -> value (_VICIRegisters -> screenMemory (i)));

	return (MCHEmul::UBytes (std::move (result)));
}

// ---
MCHEmul::UBytes COMMODORE::VICI::colorMemorySnapShot (MCHEmul::CPU* cpu) const
{
	std::vector <MCHEmul::UByte> result;
	result.reserve (0x01fa);

	for (size_t i = 0; i < 0x01fa; ++i)
		result.emplace_back
			(_videoMemory -> value (_VICIRegisters -> colourMemory (i)) & 0x0f);

	return (MCHEmul::UBytes (std::move (result)));
}

// ---
MCHEmul::Strings COMMODORE::VICI::charsDrawSnapshot (MCHEmul::CPU* cpu,
	const std::vector <size_t>& chrs) const
{
	// The number of bytes per char will depend on the mode,
	// because in the expanded mode, each character is drawn with 16 bytes instead of 8...
	const size_t bytesPerChar = 
		_VICIRegisters -> charsExpanded () ? 16 : 8;

	MCHEmul::Strings result;
	for (size_t i = 0; i < 256; i++)
	{
		if (!chrs.empty () && 
			std::find (chrs.begin (), chrs.end (), i) == chrs.end ())
			continue;

		MCHEmul::Address chrAdd = 
			_VICIRegisters -> charDataMemory (i * bytesPerChar);
		std::string dt = std::to_string (i) + "---\n$" +
			MCHEmul::removeAll0 (chrAdd.asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) + "\n";
		std::vector <MCHEmul::UByte> chrBytes;
		chrBytes.reserve (bytesPerChar);
		for (size_t j = 0; j < bytesPerChar; ++j)
			chrBytes.emplace_back
				(_videoMemory -> value
					(_VICIRegisters -> charDataMemory (i * bytesPerChar + j)));
		MCHEmul::UBytes chrDt (std::move (chrBytes));
		for (size_t j = 0; j < bytesPerChar; j++) // 8/16 lines per character...
		{
			if (j != 0)
				dt += "\n";

			for (size_t l = 0; l < 8; l++) // 8 bits per byte...
				dt += ((chrDt [j].value () & (1 << (7 - l))) != 0x00) ? "#" : " ";

			// Draws the data info...
			dt += "|$" + chrDt [j].asString (MCHEmul::UByte::OutputFormat::_HEXA, 2);
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
					_VICIRegisters -> setMousePosition (-1, -1); // No in the window...
				else
					_VICIRegisters -> setMousePosition (x, y);
			}

			break;

		// The lightpen actives when the left button of the mouse is pressed...
		case MCHEmul::InputOSSystem::_MOUSEBUTTONPRESSED:
			{
				if (std::dynamic_pointer_cast <MCHEmul::InputOSSystem::MouseButtonEvent> 
					(evnt.data ()) -> _buttonId == 1 /** Left button = 1, right will be = 3 */)
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
MCHEmul::ScreenMemory* COMMODORE::VICI::createScreenMemory ()
{
	unsigned int* cP = new unsigned int [16];

	// Mike PAL reference palette; retain the renderer's existing alpha.
	// Source: https://raw.githubusercontent.com/VICE-Team/svn-mirror/main/vice/data/VIC20/mike-pal.vpl
	cP [0]  = SDL_MapRGBA (_format, 0x00, 0x00, 0x00, 0xe0); // Black
	cP [1]  = SDL_MapRGBA (_format, 0xff, 0xff, 0xff, 0xe0); // White
	cP [2]  = SDL_MapRGBA (_format, 0xb6, 0x1f, 0x21, 0xe0); // Red
	cP [3]  = SDL_MapRGBA (_format, 0x4d, 0xf0, 0xff, 0xe0); // Cyan
	cP [4]  = SDL_MapRGBA (_format, 0xb4, 0x3f, 0xff, 0xe0); // Purple
	cP [5]  = SDL_MapRGBA (_format, 0x44, 0xe2, 0x37, 0xe0); // Green
	cP [6]  = SDL_MapRGBA (_format, 0x1a, 0x34, 0xff, 0xe0); // Blue
	cP [7]  = SDL_MapRGBA (_format, 0xdc, 0xd7, 0x1b, 0xe0); // Yellow
	cP [8]  = SDL_MapRGBA (_format, 0xca, 0x54, 0x00, 0xe0); // Orange
	cP [9]  = SDL_MapRGBA (_format, 0xe9, 0xb0, 0x72, 0xe0); // Light orange
	cP [10] = SDL_MapRGBA (_format, 0xe7, 0x92, 0x93, 0xe0); // Light red
	cP [11] = SDL_MapRGBA (_format, 0x9a, 0xf7, 0xfd, 0xe0); // Light cyan
	cP [12] = SDL_MapRGBA (_format, 0xe0, 0x9f, 0xff, 0xe0); // Light purple
	cP [13] = SDL_MapRGBA (_format, 0x8f, 0xe4, 0x93, 0xe0); // Light green
	cP [14] = SDL_MapRGBA (_format, 0x82, 0x90, 0xff, 0xe0); // Light blue
	cP [15] = SDL_MapRGBA (_format, 0xe5, 0xde, 0x85, 0xe0); // Light yellow

	return (new MCHEmul::ScreenMemory (numberColumns (), numberRows (), cP));
}

// ---
void COMMODORE::VICI::readGraphicsAndDrawVisibleZone ()
{
	_vicGraphicInfo._graphicDataReady = false;

	// Preparing MATRIX at the end of startup does not acquire data in this cycle.
	if (_vicGraphicInfo._horizontalOpened &&
		_vicGraphicInfo._fetchPhase == VICGraphicInfo::FetchPhase::_START)
		_vicGraphicInfo.advanceFetchStartup ();
	else
	if (_vicGraphicInfo.draw ())
	{
		switch (_vicGraphicInfo._fetchPhase)
		{
			case VICGraphicInfo::FetchPhase::_START:
				{
					// Startup is consumed before entering the active acquisition switch.
				}

				break;

			case VICGraphicInfo::FetchPhase::_MATRIX:
				{
					readVideoMatrixAndColorRAM ();

					_vicGraphicInfo._fetchPhase =
						VICGraphicInfo::FetchPhase::_GLYPH;
				}

				break;

			case VICGraphicInfo::FetchPhase::_GLYPH:
				{
					readGraphicalInfo ();

					// Publish the glyph and its matching retained color together.
					_vicGraphicInfo._colorData =
						_vicGraphicInfo._pendingColorData;
					_vicGraphicInfo._outputHalvesLeft = 2;

					_vicGraphicInfo._fetchPhase =
						VICGraphicInfo::FetchPhase::_MATRIX;

					// Complete the acquired column independently of the running matrix counter.
					_vicGraphicInfo.completeCharacterFetch ();
				}

				break;
		}
	}

	// A matrix read consumes the retained address before this cycle can replace it.
	_vicGraphicInfo.advanceMatrixCycle ();

	// Output continues while the next character is acquired or the window closes.
	if (_vicGraphicInfo._outputHalvesLeft != 0)
	{
		_vicGraphicInfo._graphicData =
			(_vicGraphicInfo._outputHalvesLeft == 2)
				? ((_vicGraphicInfo._graphicByteData.value () >> 4) & 0x0f)
				: (_vicGraphicInfo._graphicByteData.value () & 0x0f);

		_vicGraphicInfo._graphicDataReady = true;

		--_vicGraphicInfo._outputHalvesLeft;
	}

	calculateScreenPositions ();

	if (_raster.isInVisibleZone ())
		drawVisibleZone ();
}

// ---
void COMMODORE::VICI::treatRasterCycle ()
{
	if (_cycleInRasterLine == _cyclesPerRasterLine)
		_vicGraphicInfo.nextVertical (_VICIRegisters);

	++_cycleInRasterLine;
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
	if (_vicGraphicInfo.graphicDataReady ())
		drawGraphics (
			{
				/** _ICD */ _raster.hData ().firstDisplayPosition (),
				/** _LCD */ _raster.hData ().lastDisplayPosition (),
				/** _RC	 */ cv,		// Where the horizontal raster is (not adjusted to 8) inside the window
				/** _IRD */ _raster.vData ().firstDisplayPosition (),
				/** _LRD */ _raster.vData ().lastDisplayPosition (),
				/** _RR	 */ rv		// Where the vertical raster is inside the window (it is not the chip raster line)
			});
}

// ---
void COMMODORE::VICI::drawGraphics (const COMMODORE::VICI::DrawContext& dC)
{
	// Fill the visible part of this cycle with the common background.
	screenMemory () -> setHorizontalLine ((size_t) dC._RC, (size_t) dC._RR,
		(dC._RC + 8) > _raster.visibleColumns ()
			? (_raster.visibleColumns () - dC._RC) : 8,
		_VICIRegisters -> backgroundColor ());

	// Color RAM bit 3 selects the character decoding mode.
	drawResultToScreen (
		((_vicGraphicInfo._colorData.value () & 0x08) == 0x00)
			? drawHighResolutionMode ()
			: drawMulticolorMode (),
		dC);
}

// ---
COMMODORE::VICI::DrawResult COMMODORE::VICI::drawHighResolutionMode ()
{
	COMMODORE::VICI::DrawResult result;

	for (unsigned short i = 0 ; 
			i < 8 /** To paint always 8 pixels but in blocks of 2. */; i += 2)
	{
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
COMMODORE::VICI::DrawResult COMMODORE::VICI::drawMulticolorMode ()
{
	COMMODORE::VICI::DrawResult result;

	for (unsigned short i = 0 ; 
			i < 8 /** To paint always 8 pixels but in blocks of 4. */; i += 4)
	{
		unsigned char cs =
			(i == 0)
				? ((_vicGraphicInfo._graphicData.value () >> 2) & 0x03)
				: (_vicGraphicInfo._graphicData.value () & 0x03);

		if (cs == 0x00)
			continue;

		unsigned int fc =
			(cs == 0x01)
				? _VICIRegisters -> borderColor ()
				: ((cs == 0x02)
					? (_vicGraphicInfo._colorData.value () & 0x07)
					: _VICIRegisters -> auxiliarColor ());

		result._foregroundColorData [i] =
		result._foregroundColorData [i + 1] =
		result._foregroundColorData [i + 2] =
		result._foregroundColorData [i + 3] = fc;
	}

	return (result);
}

// ---
void COMMODORE::VICI::drawResultToScreen (
	const COMMODORE::VICI::DrawResult& cT,
	const COMMODORE::VICI::DrawContext& dC)
{
	// The caller supplies an origin inside the visible frame.
	// Only the positions remaining in the current row may be written.
	const size_t columns = screenMemory () -> columns ();
	const size_t nP = std::min (
		(size_t) 8, columns - (size_t) dC._RC);

	size_t pos = (size_t) dC._RR * columns + (size_t) dC._RC;
	for (size_t i = 0; i < nP; ++i, ++pos)
	{
		if (cT._backgroundColorData [i] != ~0)
			screenMemory () -> setPixel (pos, cT._backgroundColorData [i]);

		if (cT._foregroundColorData [i] != ~0)
			screenMemory () -> setPixel (pos, cT._foregroundColorData [i]);
	}
}

// ---
void COMMODORE::VICI::debugVICICycle (unsigned int cycle)
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeCompleteLine (className (), cycle, "Info Cycle",
		{ { "Raster", 
				std::to_string (_raster.currentColumnAtBase0 ()) + "," +
				std::to_string (_raster.currentLineAtBase0 ()) + "," +
				std::to_string (_cycleInRasterLine) },
		  { "CPU timing",
				"CyclesAdvanced=" + std::to_string (_cyclesSimulatedInAdvance) },
		  { "Fetch",
			"MatrixBaseHalf=" +
				std::to_string (_vicGraphicInfo._matrixBaseCounter) + "," +
			"MatrixHalf=" +
				std::to_string (_vicGraphicInfo._matrixCounter) + "," +
			"Column=" +
				std::to_string (_vicGraphicInfo._COL) + "," +
			"Columns=" +
				std::to_string (_vicGraphicInfo._COLMAX) + "," +
			std::string ("Phase=") +
				((_vicGraphicInfo._fetchPhase == VICGraphicInfo::FetchPhase::_START)
					? "Start"
					: ((_vicGraphicInfo._fetchPhase == VICGraphicInfo::FetchPhase::_MATRIX)
						? "Matrix" : "Glyph")) + "," +
			"StartupLeft=" +
				std::to_string (_vicGraphicInfo._startupCyclesLeft) + "," +
			"CounterStartupLeft=" +
				std::to_string (_vicGraphicInfo._counterStartupCyclesLeft) + "," +
			"CounterHalvesLeft=" +
				std::to_string (_vicGraphicInfo._counterHalvesLeft) + "," +
			"MatrixOffset=" +
				std::to_string (_vicGraphicInfo._matrixOffset) + "," +
			"PendingCode=$" +
				_vicGraphicInfo._screenCodeData.asString
					(MCHEmul::UByte::OutputFormat::_HEXA, 2) + "," +
			"PendingColor=$" +
				_vicGraphicInfo._pendingColorData.asString
					(MCHEmul::UByte::OutputFormat::_HEXA, 2) + "," +
			"OutputByte=$" +
				_vicGraphicInfo._graphicByteData.asString
					(MCHEmul::UByte::OutputFormat::_HEXA, 2) + "," +
			"OutputColor=$" +
				_vicGraphicInfo._colorData.asString
					(MCHEmul::UByte::OutputFormat::_HEXA, 2) + "," +
			"HalvesLeft=" + std::to_string (_vicGraphicInfo._outputHalvesLeft) },
		  { "Memory",
				"Screen=$" + MCHEmul::removeAll0 (_VICIRegisters -> screenMemory ().asString
					(MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) + "," +
				"Characters=$" + MCHEmul::removeAll0 (_VICIRegisters -> charDataMemory ().asString
					(MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) + "," +
				"Color=$" + MCHEmul::removeAll0 (_VICIRegisters -> colourMemory ().asString
				(MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) } });
}

// ---
void COMMODORE::VICI::debugVICILineEnd (unsigned int cycle)
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeCompleteLine (className (), cycle, "Line end",
		{ { "Raster",
			"Line=" +
				std::to_string (_raster.vData ().currentPosition ()) + "," +
			"Cycle=" +
				std::to_string (_cycleInRasterLine) },
		  { "Matrix",
			"BaseHalf=" +
				std::to_string (_vicGraphicInfo._matrixBaseCounter) + "," +
			"EndHalf=" +
				std::to_string (_vicGraphicInfo._matrixCounter) },
		  { "Acquisition",
			"Opened=" +
				std::to_string (_vicGraphicInfo._horizontalOpened) + "," +
			"Column=" +
				std::to_string (_vicGraphicInfo._COL) + "," +
			"Columns=" +
				std::to_string (_vicGraphicInfo._COLMAX) + "," +
			"StartupLeft=" +
				std::to_string (_vicGraphicInfo._startupCyclesLeft) },
		  { "Vertical",
			"Row=" +
				std::to_string (_vicGraphicInfo._ROW) + "," +
			"GlyphLine=" +
				std::to_string (_vicGraphicInfo._RC) } });
}

// ---
void COMMODORE::VICI::debugCPUAboutToExecute
	(const MCHEmul::InstructionContextEventData* dt,
	 unsigned int cycles)
{
	assert (_deepDebugFile != nullptr);
	assert (dt != nullptr);

	_deepDebugFile -> writeCompleteLine
		(className (), dt -> _cpu -> clockCycles (),
		 "CPU transaction timing",
		{ { "Transaction",
			"Type=Instruction," +
			std::string ("Address=$") +
				MCHEmul::removeAll0 (dt -> _address.asString
					(MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) + "," +
			"Code=" + std::to_string (dt -> _instruction -> code ()) + "," +
			"Cycles=" + std::to_string (dt -> _clockCycles) },
		  { "Raster origin",
			"Row=" + std::to_string (_raster.currentLine ()) + "," +
			"Cycle=" + std::to_string (_cycleInRasterLine) },
		  { "CPU read point",
			"AdvanceCycles=" + std::to_string (cycles) } });
}

// ---
bool COMMODORE::VICI::prepareCPUTransaction
	(MCHEmul::CPU* cpu, const MCHEmul::BusCycleData* busData)
{
	assert (cpu != nullptr && busData != nullptr);
	if (cpu != _timedWriteCPU || cpu -> ticksCounter () != nullptr ||
		_writeBusCycleData != nullptr || !_pendingWrites.empty () ||
		_cyclesSimulatedInAdvance != 0 || _advancedCyclePending ||
		_lastCPUCycles != cpu -> clockCycles ())
	{
		_error = MCHEmul::_CHIP_ERROR;

		return (false);
	}

	_writeBusCycleData = busData;
	_writeStartCycle = cpu -> clockCycles ();

	return (true);
}

// ---
bool COMMODORE::VICI::extractPendingWrites ()
{
	assert (_pendingWrites.empty ());
	MCHEmul::Memory::configuration ().extractMemorySetCommandsBuffered (_timedWriteSubsets, _pendingWrites);

	if (_pendingWrites.empty ())
		return (true);

	if (_writeBusCycleData == nullptr || _pendingWrites.size () != _writeBusCycleData -> _numberWriteCycles)
	{
		_error = MCHEmul::_CHIP_ERROR;

		return (false);
	}

	return (true);
}

// ---
bool COMMODORE::VICI::executePendingWriteAt (MCHEmul::CPU* cpu, unsigned int cycle)
{
	if (_pendingWrites.empty ())
		return (true);

	const size_t writeIndex = _writeBusCycleData -> _numberWriteCycles - _pendingWrites.size ();
	if (cycle != _writeStartCycle + (unsigned int) (_writeBusCycleData -> writeCycle (writeIndex)))
		return (true);

	MCHEmul::SetMemoryCommand& command = _pendingWrites.front ();
	if (command.subset () == _VICIRegisters)
	{
		const size_t position = command.position () % 0x10;
		// Finish the old audio interval before a write changes its parameters.
		if (position >= 0x0a && position <= 0x0e)
		{
			if (!_soundFunction -> simulateUntil (cpu, cycle + 1))
			{
				_error = MCHEmul::_CHIP_ERROR;
				return (false);
			}
		}
	}

	command.execute ();
	_pendingWrites.erase (_pendingWrites.begin ());

	return (true);
}

// ---
bool COMMODORE::VICI::simulate
	(MCHEmul::CPU* cpu, unsigned int cycles)
{
	assert (cpu != nullptr && cpu == _timedWriteCPU);
	assert (_writeBusCycleData != nullptr);
	assert (_cyclesSimulatedInAdvance == 0 && !_advancedCyclePending);
	assert (_pendingWrites.empty ());

	for (unsigned int i = 0; i < cycles; ++i)
	{
		// Only the final cycle waits for the CPU read before completing.
		if (!simulateRasterCycle
			(cpu, _lastCPUCycles, i + 1 != cycles))
			return (false);

		++_lastCPUCycles;
		++_cyclesSimulatedInAdvance;
	}

	_advancedCyclePending = (cycles != 0);

	return (true);
}

// ---
bool COMMODORE::VICI::simulateRasterCycle
	(MCHEmul::CPU* cpu, unsigned int cycle, bool completeCycle)
{
	_vicGraphicInfo.beginRasterCycle
		(_VICIRegisters,
		 (unsigned short) (_cycleInRasterLine - 1),
		 _raster.vData ().currentPosition ());

	_IFDEBUG debugVICICycle (cycle);

	readGraphicsAndDrawVisibleZone ();

	if (completeCycle)
		return (finishRasterCycle (cpu, cycle));

	return (true);
}

// ---
bool COMMODORE::VICI::finishRasterCycle
	(MCHEmul::CPU* cpu, unsigned int cycle)
{
	// CPU writes precede the counter updates of their own cycle.
	if (!executePendingWriteAt (cpu, cycle))
		return (false);

	if (_cycleInRasterLine == _cyclesPerRasterLine)
		_IFDEBUG debugVICILineEnd (cycle);

	treatRasterCycle ();
	treatLightPenAtCurrentRasterPosition ();

	if (_raster.moveCycles (1))
	{
		_cycleInRasterLine = 1;
		if (_raster.vData ().currentPosition () == 0)
		{
			_lightPenFrameLatched = false;
			// Frame reset preserves sampled dimensions until their regular capture points.
			_vicGraphicInfo.initializeFrame ();
		}
	}

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

	return (true);
}

// ---
MCHEmul::UByte COMMODORE::VICI::acquireVideoMemory
	(const MCHEmul::Address& address)
{
	// Resolve data using the retained bus state from before this acquisition.
	_videoBusData = _videoMemory -> value (address);

	// Both matrix and glyph acquisitions address the physical color RAM.
	_videoBusColor = _videoMemory -> value
		(MCHEmul::Address (2, 0x9400 + (address.value () & 0x03ff))) & 0x0f;

	return (_videoBusData);
}

// ---
void COMMODORE::VICI::VICGraphicInfo::beginRasterCycle
	(VICIRegisters* v, unsigned short horizontalCycle,
	 unsigned short rasterLine)
{
	_SCREENORIGINX = false;
	_SCREENORIGINY = false;
	_matrixAdvancedThisCycle = false;

	// Reload belongs to the physical line boundary, independently of window opening.
	if (horizontalCycle == 0)
		reloadMatrixCounter ();

	// Sampling is independent of vertical display and horizontal opening.
	if (horizontalCycle == 1)
		_pendingColumns = v -> charsWidthScreen ();

	if (_verticalState == VerticalState::_WAITING &&
		v -> offsetYScreen () == (rasterLine >> 1))
		_verticalState = (_ROWMAX == 0)
			? VerticalState::_DONE
			: VerticalState::_PENDING;

	if (!_horizontalOpened &&
		(_verticalState == VerticalState::_PENDING ||
		 _verticalState == VerticalState::_DISPLAY) &&
		v -> offsetXScreen () == horizontalCycle)
	{
		_horizontalOpened = true;
		_SCREENORIGINX = true;

		// Keep a provisional width until the counter controller captures its active sample.
		_COLMAX = _pendingColumns;

		_fetchPhase = FetchPhase::_START;
		_startupCyclesLeft = _FETCHSTARTDELAY;
		// Include the comparison cycle: the first increment occurs at N+3.
		_counterStartupCyclesLeft = _COUNTERSTARTDELAY + 1;

		// The first horizontal opening starts actual vertical consumption.
		if (_verticalState == VerticalState::_PENDING)
		{
			_verticalState = VerticalState::_DISPLAY;
			_SCREENORIGINY = true;
		}
	}

	// Origin comparisons use the previously retained limit in this cycle.
	if (rasterLine == 0 && horizontalCycle == 2)
		_ROWMAX = v -> charsHeightScreen ();

	setDraw ();
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
