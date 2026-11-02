#define SDL_MAIN_HANDLED
#include "TimedWrites.hpp"
#include <COMMODORE/VICI/VICI.hpp>
#include <COMMODORE/VIA/VIA.hpp>
#include <F6500/C6502.hpp>
#include <VIC20/Memory.hpp>
#include <VIC20/VIA2Registers.hpp>
#include <algorithm>
#include <iostream>

/** Guard the replacement frame on both sides and retain ownership of the original arrays. */
class VICIGuardedScreen final : public MCHEmul::ScreenMemory
{
	public:
	static constexpr unsigned int _SENTINEL = 0xdeadbeef;
	VICIGuardedScreen (size_t columns, size_t rows);
	virtual ~VICIGuardedScreen () override;
	void reset ();
	bool matches (const std::vector <unsigned int>& expected) const;

	private:
	static unsigned int* palette ();
	unsigned int* _originalFrameData;
	std::vector <unsigned int> _guardedData;
};

constexpr unsigned int VICIGuardedScreen::_SENTINEL;

// ---
VICIGuardedScreen::VICIGuardedScreen (size_t columns, size_t rows)
	: MCHEmul::ScreenMemory (columns, rows, palette ()),
	  _originalFrameData (_frameData),
	  _guardedData (columns * rows + 16, _SENTINEL)
{
	_frameData = _guardedData.data () + 8;
}

// ---
VICIGuardedScreen::~VICIGuardedScreen ()
{
	delete [] _originalFrameData;
	delete [] _colorPalette;
	_frameData = _colorPalette = nullptr;
}

// ---
void VICIGuardedScreen::reset ()
{
	std::fill (_guardedData.begin (), _guardedData.end (), _SENTINEL);
}

// ---
bool VICIGuardedScreen::matches (const std::vector <unsigned int>& expected) const
{
	for (size_t i = 0; i < 8; ++i)
		if (_guardedData [i] != _SENTINEL ||
			_guardedData [_guardedData.size () - 1 - i] != _SENTINEL)
		{
			std::cerr << "Frame guard overwritten\n";
			return (false);
		}

	if (!std::equal (expected.begin (), expected.end (), _frameData))
	{
		std::cerr << "Frame differs, including pixels outside the expected span\n";
		return (false);
	}

	return (true);
}

// ---
unsigned int* VICIGuardedScreen::palette ()
{
	unsigned int* result = new unsigned int [16];
	for (unsigned int i = 0; i < 16; ++i)
		result [i] = i;
	return (result);
}

/** The register bank needs no VIC20 ROM or expanded machine memory. */
class VICIGraphicsMemory final : public MCHEmul::Memory
{
	public:
	VICIGraphicsMemory ()
		: MCHEmul::Memory (0, content ()) { }

	private:
	virtual MCHEmul::Stack* lookForStack () override
							{ return (nullptr); }
	virtual MCHEmul::MemoryView* lookForCPUView () override
							{ return (view (0)); }
	static MCHEmul::Memory::Content content ();
};

// ---
MCHEmul::Memory::Content VICIGraphicsMemory::content ()
{
	MCHEmul::PhysicalStorage* storage = new MCHEmul::PhysicalStorage
		(0, MCHEmul::PhysicalStorage::Type::_RAM, 16);
	COMMODORE::VICIRegisters* registers = new COMMODORE::VICIRegisters
		(storage, 0, MCHEmul::Address (2, 0x9000), 16);
	registers -> setBufferMemorySetCommands (false);
	MCHEmul::Memory::Content result;
	result._physicalStorages = { { 0, storage } };
	result._subsets = { { COMMODORE::VICIRegisters::_VICREGS_SUBSET, registers } };
	MCHEmul::PhysicalStorage* ram = new MCHEmul::PhysicalStorage
		(1, MCHEmul::PhysicalStorage::Type::_RAM, 0x10000);
	result._physicalStorages.emplace (1, ram);
	result._subsets.emplace (1, new MCHEmul::PhysicalStorageSubset
		(1, ram, 0, MCHEmul::Address (2, 0), 0x9000));
	result._subsets.emplace (2, new MCHEmul::PhysicalStorageSubset
		(2, ram, 0x9010, MCHEmul::Address (2, 0x9010), 0x6ff0));
	MCHEmul::MemoryView* cpuView = new MCHEmul::MemoryView (0, result._subsets);
	MCHEmul::PhysicalStorageSubsets videoSubsets;
	videoSubsets.emplace (10, new MCHEmul::PhysicalStorageSubset
		(10, ram, 0, MCHEmul::Address (2, 0), 0x0400));
	videoSubsets.emplace (11, new MCHEmul::PhysicalStorageSubset
		(11, ram, 0x1000, MCHEmul::Address (2, 0x1000), 0x1000));
	videoSubsets.emplace (12, new MCHEmul::PhysicalStorageSubset
		(12, ram, 0x8000, MCHEmul::Address (2, 0x8000), 0x1000));
	videoSubsets.emplace (13, new MCHEmul::EmptyPhysicalStorageSubset
		(13, MCHEmul::UByte (0xff), ram, 0x0400, MCHEmul::Address (2, 0x0400), 0x0c00));
	videoSubsets.emplace (14, new MCHEmul::EmptyPhysicalStorageSubset
		(14, MCHEmul::UByte (0xff), ram, 0x9000, MCHEmul::Address (2, 0x9000), 0x0400));
	videoSubsets.emplace (15, new VIC20::VICIColorRAMSubset (15, ram));
	videoSubsets.emplace (16, new MCHEmul::EmptyPhysicalStorageSubset
		(16, MCHEmul::UByte (0xff), ram, 0x9800, MCHEmul::Address (2, 0x9800), 0x0800));
	result._subsets.insert (videoSubsets.begin (), videoSubsets.end ());
	result._views = { { 0, cpuView }, { 1, new MCHEmul::MemoryView (1, videoSubsets) } };
	return (result);
}

/** Exercise the real decode/background/copy methods with a visible frame origin. */
class VICIGraphicsProbe final : public COMMODORE::VICI
{
	public:
	VICIGraphicsProbe (bool pal, int view = 1);
	virtual ~VICIGraphicsProbe () override
							{ delete (_soundFunction); }
	void render (unsigned short x, unsigned short y, unsigned char nibble, unsigned int mode);
	bool addressReads ();
	bool isolatedVIARead ();
	VICIGuardedScreen* guardedScreen ()
							{ return (static_cast <VICIGuardedScreen*> (screenMemory ())); }

	private:
	virtual MCHEmul::ScreenMemory* createScreenMemory () override
							{ return (new VICIGuardedScreen (numberColumns (), numberRows ())); }
};

// ---
VICIGraphicsProbe::VICIGraphicsProbe (bool pal, int view)
	: COMMODORE::VICI (
		pal ? COMMODORE::VICI_PAL::_VRASTERDATA : COMMODORE::VICI_NTSC::_VRASTERDATA,
		pal ? COMMODORE::VICI_PAL::_HRASTERDATA : COMMODORE::VICI_NTSC::_HRASTERDATA,
		view, pal ? 71 : 65,
		new COMMODORE::VICISoundSimpleLibWrapper (pal ? 1108405 : 1022727, 44100))
{
	// Nothing else.
}

// ---
void VICIGraphicsProbe::render (
	unsigned short x, unsigned short y, unsigned char nibble, unsigned int mode)
{
	// Fixed independent color identities: background 2, border 3, character 5, auxiliary 4.
	_VICIRegisters -> setRegister (0x0e, MCHEmul::UByte (0x40));
	_VICIRegisters -> setRegister (0x0f, MCHEmul::UByte ((mode & 1) ? 0x23 : 0x2b));
	_vicGraphicInfo._graphicData = MCHEmul::UByte (nibble);
	_vicGraphicInfo._colorData = MCHEmul::UByte (mode >= 2 ? 13 : 5);
	_vicGraphicInfo._graphicDataReady = true;
	drawGraphics ({ _raster.hData ().firstDisplayPosition (),
		_raster.hData ().lastDisplayPosition (), x,
		_raster.vData ().firstDisplayPosition (),
		_raster.vData ().lastDisplayPosition (), y });
}

// ---
bool VICIGraphicsProbe::addressReads ()
{
	// Put different bytes at the wrapped destination and the former linear destination.
	memoryRef () -> set (MCHEmul::Address (2, 0x8000), MCHEmul::UByte (0xa5));
	memoryRef () -> set (MCHEmul::Address (2, 0x2000), MCHEmul::UByte (0x3c));
	_VICIRegisters -> setRegister (0x05, MCHEmul::UByte (0xff));
	_VICIRegisters -> setRegister (0x02, MCHEmul::UByte (0x96));
	_vicGraphicInfo._horizontalOpened = true;
	_vicGraphicInfo._verticalState =
		VICGraphicInfo::VerticalState::_DISPLAY;
	_vicGraphicInfo._COL = _vicGraphicInfo._ROW = 0;
	_vicGraphicInfo._COLMAX = _vicGraphicInfo._ROWMAX = 1;
	_vicGraphicInfo._matrixBaseCounter =
	_vicGraphicInfo._matrixCounter = 0x0400;
	_vicGraphicInfo._matrixOffset = 0x0200;
	_vicGraphicInfo._DRAW = true;
	readVideoMatrixAndColorRAM ();
	if (_vicGraphicInfo._screenCodeData.value () != 0xa5)
		return (false);

	for (unsigned int expanded = 0; expanded < 2; ++expanded)
	{
		_VICIRegisters -> setRegister (0x03, MCHEmul::UByte (0x2e | expanded));
		_vicGraphicInfo._screenCodeData = MCHEmul::UByte (expanded ? 0x40 : 0x80);
		_vicGraphicInfo._RC = 0;
		readGraphicalInfo ();
		if (_vicGraphicInfo._graphicByteData.value () != 0xa5)
			return (false);
	}
	std::cout << "Matrix and 8x8/8x16 glyph fetches use the wrapped destination\n";
	return (true);
}

// ---
bool VICIGraphicsProbe::isolatedVIARead ()
{
	_VICIRegisters -> setRegister (0x05, MCHEmul::UByte (0xf4));
	_VICIRegisters -> setRegister (0x03, MCHEmul::UByte (0x2e));
	_vicGraphicInfo._screenCodeData = MCHEmul::UByte (0x24);
	_vicGraphicInfo._RC = 4;
	readGraphicalInfo ();
	return (_vicGraphicInfo._graphicByteData.value () == 0xff);
}

class VICIGraphicsRegression final
{
	public:
	static bool run (bool pal);

	private:
	static const char* const _HIRES [16];
	static const char* const _MULTICOLOR [16];
};

// These expanded dot strings are the independent 4-bit truth tables, not raster clipping formulas.
const char* const VICIGraphicsRegression::_HIRES [16] = {
	"00000000", "00000011", "00001100", "00001111",
	"00110000", "00110011", "00111100", "00111111",
	"11000000", "11000011", "11001100", "11001111",
	"11110000", "11110011", "11111100", "11111111" };

const char* const VICIGraphicsRegression::_MULTICOLOR [16] = {
	"00000000", "00001111", "00002222", "00003333",
	"11110000", "11111111", "11112222", "11113333",
	"22220000", "22221111", "22222222", "22223333",
	"33330000", "33331111", "33332222", "33333333" };

// ---
bool VICIGraphicsRegression::run (bool pal)
{
	VICIGraphicsMemory memory;
	VICIGraphicsProbe vic (pal);
	vic.setMemoryRef (&memory);
	if (!memory.initialize () || !vic.initialize ())
		return (false);

	if (!vic.addressReads ())
	{
		std::cerr << "Wrapped video fetch differs\n";
		return (false);
	}
	VICIGuardedScreen* screen = vic.guardedScreen ();
	const size_t columns = screen -> columns ();
	const size_t rows = screen -> rows ();
	if (columns != (pal ? 467 : 420) || rows != (pal ? 284 : 233))
		return (false);

	const size_t xs [10] = { 0, 40, columns - 8, columns - 7, columns - 6,
		columns - 5, columns - 4, columns - 3, columns - 2, columns - 1 };
	const size_t spans [10] = { 8, 8, 8, 7, 6, 5, 4, 3, 2, 1 };
	const size_t ys [3] = { 0, rows / 2, rows - 1 };
	const unsigned int colors [4] = { 2, 3, 5, 4 };
	size_t cases = 0;
	for (unsigned int mode = 0; mode < 4; ++mode)
		for (unsigned char nibble = 0; nibble < 16; ++nibble)
			for (size_t x = 0; x < 10; ++x)
				for (size_t y : ys)
				{
					screen -> reset ();
					std::vector <unsigned int> expected (columns * rows, VICIGuardedScreen::_SENTINEL);
					for (size_t i = 0; i < spans [x]; ++i)
					{
						unsigned int color;
						if (mode < 2)
							color = ((_HIRES [nibble][i] == '1') != (mode == 1)) ? 5 : 2;
						else
							color = colors [_MULTICOLOR [nibble][i] - '0'];
						expected [y * columns + xs [x] + i] = color;
					}
					vic.render ((unsigned short) xs [x], (unsigned short) y, nibble, mode);
					if (!screen -> matches (expected))
					{
						std::cerr << (pal ? "PAL" : "NTSC") << " mode=" << mode
							<< " nibble=" << (unsigned int) nibble << " x=" << xs [x] << " y=" << y << '\n';
						return (false);
					}
					++cases;
				}
	std::cout << (pal ? "PAL: " : "NTSC: ") << cases << " graphics/guard cases passed\n";
	return (true);
}

/** Validate register construction and reset without adding inspection hooks to production. */
class VICIRegisterRegression final
{
	public:
	static bool run ();

	private:
	static bool defaults (const COMMODORE::VICIRegisters& registers);
	static bool matrixAddresses ();
	static bool addressOffsets ();
};

// ---
bool VICIRegisterRegression::run ()
{
	if (!matrixAddresses () || !addressOffsets ())
		return (false);

	MCHEmul::PhysicalStorage storage (1, MCHEmul::PhysicalStorage::Type::_RAM, 16);
	COMMODORE::VICISoundSimpleLibWrapper sound (1108405, 44100);
	COMMODORE::VICIRegisters registers (&storage, 0, MCHEmul::Address (2, 0x9000), 16);

	// Construction must establish the presets without requiring a sound or raster connection.
	if (!defaults (registers))
	{
		std::cerr << "VIC-I constructor presets differ\n";
		return (false);
	}

	sound.initialize ();
	MCHEmul::Raster raster (COMMODORE::VICI_PAL::_VRASTERDATA,
		COMMODORE::VICI_PAL::_HRASTERDATA, 8);
	raster.initialize ();
	registers.linkToRaster (&raster);
	registers.setSoundLibWrapper (&sound);
	for (unsigned int cycle = 0; cycle < 4 * 71; ++cycle)
		raster.moveCycles (1);

	for (unsigned int round = 0; round < 2; ++round)
	{
		registers.setRegister (0x02, MCHEmul::UByte (0x15));
		registers.setRegister (0x05, MCHEmul::UByte (0xcd));
		registers.setRegister (0x03, MCHEmul::UByte (0x01));
		registers.setRegister (0x00, MCHEmul::UByte (0x80));
		registers.setRegister (0x0a, MCHEmul::UByte (0xcf));
		registers.setRegister (0x0e, MCHEmul::UByte (0xa7));
		if (registers.screenMemory ().value () != 0x1000 ||
			registers.charDataMemory ().value () != 0x1400 ||
			registers.colourMemory ().value () != 0x9400 ||
			sound.readValue (0x0a).value () != 0xcf ||
			sound.readValue (0x0e).value () != 0xa7)
			return (false);

		registers.initialize ();
		// A retained sound link receives reset writes; a retained raster link still reports lines 4/6.
		if (!defaults (registers) ||
			sound.readValue (0x0a).value () != 0 ||
			sound.readValue (0x0e).value () != 0)
		{
			std::cerr << "VIC-I reset did not restore presets through the connected sound wrapper\n";
			return (false);
		}

		const unsigned char expected [16] = { 0x0e, 0x27, 0x96, 0x2e,
			(unsigned char) (round == 0 ? 2 : 3), 0xf0, 0, 0,
			0xff, 0xff, 0, 0, 0, 0, 0, 0x1b };
		for (size_t i = 0; i < 16; ++i)
			if (registers.readRegister (i).value () != expected [i])
			{
				std::cerr << "VIC-I reset readback differs: round=" << round << " register=" << i << '\n';
				return (false);
			}
		for (unsigned int cycle = 0; cycle < 2 * 71; ++cycle)
			raster.moveCycles (1);
	}
	std::cout << "VIC-I constructor and two connected register resets passed\n";
	return (true);
}

// ---
bool VICIRegisterRegression::defaults (const COMMODORE::VICIRegisters& registers)
{
	return (registers.screenMemory ().value () == 0x1e00 &&
		registers.charDataMemory ().value () == 0x8000 &&
		registers.colourMemory ().value () == 0x9600 &&
		registers.offsetXScreen () == 14 && registers.offsetYScreen () == 39 &&
		registers.charsWidthScreen () == 22 && registers.charsHeightScreen () == 23 &&
		!registers.interlacedActive () && !registers.charsExpanded () &&
		!registers.inverseMode () && registers.auxiliarColor () == 0 &&
		registers.backgroundColor () == 1 && registers.borderColor () == 3);
}

// ---
bool VICIRegisterRegression::matrixAddresses ()
{
	MCHEmul::PhysicalStorage storage (2, MCHEmul::PhysicalStorage::Type::_RAM, 16);
	COMMODORE::VICIRegisters registers (&storage, 0, MCHEmul::Address (2, 0x9000), 16);

	// Literal CPU-view address pairs for each matrix nibble, with A9 clear/set.
	const unsigned short expected [16][2] = {
		{ 0x8000, 0x8200 }, { 0x8400, 0x8600 }, { 0x8800, 0x8a00 }, { 0x8c00, 0x8e00 },
		{ 0x9000, 0x9200 }, { 0x9400, 0x9600 }, { 0x9800, 0x9a00 }, { 0x9c00, 0x9e00 },
		{ 0x0000, 0x0200 }, { 0x0400, 0x0600 }, { 0x0800, 0x0a00 }, { 0x0c00, 0x0e00 },
		{ 0x1000, 0x1200 }, { 0x1400, 0x1600 }, { 0x1800, 0x1a00 }, { 0x1c00, 0x1e00 } };
	const unsigned short colors [2] = { 0x9400, 0x9600 };
	for (unsigned int nibble = 0; nibble < 16; ++nibble)
		for (unsigned int a9 = 0; a9 < 2; ++a9)
		{
			registers.setRegister (0x05, MCHEmul::UByte (nibble << 4));
			registers.setRegister (0x02, MCHEmul::UByte ((a9 << 7) | 22));
			if (registers.screenMemory ().value () != expected [nibble][a9] ||
				registers.charDataMemory ().value () != 0x8000 ||
				registers.colourMemory ().value () != colors [a9])
			{
				std::cerr << "Matrix base differs: nibble=" << nibble << " A9=" << a9 << '\n';
				return (false);
			}

			// The low $9005 nibble changes the character base without changing matrix or color bases.
			for (unsigned int character = 0; character < 16; ++character)
			{
				registers.setRegister (0x05, MCHEmul::UByte ((nibble << 4) | character));
				if (registers.screenMemory ().value () != expected [nibble][a9] ||
					registers.charDataMemory ().value () != expected [character][0] ||
					registers.colourMemory ().value () != colors [a9])
				{
					std::cerr << "Character selector changed an unrelated base\n";
					return (false);
				}
			}

			// Keep A9 while exercising all seven column bits; character selection remains $f.
			for (unsigned int columns = 0; columns < 128; ++columns)
			{
				registers.setRegister (0x02, MCHEmul::UByte ((a9 << 7) | columns));
				if (registers.screenMemory ().value () != expected [nibble][a9] ||
					registers.charDataMemory ().value () != 0x1c00 ||
					registers.colourMemory ().value () != colors [a9] ||
					registers.charsWidthScreen () != columns)
				{
					std::cerr << "Column count changed an address base\n";
					return (false);
				}
			}
		}

	std::cout << "32 matrix bases, 512 character-selector and 4096 column-independence cases passed\n";
	return (true);
}

// ---
bool VICIRegisterRegression::addressOffsets ()
{
	MCHEmul::PhysicalStorage storage (3, MCHEmul::PhysicalStorage::Type::_RAM, 16);
	COMMODORE::VICIRegisters registers (&storage, 0, MCHEmul::Address (2, 0x9000), 16);
	// CPU-view starts of the thirty-two successive 512-byte blocks of the VIC bus.
	const unsigned short blocks [32] = {
		0x8000, 0x8200, 0x8400, 0x8600, 0x8800, 0x8a00, 0x8c00, 0x8e00,
		0x9000, 0x9200, 0x9400, 0x9600, 0x9800, 0x9a00, 0x9c00, 0x9e00,
		0x0000, 0x0200, 0x0400, 0x0600, 0x0800, 0x0a00, 0x0c00, 0x0e00,
		0x1000, 0x1200, 0x1400, 0x1600, 0x1800, 0x1a00, 0x1c00, 0x1e00 };
	for (unsigned int base = 0; base < 32; ++base)
	{
		registers.setRegister (0x05, MCHEmul::UByte ((unsigned char) (((base / 2) << 4) | (base / 2))));
		registers.setRegister (0x02, MCHEmul::UByte ((unsigned char) ((base % 2) << 7)));
		if (registers.screenMemory (0) != registers.screenMemory () ||
			registers.charDataMemory (0) != registers.charDataMemory ())
			return (false);
		for (size_t offset = 0; offset < 0x4000; ++offset)
		{
			const unsigned int expected = blocks [(base + offset / 512) % 32] + offset % 512;
			if (registers.screenMemory (offset).value () != expected ||
				registers.screenMemory (offset + 0x4000).value () != expected)
				return (false);
			if (base % 2 == 0 &&
				(registers.charDataMemory (offset).value () != expected ||
				 registers.charDataMemory (offset + 0x4000).value () != expected))
				return (false);
		}
	}
	std::cout << "524288 matrix and 262144 glyph offsets, each with a second wrap, passed\n";
	return (true);
}

/** Access the real VIA timer without acknowledging its pending flag. */
class VICIVIAProbe final : public COMMODORE::VIA
{
	public:
	VICIVIAProbe ()
		: COMMODORE::VIA (0, VIC20::VIA2Registers::_VIA2_SUBSET, 0)
							{ }
	bool armTimer (MCHEmul::CPU* cpu);
	bool pending () const
							{ return (_T1.peekInterruptRequested ()); }
};

// ---
bool VICIVIAProbe::armTimer (MCHEmul::CPU* cpu)
{
	_T1.setInitialValue (1);
	_T1.loadCounter ();
	_T1.simulate (cpu);
	_T1.simulate (cpu);
	return (pending ());
}

/** Check the production machine map; run from a directory containing its bios folder. */
class VICIVideoViewRegression final
{
	public:
	static bool run ();
};

// ---
bool VICIVideoViewRegression::run ()
{
	VIC20::Memory memory (VIC20::Memory::Configuration::_NOEXPANDED);
	if (!memory.initialize ())
	{
		std::cerr << "Cannot initialize production VIC20 memory; run with VIC20 bios available\n";
		return (false);
	}
	MCHEmul::MemoryView* cpu = memory.view (VIC20::Memory::_CPU_VIEW);
	MCHEmul::MemoryView* video = memory.view (VIC20::Memory::_VICI_VIEW);
	if (cpu == nullptr || video == nullptr || cpu == video || video -> subsets ().size () != 9)
		return (false);
	for (int id : { VIC20::Memory::_VICICOLOR_SUBSET, VIC20::Memory::_VICIUNCONNECTEDLOW_SUBSET,
		VIC20::Memory::_VICIUNCONNECTEDIO_SUBSET, VIC20::Memory::_VICIUNCONNECTEDEXPANSION_SUBSET })
		if (cpu -> existsSubset (id))
			return (false);
	if (video -> existsSubset (COMMODORE::VICIRegisters::_VICREGS_SUBSET) ||
		video -> existsSubset (VIC20::VIA2Registers::_VIA2_SUBSET))
		return (false);

	// Every implemented RAM/ROM byte must refer to the same backing in both views.
	for (unsigned int address = 0; address <= 0x8fff; ++address)
		if (address < 0x0400 || (address >= 0x1000 && address < 0x2000) || address >= 0x8000)
		{
			if (address < 0x2000)
				cpu -> set (MCHEmul::Address (2, address), MCHEmul::UByte ((unsigned char) (address ^ 0x5a)));
			const MCHEmul::UByte expected = cpu -> value (MCHEmul::Address (2, address));
			if (video -> value (MCHEmul::Address (2, address)) != expected)
				return (false);
		}
	for (unsigned int configuration = 0; configuration < 10; ++configuration)
	{
		memory.setConfiguration ((VIC20::Memory::Configuration) (configuration));
		for (unsigned int address = 0x0400; address < 0xa000; ++address)
			if (address < 0x1000 || (address >= 0x9000 && address < 0x9400) || address >= 0x9800)
				if (video -> value (MCHEmul::Address (2, address)).value () != 0xff)
					return (false);
		for (unsigned int offset = 0; offset < 1024; ++offset)
		{
			memory.physicalStorage (VIC20::Memory::_RAM) -> set
				(0x9400 + offset, MCHEmul::UByte ((unsigned char) (offset ^ 0xa5)));
			if (video -> value (MCHEmul::Address (2, 0x9400 + offset)).value () !=
				(0xf0 | ((offset ^ 0xa5) & 0x0f)))
				return (false);
		}
	}
	// All video-only aliases must ignore initialization and forced writes.
	const int aliases [4] = { VIC20::Memory::_VICICOLOR_SUBSET,
		VIC20::Memory::_VICIUNCONNECTEDLOW_SUBSET, VIC20::Memory::_VICIUNCONNECTEDIO_SUBSET,
		VIC20::Memory::_VICIUNCONNECTEDEXPANSION_SUBSET };
	const unsigned int aliasAddresses [4] = { 0x9400, 0x0400, 0x9000, 0x9800 };
	for (size_t i = 0; i < 4; ++i)
	{
		memory.physicalStorage (VIC20::Memory::_RAM) -> set (aliasAddresses [i], MCHEmul::UByte (0x6c));
		memory.subset (aliases [i]) -> initialize ();
		video -> set (MCHEmul::Address (2, aliasAddresses [i]), MCHEmul::UByte (0), true);
		if (memory.physicalStorage (VIC20::Memory::_RAM) -> value (aliasAddresses [i]).value () != 0x6c)
			return (false);
	}
	std::cout << "Production maps, ten configurations and alias initialization passed\n" << std::flush;

	memory.setCPUView ();
	VICIGraphicsProbe vic (true);
	VICIVIAProbe via;
	F6500::C6502 processor (0);
	processor.setMemoryRef (&memory);
	vic.setMemoryRef (&memory);
	via.setMemoryRef (&memory);
	if (!via.initialize () || !vic.initialize () || !via.armTimer (&processor))
		return (false);
	if (!vic.isolatedVIARead () || !via.pending ())
		return (false);
	COMMODORE::VICIRegisters* registers = dynamic_cast <COMMODORE::VICIRegisters*>
		(memory.subset (COMMODORE::VICIRegisters::_VICREGS_SUBSET));
	const unsigned int colorStarts [2] = { 0, 512 };
	for (unsigned int bank = 0; bank < 2; ++bank)
	{
		registers -> setRegister (2, MCHEmul::UByte ((unsigned char) (0x16 | (bank << 7))));
		for (size_t offset = 0; offset < 2048; ++offset)
			if (registers -> colourMemory (offset).value () != 0x9400 + ((colorStarts [bank] + offset) % 1024))
				return (false);
	}
	registers -> setRegister (2, MCHEmul::UByte (0x96));
	// Inspect every character, so the monitor path also crosses the VIA register range.
	const MCHEmul::UBytes matrix = vic.screenMemorySnapShot (nullptr);
	const MCHEmul::UBytes colors = vic.colorMemorySnapShot (nullptr);
	const MCHEmul::Strings characters = vic.charsDrawSnapshot (nullptr);
	if (matrix.size () != 506 || colors.size () != 506 || characters.size () != 257)
		return (false);
	for (size_t i = 0; i < 506; ++i)
		if (matrix [i] != memory.physicalStorage (VIC20::Memory::_RAM) -> value (0x1e00 + i) ||
			colors [i].value () != ((unsigned int) ((512 + i) ^ 0xa5) & 0x0f))
			return (false);

	if (!via.pending ())
		return (false);
	memory.setActiveView (VIC20::Memory::_VICI_VIEW);
	if (!vic.isolatedVIARead () || !via.pending ())
		return (false);
	memory.setCPUView ();
	cpu -> value (MCHEmul::Address (2, 0x9124));
	if (via.pending ())
		return (false);
	VICIGraphicsProbe disconnected (true, 99);
	disconnected.setMemoryRef (&memory);
	if (disconnected.initialize ())
		return (false);
	std::cout << "Production video view: shared RAM/ROM, ten configurations, color, VIA IRQ and snapshots passed\n";
	return (true);
}

/** CPU accesses must reach every physical color nibble in every memory configuration. */
class VICICPUColorRegression final
{
	public:
	static bool run ();
};

// ---
bool VICICPUColorRegression::run ()
{
	VIC20::Memory memory (VIC20::Memory::Configuration::_NOEXPANDED);
	if (!memory.initialize ())
	{
		std::cerr << "Color regression requires the VIC20 bios directory\n";
		return (false);
	}
	MCHEmul::MemoryView* cpu = memory.view (VIC20::Memory::_CPU_VIEW);
	MCHEmul::MemoryView* video = memory.view (VIC20::Memory::_VICI_VIEW);
	// Seed the inactive half explicitly, so a discarded write cannot accidentally pass.
	memory.physicalStorage (VIC20::Memory::_RAM) -> set (0x9400, MCHEmul::UByte (0));
	cpu -> set (MCHEmul::Address (2, 0x9400), MCHEmul::UByte (0xa5));
	if (memory.physicalStorage (VIC20::Memory::_RAM) -> value (0x9400).value () != 5)
	{
		std::cerr << "CPU $9400 write failed: expected physical nibble 5, got "
			<< (unsigned int) (memory.physicalStorage (VIC20::Memory::_RAM) -> value (0x9400).value ()) << '\n';
		return (false);
	}
	const unsigned int boundaries [4] = { 0x9400, 0x95ff, 0x9600, 0x97ff };
	for (unsigned int configuration = 0; configuration < 10; ++configuration)
	{
		memory.setConfiguration ((VIC20::Memory::Configuration) (configuration));
		for (unsigned int offset = 0; offset < 1024; ++offset)
		{
			const MCHEmul::Address address (2, 0x9400 + offset);
			cpu -> set (address, MCHEmul::UByte ((unsigned char) (offset ^ 0xa5)));
			if (memory.physicalStorage (VIC20::Memory::_RAM) -> value (0x9400 + offset).value () !=
				((offset ^ 0xa5) & 15) || cpu -> value (address).value () != (0xf0 | ((offset ^ 0xa5) & 15)) ||
				video -> value (address).value () != (0xf0 | ((offset ^ 0xa5) & 15)))
			{
				std::cerr << "CPU/video color mismatch in configuration " << configuration << " offset " << offset << '\n';
				return (false);
			}
		}
		for (unsigned int address : boundaries)
			for (unsigned int value = 0; value < 256; ++value)
			{
				cpu -> set (MCHEmul::Address (2, address), MCHEmul::UByte ((unsigned char) (value)));
				if (memory.physicalStorage (VIC20::Memory::_RAM) -> value (address).value () != (value % 16) ||
					cpu -> value (MCHEmul::Address (2, address)).value () != (0xf0 | (value % 16)))
					return (false);
			}
	}
	// Establish a full pattern, then exercise all enum and all byte-mask configurations without rewriting it.
	for (unsigned int offset = 0; offset < 1024; ++offset)
		cpu -> set (MCHEmul::Address (2, 0x9400 + offset), MCHEmul::UByte ((unsigned char) (offset ^ 0x5a)));
	for (unsigned int configuration = 0; configuration < 266; ++configuration)
	{
		if (configuration < 10)
			memory.setConfiguration ((VIC20::Memory::Configuration) (configuration));
		else
			memory.setConfiguration ((unsigned char) (configuration - 10));
		for (unsigned int offset = 0; offset < 1024; ++offset)
			if ((cpu -> value (MCHEmul::Address (2, 0x9400 + offset)).value () & 15) != ((offset ^ 0x5a) % 16) ||
				(memory.physicalStorage (VIC20::Memory::_RAM) -> value (0x9400 + offset).value ()) != ((offset ^ 0x5a) % 16))
				return (false);
	}
	// The inherited CPU initialization writes four-bit cells; the video alias must not reset them.
	for (int id : { VIC20::Memory::_SCREENC1RAM_SUBSET, VIC20::Memory::_SCREENC2RAM_SUBSET })
		memory.subset (id) -> initialize ();
	for (unsigned int offset = 0; offset < 1024; ++offset)
		if (memory.physicalStorage (VIC20::Memory::_RAM) -> value (0x9400 + offset).value () != 0)
			return (false);
	cpu -> set (MCHEmul::Address (2, 0x9400), MCHEmul::UByte (0xab), true);
	memory.subset (VIC20::Memory::_VICICOLOR_SUBSET) -> initialize ();
	if (memory.physicalStorage (VIC20::Memory::_RAM) -> value (0x9400).value () != 11)
		return (false);
	std::cout << "CPU color: 10240 cells, 10240 boundary writes, 266 configuration transitions and initialization passed\n";
	return (true);
}

int main (int argc, char** argv)
{
	if (argc == 2 && std::string (argv [1]) == "--timed")
		return (VICITimedWritesRegression::run () ? 0 : 1);
	if (argc == 2 && std::string (argv [1]) == "--color")
		return (VICICPUColorRegression::run () ? 0 : 1);
	return ((VICITimedWritesRegression::run () && VICICPUColorRegression::run () && VICIVideoViewRegression::run () && VICIRegisterRegression::run () &&
		VICIGraphicsRegression::run (true) && VICIGraphicsRegression::run (false)) ? 0 : 1);
}
