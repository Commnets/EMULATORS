#include <ZX81/ULA.hpp>
#include <ZX81/ULARegisters.hpp>
#include <ZX81/ZX81.hpp>
#include <ZX81/OSIO.hpp>
#include <FZ80/NMIInterrupt.hpp>

// ---
const MCHEmul::RasterData ZX81::ULA_PAL::_VRASTERDATA
	(0, 6 /** 6 VSYNC. */, 62 /** 56 top border. */, 253 /** 192 draw. */, 309 /** 56 bottom. */, 309, 310, 0, 0);
/** In terms of ULA cycles, there would be double:
	CPU cycles = (0, 45, 75, 202, 232, 232, 233, 0, 0); */
const MCHEmul::RasterData ZX81::ULA_PAL::_HRASTERDATA
	(0, 90 /** Including 20 (*2) HSYNC + part INT routine execution */, 
		150 /** 30 (*2) left border. */, 405 /** 128 (32 chars * 4 ULA cycles * 2) draw. */, 
		465 /** 30 right border. */, 465 /** When the INT is issued. */, 466, 0, 0);
// NTSC is explained in similar terms than PAL...
const MCHEmul::RasterData ZX81::ULA_NTSC::_VRASTERDATA (0, 6, 38, 229, 261, 261, 262, 0, 0);
const MCHEmul::RasterData ZX81::ULA_NTSC::_HRASTERDATA (0, 90, 150, 405, 465, 465, 466, 0, 0);

// ---
ZX81::ULA::ULA (const MCHEmul::RasterData& vd, const MCHEmul::RasterData& hd, 
		int vV, const MCHEmul::Attributes& attrs)
	: MCHEmul::GraphicalChip (_ID, 
		{ { "Name", "ULA" },
		  { "Code", "2C184E" },
		  { "Manufacturer", "Ferranti" },
		  { "Year", "1980" } }),
	  _ULARegisters (new ZX81::ULARegisters),
	  _ULAView (vV),
	  _raster (vd, hd, 1 /** The step is 1 pixel. */),
	  _lastCPUCycles (0),
	  _format (nullptr),
	  _firstVBlankEntered (false),
	  _INTActive (false), _NMIActive (false), _HALTActive (false)
{
	setClassName ("ULA");

	_format = SDL_AllocFormat (SDL_PIXELFORMAT_ARGB8888);
}

// ---
ZX81::ULA::~ULA ()
{
	SDL_FreeFormat (_format);
}

// ---
bool ZX81::ULA::initialize ()
{
	assert (MCHEmul::GraphicalChip::memoryRef () != nullptr);

	if (!MCHEmul::GraphicalChip::initialize ())
		return (false);

	_raster.initialize ();

	_ULARegisters -> initialize ();

	_lastCPUCycles = 0;

	_firstVBlankEntered = false;

	return (true);
}

// ---
bool ZX81::ULA::simulate (MCHEmul::CPU* cpu)
{
	// First time?
	if (_lastCPUCycles == 0)
	{ 
		_lastCPUCycles = cpu -> clockCycles (); // Nothing to do...

		return (true);
	}

	// Simulate the visulization...
	for (unsigned int i = 
			((cpu -> clockCycles  () - _lastCPUCycles) << 1 /** ULA cycles = 2 * CPU Cycles. */); 
			i > 0; i--)
	{
		if (MCHEmul::GraphicalChip::deepDebugActive ())
		{
			*MCHEmul::GraphicalChip::_deepDebugFile
				// Where
				<< "ULA\t" 
				// When
				<< std::to_string (cpu -> clockCycles () - (i >> 1)) << "\t" // CPU clock cycles at that point
				// What
				<< "Info cycle\t\t"
				// Data
				<< "Raster:["
				<< std::to_string (_raster.currentColumnAtBase0 ()) << "," 
				<< std::to_string (_raster.currentLineAtBase0 ()) 
				<< "], Internal:["
				<< (_ULARegisters -> NMIGenerator () ? "NMI_ON" : "NMI_OFF") << ","
				<< (_ULARegisters -> syncOutputWhite () ? "WHITE" : "BLACK") << ","
				<< "LNCTRL:" << std::to_string (_ULARegisters -> LINECNTRL ()) << "]\n";

			// Keep track of different situations to be drawn later if needed...
			// When a INT interrupt is in execution...
			if (!_INTActive.peekValue () && 
				cpu -> programCounter ().internalRepresentation () == 0x038)
				_INTActive = true;
			// When a NMI interrupt is in execution...
			if (!_NMIActive.peekValue () &&
				cpu -> programCounter ().internalRepresentation () == 0x066)
				_NMIActive = true;
			// When the HALT situation is active...
			if (!_HALTActive.peekValue () &&
				cpu -> lastInstruction () -> code () == 0x076)
				_HALTActive = true;
		}

		// Read the graphics and draw the visible zone, 
		// if it is the case...
		if (_raster.isInVisibleZone ())
			readGraphicsAndDrawVisibleZone (cpu);

		// Moves the internal cycles counter 1...(from 0 to 415)
		if (_raster.hData ().add (1) || // ...and if reaches the end...
			_ULARegisters -> INTack ()) // ...or a INT has been launched externally...
		{
			// the HSYNC happens...
			_raster.vData ().add (1); _raster.hData ().reset ();
			// ...and also the line of control is incremented...
			_ULARegisters -> incLINECTRL ();
			
			// An NMI interrupt could be launched, if the ULA generator was on...
			if (_ULARegisters -> NMIGenerator ()) // The generator has to be active! (@see PortManager)
				cpu -> requestInterrupt (FZ80::NMIInterrupt::_ID, _lastCPUCycles + i, this, 1 /** HSYNC reason */);
			// This shouldn't happen if INTack was on in a well programmed ZX80 series...but
		}

		// Notice that the VSYNC doesn't happen here as it is the port output the one launching that...
		// (@see ZX81::PortManager class)

		// When the raster enters the non visible part of the screen,
		// a notification is sent (to the Screen class usually) 
		// just to draw the screen...
		if (_raster.isInFirstVBlankZone ())
		{
			if (!_firstVBlankEntered)
			{
				_firstVBlankEntered = true;

				MCHEmul::GraphicalChip::notify (MCHEmul::Event (_GRAPHICSREADY));
			}
		}
		else
			_firstVBlankEntered = false;
	}

	_lastCPUCycles = cpu -> clockCycles ();

	return (true);
}

// ---
MCHEmul::InfoStructure ZX81::ULA::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::GraphicalChip::getInfoStructure ());

	result.remove ("Memory"); // This info is not neccesary...
	result.add ("ULARegisters",	std::move (_ULARegisters -> getInfoStructure ()));
	result.add ("Raster",		std::move (_raster.getInfoStructure ()));

	return (result);
}

// ---
void ZX81::ULA::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	switch (evnt.id ())
	{
		case MCHEmul::InputOSSystem::_KEYBOARDKEYPRESSED:
			{
				const ZX81::InputOSSystem::Keystrokes& ks = ((ZX81::InputOSSystem*) n) -> keystrokesFor
					(std::static_pointer_cast <MCHEmul::InputOSSystem::KeyboardEvent> (evnt.data ()) -> _key);
				if (!ks.empty ()) // The key has to be defined...
					for (const auto& j : ks)
						_ULARegisters -> setKeyboardStatus (j.first, j.second, true);
			}

			break;

		case MCHEmul::InputOSSystem::_KEYBOARDKEYRELEASED:
			{
				const ZX81::InputOSSystem::Keystrokes& ks = ((ZX81::InputOSSystem*) n) -> keystrokesFor
				(std::static_pointer_cast <MCHEmul::InputOSSystem::KeyboardEvent> (evnt.data ()) -> _key);
				if (!ks.empty ()) // The key has to be defined...
					for (const auto& j : ks)
						_ULARegisters -> setKeyboardStatus (j.first, j.second, false);
			}

			break;

		// The rest of the events are not taken here into account!

		default:
			break;
	}
}

// ---
MCHEmul::ScreenMemory* ZX81::ULA::createScreenMemory ()
{
	unsigned int* cP = new unsigned int [16];
	// The colors are partially transparents to allow the blending...
	cP [0]  = SDL_MapRGBA (_format, 0x00, 0x00, 0x00, 0xe0); // Black
	cP [1]  = SDL_MapRGBA (_format, 0xff, 0xff, 0xff, 0xe0); // White
	// These other colors doesn't exist in ZX81, but are used to draw borders, bebug information, etc...
	cP [2]  = SDL_MapRGBA (_format, 0x92, 0x4a, 0x40, 0xe0); // Red
	cP [3]  = SDL_MapRGBA (_format, 0x84, 0xc5, 0xcc, 0xe0); // Cyan
	cP [4]  = SDL_MapRGBA (_format, 0x93, 0x51, 0xb6, 0xe0); // Violet
	cP [5]  = SDL_MapRGBA (_format, 0x72, 0xb1, 0x4b, 0xe0); // Green
	cP [6]  = SDL_MapRGBA (_format, 0x48, 0x3a, 0xaa, 0xe0); // Blue
	cP [7]  = SDL_MapRGBA (_format, 0xd5, 0xdf, 0x7c, 0xe0); // Yellow
	cP [8]  = SDL_MapRGBA (_format, 0x99, 0x69, 0x2d, 0xe0); // Brown
	cP [9]  = SDL_MapRGBA (_format, 0x67, 0x52, 0x00, 0xe0); // Light Red
	cP [10] = SDL_MapRGBA (_format, 0xc1, 0x81, 0x78, 0xe0); // Orange
	cP [11] = SDL_MapRGBA (_format, 0x60, 0x60, 0x60, 0xe0); // Dark Grey
	cP [12] = SDL_MapRGBA (_format, 0x8a, 0x8a, 0x8a, 0xe0); // Medium Grey
	cP [13] = SDL_MapRGBA (_format, 0xb3, 0xec, 0x91, 0xe0); // Light Green
	cP [14] = SDL_MapRGBA (_format, 0x86, 0x7a, 0xde, 0xe0); // Light Blue
	cP [15] = SDL_MapRGBA (_format, 0xb3, 0xb3, 0xb3, 0xe0); // Light Grey

	return (new MCHEmul::ScreenMemory (numberColumns (), numberRows (), cP));
}

// --
void ZX81::ULA::readGraphicsAndDrawVisibleZone (MCHEmul::CPU* cpu)
{
	_ULARegisters -> setInVSync (false); 
	// Finally out of the vsync cycle!...
	// Hopefully when the code reaches this position, 
	// an OUT FF,A instruction will have been executed already and the VSYNC has finished!

	unsigned short x = 0, y = 0;
	_raster.currentVisiblePosition (x, y);

	// Draws the background first...
	// ...that is always at color 1 (white)....
	_screenMemory -> setPixel (x, y, 1);

	if (MCHEmul::GraphicalChip::deepDebugActive ())
	{
		if (_HALTActive) _screenMemory -> setPixel (x, y, 12);
		if (_INTActive) _screenMemory -> setPixel (x, y, 2);
		if (_NMIActive) _screenMemory -> setPixel (x, y, 7);
	}

	bool px = true;
	// When x is a multiple of 8...
	if (x % 8 == 0)
	{
		// ...reads the char code to draw...
		// ...and how to draw that char code...
		// Remember that the way that the ULA sees the memory is different than the way CPU does...
		memoryRef () -> setActiveView (_ULAView);
		MCHEmul::UByte chrCode = 
			memoryRef () -> value (cpu -> programCounter ().asAddress ());
		_ULARegisters -> setReverseVideo (chrCode.bit (7));
		_ULARegisters -> setSHIFTRegister (chrCode.bit (6) 
			? 0x00 // It is an special c0de and has to be treated as an space from the graphical perspective!
			: memoryRef () -> value (MCHEmul::Address (2, 
				(unsigned int (static_cast <FZ80::CZ80*> (cpu) -> iRegister ().values ()[0].value ()) << 8) |
				(unsigned int ((chrCode.value () & 0b00111111) << 3)) |
				(unsigned int) _ULARegisters -> LINECNTRL ())));
		memoryRef () -> setCPUView ();
	}

	// Shift the data to draw...
	px = _ULARegisters -> shiftOutData ();

	// and then draws the character, if it is allowed!
	if (_raster.isInDisplayZone () &&
		_ULARegisters -> syncOutputWhite () &&
		(_ULARegisters -> reverseVideo () ^ px)) // normal video and pixel on?, or inverse video and pixels off?
			_screenMemory -> setPixel (x, y, 0); // ...then put it in black...
}

// ---
ZX81::ULA_PAL::ULA_PAL (int vV)
	: ZX81::ULA (_VRASTERDATA, _HRASTERDATA, vV,
		 { { "Name", "ULA" },
		   { "Code", "2C184E (PAL)" },
		   { "Manufacturer", "Ferranti"},
		   { "Year", "1980" } })
{
	_ULARegisters -> setNTSC (false);
}

// ---
ZX81::ULA_NTSC::ULA_NTSC (int vV)
	: ZX81::ULA (_VRASTERDATA, _HRASTERDATA, vV,
		 { { "Name", "ULA" },
		   { "Code", "2C184E (PAL)" },
		   { "Manufacturer", "Ferranti"},
		   { "Year", "1980" } })
{
	_ULARegisters -> setNTSC (true);
}
