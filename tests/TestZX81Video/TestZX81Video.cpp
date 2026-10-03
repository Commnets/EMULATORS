/** ZX80/ZX81 video timing regression tests.
 * Run from emulators/ZX81Commons so the existing ROM files are available.
 * Expected load phases follow the nominal model agreed for this change;
 * screen columns 16..271 come from the pre-change standard display traces.
 * These component tests do not claim cycle-exact prefix or port replay.
 */
#include <ZX81/ULA.hpp>
#include <ZX81/Memory.hpp>
#include <ZX81/ZX81.hpp>
#include <FZ80/CZ80.hpp>
#include <iostream>
#include <crtdbg.h>

#undef main

class VideoULA final : public ZX81::ULA
{
	public:
	VideoULA (ZX81::Type t, bool ntsc)
		: ZX81::ULA (ntsc ? ZX81::ULA_NTSC::_VRASTERDATA : ZX81::ULA_PAL::_VRASTERDATA,
			ZX81::ULA_PAL::_HRASTERDATA, t, ZX81::Memory::_ULA_VIEW)
							{ }
	using ZX81::ULA::captureCharData;
	using ZX81::ULA::loadPendingCharData;
	using ZX81::ULA::restartRaster;
	void prime (unsigned int c, unsigned short h = 0, unsigned short v = 100);
	bool syncActive () const
							{ return (_lineSyncActive); }
};

// ---
void VideoULA::prime (unsigned int c, unsigned short h, unsigned short v)
{
	restartRaster ();
	_raster.hData ().add (h);
	_raster.vData ().add (v);
	_ULARegisters -> initialize ();
	_ULARegisters -> setLINECTRLNBlocked (false);
	_ULARegisters -> setLINECNTRL (0);
	_ULARegisters -> setNMIGenerator (false);
	_ULARegisters -> setSyncOutputWhite (true);
	_pendingCharacters.clear ();
	_nextPendingCharacter = 0;
	_simulationStarted = true;
	_lastCPUCycles = c;
	for (unsigned short y = 0; y < numberRows (); y++)
		_screenMemory -> setHorizontalLine (0, y, numberColumns (), 1);
}

class VideoTests final
{
	public:
	static int run ();

	private:
	static void check (bool ok, const char* message);
	static void advance (FZ80::CZ80& cpu, VideoULA& ula, unsigned int cycles);
	static void model (ZX81::Type t, bool ntsc);
	static void romMirrors (ZX81::Type t, ZX81::Memory::Configuration cfg);
	static unsigned int _checks, _failures;
};

unsigned int VideoTests::_checks = 0, VideoTests::_failures = 0;

// ---
void VideoTests::check (bool ok, const char* message)
{
	_checks++;
	if (!ok)
	{
		_failures++;
		std::cerr << "FAIL: " << message << '\n';
	}
}

// ---
void VideoTests::advance (FZ80::CZ80& cpu, VideoULA& ula, unsigned int cycles)
{
	cpu.addClockCycles (cycles);
	check (ula.simulate (&cpu), "ULA simulate");
}

// ---
void VideoTests::model (ZX81::Type t, bool ntsc)
{
	std::cout << "Model=" << (int) (t) << ",NTSC=" << ntsc << '\n';
	ZX81::Memory memory (t == ZX81::Type::_ZX80
		? ZX81::Memory::Configuration::_NOEXPANDED
		: ZX81::Memory::Configuration::_16KEXPANSION, t);
	FZ80::CZ80 cpu (0);
	cpu.setMemoryRef (&memory);
	VideoULA ula (t, ntsc);
	ula.setMemoryRef (&memory);
	check (memory.initialize () && cpu.initialize () && ula.initialize (), "initialize");
	// Fixtures own short-lived memories; do not retain writes in the global buffer.
	MCHEmul::Memory::configuration ().setBufferMemorySetCommands (false);
	cpu.iRegister ().set ({ MCHEmul::UByte (t == ZX81::Type::_ZX80 ? 0x0e : 0x1e) });
	bool zx80 = (t == ZX81::Type::_ZX80);
	cpu.addClockCycles (100);

	// At the load boundary, polarity and all eight bits change together.
	ula.prime (cpu.clockCycles ());
	ula.captureCharData (&cpu, MCHEmul::UByte (0x80));
	ula.loadPendingCharData (cpu.clockCycles () + 3, 0);
	check (ula.registers () -> pendingSHIFTBits () == 0, "not loaded at T4 rising");
	ula.loadPendingCharData (cpu.clockCycles () + 3, 1);
	check (ula.registers () -> pendingSHIFTBits () == (zx80 ? 0 : 8), "T4 falling load phase");
	ula.loadPendingCharData (cpu.clockCycles () + 4, 0);
	check (ula.registers () -> pendingSHIFTBits () == 8 &&
		ula.registers () -> reverseVideo (), "pattern and inverse latched");

	// The current line counter, rather than its capture-time value, selects the row.
	ula.prime (cpu.clockCycles ());
	ula.captureCharData (&cpu, MCHEmul::UByte (0x30));
	ula.registers () -> setLINECNTRL (1);
	ula.loadPendingCharData (cpu.clockCycles () + 4, 0);
	int originalView = memory.activeView () -> id ();
	memory.setActiveView (ZX81::Memory::_ULA_VIEW);
	MCHEmul::UByte expected = memory.value (MCHEmul::Address (2, zx80 ? 0x0f81 : 0x1f81));
	memory.setActiveView (originalView);
	check (ula.registers () -> originalSHIFTRegister () == expected, "row selected at load");
	check (memory.activeView () -> id () == originalView, "memory view restored");

	// Captures collected before catch-up must retain both codes and phases.
	ula.prime (cpu.clockCycles ());
	ula.captureCharData (&cpu, MCHEmul::UByte (0x80));
	cpu.addClockCycles (4);
	ula.captureCharData (&cpu, MCHEmul::UByte (0));
	cpu.addClockCycles (4);
	check (ula.simulate (&cpu), "batched captures");
	if (zx80)
		advance (cpu, ula, 1);
	check (!ula.registers () -> reverseVideo (), "second captured polarity survives");
	advance (cpu, ula, 4);
	check (ula.registers () -> pendingSHIFTBits () == 0, "batch drained");

	// Exactly one logical count and one row for each 414-pixel autonomous period.
	ula.prime (cpu.clockCycles ());
	advance (cpu, ula, 207);
	check (ula.raster ().hData ().currentPositionAtBase0 () == 0 &&
		ula.raster ().vData ().currentPositionAtBase0 () == 101 &&
		ula.registers () -> LINECNTRL () == 1, "autonomous first period");
	advance (cpu, ula, 207);
	check (ula.raster ().vData ().currentPositionAtBase0 () == 102 &&
		ula.registers () -> LINECNTRL () == 2, "autonomous second period");

	// An INT during the tail may realign it, but must not count another line.
	ula.prime (cpu.clockCycles (), zx80 ? 405 : 406);
	advance (cpu, ula, 1);
	check (ula.syncActive () && ula.registers () -> LINECNTRL () == 1,
		"sync precedes wrap");
	ula.setINTack (cpu.clockCycles ());
	advance (cpu, ula, 1);
	check (ula.registers () -> LINECNTRL () == 1, "coalesced INT during tail");
	advance (cpu, ula, 4);
	check (ula.raster ().vData ().currentPositionAtBase0 () == 101 &&
		!ula.syncActive (), "single eventual wrap");

	// An INT at the old wrap point must defer the vertical change.
	ula.prime (cpu.clockCycles (), 413);
	ula.setINTack (cpu.clockCycles ());
	advance (cpu, ula, 1);
	check (ula.raster ().vData ().currentPositionAtBase0 () == 100,
		"external sync wins over coincident wrap");
	ula.restartRaster ();
	check (!ula.syncActive (), "VSYNC cancels tail state");

	// A full inverse-space row is a solid 256-pixel bar. The baseline traces
	// start its first fetch 71 T after INT. No host rendering or timing is involved.
	ula.prime (cpu.clockCycles (), 405);
	ula.setINTack (cpu.clockCycles ());
	advance (cpu, ula, 71);
	unsigned short row = ula.raster ().vData ().currentVisiblePosition ();
	for (unsigned int n = 0; n < 32; n++)
	{
		ula.captureCharData (&cpu, MCHEmul::UByte (0x80));
		advance (cpu, ula, 4);
	}
	// ZX80's last pattern is still queued at the boundary; ZX81 has emitted
	// its first pixel at T4 falling and retains seven bits.
	check (ula.registers () -> pendingSHIFTBits () == (zx80 ? 0 : 7),
		"last character at following HALT boundary");
	advance (cpu, ula, 1);
	check (ula.registers () -> pendingSHIFTBits () == (zx80 ? 6 : 5),
		"last character continues during HALT interval");
	advance (cpu, ula, 5);
	check (ula.registers () -> pendingSHIFTBits () == 0, "last character fully emitted");
	const unsigned int* pixels = ula.screenMemory () -> frameData () + row * 289;
	unsigned int white = pixels [0];
	bool framed = true;
	for (unsigned int x = 0; x < 289; x++)
		framed &= ((pixels [x] != white) == (x >= 16 && x <= 271));
	check (framed, "unchanged 16/17 borders and 256 pixels");
}

// ---
void VideoTests::romMirrors (ZX81::Type t, ZX81::Memory::Configuration cfg)
{
	std::cout << "ROM model=" << (int) (t) << ",configuration=" << (int) (cfg) << std::endl;
	// Use the production machine initialization to connect MemoryVideoCode to CPU/ULA.
	// No device simulation or host-window rendering is exercised by these tests.
	MCHEmul::ASCIIToCodeConverter converter;
	ZX81::SinclairZX81 machine (&converter, cfg, ZX81::SinclairZX81::VisualSystem::_PAL, t);
	bool initialized = machine.initialize ();
	check (initialized, "ROM mirror machine initialization");
	MCHEmul::Memory::configuration ().setBufferMemorySetCommands (false);
	if (!initialized)
		return;
	FZ80::CZ80* cpu = static_cast <FZ80::CZ80*> (machine.cpu ());
	MCHEmul::Memory* memory = machine.memory ();
	ZX81::ULA* ula = static_cast <ZX81::ULA*> (machine.chip (ZX81::ULA::_ID));
	unsigned int offset = (t == ZX81::Type::_ZX80) ? 3 : 0x9a;
	cpu -> iRegister ().set ({ MCHEmul::UByte (t == ZX81::Type::_ZX80 ? 0x0e : 0x1e) });
	for (unsigned int base : { 0x8000U, 0xa000U })
	{
		MCHEmul::Address address (2, base + offset);
		check (memory -> value (address).value () == 0x3e, "ROM mirror ordinary read");
		memory -> set (address, MCHEmul::UByte (0));
		check (memory -> value (address).value () == 0x3e, "ROM mirror write protected");

		// Deliberately keep PC below A15: interception must use the accessed address.
		cpu -> programCounter ().setAddress (MCHEmul::Address (2, 0x7fff));
		check (cpu -> fetchInstructionCodeAt (memory, address) == 0, "upper ROM fetch is NOP");
		check (memory -> value (address).value () == 0x3e, "fetch did not modify ROM");
		check (cpu -> fetchInstructionCodeAt (memory, MCHEmul::Address (2, offset)) == 0x3e,
			"lower ROM fetch remains opcode");
		memory -> setActiveView (ZX81::Memory::_ULA_VIEW);
		check (cpu -> fetchInstructionCodeAt (memory, address) == 0x3e, "ULA view bypasses interception");
		memory -> setCPUView ();
		cpu -> setHalt ();
		check (cpu -> fetchInstructionCodeAt (memory, address) == 0x3e, "HALT inhibits capture");
		cpu -> resetHalt ();

		// Reset the queue before running a real instruction sequence from the ROM mirror.
		ula -> initialize ();
		cpu -> aRegister ().set ({ MCHEmul::UByte (0x55) });
		cpu -> programCounter ().setAddress (address);
		unsigned int clock = cpu -> clockCycles ();
		check (cpu -> executeNextCycle (), "execute intercepted ROM opcode");
		check (cpu -> programCounter ().internalRepresentation () == base + offset + 1 &&
			cpu -> aRegister ().values () [0].value () == 0x55 && cpu -> clockCycles () == clock + 4,
			"ROM opcode executes one NOP without operand");
		check (ula -> simulate (cpu), "ROM character simulation");
		check (ula -> registers () -> pendingSHIFTBits () == (t == ZX81::Type::_ZX80 ? 0 : 7),
			"ROM fetch queues character load");
		if (t != ZX81::Type::_ZX80)
		{
			check (cpu -> executeNextCycle () && cpu -> haltActive (), "ROM NOP followed by HALT");
			cpu -> resetHalt ();
		}
	}

	// A real immediate operand in the upper ROM must retain its original byte.
	// At reset ZX81 contains OUT FD,A; ZX80 contains JP 0261 at offset 5.
	cpu -> programCounter ().setAddress (MCHEmul::Address (2, t == ZX81::Type::_ZX80 ? 0x8005 : 0x8000));
	ula -> registers () -> setNMIGenerator (true);
	check (cpu -> executeNextCycle (), "upper ROM instruction with operand");
	check (t == ZX81::Type::_ZX80
		? cpu -> programCounter ().internalRepresentation () == 0x0261
		: cpu -> programCounter ().internalRepresentation () == 0x8002 && !ula -> registers () -> NMIGenerator (),
		"upper ROM operands remain intact");

	// Place a real prefix on the last byte of RAM. Its following M1 enters ROM.
	if (cfg == ZX81::Memory::Configuration::_16KEXPANSION)
	{
		memory -> set (MCHEmul::Address (2, 0x7fff), MCHEmul::UByte (0xcb));
		cpu -> programCounter ().setAddress (MCHEmul::Address (2, 0x7fff));
		// ROM[0] is D3 (D6 set), so this opcode must remain unchanged.
		check (cpu -> executeNextCycle () && cpu -> programCounter ().internalRepresentation () == 0x8001,
			"prefixed fetch crosses into upper ROM");
	}
}

// ---
int VideoTests::run ()
{
	for (unsigned int t = 0; t < 4; t++)
	{
		model (static_cast <ZX81::Type> (t), false);
		model (static_cast <ZX81::Type> (t), true);
	}
	// Headless SDL prevents host windows; these checks only exercise CPU/memory/ULA state.
	SDL_setenv ("SDL_VIDEODRIVER", "dummy", 1);
	for (unsigned int t = 0; t < 4; t++)
	{
		romMirrors (static_cast <ZX81::Type> (t), ZX81::Memory::Configuration::_NOEXPANDED);
		if (t != 0)
			romMirrors (static_cast <ZX81::Type> (t), ZX81::Memory::Configuration::_16KEXPANSION);
	}
	std::cout << "Checks=" << _checks << ",Failures=" << _failures << '\n';
	return (_failures == 0 ? 0 : 1);
}

// ---
int main ()
{
	std::cout << std::unitbuf;
	// Report assertions to the test runner instead of opening an interactive dialog.
	_CrtSetReportMode (_CRT_ASSERT, _CRTDBG_MODE_FILE);
	_CrtSetReportFile (_CRT_ASSERT, _CRTDBG_FILE_STDERR);
	return (VideoTests::run ());
}
