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

#include <algorithm>
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
	FixedInputPort (unsigned int delay = 0)
		: FZ80::Z80Port (0x10, "Memory access test"),
		  _delay (delay), _reads (0), _queries (0) { }

	unsigned int reads () const
							{ return (_reads); }
	unsigned int queries () const { return (_queries); }
	virtual MCHEmul::UByte value
		(unsigned short, unsigned char) const override;
	virtual MCHEmul::UByte peekValue
		(unsigned short, unsigned char) const override
							{ return (MCHEmul::UByte (0xa5)); }
	virtual void setValue
		(unsigned short, unsigned char, const MCHEmul::UByte&) override { }
	virtual unsigned int additionalClockCyclesForIO
		(unsigned short, unsigned int) const override;
	virtual void initialize () override;

	private:
	unsigned int _delay;

	// Implementation
	mutable unsigned int _reads, _queries;
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
unsigned int FixedInputPort::additionalClockCyclesForIO
	(unsigned short, unsigned int) const
{
	_queries++;

	return (_delay);
}

// ---
void FixedInputPort::initialize ()
{
	_reads = 0;
	_queries = 0;
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

			{ "Alternative structure is selected", 0x5000, 0x6000,
				{ 0x20, 0x00 },
				{ { 0x5000, Type::_READ, Origin::_INSTRUCTION_BYTE, 0, 0, 2 },
				  { 0x5001, Type::_READ, Origin::_INSTRUCTION_BYTE, 1, 4, 6 } },
				true, 12, false },

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
		check (cpu.memoryAccessMCyclesStructure () ==
			(test._timingAvailable ? (test._code [0].value () == 0x20 ? 1 : 0)
				: FZ80::Instruction::_NOMCYCLESSTRUCTURE), "associated alternative index");

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

/** Verifies the production port manager's timestamp source with distinguishable bus values. */
class PortTimestampTests final
{
	public:
	PortTimestampTests () : _checks (0), _failures (0), _caseName () { }
	int run ();

	private:
	enum class Kind { _IMMEDIATE, _PREFIXED, _BLOCK, _REPEATED };

	void check (bool ok, const std::string& message);
	void runCase (Kind kind, bool perCycle, bool pal);

	unsigned int _checks, _failures;
	std::string _caseName;
};

/** Counts actual transfers and timing queries while using the production ULA arbiter. */
class ObservedContendedPort final : public FZ80::Z80Port
{
	public:
	ObservedContendedPort (const ZXSPECTRUM::ULA& ula)
		: FZ80::Z80Port (0x10, "Observed contended port"),
		  _ula (ula), _queries (0), _reads (0), _writes (0),
		  _queryClock (0), _transferClock (0), _address (0), _value (0) { }

	unsigned int queries () const { return (_queries); }
	unsigned int reads () const { return (_reads); }
	unsigned int writes () const { return (_writes); }
	unsigned int queryClock () const { return (_queryClock); }
	unsigned int transferClock () const { return (_transferClock); }
	unsigned short address () const { return (_address); }
	unsigned char lastValue () const { return (_value); }
	virtual MCHEmul::UByte value (unsigned short ab, unsigned char) const override;
	virtual MCHEmul::UByte peekValue (unsigned short, unsigned char) const override
		{ return (MCHEmul::UByte (0xa5)); }
	virtual void setValue (unsigned short ab, unsigned char, const MCHEmul::UByte& v) override;
	virtual unsigned int additionalClockCyclesForIO
		(unsigned short ab, unsigned int cC) const override;

	private:
	const ZXSPECTRUM::ULA& _ula;

	// Implementation
	mutable unsigned int _queries, _reads;
	unsigned int _writes;
	mutable unsigned int _queryClock, _transferClock;
	mutable unsigned short _address;
	mutable unsigned char _value;
};

/** Exercises memory-before/I-O/memory-after timing with literal expected delays. */
class MixedIOContentionTests final
{
	public:
	MixedIOContentionTests () : _checks (0), _failures (0), _caseName () { }
	int run ();

	private:
	enum class Operation { _IN, _OUT, _IN_BLOCK, _OUT_BLOCK };

	struct Case
	{
		const char* _name;
		Operation _operation;
		std::vector <MCHEmul::UByte> _code;
		unsigned short _pc, _hl, _bc;
		unsigned char _a;
		/** Start relative to the first contention T-state, not INT acknowledge. */
		int _startOffset;
		/** Literal nominal duration and the three expected delay components. */
		unsigned int _nominal, _before, _port, _after;
		/** Nominal I/O contention and data points relative to instruction start. */
		unsigned int _ioStart, _sample;
		size_t _alternative = 0;
	};

	void check (bool ok, const std::string& message);
	void runCase (const Case& test, bool perCycle, bool pal,
		ZXSPECTRUM::Memory::Configuration configuration);

	// Implementation
	unsigned int _checks, _failures;
	std::string _caseName;
};

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
		/** Initial flags and expected selected alternative for conditional cases. */
		unsigned char _flags = 0;
		size_t _alternative = 0;
		/** Prepares a real stack containing the following instruction's address. */
		bool _stack = false;
		/** Memory and port delays already included in instruction duration. */
		unsigned int _included = 0, _portDelay = 0;
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
	if (cpu ().clockCycles () != 4 || rasterClocks () != 0)
		return (false);

	// Fixture-only idle advance positions the actual ULA without decoding filler code.
	// Subsequent measured steps always use runComputerCycle, including CPU/chip coordination.
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
int PortTimestampTests::run ()
{
	for (Kind kind : { Kind::_IMMEDIATE, Kind::_PREFIXED, Kind::_BLOCK, Kind::_REPEATED })
		for (bool perCycle : { false, true })
			for (bool pal : { true, false })
				runCase (kind, perCycle, pal);

	std::cout << "\nPort timestamp checks=" << _checks << ", failures=" << _failures << '\n';

	return (_failures == 0 ? 0 : 1);
}

// ---
void PortTimestampTests::check (bool ok, const std::string& message)
{
	_checks++;
	if (!ok)
	{
		_failures++;
		std::cerr << "FAIL: " << _caseName << ": " << message << '\n';
	}
}

// ---
void PortTimestampTests::runCase (Kind kind, bool perCycle, bool pal)
{
	bool immediate = kind == Kind::_IMMEDIATE;
	bool block = kind == Kind::_BLOCK || kind == Kind::_REPEATED;
	_caseName = std::string (immediate ? "IN A,(n)" :
		(kind == Kind::_PREFIXED ? "IN A,(C)" : (kind == Kind::_BLOCK ? "INI" : "INIR"))) +
		" timestamp" + (perCycle ? " [PerCycle]" : " [Full]") + (pal ? " [PAL]" : " [NTSC]");
	std::cout << _caseName << '\n';
	unsigned int failuresBefore = _failures;
	ContentionMachine machine (ZXSPECTRUM::Memory::Configuration::_48K, pal);
	unsigned int sampleOffset = immediate ? 10 : (block ? 12 : 11);
	// The scheduled sample is at the first bitmap phase (line T=11).
	// Completion is later: attribute 0x34 or idle 0xff, deliberately distinct from bitmap 0x12.
	unsigned int startRaster = (pal ? 14336 : 10752) + 11 - sampleOffset;
	if (!machine.initialize (startRaster, perCycle))
	{
		check (false, "port fixture initialization");

		return;
	}

	ZXSPECTRUM::CZ80& cpu = machine.cpu ();
	MCHEmul::Memory& memory = machine.memory ();
	std::vector <MCHEmul::UByte> code;
	if (immediate)
		code = { 0xdb, 0xff };
	else
		code = { 0xed, (unsigned char) (block ? (kind == Kind::_BLOCK ? 0xa2 : 0xb2) : 0x78) };
	cpu.setIFF1 (false);
	cpu.setIFF2 (false);
	cpu.aRegister ().set ({ 0x02 });
	cpu.bRegister ().set ({ (unsigned char) (kind == Kind::_BLOCK ? 1 : 2) });
	cpu.cRegister ().set ({ 0xff });
	cpu.hRegister ().set ({ 0x90 }); cpu.lRegister ().set ({ 0x00 });
	memory.put (MCHEmul::Address (2, 0x4000), MCHEmul::UByte (0x12));
	memory.put (MCHEmul::Address (2, 0x5800), MCHEmul::UByte (0x34));
	memory.put (MCHEmul::Address (2, 0x9000), MCHEmul::UByte (0xab));
	memory.put (MCHEmul::Address (2, 0x2000), code, true);
	cpu.programCounter ().setAddress (MCHEmul::Address (2, 0x2000));
	unsigned int startClock = cpu.clockCycles ();
	bool completed = false;
	for (unsigned int step = 0; step < 64 && !completed; step++)
	{
		if (!machine.step ())
			break;
		completed = cpu.memoryAccessState () ==
			ZXSPECTRUM::CZ80::MemoryAccessState::_PENDING_PORT_COMPLETION;
	}
	check (completed && cpu.hasPendingPortAccess (), "deferred input is available");
	if (!completed || !cpu.hasPendingPortAccess ())
		return;

	check (cpu.currentInstruction () == nullptr, "input completion occurs outside instruction execution");
	unsigned int sampleClock = startClock + sampleOffset;
	unsigned short portAddress = cpu.pendingPortAccess ()._address;
	check (cpu.pendingPortAccess ()._clockCycle == sampleClock, "descriptor retains literal sample time");
	check (machine.ula ().floatingBusValueAt (sampleClock).value () == 0x12,
		"scheduled sample is the prepared bitmap byte");
	unsigned char directValue = machine.ula ().floatingBusValueAt (cpu.clockCycles ()).value ();
	check (directValue != 0x12, "sample time and completion time are distinguishable");

	const MCHEmul::Instruction* instruction = cpu.lastInstruction ();
	const auto* undefined = dynamic_cast <const MCHEmul::InstructionUndefined*> (instruction);
	if (undefined != nullptr)
		instruction = undefined -> lastDefinedInstruction ();
	const auto* defined = dynamic_cast <const FZ80::Instruction*> (instruction);
	check (defined != nullptr && defined == cpu.pendingPortAccess ()._instruction,
		"container resolves to the pending input's actual owner");
	if (defined != nullptr)
		check (defined -> IOAccessClockCycle () == sampleClock, "defined instruction retains local timing");

	const FZ80::Z80Port* port = cpu.ports ().find (0xff) -> second;
	check (port -> peekValue (portAddress, 0xff).value () == directValue,
		"inspection uses current time even while an input is pending");
	cpu.executeBufferedCommands (true);
	check (!cpu.hasPendingPortAccess (), "pending input released once");
	if (block)
		check (memory.value (MCHEmul::Address (2, 0x9000)).value () == 0x12,
			"block input stores the scheduled sample at the original HL");
	else
		check (cpu.aRegister ().values ()[0].value () == 0x12,
			"register input receives the scheduled sample");
	check (port -> value (portAddress, 0xff).value () == directValue,
		"direct read without pending access uses current time");
	check (port -> peekValue (portAddress, 0xff).value () == directValue,
		"inspection remains tied to current time after completion");
	if (_failures == failuresBefore)
		std::cout << "PASS\n";
}

// ---
MCHEmul::UByte ObservedContendedPort::value (unsigned short ab, unsigned char) const
{
	_reads++;
	_address = ab;
	_transferClock = cpu () -> pendingPortAccess ()._clockCycle;
	_value = 0xa5;

	return (MCHEmul::UByte (_value));
}

// ---
void ObservedContendedPort::setValue
	(unsigned short ab, unsigned char, const MCHEmul::UByte& v)
{
	_writes++;
	_address = ab;
	_transferClock = cpu () -> pendingPortAccess ()._clockCycle;
	_value = v.value ();
}

// ---
unsigned int ObservedContendedPort::additionalClockCyclesForIO
	(unsigned short ab, unsigned int cC) const
{
	_queries++;
	_queryClock = cC;

	return (_ula.IOContentionDelayAt (ab, cC));
}

// ---
int MixedIOContentionTests::run ()
{
	const std::vector <Case> cases (
		{
			// At F, the two contained instruction bytes wait 6+4 T-states.
			{ "OUT immediate even low", Operation::_OUT, { 0xd3, 0xfe }, 0x5000, 0x6000, 0, 0x02,
				0, 11, 10, 4, 0, 7, 10 },
			{ "OUT immediate even high", Operation::_OUT, { 0xd3, 0xfe }, 0x5000, 0x6000, 0, 0x40,
				0, 11, 10, 5, 0, 7, 10 },
			{ "OUT immediate odd low", Operation::_OUT, { 0xd3, 0xff }, 0x5000, 0x6000, 0, 0x02,
				0, 11, 10, 0, 0, 7, 10 },
			{ "OUT immediate odd high", Operation::_OUT, { 0xd3, 0xff }, 0x5000, 0x6000, 0, 0x40,
				0, 11, 10, 11, 0, 7, 10 },
			{ "IN immediate even low", Operation::_IN, { 0xdb, 0xfe }, 0x5000, 0x6000, 0, 0x02,
				0, 11, 10, 4, 0, 7, 10 },
			{ "IN immediate even high", Operation::_IN, { 0xdb, 0xfe }, 0x5000, 0x6000, 0, 0x40,
				0, 11, 10, 5, 0, 7, 10 },
			{ "IN immediate odd low", Operation::_IN, { 0xdb, 0xff }, 0x5000, 0x6000, 0, 0x02,
				0, 11, 10, 0, 0, 7, 10 },
			{ "IN immediate odd high", Operation::_IN, { 0xdb, 0xff }, 0x5000, 0x6000, 0, 0x40,
				0, 11, 10, 11, 0, 7, 10 },
			{ "IN register even low", Operation::_IN, { 0xed, 0x78 }, 0x5000, 0x6000, 0x02fe, 0x30,
				0, 12, 10, 3, 0, 8, 11 },
			{ "IN register even high", Operation::_IN, { 0xed, 0x78 }, 0x5000, 0x6000, 0x40fe, 0x30,
				0, 12, 10, 4, 0, 8, 11 },
			{ "IN register odd low", Operation::_IN, { 0xed, 0x78 }, 0x5000, 0x6000, 0x02ff, 0x30,
				0, 12, 10, 0, 0, 8, 11 },
			{ "IN register odd high", Operation::_IN, { 0xed, 0x78 }, 0x5000, 0x6000, 0x40ff, 0x30,
				0, 12, 10, 10, 0, 8, 11 },
			{ "OUT register even low", Operation::_OUT, { 0xed, 0x79 }, 0x5000, 0x6000, 0x02fe, 0x30,
				0, 12, 10, 3, 0, 8, 11 },
			{ "OUT register even high", Operation::_OUT, { 0xed, 0x79 }, 0x5000, 0x6000, 0x40fe, 0x30,
				0, 12, 10, 4, 0, 8, 11 },
			{ "OUT register odd low", Operation::_OUT, { 0xed, 0x79 }, 0x5000, 0x6000, 0x02ff, 0x30,
				0, 12, 10, 0, 0, 8, 11 },
			{ "OUT register odd high", Operation::_OUT, { 0xed, 0x79 }, 0x5000, 0x6000, 0x40ff, 0x30,
				0, 12, 10, 10, 0, 8, 11 },
			// INI starts at F-9: port waits 5; the following write also waits 5.
			{ "INI following memory", Operation::_IN_BLOCK, { 0xed, 0xa2 }, 0x2000, 0x6000, 0x01fe, 0x30,
				-9, 16, 0, 5, 5, 9, 12 },
			{ "IND following memory", Operation::_IN_BLOCK, { 0xed, 0xaa }, 0x2000, 0x6000, 0x01fe, 0x30,
				-9, 16, 0, 5, 5, 9, 12 },
			{ "INIR repeating", Operation::_IN_BLOCK, { 0xed, 0xb2 }, 0x2000, 0x6000, 0x02fe, 0x30,
				-9, 21, 0, 5, 5, 9, 12, 1 },
			{ "INIR terminal", Operation::_IN_BLOCK, { 0xed, 0xb2 }, 0x2000, 0x6000, 0x01fe, 0x30,
				-9, 16, 0, 5, 5, 9, 12 },
			{ "INDR repeating", Operation::_IN_BLOCK, { 0xed, 0xba }, 0x2000, 0x6000, 0x02fe, 0x30,
				-9, 21, 0, 5, 5, 9, 12, 1 },
			{ "INDR terminal", Operation::_IN_BLOCK, { 0xed, 0xba }, 0x2000, 0x6000, 0x01fe, 0x30,
				-9, 16, 0, 5, 5, 9, 12 },
			{ "INIR B wraps", Operation::_IN_BLOCK, { 0xed, 0xb2 }, 0x2000, 0x6000, 0x00fe, 0x30,
				-9, 21, 0, 5, 5, 9, 12, 1 },
			{ "INI contained code and data", Operation::_IN_BLOCK, { 0xed, 0xa2 }, 0x5000, 0x6000, 0x02fe, 0x30,
				0, 16, 10, 2, 5, 9, 12 },
			{ "INIR contained code and data", Operation::_IN_BLOCK, { 0xed, 0xb2 }, 0x5000, 0x6000, 0x02fe, 0x30,
				0, 21, 10, 2, 5, 9, 12, 1 },
			{ "OUTI preceding memory", Operation::_OUT_BLOCK, { 0xed, 0xa3 }, 0x2000, 0x6000, 0x02ff, 0x30,
				-9, 16, 6, 0, 0, 12, 15 },
			{ "OUTD preceding memory", Operation::_OUT_BLOCK, { 0xed, 0xab }, 0x2000, 0x6000, 0x02ff, 0x30,
				-9, 16, 6, 0, 0, 12, 15 },
			{ "OUTI decremented high byte", Operation::_OUT_BLOCK, { 0xed, 0xa3 }, 0x2000, 0x6000, 0x41fe, 0x30,
				-9, 16, 6, 5, 0, 12, 15 },
			{ "OTIR repeating", Operation::_OUT_BLOCK, { 0xed, 0xb3 }, 0x2000, 0x6000, 0x02ff, 0x30,
				-9, 21, 6, 0, 0, 12, 15, 1 },
			{ "OTIR terminal", Operation::_OUT_BLOCK, { 0xed, 0xb3 }, 0x2000, 0x6000, 0x01ff, 0x30,
				-9, 16, 6, 0, 0, 12, 15 },
			{ "OTDR repeating", Operation::_OUT_BLOCK, { 0xed, 0xbb }, 0x2000, 0x6000, 0x02ff, 0x30,
				-9, 21, 6, 0, 0, 12, 15, 1 },
			{ "OTDR terminal", Operation::_OUT_BLOCK, { 0xed, 0xbb }, 0x2000, 0x6000, 0x01ff, 0x30,
				-9, 16, 6, 0, 0, 12, 15 },
			{ "OUTI contained code and data", Operation::_OUT_BLOCK, { 0xed, 0xa3 }, 0x5000, 0x6000, 0x02fe, 0x30,
				0, 16, 13, 4, 0, 12, 15 },
			{ "INI uncontained destination", Operation::_IN_BLOCK, { 0xed, 0xa2 }, 0x5000, 0x9000, 0x02fe, 0x30,
				0, 16, 10, 2, 0, 9, 12 },
			{ "OUTI uncontained source", Operation::_OUT_BLOCK, { 0xed, 0xa3 }, 0x2000, 0x9000, 0x02ff, 0x30,
				-9, 16, 0, 0, 0, 12, 15 },
			{ "Outside active contention", Operation::_OUT, { 0xd3, 0xfe }, 0x5000, 0x6000, 0, 0x40,
				-100, 11, 0, 0, 0, 7, 10 } });

	for (const auto& test : cases)
		for (bool perCycle : { false, true })
			for (bool pal : { true, false })
				for (auto configuration : { ZXSPECTRUM::Memory::Configuration::_48K,
					ZXSPECTRUM::Memory::Configuration::_16K })
					if (test._hl < 0x8000 || configuration == ZXSPECTRUM::Memory::Configuration::_48K)
						runCase (test, perCycle, pal, configuration);

	std::cout << "\nMixed I/O checks=" << _checks << ", failures=" << _failures << '\n';

	return (_failures == 0 ? 0 : 1);
}

// ---
void MixedIOContentionTests::check (bool ok, const std::string& message)
{
	_checks++;
	if (!ok)
	{
		_failures++;
		std::cerr << "FAIL: " << _caseName << ": " << message << '\n';
	}
}

// ---
void MixedIOContentionTests::runCase (const Case& test, bool perCycle, bool pal,
	ZXSPECTRUM::Memory::Configuration configuration)
{
	_caseName = std::string (test._name) + (perCycle ? " [PerCycle]" : " [Full]") +
		(pal ? "[PAL]" : "[NTSC]") +
		(configuration == ZXSPECTRUM::Memory::Configuration::_16K ? "[16K]" : "[48K]");
	std::cout << _caseName << '\n';
	unsigned int failuresBefore = _failures;
	ContentionMachine machine (configuration, pal);
	unsigned int startRaster = (unsigned int) ((pal ? 14335 : 10751) + test._startOffset);
	if (!machine.initialize (startRaster, perCycle))
	{
		check (false, "mixed fixture initialization");

		return;
	}

	ZXSPECTRUM::CZ80& cpu = machine.cpu ();
	MCHEmul::Memory& memory = machine.memory ();
	bool input = test._operation == Operation::_IN || test._operation == Operation::_IN_BLOCK;
	bool block = test._operation == Operation::_IN_BLOCK || test._operation == Operation::_OUT_BLOCK;
	unsigned short portAddress = test._code [0].value () != 0xed
		? (unsigned short) (((unsigned short) test._a << 8) | test._code [1].value ())
		: (unsigned short) (test._bc - (test._operation == Operation::_OUT_BLOCK ? 0x0100 : 0));
	unsigned char portNumber = (unsigned char) (portAddress & 0x00ff);
	ObservedContendedPort* port = new ObservedContendedPort (machine.ula ());
	cpu.addPorts ({ { portNumber, port } });
	// Route reads and timing queries through the observer using the public port-list API.
	// Output still fans out to the production PortManager and this observer.
	auto& routes = cpu.port (portNumber);
	std::rotate (routes.begin (), routes.end () - 1, routes.end ());

	cpu.aRegister ().set ({ test._a });
	cpu.bRegister ().set ({ (unsigned char) (test._bc >> 8) });
	cpu.cRegister ().set ({ (unsigned char) (test._bc & 0xff) });
	cpu.hRegister ().set ({ (unsigned char) (test._hl >> 8) });
	cpu.lRegister ().set ({ (unsigned char) (test._hl & 0xff) });
	memory.put (MCHEmul::Address (2, test._hl), MCHEmul::UByte (0x20));
	memory.put (MCHEmul::Address (2, test._pc), test._code, true);
	memory.put (MCHEmul::Address (2, test._pc + test._code.size ()), MCHEmul::UByte (0), true);
	cpu.programCounter ().setAddress (MCHEmul::Address (2, test._pc));
	unsigned int startClock = cpu.clockCycles ();
	unsigned int duration = test._nominal + test._before + test._port + test._after;
	bool pending = false;
	for (unsigned int step = 0; step < 64 && !pending; step++)
	{
		if (!machine.step ())
			break;
		pending = cpu.memoryAccessState () ==
			ZXSPECTRUM::CZ80::MemoryAccessState::_PENDING_PORT_COMPLETION;
	}
	check (pending && cpu.hasPendingPortAccess (), "one prepared port access");
	if (!pending || !cpu.hasPendingPortAccess ())
		return;

	unsigned int sample = startClock + test._sample + test._before + test._port;
	check (cpu.clockCycles () - startClock == duration, "literal nominal plus three delay components");
	check (machine.rasterClocks () == (startRaster + duration) % machine.frameClocks (),
		"ULA advances through the complete mixed duration");
	check (port -> queries () == 1 && port -> queryClock () == startClock + test._ioStart + test._before,
		"one port query at the memory-shifted contention point");
	check (cpu.pendingPortAccess ()._clockCycle == sample, "sampling includes preceding waits only");
	check (cpu.pendingPortAccess ()._address == portAddress, "captured full port address");
	check (port -> reads () == 0 && port -> writes () == 0, "preparation has no actual port effects");
	check (cpu.memoryContentionHandled (), "mixed operation has a calculated memory delay");
	check (cpu.memoryContentionClockCycles () == test._before + test._after,
		"memory total excludes the port's own delay");
	check (cpu.includedMemoryContentionClockCycles () == test._before + test._after &&
		cpu.pendingMemoryContentionClockCycles () == 0, "included memory wait is not queued again");
	check (!cpu.memoryAccessTimingAvailable (), "deferred completion has not published final association");
	check (!cpu.stopped (), "no additional CPU stop");
	if (input && block)
	{
		check (memory.value (MCHEmul::Address (2, test._hl)).value () == 0x20,
			"input destination unchanged before release");
		check (cpu.pendingPortAccess ()._readContext._memoryAddress == test._hl,
			"future write retains original HL");
	}
	if (block)
	{
		int direction = test._code [1].bit (3) ? -1 : 1;
		unsigned short hl = (unsigned short) (((unsigned short) cpu.hRegister ().values ()[0].value () << 8) |
			cpu.lRegister ().values ()[0].value ());
		check (hl == (unsigned short) (test._hl + direction), "live HL advances before deferred transfer");
		check (cpu.bRegister ().values ()[0].value () == (unsigned char) ((test._bc >> 8) - 1),
			"B decrements once, including wraparound");
	}

	size_t beforeCount = cpu.memoryAccesses ().size ();
	cpu.executeBufferedCommands (true);
	check (!cpu.hasPendingPortAccess (), "port released once");
	check (port -> queries () == 1 && port -> reads () == (input ? 1 : 0) &&
		port -> writes () == (input ? 0 : 1), "release neither recalculates nor repeats I/O");
	check (port -> transferClock () == sample && port -> address () == portAddress,
		"actual transfer sees its original descriptor");
	if (input)
	{
		if (block)
			check (memory.value (MCHEmul::Address (2, test._hl)).value () == 0xa5,
				"input written to captured destination");
		else
			check (cpu.aRegister ().values ()[0].value () == 0xa5, "register input completed");
	}
	else
		check (port -> lastValue () == (block ? 0x20 : test._a), "output preserves captured data");

	check (cpu.memoryAccessTimingAvailable () && cpu.memoryAccessMCyclesStructure () == test._alternative,
		"final accesses associated with the selected alternative");
	check (cpu.memoryContentionClockCycles () == test._before + test._after &&
		cpu.includedMemoryContentionClockCycles () == test._before + test._after &&
		cpu.pendingMemoryContentionClockCycles () == 0, "completion preserves the planned wait accounting");
	bool containedWrite = input && block && (test._hl & 0xc000) == 0x4000;
	check (cpu.memoryAccesses ().size () == beforeCount + (containedWrite ? 1 : 0),
		"planned write is recorded only when actually requested");
	cpu.executeBufferedCommands (true);
	check (port -> reads () == (input ? 1 : 0) && port -> writes () == (input ? 0 : 1),
		"second release has no port effects");
	check (cpu.clockCycles () - startClock == duration, "release consumes no additional CPU time");

	unsigned short expectedPC = test._alternative == 1
		? test._pc : (unsigned short) (test._pc + test._code.size ());
	check (cpu.programCounter ().asAddress ().value () == expectedPC, "terminal/repeating PC behavior");
	if (test._alternative == 0)
	{
		if (!machine.step ())
			check (false, "next instruction cycle");
		else
			check (cpu.memoryAccessStartClockCycle () == startClock + duration &&
				cpu.memoryAccessPC () == expectedPC, "next instruction starts without another contention wait");
	}
	if (_failures == failuresBefore)
		std::cout << "PASS\n";
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
			// Alternatives and mixed I/O both use their executed path.
			{ "Taken alternative is handled", 0x5000, 0x3000, { 0x20, 0x00 },
				0, 12, 10, 2, true, true, false, false, 0x00, 1 },
			{ "Not-taken JR alternative", 0x5000, 0x3000, { 0x20, 0x00 },
				0, 7, 10, 2, true, true, false, false, 0x40, 0 },
			// Return/call targets coincide with the marker, isolating stack bus timing.
			// CALL waits 6+4+5 for code, then 4+5 for its two stack writes.
			{ "Taken CALL with contended stack", 0x5000, 0x3000, { 0xc4, 0x03, 0x50 },
				0, 17, 24, 5, true, true, false, false, 0x00, 1, true },
			{ "Not-taken CALL", 0x5000, 0x3000, { 0xc4, 0x03, 0x50 },
				0, 10, 15, 3, true, true, false, false, 0x40, 0, true },
			// RET waits 6 for code, then 3+5 for its two stack reads.
			{ "Taken RET with contended stack", 0x5000, 0x3000, { 0xc0 },
				0, 11, 14, 3, true, true, false, false, 0x00, 1, true },
			{ "Not-taken RET", 0x5000, 0x3000, { 0xc0 },
				0, 5, 6, 1, true, true, false, false, 0x40, 0, true },
			{ "I/O includes preceding memory", 0x5000, 0x3000, { 0xd3, 0xfe },
				0, 11, 10, 2, true, true, false, false, 0x00, 0, false, 10, 4 } });

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

	MixedIOContentionTests mixedTests;
	int mixedResult = mixedTests.run ();

	PortTimestampTests portTests;
	int portResult = portTests.run ();

	SDL_Quit ();

	std::cout << "\nContention checks=" << _checks
		<< ", failures=" << _failures << '\n';

	return (_failures == 0 && portResult == 0 && mixedResult == 0 ? 0 : 1);
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
	cpu.fRegister ().set ({ test._flags });
	cpu.bRegister ().set ({ 0x11 });
	cpu.aRegister ().set ({ 0xf0 });
	cpu.hRegister ().set ({ (unsigned char) (test._hl >> 8) });
	cpu.lRegister ().set ({ (unsigned char) (test._hl & 0xff) });
	memory.put (MCHEmul::Address (2, test._hl), MCHEmul::UByte (0x20), true);
	memory.put (MCHEmul::Address (2, test._pc), test._code, true);
	// INC B provides an observable next-instruction effect without another data transfer.
	memory.put (MCHEmul::Address (2, test._pc + test._code.size ()), MCHEmul::UByte (0x04), true);
	if (test._stack)
	{
		unsigned short returnAddress = (unsigned short) (test._pc + test._code.size ());
		memory.stack () -> reset ();
		memory.stack () -> setPosition (0x6202, false);
		memory.stack () -> push (MCHEmul::UBytes
			({ (unsigned char) (returnAddress >> 8), (unsigned char) (returnAddress & 0xff) }));
		cpu.executeBufferedCommands (true);
		if (test._code [0].value () == 0xc4)
			memory.stack () -> setPosition (0x6202, false);
	}
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
	check (cpu.clockCycles () - startClock == test._cycles + test._included + test._portDelay,
		"duration before terminal wait");
	check (cpu.memoryAccessMCyclesStructure () == test._alternative, "associated execution alternative");
	check (cpu.memoryContentionClockCycles () == test._delay, "literal expected total wait");
	check (cpu.includedMemoryContentionClockCycles () == test._included, "included memory wait");
	check (cpu.pendingMemoryContentionClockCycles () == test._delay - test._included, "initial pending wait");
	check (!cpu.stopped (), "no additional CPU stop");

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
			cpu.memoryAccessTimingAvailable () &&
			cpu.memoryAccessMCyclesStructure () == test._alternative, "transaction identity and timing retained");
		check (!cpu.recordingMemoryAccesses (), "capture remains suspended during wait");
		check (cpu.memoryAccessOperation () == ZXSPECTRUM::CZ80::MemoryAccessOperation::_INSTRUCTION,
			"no interrupt accepted during wait");
		check (!cpu.stopped (),
			"wait does not introduce a separate CPU stop");
		if (test._interrupt)
			check (cpu.IFF1 (), "interrupt remains unacknowledged");
		if (test._writes)
			check (memory.value (MCHEmul::Address (2, test._hl)).value () == 0x20,
				"write stays buffered through the last wait step");
	}

	check (cpu.pendingMemoryContentionClockCycles () == 0, "wait exhausted within bound");
	if (cpu.pendingMemoryContentionClockCycles () != 0)
		return;

	check (cpu.clockCycles () - startClock == test._cycles + test._delay + test._portDelay, "nominal plus wait duration");
	check (machine.rasterClocks () ==
		(startRaster + test._cycles + test._delay + test._portDelay) % machine.frameClocks (), "final ULA position");

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

/** Exercises selection invalidation without depending on a hardware execution error. \n
	Equal-duration alternatives also prevent duration-based inference from passing this test. */
class AlternativeExecutionProbe final : public FZ80::Instruction
{
	public:
	AlternativeExecutionProbe ()
		: FZ80::Instruction (0, 1, 4, "NOP",
			FZ80::MCyclesStructures
				{ FZ80::MCyclesStructure { _Z80_M1 () },
				  FZ80::MCyclesStructure { _Z80_M1 () } }),
		  _success (true) { }

	void setSuccess (bool success) { _success = success; }

	protected:
	virtual bool executeImpl (bool&) override;

	private:
	bool _success;
};

// ---
bool AlternativeExecutionProbe::executeImpl (bool&)
{
	selectMCyclesStructure (1);

	return (_success);
}

/** Reuses each alternative-bearing instruction across both runtime paths. \n
	All expected transfer points are literal, independent of selected metadata. */
class AlternativeSelectionTests final
{
	public:
	static int run ();

	private:
	enum class Family { _JR, _DJNZ, _CALL, _RET, _MOVE, _COMPARE, _INPUT, _OUTPUT };

	struct Case
	{
		const char* _name;
		Family _family;
		std::vector <MCHEmul::UByte> _code;
		/** Flags selecting alternative one and zero, respectively. */
		unsigned char _takenFlags, _notTakenFlags;
		/** Literal nominal durations for alternatives zero and one. */
		unsigned int _cycles0, _cycles1;
	};

	static void check (bool ok, const std::string& message);
	static void runCase (const Case& test, bool perCycle);
	static ZXSPECTRUM::CZ80::MemoryAccesses expectedAccesses
		(const Case& test, bool alternativeOne);
	static void checkLifecycle ();

	static unsigned int _checks, _failures;
	static std::string _caseName;
};

unsigned int AlternativeSelectionTests::_checks = 0, AlternativeSelectionTests::_failures = 0;
std::string AlternativeSelectionTests::_caseName;

// ---
int AlternativeSelectionTests::run ()
{
	const std::vector <Case> cases (
		{
			{ "JR NZ", Family::_JR, { 0x20, 0x02 }, 0x00, 0x40, 7, 12 },
			{ "JR Z", Family::_JR, { 0x28, 0x02 }, 0x40, 0x00, 7, 12 },
			{ "JR NC", Family::_JR, { 0x30, 0x02 }, 0x00, 0x01, 7, 12 },
			{ "JR C", Family::_JR, { 0x38, 0x02 }, 0x01, 0x00, 7, 12 },
			{ "DJNZ", Family::_DJNZ, { 0x10, 0x02 }, 0x00, 0x00, 8, 13 },
			{ "CALL NZ", Family::_CALL, { 0xc4, 0x00, 0x20 }, 0x00, 0x40, 10, 17 },
			{ "CALL Z", Family::_CALL, { 0xcc, 0x00, 0x20 }, 0x40, 0x00, 10, 17 },
			{ "CALL NC", Family::_CALL, { 0xd4, 0x00, 0x20 }, 0x00, 0x01, 10, 17 },
			{ "CALL C", Family::_CALL, { 0xdc, 0x00, 0x20 }, 0x01, 0x00, 10, 17 },
			{ "CALL PO", Family::_CALL, { 0xe4, 0x00, 0x20 }, 0x00, 0x04, 10, 17 },
			{ "CALL PE", Family::_CALL, { 0xec, 0x00, 0x20 }, 0x04, 0x00, 10, 17 },
			{ "CALL P", Family::_CALL, { 0xf4, 0x00, 0x20 }, 0x00, 0x80, 10, 17 },
			{ "CALL M", Family::_CALL, { 0xfc, 0x00, 0x20 }, 0x80, 0x00, 10, 17 },
			{ "RET NZ", Family::_RET, { 0xc0 }, 0x00, 0x40, 5, 11 },
			{ "RET Z", Family::_RET, { 0xc8 }, 0x40, 0x00, 5, 11 },
			{ "RET NC", Family::_RET, { 0xd0 }, 0x00, 0x01, 5, 11 },
			{ "RET C", Family::_RET, { 0xd8 }, 0x01, 0x00, 5, 11 },
			{ "RET PO", Family::_RET, { 0xe0 }, 0x00, 0x04, 5, 11 },
			{ "RET PE", Family::_RET, { 0xe8 }, 0x04, 0x00, 5, 11 },
			{ "RET P", Family::_RET, { 0xf0 }, 0x00, 0x80, 5, 11 },
			{ "RET M", Family::_RET, { 0xf8 }, 0x80, 0x00, 5, 11 },
			{ "LDIR", Family::_MOVE, { 0xed, 0xb0 }, 0, 0, 16, 21 },
			{ "LDDR", Family::_MOVE, { 0xed, 0xb8 }, 0, 0, 16, 21 },
			{ "CPIR", Family::_COMPARE, { 0xed, 0xb1 }, 0, 0, 16, 21 },
			{ "CPDR", Family::_COMPARE, { 0xed, 0xb9 }, 0, 0, 16, 21 },
			{ "INIR", Family::_INPUT, { 0xed, 0xb2 }, 0, 0, 16, 21 },
			{ "INDR", Family::_INPUT, { 0xed, 0xba }, 0, 0, 16, 21 },
			{ "OTIR", Family::_OUTPUT, { 0xed, 0xb3 }, 0, 0, 16, 21 },
			{ "OTDR", Family::_OUTPUT, { 0xed, 0xbb }, 0, 0, 16, 21 } });

	for (const auto& test : cases)
		for (bool perCycle : { false, true })
			runCase (test, perCycle);

	checkLifecycle ();

	std::cout << "\nAlternative checks=" << _checks << ", failures=" << _failures << '\n';

	return (_failures == 0 ? 0 : 1);
}

// ---
void AlternativeSelectionTests::check (bool ok, const std::string& message)
{
	_checks++;
	if (!ok)
	{
		_failures++;
		std::cerr << "FAIL: " << _caseName << ": " << message << '\n';
	}
}

// ---
void AlternativeSelectionTests::runCase (const Case& test, bool perCycle)
{
	_caseName = std::string (test._name) + (perCycle ? " [PerCycle]" : " [Full]");
	std::cout << _caseName << '\n';
	unsigned int failuresBefore = _failures;
	ZXSPECTRUM::Memory memory
		(ZXSPECTRUM::Memory::Configuration::_48K, ZXSPECTRUM::Type::_STANDARD, "ENG");
	MCHEmul::TicksCounterDelayed ticks (1);
	ZXSPECTRUM::CZ80 cpu (0);
	cpu.setMemoryRef (&memory);
	memory.linkToCPU (&cpu);
	cpu.addPorts ({ { 0x10, new FixedInputPort } });
	MCHEmul::Memory::configuration ().setBufferMemorySetCommands (false);
	if (!memory.initialize () || !cpu.initialize ())
	{
		check (false, "initialization; check ROM working directory");

		return;
	}

	if (perCycle)
		cpu.assignTicksCounter (ticks);
	cpu.setIFF1 (false);
	cpu.setIFF2 (false);
	memory.put (MCHEmul::Address (2, 0x5000), test._code);
	const FZ80::Instruction* previousLeaf = nullptr;

	// The same object must overwrite its selection in both directions.
	// Compare also terminates on a match before BC reaches zero (iteration four).
	unsigned int iterations = test._family == Family::_COMPARE ? 5 : 4;
	for (unsigned int iteration = 0; iteration < iterations; iteration++)
	{
		bool alternativeOne = (iteration & 1) != 0;
		bool compareMatch = iteration == 4;
		unsigned char count = alternativeOne || compareMatch ? 2 : 1;
		cpu.fRegister ().set ({ alternativeOne ? test._takenFlags : test._notTakenFlags });
		cpu.aRegister ().set ({ (unsigned char) (compareMatch ? 0x20 : 0x30) });
		cpu.hRegister ().set ({ 0x60 }); cpu.lRegister ().set ({ 0x00 });
		cpu.dRegister ().set ({ 0x61 }); cpu.eRegister ().set ({ 0x00 });
		if (test._family == Family::_MOVE || test._family == Family::_COMPARE)
		{
			cpu.bRegister ().set ({ 0x00 }); cpu.cRegister ().set ({ count });
		}
		else
		{
			cpu.bRegister ().set ({ count }); cpu.cRegister ().set ({ 0x10 });
		}
		memory.put (MCHEmul::Address (2, 0x6000), MCHEmul::UByte (0x20));

		// A real push marks the stack as used; merely setting SP cannot prepare RET.
		memory.stack () -> reset ();
		memory.stack () -> setPosition (0x6202, false);
		memory.stack () -> push (MCHEmul::UBytes ({ 0x20, 0x00 }));
		cpu.executeBufferedCommands (true);
		if (test._family == Family::_CALL)
			memory.stack () -> setPosition (0x6202, false);

		cpu.programCounter ().setAddress (MCHEmul::Address (2, 0x5000));
		unsigned int startClock = cpu.clockCycles ();
		bool completed = false;
		for (unsigned int step = 0; step < 64 && !completed; step++)
		{
			if (perCycle)
				ticks.count (1);
			if (!cpu.executeNextCycle ())
				break;
			completed = cpu.memoryAccessState () == ZXSPECTRUM::CZ80::MemoryAccessState::_COMPLETED ||
				cpu.memoryAccessState () == ZXSPECTRUM::CZ80::MemoryAccessState::_PENDING_PORT_COMPLETION;
		}
		check (completed && cpu.error () == MCHEmul::_NOERROR, "execution completed within bound");
		if (!completed || cpu.error () != MCHEmul::_NOERROR)
			break;

		cpu.executeBufferedCommands (true);
		const MCHEmul::Instruction* instruction = cpu.lastInstruction ();
		const auto* undefined = dynamic_cast <const MCHEmul::InstructionUndefined*> (instruction);
		if (undefined != nullptr)
			instruction = undefined -> lastDefinedInstruction ();
		const auto* leaf = dynamic_cast <const FZ80::Instruction*> (instruction);
		check (leaf != nullptr, "executed leaf available");
		if (leaf == nullptr)
			break;
		check (previousLeaf == nullptr || previousLeaf == leaf, "same instruction object reused");
		previousLeaf = leaf;
		check (leaf -> hasExecutedMCyclesStructure (), "execution selected an alternative");
		check (leaf -> executedMCyclesStructure () == (alternativeOne ? 1 : 0), "explicit alternative index");
		check (cpu.memoryAccessTimingAvailable (), "retained accesses associated");
		check (cpu.memoryAccessMCyclesStructure () == (alternativeOne ? 1 : 0), "transaction retains alternative");
		check (cpu.clockCycles () - startClock == (alternativeOne ? test._cycles1 : test._cycles0),
			"existing duration preserved");
		cpu.nextInstruction ();
		check (leaf -> executedMCyclesStructure () == (alternativeOne ? 1 : 0),
			"instruction inspection preserves selection");

		unsigned int expectedPC = 0x5000 + (unsigned int) test._code.size ();
		if ((test._family == Family::_CALL || test._family == Family::_RET) && alternativeOne)
			expectedPC = 0x2000;
		else if ((test._family == Family::_JR || test._family == Family::_DJNZ) && alternativeOne)
			expectedPC += 2;
		else if (alternativeOne && (test._family == Family::_MOVE || test._family == Family::_COMPARE ||
			test._family == Family::_INPUT || test._family == Family::_OUTPUT))
			expectedPC = 0x5000;
		check (cpu.programCounter ().asAddress ().value () == expectedPC, "execution path preserves PC behavior");

		const auto expected = expectedAccesses (test, alternativeOne);
		const auto& actual = cpu.memoryAccesses ();
		check (actual.size () == expected.size (), "transfer count for selected path");
		for (size_t i = 0; i < actual.size () && i < expected.size (); i++)
			check (actual [i]._address == expected [i]._address && actual [i]._type == expected [i]._type &&
				actual [i]._origin == expected [i]._origin && actual [i]._mCycle == expected [i]._mCycle &&
				actual [i]._contentionCycle == expected [i]._contentionCycle &&
				actual [i]._dataCycle == expected [i]._dataCycle, "literal access " + std::to_string (i));
	}

	cpu.executeBufferedCommands (true);
	MCHEmul::Memory::configuration ().setBufferMemorySetCommands (false);
	if (_failures == failuresBefore)
		std::cout << "PASS\n";
}

// ---
ZXSPECTRUM::CZ80::MemoryAccesses AlternativeSelectionTests::expectedAccesses
	(const Case& test, bool alternativeOne)
{
	using Access = ZXSPECTRUM::CZ80::MemoryAccess;
	using Type = Access::Type;
	using Origin = Access::Origin;
	ZXSPECTRUM::CZ80::MemoryAccesses result
		{ { 0x5000, Type::_READ, Origin::_INSTRUCTION_BYTE, 0, 0, 2 } };
	if (test._code.size () >= 2)
		result.push_back ({ 0x5001, Type::_READ, Origin::_INSTRUCTION_BYTE, 1,
			test._family == Family::_DJNZ ? 5U : 4U, test._family == Family::_DJNZ ? 7U : 6U });
	if (test._code.size () == 3)
		result.push_back ({ 0x5002, Type::_READ, Origin::_INSTRUCTION_BYTE, 2, 7, 9 });

	switch (test._family)
	{
		case Family::_CALL:
			{
				if (alternativeOne)
				{
					result.push_back ({ 0x6201, Type::_WRITE, Origin::_DATA, 3, 11, 13 });
					result.push_back ({ 0x6200, Type::_WRITE, Origin::_DATA, 4, 14, 16 });
				}
			}

			break;

		case Family::_RET:
			{
				if (alternativeOne)
				{
					result.push_back ({ 0x6200, Type::_READ, Origin::_DATA, 1, 5, 7 });
					result.push_back ({ 0x6201, Type::_READ, Origin::_DATA, 2, 8, 10 });
				}
			}

			break;

		case Family::_MOVE:
			{
				result.push_back ({ 0x6000, Type::_READ, Origin::_DATA, 2, 8, 10 });
				result.push_back ({ 0x6100, Type::_WRITE, Origin::_DATA, 3, 11, 13 });
			}

			break;

		case Family::_COMPARE:
			{
				result.push_back ({ 0x6000, Type::_READ, Origin::_DATA, 2, 8, 10 });
			}

			break;

		case Family::_INPUT:
			{
				result.push_back ({ 0x6000, Type::_WRITE, Origin::_DATA, 3, 13, 15 });
			}

			break;

		case Family::_OUTPUT:
			{
				result.push_back ({ 0x6000, Type::_READ, Origin::_DATA, 2, 9, 11 });
			}

			break;

		default:
			break;
	}

	return (result);
}

// ---
void AlternativeSelectionTests::checkLifecycle ()
{
	_caseName = "Alternative selection lifecycle";
	std::cout << _caseName << '\n';
	unsigned int failuresBefore = _failures;
	ZXSPECTRUM::Memory memory
		(ZXSPECTRUM::Memory::Configuration::_48K, ZXSPECTRUM::Type::_STANDARD, "ENG");
	ZXSPECTRUM::CZ80 cpu (0);
	cpu.setMemoryRef (&memory);
	memory.linkToCPU (&cpu);
	MCHEmul::Memory::configuration ().setBufferMemorySetCommands (false);
	if (!memory.initialize () || !cpu.initialize ())
	{
		check (false, "lifecycle fixture initialization");

		return;
	}
	memory.put (MCHEmul::Address (2, 0x5000), MCHEmul::UByte (0x00));
	AlternativeExecutionProbe instruction;
	check (!instruction.hasExecutedMCyclesStructure (), "construction has no execution selection");

	for (bool success : { true, false, true })
	{
		instruction.setSuccess (success);
		cpu.programCounter ().setAddress (MCHEmul::Address (2, 0x5000));
		check (instruction.execute (&cpu, &memory, memory.stack (), &cpu.programCounter ()) == success,
			"probe preserves execution result");
		check (instruction.hasExecutedMCyclesStructure () == success, "failed execution invalidates selection");
		check (instruction.executedMCyclesStructure () ==
			(success ? 1 : FZ80::Instruction::_NOMCYCLESSTRUCTURE), "selection can be published again after failure");
	}

	MCHEmul::Memory::configuration ().setBufferMemorySetCommands (false);
	if (_failures == failuresBefore)
		std::cout << "PASS\n";
}

/** Selects among existing non-owned instructions without introducing CPU dependencies. */
class SelectableInstruction final : public MCHEmul::InstructionUndefined
{
	public:
	SelectableInstruction (unsigned int c, const MCHEmul::Instructions& instructions)
		: MCHEmul::InstructionUndefined (c, instructions),
		  _selection (nullptr), _selectionCalls (0) { }

	void choose (const MCHEmul::Instruction* instruction) { _selection = instruction; }
	unsigned int selectionCalls () const { return (_selectionCalls); }

	protected:
	virtual const MCHEmul::Instruction* selectInstruction
		(MCHEmul::Memory*, const MCHEmul::Address&) const override;

	private:
	/** Non-owning selector result; the fixture owns all possible targets. */
	const MCHEmul::Instruction* _selection;
	mutable unsigned int _selectionCalls;
};

// ---
const MCHEmul::Instruction* SelectableInstruction::selectInstruction
	(MCHEmul::Memory*, const MCHEmul::Address&) const
{
	_selectionCalls++;

	return (_selection);
}

/** Exercises the generic retained-selection contract without executing host I/O. */
class InstructionResolutionTests final
{
	public:
	InstructionResolutionTests () : _checks (0), _failures (0) { }
	int run ();

	private:
	void check (bool ok, const std::string& message);

	unsigned int _checks, _failures;
};

// ---
int InstructionResolutionTests::run ()
{
	std::cout << "Recursive defined-instruction resolution\n";
	ZXSPECTRUM::Memory memory
		(ZXSPECTRUM::Memory::Configuration::_48K, ZXSPECTRUM::Type::_STANDARD, "ENG");
	ZXSPECTRUM::CZ80 cpu (0);
	cpu.setMemoryRef (&memory);
	memory.linkToCPU (&cpu);
	MCHEmul::Memory::configuration ().setBufferMemorySetCommands (false);
	if (!memory.initialize () || !cpu.initialize ())
	{
		check (false, "resolution fixture initialization; check ROM working directory");

		return (1);
	}

	memory.put (MCHEmul::Address (2, 0x5000), MCHEmul::UByte (0x00));
	const MCHEmul::Address address (2, 0x5000);
	auto* nop = dynamic_cast <MCHEmul::InstructionDefined*> (cpu.instruction (0x00));
	auto* inc = dynamic_cast <MCHEmul::InstructionDefined*> (cpu.instruction (0x04));
	AlternativeExecutionProbe failing;
	SelectableInstruction inner (0xcb, { { 0x00, nop }, { 0x04, inc }, { 0x100, &failing } });
	SelectableInstruction middle (0xdd, { { 0xcb, &inner } });
	SelectableInstruction outer (0xfd, { { 0xdd, &middle } });
	inner.choose (nop);
	middle.choose (&inner);
	outer.choose (&middle);
	check (inner.lastDefinedInstruction () == nullptr && outer.lastDefinedInstruction () == nullptr,
		"construction and selector preparation do not invent a retained selection");

	cpu.programCounter ().setAddress (address);
	check (inner.execute (&cpu, &memory, memory.stack (), &cpu.programCounter ()), "execute one level");
	check (inner.lastInstruction () == nop && inner.lastDefinedInstruction () == nop,
		"one-level immediate and final selections agree");
	cpu.programCounter ().setAddress (address);
	check (middle.execute (&cpu, &memory, memory.stack (), &cpu.programCounter ()), "execute two levels");
	check (middle.lastInstruction () == &inner && middle.lastDefinedInstruction () == nop,
		"two levels preserve their immediate link and resolve the leaf");
	cpu.programCounter ().setAddress (address);
	check (outer.execute (&cpu, &memory, memory.stack (), &cpu.programCounter ()), "execute three levels");
	check (outer.lastInstruction () == &middle && outer.lastDefinedInstruction () == nop,
		"three-level resolution");

	inner.choose (inc);
	check (outer.lastDefinedInstruction () == nop, "choosing alone does not replace execution selection");
	cpu.programCounter ().setAddress (address);
	check (outer.execute (&cpu, &memory, memory.stack (), &cpu.programCounter ()), "execute changed leaf");
	check (outer.lastDefinedInstruction () == inc, "next successful execution replaces the leaf");

	unsigned int calls = inner.selectionCalls () + middle.selectionCalls () + outer.selectionCalls ();
	unsigned int clock = cpu.clockCycles ();
	unsigned int pc = cpu.programCounter ().asAddress ().value ();
	unsigned char b = cpu.bRegister ().values ()[0].value ();
	for (unsigned int i = 0; i < 3; i++)
		check (outer.lastDefinedInstruction () == inc, "repeated resolution returns the same leaf");
	check (calls == inner.selectionCalls () + middle.selectionCalls () + outer.selectionCalls (),
		"resolution does not invoke memory selectors");
	check (cpu.clockCycles () == clock && cpu.programCounter ().asAddress ().value () == pc &&
		cpu.bRegister ().values ()[0].value () == b, "resolution does not modify CPU state");

	inner.choose (nop);
	check (outer.defineInstructionFrom (&memory, address), "define through nested selectors");
	check (outer.lastDefinedInstruction () == nop, "definition publishes its own retained selection");
	check (cpu.bRegister ().values ()[0].value () == b, "definition does not execute the selected instruction");

	failing.setSuccess (false);
	inner.choose (&failing);
	cpu.programCounter ().setAddress (address);
	check (!outer.execute (&cpu, &memory, memory.stack (), &cpu.programCounter ()), "nested execution can fail");
	check (outer.lastDefinedInstruction () == nop, "failure does not publish the attempted leaf");
	inner.choose (nullptr);
	check (!outer.defineInstructionFrom (&memory, address), "definition can leave an unresolved inner link");
	check (outer.lastDefinedInstruction () == nullptr, "unresolved nested chain returns null");

	cpu.executeBufferedCommands (true);
	MCHEmul::Memory::configuration ().setBufferMemorySetCommands (false);
	if (_failures == 0)
		std::cout << "PASS\n";
	std::cout << "Resolution checks=" << _checks << ", failures=" << _failures << '\n';

	return (_failures == 0 ? 0 : 1);
}

// ---
void InstructionResolutionTests::check (bool ok, const std::string& message)
{
	_checks++;
	if (!ok)
	{
		_failures++;
		std::cerr << "FAIL: recursive resolution: " << message << '\n';
	}
}

/** Confirms the default hook preserves port-only timing without a Spectrum ULA link. */
class PortOnlyTimingTests final
{
	public:
	PortOnlyTimingTests () : _checks (0), _failures (0) { }
	int run ();

	private:
	void check (bool ok, const std::string& message);

	// Implementation
	unsigned int _checks, _failures;
};

// ---
int PortOnlyTimingTests::run ()
{
	for (bool perCycle : { false, true })
	{
		std::cout << "Port-only timing" << (perCycle ? " [PerCycle]\n" : " [Full]\n");
		unsigned int failuresBefore = _failures;
		ZXSPECTRUM::Memory memory
			(ZXSPECTRUM::Memory::Configuration::_48K, ZXSPECTRUM::Type::_STANDARD, "ENG");
		MCHEmul::TicksCounterDelayed ticks (1);
		ZXSPECTRUM::CZ80 cpu (0);
		cpu.setMemoryRef (&memory);
		memory.linkToCPU (&cpu);
		FixedInputPort* port = new FixedInputPort (3);
		cpu.addPorts ({ { 0x10, port } });
		MCHEmul::Memory::configuration ().setBufferMemorySetCommands (false);
		if (!memory.initialize () || !cpu.initialize ())
		{
			check (false, "port-only initialization");

			continue;
		}
		if (perCycle)
			cpu.assignTicksCounter (ticks);
		cpu.setIFF1 (false);
		cpu.setIFF2 (false);
		cpu.bRegister ().set ({ 0x02 }); cpu.cRegister ().set ({ 0x10 });
		cpu.hRegister ().set ({ 0x60 }); cpu.lRegister ().set ({ 0x00 });
		memory.put (MCHEmul::Address (2, 0x2000), std::vector <MCHEmul::UByte> { 0xed, 0xb2 }, true);
		cpu.programCounter ().setAddress (MCHEmul::Address (2, 0x2000));

		for (unsigned int iteration = 0; iteration < 2; iteration++)
		{
			unsigned int startClock = cpu.clockCycles ();
			bool pending = false;
			for (unsigned int step = 0; step < 64 && !pending; step++)
			{
				if (perCycle)
					ticks.count (1);
				if (!cpu.executeNextCycle ())
					break;
				pending = cpu.memoryAccessState () ==
					ZXSPECTRUM::CZ80::MemoryAccessState::_PENDING_PORT_COMPLETION;
			}
			check (pending && cpu.hasPendingPortAccess (), "prepared repeating/terminal input");
			if (!pending || !cpu.hasPendingPortAccess ())
				break;
			check (cpu.clockCycles () - startClock == (iteration == 0 ? 24 : 19),
				"three port waits plus existing repetition duration");
			check (cpu.pendingPortAccess ()._clockCycle == startClock + 15,
				"port-only sample offset unchanged by repetition");
			check (port -> queries () == iteration + 1 && port -> reads () == iteration,
				"one query and no early input");
			check (!cpu.memoryContentionHandled () && cpu.includedMemoryContentionClockCycles () == 0,
				"no memory plan without ULA");
			cpu.executeBufferedCommands (true);
			check (port -> reads () == iteration + 1, "input completes once");
			check (cpu.memoryAccessTimingAvailable () && cpu.memoryAccessMCyclesStructure () == (iteration == 0 ? 1 : 0),
				"both alternatives retain their association");
		}
		cpu.executeBufferedCommands (true);
		MCHEmul::Memory::configuration ().setBufferMemorySetCommands (false);
		if (_failures == failuresBefore)
			std::cout << "PASS\n";
	}
	std::cout << "Port-only checks=" << _checks << ", failures=" << _failures << '\n';

	return (_failures == 0 ? 0 : 1);
}

// ---
void PortOnlyTimingTests::check (bool ok, const std::string& message)
{
	_checks++;
	if (!ok)
	{
		_failures++;
		std::cerr << "FAIL: port-only timing: " << message << '\n';
	}
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
	int alternativeResult = AlternativeSelectionTests::run ();
	PortOnlyTimingTests portOnlyTests;
	int portOnlyResult = portOnlyTests.run ();
	InstructionResolutionTests resolutionTests;
	int resolutionResult = resolutionTests.run ();
	int contentionResult = MemoryContentionTests::run ();

	return (accessResult == 0 && alternativeResult == 0 && portOnlyResult == 0 &&
		resolutionResult == 0 && contentionResult == 0 ? 0 : 1);
}

// End of the file
/*@}*/
