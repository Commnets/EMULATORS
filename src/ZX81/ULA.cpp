#include <ZX81/ULA.hpp>
#include <ZX81/ULARegisters.hpp>
#include <ZX81/ZX81.hpp>
#include <ZX81/OSIO.hpp>
#include <FZ80/NMIInterrupt.hpp>

// ---
const MCHEmul::RasterData ZX81::ULA_PAL::_VRASTERDATA
	(0, 6 /** 6 VSYNC. */, 62 /** 56 top border. */, 253 /** 192 draw. */, 309 /** 56 bottom. */, 309, 310, 0, 0);
/** In terms of ULA cycles, there would be double:
	CPU cycles = (0, 15, 47, 174, 206, 206, 207, 0, 0); */
const MCHEmul::RasterData ZX81::ULA_PAL::_HRASTERDATA
	(0, 30 /** HSYNC. */, 94 /** 32 left border. */, 349 /** 128 draw. */, 413 /** 32 right border. */, 413, 414, 0, 0);
const MCHEmul::RasterData ZX81::ULA_NTSC::_VRASTERDATA
	(0, 6 /** 6 VSYNC. */, 38 /** 32 top border. */, 229 /** 192 draw. */, 261 /** 32 bottom. */, 261, 262, 0, 0);
const MCHEmul::RasterData ZX81::ULA_NTSC::_HRASTERDATA
	(0, 30 /** HSYNC. */, 94 /** 32 left border. */, 349 /** 128 draw. */, 413 /** 32 right border. */, 413, 414, 0, 0);

// ---
ZX81::ULA::ULA (const MCHEmul::RasterData& vd, const MCHEmul::RasterData& hd, 
		int vV, unsigned short cRL, const MCHEmul::Attributes& attrs)
	: MCHEmul::GraphicalChip (_ID, 
		{ { "Name", "ULA" },
		  { "Code", "2C184E" },
		  { "Manufacturer", "Ferranti" },
		  { "Year", "1980" } }),
	  _ULARegisters (new ZX81::ULARegisters),
	  _ULAView (vV),
	  _cyclesPerRasterLine (cRL),
	  _cycleInRasterLine (0),
	  _lastVBlankEntered (false),
	  _raster (vd, hd, 1 /** The step is 1 pixel. */),
	  _lastCPUCycles (0),
	  _format (nullptr)
{
	assert (_cyclesPerRasterLine >= 65);

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
	
	_cycleInRasterLine = 1;

	_lastVBlankEntered = false;

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

	// If the video signal is clamped, there is no need to continue...
	if (_ULARegisters -> videoSignalClamped ())
	{
		_lastCPUCycles = cpu -> clockCycles ();

		return (true);
	}

	// Simulate the visulization...
	for (unsigned int i = ((cpu -> clockCycles  () - _lastCPUCycles) * 2 /** ULA cycles = 2 * CPU Cycles. */); 
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
				<< std::to_string (_raster.currentLineAtBase0 ()) << "]\n";
		}

		// Read the graphics and draw the visible zone, 
		// if it is the case...
		readGraphicsAndDrawVisibleZone (cpu);
		// ...and then treat the current cycle...
		treatRasterCycle ();

		// ...and also moves 8 pixels right in the raster line and jump to other line is possible...
		if (_raster.moveCycles (1))
		{ 
			_cycleInRasterLine = 1;

			// Increment the line of control...
			_ULARegisters -> incLINECTRL ();

			// ...this is critical for the screen to work
			// a NMI interruption is generated at the end of every horizontal line...
			// What to do is managed by the program!
			if (_ULARegisters -> NMIGenerator ()) // The generator has to be active! (@see PortManager)
				cpu -> requestInterrupt (FZ80::NMIInterrupt::_ID, _lastCPUCycles + i, this, 1 /** HSYNC reason */);
		}

		// When the raster enters the non visible part of the screen,
		// a notification is sent (to the Screen class usually) 
		// just to draw the screen...
		if (_raster.isInFirstVBlankZone ())
		{
			if (!_lastVBlankEntered)
			{
				_lastVBlankEntered = true;

				MCHEmul::GraphicalChip::notify (MCHEmul::Event (_GRAPHICSREADY));
			}
		}
		else
			_lastVBlankEntered = false;
	}

	_ULARegisters -> setCurrentRasterLine (_raster.currentLine ()); 

	_lastCPUCycles = cpu -> clockCycles ();

	return (true);
}

// ---
MCHEmul::InfoStructure ZX81::ULA::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::GraphicalChip::getInfoStructure ());

	result.remove ("Memory"); // This info is not neccesary...
	result.add ("Registers",	std::move (_ULARegisters -> getInfoStructure ()));
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
	unsigned int* cP = new unsigned int [4];
	// The colors are partially transparents to allow the blending...
	cP [0]  = SDL_MapRGBA (_format, 0x00, 0x00, 0x00, 0xe0); // Black
	cP [1]  = SDL_MapRGBA (_format, 0xff, 0xff, 0xff, 0xe0); // White
	// Tese other two colors doesn't exist in ZX81, but are used to draw borders...
	cP [2]  = SDL_MapRGBA (_format, 0x92, 0x4a, 0x40, 0xe0); // Red
	cP [3]  = SDL_MapRGBA (_format, 0x72, 0xb1, 0x4b, 0xe0); // Green

	return (new MCHEmul::ScreenMemory (numberColumns (), numberRows (), cP));
}

// ---
void ZX81::ULA::treatRasterCycle ()
{
	// TODO

	_cycleInRasterLine++;
}

// --
void ZX81::ULA::readGraphicsAndDrawVisibleZone (MCHEmul::CPU* cpu)
{
	// Read the char code to draw...
	// ...and how to draw that char code...
	// Remember that the way that the ULA sees the memory is different than the way CPU does...
	memoryRef () -> setActiveView (_ULAView);
	MCHEmul::UByte chrCode = 
		memoryRef () -> value (cpu -> lastInstruction () -> lastINOUTAddress ());
	MCHEmul::UByte chrData = memoryRef () -> value 
		(MCHEmul::Address (2, 
			(unsigned int (static_cast <FZ80::CZ80*> (cpu) -> iRegister ().values ()[0].value ()) << 9) |
			(unsigned int ((chrCode & 0b00111111).value ()) << 3) |
			(unsigned int) _ULARegisters -> LINECNTRL ()));
	memoryRef () -> setCPUView ();

	// Only draw when it is in visible zone...
	if (_raster.isInVisibleZone ())
	{
		unsigned short x, y;
		_raster.currentVisiblePosition (x, y);

		// Draws the background first...
		// ...that is always at color 1 (white)....
		_screenMemory -> setPixel (x, y, 1);
		// Then draws the character...
		if (_raster.isInDisplayZone ())
		{
			if (!chrCode.bit (6)) // It has to be NOP!
			{
				bool cp = chrData.bit (7 - (x % 8));
				if ((!chrCode.bit (7) && cp) || // normal video and pixel on?
					(chrCode.bit (7) && !cp)) // or inverse video and pixels off?
					_screenMemory -> setPixel (x,y, 0); // ...then put it in black...
			}
		}
	}
}

// ---
ZX81::ULA_PAL::ULA_PAL (int vV)
	: ZX81::ULA (_VRASTERDATA, _HRASTERDATA, vV, ZX81::ULA_PAL::_CYCLESPERRASTERLINE,
		 { { "Name", "ULA" },
		   { "Code", "2C184E (PAL)" },
		   { "Manufacturer", "Ferranti"},
		   { "Year", "1980" } })
{
	_ULARegisters -> setNTSC (false);
}

// ---
ZX81::ULA_NTSC::ULA_NTSC (int vV)
	: ZX81::ULA (_VRASTERDATA, _HRASTERDATA, vV, ZX81::ULA_PAL::_CYCLESPERRASTERLINE,
		 { { "Name", "ULA" },
		   { "Code", "2C184E (PAL)" },
		   { "Manufacturer", "Ferranti"},
		   { "Year", "1980" } })
{
	_ULARegisters -> setNTSC (true);
}
