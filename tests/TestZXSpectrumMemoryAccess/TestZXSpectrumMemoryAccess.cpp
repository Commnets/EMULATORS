/** \ingroup TESTS */
/*@{*/

/**
 *	@file
 *	File: TestZXSpectrumMemoryAccess.cpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 27/10/2026 \n
 *	Description: Spectrum memory-access capture, timing and contention integration tests. \n
 *	Run from emulators/ZXSpectrumCommons for the existing ROM files. \n
 *	Integration expectations follow the current PAL/NTSC raster convention. \n
 *	These tests do not claim complete bus or hardware timing accuracy.
 *	Versions: 1.0 Initial
 */

#include <ZXSpectrum/CZ80.hpp>
#include <ZXSpectrum/Memory.hpp>
#include <ZXSpectrum/ZXSpectrum.hpp>
#include <ZXSpectrum/ULA.hpp>
#include <FZ80/INTInterrupt.hpp>
#include <CORE/Ticks.hpp>
#include <SDL.h>

#include <iostream>
#include <string>
#include <vector>

#if defined (_MSC_VER)
#include <crtdbg.h>
#endif

#undef main

/** Deterministic input used to test deferred port completion. */
class FixedInputPort final : public FZ80::Z80Port
{
	public:
	FixedInputPort ()
		: FZ80::Z80Port (0x10, "Memory access test"),
		  _reads (0) { }

	unsigned int reads () const
							{ return (_reads); }
	virtual MCHEmul::UByte value
		(unsigned short, unsigned char) const override;
	virtual MCHEmul::UByte peekValue
		(unsigned short, unsigned char) const override
							{ return (MCHEmul::UByte (0xa5)); }
	virtual void setValue
		(unsigned short, unsigned char, const MCHEmul::UByte&) override { }
	virtual void initialize () override
							{ _reads = 0; }

	private:
	mutable unsigned int _reads;
};

/** Exercises production CPU and memory entry points with literal access expectations. */
class MemoryAccessTests final
{
	public:
	using Access = ZXSPECTRUM::CZ80::MemoryAccess;
	using Accesses = ZXSPECTRUM::CZ80::MemoryAccesses;
	using Type = Access::Type;
	using Origin = Access::Origin;

	struct Case
	{
		const char* _name;
		unsigned short _pc;
		unsigned short _hl;
		std::vector <MCHEmul::UByte> _code;
		Accesses _expected;
		bool _timingAvailable;
		unsigned int _cycles;
		bool _pendingInput;
	};

	static int run ();

	private:
	static void check (bool ok, const std::string& message);
	static bool executeOperation
		(ZXSPECTRUM::CZ80& cpu, MCHEmul::TicksCounterDelayed& ticks,
		 bool perCycle, unsigned short expectedPC);
	static void checkAccess
		(const Access& actual, const Access& expected,
		 bool timingAvailable, size_t index);
	static void runCase
		(const Case& test, bool perCycle,
		 ZXSPECTRUM::Memory::Configuration configuration);

	static unsigned int _checks, _failures;
};

unsigned int MemoryAccessTests::_checks = 0, MemoryAccessTests::_failures = 0;

// ---
MCHEmul::UByte FixedInputPort::value
	(unsigned short, unsigned char) const
{
	_reads++;

	return (MCHEmul::UByte (0xa5));
}

// ---
int MemoryAccessTests::run ()
{
	// Expected points are literal, instruction-relative T-state indexes.
	// They must not be generated from the metadata under test.
	const std::vector <Case> cases (
		{
			{ "LD: contended data", 0x8000, 0x6000, { 0x7e },
				{ { 0x6000, Type::_READ, Origin::_DATA, 1, 4, 6 } },
				true, 7, false },

			{ "LD: contended instruction", 0x5000, 0x9000, { 0x7e },
				{ { 0x5000, Type::_READ, Origin::_INSTRUCTION_BYTE, 0, 0, 2 } },
				true, 7, false },

			{ "LD: instruction and data", 0x5000, 0x6000, { 0x7e },
				{ { 0x5000, Type::_READ, Origin::_INSTRUCTION_BYTE, 0, 0, 2 },
				  { 0x6000, Type::_READ, Origin::_DATA, 1, 4, 6 } },
				true, 7, false },

			{ "LD: no contended address", 0x8000, 0x9000, { 0x7e },
				{ }, true, 7, false },

			{ "LD: PC equals HL", 0x5000, 0x5000, { 0x7e },
				{ { 0x5000, Type::_READ, Origin::_INSTRUCTION_BYTE, 0, 0, 2 },
				  { 0x5000, Type::_READ, Origin::_DATA, 1, 4, 6 } },
				true, 7, false },

			{ "INC: read and write", 0x8000, 0x6000, { 0x34 },
				{ { 0x6000, Type::_READ, Origin::_DATA, 1, 4, 6 },
				  { 0x6000, Type::_WRITE, Origin::_DATA, 2, 8, 10 } },
				true, 11, false },

			{ "LD indexed: selected instruction", 0x8000, 0x6000,
				{ 0xdd, 0x7e, 0x01 },
				{ { 0x6001, Type::_READ, Origin::_DATA, 4, 16, 18 } },
				true, 19, false },

			{ "Instruction crosses 7FFF", 0x7fff, 0x6000,
				{ 0x21, 0x34, 0x12 },
				{ { 0x7fff, Type::_READ, Origin::_INSTRUCTION_BYTE, 0, 0, 2 } },
				true, 10, false },

			{ "Partial data reads are ambiguous", 0x8000, 0x7fff,
				{ 0x2a, 0xff, 0x7f },
				{ { 0x7fff, Type::_READ, Origin::_DATA, 0, 0, 0 } },
				false, 16, false },

			{ "Alternative structure is not supported yet", 0x5000, 0x6000,
				{ 0x20, 0x00 },
				{ { 0x5000, Type::_READ, Origin::_INSTRUCTION_BYTE, 0, 0, 0 },
				  { 0x5001, Type::_READ, Origin::_INSTRUCTION_BYTE, 0, 0, 0 } },
				false, 12, false },

			{ "INI: deferred memory write", 0x8000, 0x6000,
				{ 0xed, 0xa2 },
				{ { 0x6000, Type::_WRITE, Origin::_DATA, 3, 13, 15 } },
				true, 16, true } });

	for (const auto& test : cases)
	{
		runCase (test, false, ZXSPECTRUM::Memory::Configuration::_48K);
		runCase (test, true, ZXSPECTRUM::Memory::Configuration::_48K);
	}

	const Case test16K =
		{ "16K RAM subset", 0x5000, 0x6000, { 0x7e },
			{ { 0x5000, Type::_READ, Origin::_INSTRUCTION_BYTE, 0, 0, 2 },
			  { 0x6000, Type::_READ, Origin::_DATA, 1, 4, 6 } },
			true, 7, false };

	runCase (test16K, false, ZXSPECTRUM::Memory::Configuration::_16K);
	runCase (test16K, true, ZXSPECTRUM::Memory::Configuration::_16K);

	std::cout << "\nChecks=" << _checks
		<< ", failures=" << _failures << '\n';

	return (_failures == 0 ? 0 : 1);
}

// ---
void MemoryAccessTests::check (bool ok, const std::string& message)
{
	_checks++;

	if (!ok)
	{
		_failures++;
		std::cerr << "FAIL: " << message << '\n';
	}
}

// ---
bool MemoryAccessTests::executeOperation
	(ZXSPECTRUM::CZ80& cpu, MCHEmul::TicksCounterDelayed& ticks,
	 bool perCycle, unsigned short expectedPC)
{
	// Bound execution so a regression cannot hang the test.
	for (unsigned int step = 0; step < 64; step++)
	{
		if (perCycle)
			ticks.count (1);

		if (!cpu.executeNextCycle ())
		{
			check (false, "CPU execution failed");

			return (false);
		}

		check (!cpu.recordingMemoryAccesses (),
			"capture disabled when execution returns");
		check (cpu.memoryAccessContext () ==
			MCHEmul::CPU::MemoryAccessContext::_NORMAL,
			"memory access context restored");
		check (cpu.memoryAccessPC () == expectedPC,
			"operation retains its initial PC");
		check (cpu.memoryAccessStartClockCycle () == 0,
			"operation retains its initial clock");

		if (cpu.memoryAccessState () ==
				ZXSPECTRUM::CZ80::MemoryAccessState::_COMPLETED ||
			cpu.memoryAccessState () ==
				ZXSPECTRUM::CZ80::MemoryAccessState::_PENDING_PORT_COMPLETION)
			return (true);

		if (cpu.memoryAccessState () ==
			ZXSPECTRUM::CZ80::MemoryAccessState::_FAILED)
		{
			check (false, "access record marked as failed");

			return (false);
		}
	}

	check (false, "operation exceeded the execution limit");

	return (false);
}

// ---
void MemoryAccessTests::checkAccess
	(const Access& actual, const Access& expected,
	 bool timingAvailable, size_t index)
{
	bool same =
		actual._address == expected._address &&
		actual._type == expected._type &&
		actual._origin == expected._origin;

	if (timingAvailable)
		same = same &&
			actual._mCycle == expected._mCycle &&
			actual._contentionCycle == expected._contentionCycle &&
			actual._dataCycle == expected._dataCycle;

	check (same, "access " + std::to_string (index));

	if (!same)
	{
		std::cerr
			<< "  Expected address=" << expected._address
			<< ", type=" << (int) (expected._type)
			<< ", origin=" << (int) (expected._origin)
			<< "\n  Actual   address=" << actual._address
			<< ", type=" << (int) (actual._type)
			<< ", origin=" << (int) (actual._origin) << '\n';

		if (timingAvailable)
			std::cerr
				<< "  Expected M=" << expected._mCycle
				<< ", contention=" << expected._contentionCycle
				<< ", data=" << expected._dataCycle
				<< "\n  Actual   M=" << actual._mCycle
				<< ", contention=" << actual._contentionCycle
				<< ", data=" << actual._dataCycle << '\n';
	}
}

// ---
void MemoryAccessTests::runCase
	(const Case& test, bool perCycle,
	 ZXSPECTRUM::Memory::Configuration configuration)
{
	std::cout << test._name
		<< (perCycle ? " [PerCycle]" : " [Full]")
		<< (configuration == ZXSPECTRUM::Memory::Configuration::_16K
			? " [16K]" : " [48K]") << '\n';

	unsigned int failuresBefore = _failures;

	// Memory and ticks outlive the CPU and its non-owning references.
	ZXSPECTRUM::Memory memory
		(configuration, ZXSPECTRUM::Type::_STANDARD, "ENG");
	MCHEmul::TicksCounterDelayed ticks (1);
	ZXSPECTRUM::CZ80 cpu (0);

	cpu.setMemoryRef (&memory);
	memory.linkToCPU (&cpu);

	// The production CPU owns and deletes registered ports.
	FixedInputPort* port = new FixedInputPort;
	cpu.addPorts ({ { 0x10, port } });

	// Initialization writes must not survive in the global command buffer.
	MCHEmul::Memory::configuration ().setBufferMemorySetCommands (false);

	if (!memory.initialize () || !cpu.initialize ())
	{
		check (false,
			"initialization; run from emulators/ZXSpectrumCommons");

		return;
	}

	if (perCycle)
		cpu.assignTicksCounter (ticks);

	cpu.setIFF1 (false);
	cpu.setIFF2 (false);
	cpu.fRegister ().set ({ 0x00 });
	cpu.hRegister ().set ({ (unsigned char) (test._hl >> 8) });
	cpu.lRegister ().set ({ (unsigned char) (test._hl & 0xff) });
	cpu.ixhRegister ().set ({ 0x60 });
	cpu.ixlRegister ().set ({ 0x00 });
	cpu.bRegister ().set ({ 0x01 });
	cpu.cRegister ().set ({ 0x10 });
	cpu.programCounter ().setAddress (MCHEmul::Address (2, test._pc));

	// put prepares the fixture without generating execution notifications.
	// Load code last so PC == HL deliberately reads its own opcode as data.
	memory.put (MCHEmul::Address (2, test._hl), MCHEmul::UByte (0x20));
	memory.put (MCHEmul::Address (2, 0x6001), MCHEmul::UByte (0x20));
	memory.put (MCHEmul::Address (2, test._pc), test._code);

	check (cpu.memoryAccesses ().empty (),
		"fixture preparation does not generate records");

	if (executeOperation (cpu, ticks, perCycle, test._pc))
	{
		check (cpu.clockCycles () == test._cycles,
			"expected instruction duration");

		if (test._pendingInput)
		{
			check (cpu.memoryAccessState () ==
				ZXSPECTRUM::CZ80::MemoryAccessState::_PENDING_PORT_COMPLETION,
				"input completion is pending");
			check (!cpu.memoryAccessTimingAvailable (),
				"timing unavailable before input completion");
			check (port -> reads () == 0,
				"input has not been read during preparation");
			check (memory.value (MCHEmul::Address (2, test._hl)).value () == 0x20,
				"input destination unchanged before completion");
		}

		cpu.executeBufferedCommands (true);

		check (!cpu.recordingMemoryAccesses (),
			"capture disabled after buffered completion");
		check (cpu.memoryAccessState () ==
			ZXSPECTRUM::CZ80::MemoryAccessState::_COMPLETED,
			"operation completed");
		check (cpu.memoryAccessTimingAvailable () == test._timingAvailable,
			"expected timing availability");

		const Accesses& actual = cpu.memoryAccesses ();

		check (actual.size () == test._expected.size (),
			"expected number of accesses");

		for (size_t i = 0;
			i < actual.size () && i < test._expected.size (); i++)
			checkAccess
				(actual [i], test._expected [i], test._timingAvailable, i);

		if (test._code [0].value () == 0x7e)
			check (cpu.aRegister ().values ()[0].value () ==
				(test._pc == test._hl ? 0x7e : 0x20),
				"LD returned the expected data");

		if (test._code [0].value () == 0x34)
			check (memory.value (MCHEmul::Address (2, test._hl)).value () == 0x21,
				"INC applied its buffered write");

		if (test._pendingInput)
		{
			check (port -> reads () == 1, "input read exactly once");
			check (memory.value (MCHEmul::Address (2, test._hl)).value () == 0xa5,
				"input written to the original HL");
		}

		size_t recordsBefore = actual.size ();
		cpu.executeBufferedCommands (true);

		check (cpu.memoryAccesses ().size () == recordsBefore,
			"another flush does not duplicate records");
		if (test._pendingInput)
			check (port -> reads () == 1,
				"another flush does not repeat the input");

		cpu.nextInstruction ();

		check (cpu.memoryAccessContext () ==
			MCHEmul::CPU::MemoryAccessContext::_NORMAL,
			"instruction inspection restores its context");
		check (cpu.memoryAccesses ().size () == recordsBefore,
			"instruction inspection leaves the completed record unchanged");
	}

	// Flush before the short-lived memory is destroyed.
	cpu.executeBufferedCommands (true);
	MCHEmul::Memory::configuration ().setBufferMemorySetCommands (false);

	if (_failures == failuresBefore)
		std::cout << "PASS\n";
}

/** Owns the production computer while SDL dummy drivers replace host outputs. \n
	Only computer cycles are executed; the host I/O presentation loop is unused. */
class ContentionMachine final
{
	public:
	ContentionMachine (ZXSPECTRUM::Memory::Configuration cfg, bool pal);
	~ContentionMachine ();
	/** Establishes the ULA epoch before selecting the measured execution mode. */
	bool initialize (unsigned int frameTState, bool perCycle);
	/** Executes the real CPU, machine hook and chip sequence without host throttling. */
	bool step ();
	inline ZXSPECTRUM::CZ80& cpu ();
	MCHEmul::Memory& memory () { return (*_computer.memory ()); }
	ZXSPECTRUM::ULA& ula () { return (*_computer.ula ()); }
	unsigned int frameClocks () const { return (_frameClocks); }
	unsigned int rasterClocks () const;

	private:
	// References retained by the computer and CPU outlive their owners' execution.
	MCHEmul::ASCIIToCodeConverter _converter;
	MCHEmul::TicksCounterDelayed _ticks;
	ZXSPECTRUM::SinclairZXSpectrum _computer;
	bool _perCycle;
	unsigned int _frameClocks;
};

// ---
inline ZXSPECTRUM::CZ80& ContentionMachine::cpu ()
{
	return (*static_cast <ZXSPECTRUM::CZ80*> (_computer.cpu ()));
}

/** Tests terminal memory contention independently of instruction timing capture. \n
	Expected delays are literal vectors, never obtained from the ULA query under test. */
class MemoryContentionTests final
{
	public:
	static int run ();

	private:
	struct Case
	{
		/** Diagnostic name identifying the first failing scenario. */
		const char* _name;
		/** CPU-visible code and data addresses; ROM fixture writes are forced. */
		unsigned short _pc, _hl;
		std::vector <MCHEmul::UByte> _code;
		/** Instruction start relative to the first active contention T-state. */
		int _startOffset;
		/** Nominal instruction duration and independently expected memory delay. */
		unsigned int _cycles, _delay;
		/** Expected retained byte transfers after execution. */
		size_t _accesses;
		/** Whether the new path is expected to accept this operation. */
		bool _handled, _timingAvailable;
		/** Checks buffered memory effects when the instruction writes a byte. */
		bool _writes;
		/** Requests an eligible IM1 response while terminal wait is pending. */
		bool _interrupt;
	};

	static void check (bool ok, const std::string& message);
	static bool completeOperation (ContentionMachine& machine);
	static bool sameAccesses
		(const ZXSPECTRUM::CZ80::MemoryAccesses& a,
		 const ZXSPECTRUM::CZ80::MemoryAccesses& b);
	static void runCase
		(const Case& test, bool perCycle, bool pal,
		 ZXSPECTRUM::Memory::Configuration configuration);

	static unsigned int _checks, _failures;
	static std::string _caseName;
};

unsigned int MemoryContentionTests::_checks = 0, MemoryContentionTests::_failures = 0;
std::string MemoryContentionTests::_caseName;

// ---
ContentionMachine::ContentionMachine
	(ZXSPECTRUM::Memory::Configuration cfg, bool pal)
	: _converter (),
	  _ticks (1),
	  _computer (&_converter, cfg,
		pal ? ZXSPECTRUM::SinclairZXSpectrum::VisualSystem::_PAL
			: ZXSPECTRUM::SinclairZXSpectrum::VisualSystem::_NTSC,
		ZXSPECTRUM::Type::_STANDARD, "ENG"),
	  _perCycle (false),
	  _frameClocks (pal ? 69888 : 59136)
{
	// Nothing else.
}

// ---
ContentionMachine::~ContentionMachine ()
{
	// Failure cleanup must release global commands before their memory is destroyed.
	// This direct cleanup is outside every timing and visibility assertion.
	MCHEmul::Memory::configuration ().executeMemorySetCommandsBuffered ();
	MCHEmul::Memory::configuration ().setBufferMemorySetCommands (false);
}

// ---
bool ContentionMachine::initialize (unsigned int frameTState, bool perCycle)
{
	MCHEmul::Memory::configuration ().setBufferMemorySetCommands (false);
	if (!_computer.initialize ())
		return (false);

	cpu ().setIFF1 (false);
	cpu ().setIFF2 (false);
	// A forced ROM patch gives both 16K and 48K an uncontended bootstrap NOP.
	memory ().put (MCHEmul::Address (2, 0x2000), MCHEmul::UByte (0x00), true);
	_computer.setStartingAddress (MCHEmul::Address (2, 0x2000));

	for (unsigned int i = 0; i < 32 && cpu ().clockCycles () == 0; i++)
		if (!step ())
			return (false);

	// The ULA's first nonzero CPU timestamp establishes its epoch, without raster advance.
	if (cpu ().clockCycles () != 4 || rasterClocks () != 0 ||
		ula ().legacyMemoryContentionPending ())
		return (false);

	// Fixture-only idle advance positions the actual ULA without decoding filler code.
	// Subsequent measured steps always use runComputerCycle, including its legacy hook.
	cpu ().addClockCycles (frameTState);
	for (const auto& chip : _computer.chips ())
		if (!chip.second -> simulate (&cpu ()))
			return (false);

	if (rasterClocks () != frameTState)
		return (false);

	_perCycle = perCycle;
	if (_perCycle)
		cpu ().assignTicksCounter (_ticks);

	return (true);
}

// ---
bool ContentionMachine::step ()
{
	// Reset host pacing only; CPU time and the external single-T-state counter persist.
	_computer.clock ().start ();
	if (_perCycle)
		_ticks.count (1);

	return (_computer.runComputerCycle ());
}

// ---
unsigned int ContentionMachine::rasterClocks () const
{
	return ((unsigned int) _computer.ula () -> raster ().currentLineAtBase0 () * 224 +
		(unsigned int) _computer.ula () -> raster ().currentColumnAtBase0 () / 2);
}

// ---
int MemoryContentionTests::run ()
{
	// Dummy video cannot create the production screen's OpenGL surface.
	// Its unused presentation handles can remain null: no runIOCycle is called.
	// Audio uses SDL's dummy device and never reaches a physical speaker.
	if (SDL_setenv ("SDL_VIDEODRIVER", "dummy", 1) != 0 ||
		SDL_setenv ("SDL_AUDIODRIVER", "dummy", 1) != 0 ||
		SDL_Init (SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	{
		std::cerr << "FAIL: headless SDL initialization: " << SDL_GetError () << '\n';

		SDL_Quit ();

		return (1);
	}

	const std::vector <Case> cases (
		{
			{ "No contended address", 0x2000, 0x3000, { 0x7e },
				0, 7, 0, 0, true, true, false, false },
			{ "Top border", 0x2000, 0x6000, { 0x7e },
				-100, 7, 0, 1, true, true, false, false },
			{ "Instruction fetch", 0x5000, 0x3000, { 0x00 },
				0, 4, 6, 1, true, true, false, false },
			// Fetch at F waits 6; data at nominal F+4 moves to F+10 and waits 4.
			{ "Fetch shifts data access", 0x5000, 0x6000, { 0x7e },
				0, 7, 10, 2, true, true, false, false },
			{ "Fetch shifts immediate operand", 0x5000, 0x3000, { 0x3e, 0x77 },
				0, 7, 10, 2, true, true, false, false },
			// Read at F waits 6; write at nominal F+4 moves to F+10 and waits 4.
			{ "Read-modify-write accumulation", 0x2000, 0x6000, { 0x34 },
				-4, 11, 10, 2, true, true, true, false },
			{ "Buffered store", 0x2000, 0x6000, { 0x77 },
				-4, 7, 6, 1, true, true, true, false },
			{ "Interrupt waits for store", 0x2000, 0x6000, { 0x77 },
				-4, 7, 6, 1, true, true, true, true },
			{ "Just before contention", 0x5000, 0x3000, { 0x00 },
				-1, 4, 0, 1, true, true, false, false },
			{ "End of active window", 0x5000, 0x3000, { 0x00 },
				124, 4, 2, 1, true, true, false, false },
			{ "Last active T-state", 0x5000, 0x3000, { 0x00 },
				127, 4, 0, 1, true, true, false, false },
			{ "Right border", 0x5000, 0x3000, { 0x00 },
				128, 4, 0, 1, true, true, false, false },
			{ "Next contention line", 0x5000, 0x3000, { 0x00 },
				224, 4, 6, 1, true, true, false, false },
			{ "Last contention line", 0x5000, 0x3000, { 0x00 },
				42784, 4, 6, 1, true, true, false, false },
			{ "After last contention line", 0x5000, 0x3000, { 0x00 },
				43008, 4, 0, 1, true, true, false, false },
			// Preserve the exclusions until the later functional blocks are implemented.
			{ "Alternative stays excluded", 0x5000, 0x3000, { 0x20, 0x00 },
				0, 12, 0, 2, false, false, false, false },
			{ "I/O stays excluded", 0x5000, 0x3000, { 0xd3, 0xfe },
				0, 11, 0, 2, false, true, false, false } });

	// Literal oracle for all eight phases of a contended data read.
	const unsigned int phaseDelays [8] = { 6, 5, 4, 3, 2, 1, 0, 0 };
	for (bool pal : { true, false })
		for (auto configuration : { ZXSPECTRUM::Memory::Configuration::_48K,
			ZXSPECTRUM::Memory::Configuration::_16K })
			for (bool perCycle : { false, true })
			{
				for (const auto& test : cases)
					runCase (test, perCycle, pal, configuration);

				for (unsigned int phase = 0; phase < 8; phase++)
				{
					const Case test =
						{ "Data-read phase", 0x2000, 0x6000, { 0x7e },
							(int) phase - 4, 7, phaseDelays [phase], 1,
							true, true, false, false };

					runCase (test, perCycle, pal, configuration);
				}
			}

	SDL_Quit ();

	std::cout << "\nContention checks=" << _checks
		<< ", failures=" << _failures << '\n';

	return (_failures == 0 ? 0 : 1);
}

// ---
void MemoryContentionTests::check (bool ok, const std::string& message)
{
	_checks++;
	if (!ok)
	{
		_failures++;
		std::cerr << "FAIL: " << _caseName << ": " << message << '\n';
	}
}

// ---
bool MemoryContentionTests::completeOperation (ContentionMachine& machine)
{
	for (unsigned int step = 0; step < 64; step++)
	{
		if (!machine.step ())
		{
			check (false, "computer cycle failed");

			return (false);
		}

		check (!machine.cpu ().recordingMemoryAccesses (), "capture suspended after step");
		check (machine.cpu ().memoryAccessContext () ==
			MCHEmul::CPU::MemoryAccessContext::_NORMAL, "memory context restored after step");
		if (machine.cpu ().memoryAccessState () ==
				ZXSPECTRUM::CZ80::MemoryAccessState::_COMPLETED ||
			machine.cpu ().memoryAccessState () ==
				ZXSPECTRUM::CZ80::MemoryAccessState::_PENDING_PORT_COMPLETION)
			return (true);
	}

	check (false, "operation exceeded 64 computer steps");

	return (false);
}

// ---
bool MemoryContentionTests::sameAccesses
	(const ZXSPECTRUM::CZ80::MemoryAccesses& a,
	 const ZXSPECTRUM::CZ80::MemoryAccesses& b)
{
	if (a.size () != b.size ())
		return (false);

	for (size_t i = 0; i < a.size (); i++)
		if (a [i]._address != b [i]._address || a [i]._type != b [i]._type ||
			a [i]._origin != b [i]._origin || a [i]._mCycle != b [i]._mCycle ||
			a [i]._contentionCycle != b [i]._contentionCycle ||
			a [i]._dataCycle != b [i]._dataCycle)
			return (false);

	return (true);
}

// ---
void MemoryContentionTests::runCase
	(const Case& test, bool perCycle, bool pal,
	 ZXSPECTRUM::Memory::Configuration configuration)
{
	_caseName = std::string (test._name) + " [offset=" + std::to_string (test._startOffset) +
		(perCycle ? "][PerCycle]" : "][Full]") + (pal ? "[PAL]" : "[NTSC]") +
		(configuration == ZXSPECTRUM::Memory::Configuration::_16K ? "[16K]" : "[48K]");
	std::cout << _caseName << '\n';
	unsigned int failuresBefore = _failures;

	ContentionMachine machine (configuration, pal);
	// Literal origins use the implementation's base-zero raster convention, not INT acknowledge.
	unsigned int startRaster = (unsigned int) ((pal ? 14335 : 10751) + test._startOffset);
	if (!machine.initialize (startRaster, perCycle))
	{
		check (false, "machine initialization or raster synchronization; check ROM working directory");

		return;
	}

	ZXSPECTRUM::CZ80& cpu = machine.cpu ();
	MCHEmul::Memory& memory = machine.memory ();
	unsigned int startClock = cpu.clockCycles ();
	cpu.fRegister ().set ({ 0x00 });
	cpu.bRegister ().set ({ 0x11 });
	cpu.aRegister ().set ({ 0xf0 });
	cpu.hRegister ().set ({ (unsigned char) (test._hl >> 8) });
	cpu.lRegister ().set ({ (unsigned char) (test._hl & 0xff) });
	memory.put (MCHEmul::Address (2, test._hl), MCHEmul::UByte (0x20), true);
	memory.put (MCHEmul::Address (2, test._pc), test._code, true);
	// INC B provides an observable next-instruction effect without another data transfer.
	memory.put (MCHEmul::Address (2, test._pc + test._code.size ()), MCHEmul::UByte (0x04), true);
	cpu.programCounter ().setAddress (MCHEmul::Address (2, test._pc));

	if (!completeOperation (machine))
		return;

	// Deferred I/O publishes its association only after completing the port effect.
	if (cpu.memoryAccessState () ==
		ZXSPECTRUM::CZ80::MemoryAccessState::_PENDING_PORT_COMPLETION)
		cpu.executeBufferedCommands (true);

	check (cpu.memoryAccessStartClockCycle () == startClock, "transaction start clock");
	check (cpu.memoryAccessPC () == test._pc, "transaction start PC");
	check (cpu.memoryAccessTimingAvailable () == test._timingAvailable, "timing availability");
	check (cpu.memoryAccesses ().size () == test._accesses, "retained transfer count");
	check (cpu.memoryContentionHandled () == test._handled, "new-path acceptance");

	if (!test._handled)
	{
		check (cpu.pendingMemoryContentionClockCycles () == 0, "excluded operation has no new wait");

		if (_failures == failuresBefore)
			std::cout << "PASS\n";

		return;
	}

	check (cpu.memoryAccessState () == ZXSPECTRUM::CZ80::MemoryAccessState::_COMPLETED,
		"execution completed before terminal wait");
	check (cpu.clockCycles () - startClock == test._cycles, "nominal duration before wait");
	check (cpu.memoryContentionClockCycles () == test._delay, "literal expected total wait");
	check (cpu.pendingMemoryContentionClockCycles () == test._delay, "initial pending wait");
	check (!machine.ula ().legacyMemoryContentionPending (), "no legacy reconstruction");
	check (!cpu.stopped (), "no additional legacy CPU stop");

	if (test._code [0].value () == 0x7e)
		check (cpu.aRegister ().values ()[0].value () == 0x20, "LD reads fixture data");
	else if (test._code [0].value () == 0x3e)
		check (cpu.aRegister ().values ()[0].value () == 0x77, "LD reads immediate operand");

	const ZXSPECTRUM::CZ80::MemoryAccesses accesses = cpu.memoryAccesses ();
	unsigned int completedPC = cpu.programCounter ().asAddress ().value ();
	unsigned char expectedValue = test._code [0].value () == 0x34 ? 0x21 : 0xf0;
	if (test._writes)
		check (memory.value (MCHEmul::Address (2, test._hl)).value () == 0x20,
			"write remains buffered before wait");

	if (test._interrupt)
	{
		cpu.setINTMode (1);
		cpu.setIFF1 (true);
		cpu.setIFF2 (true);
		memory.stack () -> setPosition (0x7ff0, false);
		cpu.requestInterrupt (FZ80::INTInterrupt::_ID, cpu.clockCycles ());
	}

	// Bound debt consumption independently of the reported pending counter.
	for (unsigned int step = 0;
		step < 64 && cpu.pendingMemoryContentionClockCycles () != 0; step++)
	{
		unsigned int pending = cpu.pendingMemoryContentionClockCycles ();
		unsigned int consumed = perCycle ? 1 : pending;
		unsigned int beforeClock = cpu.clockCycles ();
		unsigned int beforeRaster = machine.rasterClocks ();

		cpu.executeBufferedCommands (true);
		check (cpu.pendingMemoryContentionClockCycles () == pending, "forced flush preserves debt");
		if (test._writes)
			check (memory.value (MCHEmul::Address (2, test._hl)).value () == 0x20,
				"forced flush cannot expose pending write");

		if (!machine.step ())
		{
			check (false, "computer failed during wait");

			return;
		}

		check (cpu.clockCycles () - beforeClock == consumed, "CPU wait clock increment");
		check (machine.rasterClocks () == (beforeRaster + consumed) % machine.frameClocks (),
			"ULA advances with consumed wait");
		check (cpu.pendingMemoryContentionClockCycles () == pending - consumed, "pending wait decreases");
		check (cpu.memoryContentionClockCycles () == test._delay, "total wait remains available");
		check (cpu.programCounter ().asAddress ().value () == completedPC, "PC unchanged during wait");
		check (cpu.bRegister ().values ()[0].value () == 0x11, "next instruction not executed during wait");
		check (sameAccesses (cpu.memoryAccesses (), accesses), "access record retained during wait");
		check (cpu.memoryAccessPC () == test._pc &&
			cpu.memoryAccessStartClockCycle () == startClock &&
			cpu.memoryAccessTimingAvailable (), "transaction identity and timing retained");
		check (!cpu.recordingMemoryAccesses (), "capture remains suspended during wait");
		check (cpu.memoryAccessOperation () == ZXSPECTRUM::CZ80::MemoryAccessOperation::_INSTRUCTION,
			"no interrupt accepted during wait");
		check (!cpu.stopped () && !machine.ula ().legacyMemoryContentionPending (),
			"wait not duplicated by legacy path");
		if (test._interrupt)
			check (cpu.IFF1 (), "interrupt remains unacknowledged");
		if (test._writes)
			check (memory.value (MCHEmul::Address (2, test._hl)).value () == 0x20,
				"write stays buffered through the last wait step");
	}

	check (cpu.pendingMemoryContentionClockCycles () == 0, "wait exhausted within bound");
	if (cpu.pendingMemoryContentionClockCycles () != 0)
		return;

	check (cpu.clockCycles () - startClock == test._cycles + test._delay, "nominal plus wait duration");
	check (machine.rasterClocks () ==
		(startRaster + test._cycles + test._delay) % machine.frameClocks (), "final ULA position");

	cpu.executeBufferedCommands (true);
	if (test._writes)
		check (memory.value (MCHEmul::Address (2, test._hl)).value () == expectedValue,
			"write exposed after wait");
	cpu.executeBufferedCommands (true);
	check (sameAccesses (cpu.memoryAccesses (), accesses), "flush does not duplicate recorded transfers");

	if (!completeOperation (machine))
		return;

	if (test._interrupt)
	{
		check (cpu.memoryAccessOperation () == ZXSPECTRUM::CZ80::MemoryAccessOperation::_INTERRUPT,
			"pending interrupt accepted after wait");
		check (cpu.programCounter ().asAddress ().value () == 0x0038, "IM1 reached its vector");
		check (!cpu.IFF1 (), "interrupt acknowledged after wait");
		check (cpu.bRegister ().values ()[0].value () == 0x11, "interrupt precedes next instruction");
	}
	else
	{
		check (cpu.memoryAccessPC () == completedPC, "next transaction starts at following instruction");
		check (cpu.bRegister ().values ()[0].value () == 0x12, "next instruction executes after wait");
	}

	if (_failures == failuresBefore)
		std::cout << "PASS\n";
}

int main ()
{
	std::cout << std::unitbuf;

#if defined (_MSC_VER) && defined (_DEBUG)
	// Report assertions to stderr instead of opening an interactive dialog.
	_CrtSetReportMode (_CRT_ASSERT, _CRTDBG_MODE_FILE);
	_CrtSetReportFile (_CRT_ASSERT, _CRTDBG_FILE_STDERR);
#endif

	int accessResult = MemoryAccessTests::run ();
	int contentionResult = MemoryContentionTests::run ();

	return (accessResult == 0 && contentionResult == 0 ? 0 : 1);
}

// End of the file
/*@}*/
