#include <ZX81/ULA.hpp>
#include <ZX81/ULARegisters.hpp>
#include <ZX81/ZX81.hpp>
#include <ZX81/OSIO.hpp>
#include <FZ80/CZ80.hpp>
#include <FZ80/NMIInterrupt.hpp>

// ---
/** Standard PAL requires 64us to draw a line. \n
	ULA runs in parallel with CPU and runs at double speed than the CPU. \n
	CPU runs at 3,25Mhz and ULA runs at 6,50Mhz. \n
	CPU need 207 cycles (CPU type) to draw a line, so ULA needs 414 cycles (ULA type). \n
	The structure of a line is the following: \n
	-----------------------------------------------------------------------------\n
	|Zone				|Time aprox	|ULA Cicles (at 6.5 MHz, double than CPU)	|\n
	|------------------	|-----------|-------------------------------------------|\n
	|**HSYNC**			|\~4.7 µs	|4.7 µs × 6.5 MHz ≈ **30.6 → 31 cycles**		|\n
	|**Back Porch**		|\~5.7 µs	|5.7 µs × 6.5 MHz ≈ **37.1 → 36 cycles**		|\n
	|**Video Active**	|\~52 µs		|52 µs × 6.5 MHz ≈ **338 cycles**				|\n
	|**Front Porch**	|\~1.5 µs	|1.5 µs × 6.5 MHz ≈ **9.75 → 9 cycles**		|\n
	|------------------	|-----------|-------------------------------------------|\n
	|**TOTAL**			|\~63.9 µs	|**414 cycles** (rounded)					|\n
	-----------------------------------------------------------------------------\n 
	Standard PAL requires 312 lines to draw a frame. \n
	The structure of the frame is the following: \n
	-----------------------------------------------------------------\n 
	|Zone							|Lines aprox.	|Time aprox.	|\n
	|-------------------------------|--------------	|-------------	|\n
	|**Vertical sync pulse** (VSYNC)|\~5 lines		|\~320 µs		|\n
	|**Vertical back porch**		|\~25 lines		|\~1.6 ms		|\n
	|**Zona de imagen activa**		|\~284 lines	|\~18.2 ms		|\n
	|**Vertical front porch**		|\~3 lines		|\~192 µs		|\n
	|-------------------------------|--------------	|-------------	|\n
	|**TOTAL**						|**312 líneas**	|**20 ms**		|\n
	-----------------------------------------------------------------\n 
	\n
	Where the border and the drawing zone that is seen is the phisical screen is actually, 
	cannot be defined in advance!, like happens in e.g. VICII (C64). \n
	It is really the software and the CPU the ones building up the screen: \n
	---
	When the address bus connected to ULA have bit 14 set (1) and bit 15 reset (0),
	the ULA will know that the CPU is accesing the video memory. \n
	Then uses the info at the data bus to load the shift register and start to shift it left (see below). \n
	Just after loading that info the ULA put back "0" in the data bus (unless the bit 6 of the data is on)
	to "confuse" the CPU that will execute a NOP operation. \n
	Take a look to ZX81::MemoryVideoCode class. \n
	If the bit 6 is set, the ULA doesn't change the value in the data bus (usually equivalent to HALT), \n
	The value read is used (together with the LINECNTRL register) to build up 
	the address where to read the value to load in the shift register. \n
	Then every clock (ULAwise) the register shifts one position left and draws a pixel according to the value shifted. \n
	When the shift ends, there won't be more pixels to draw until another byte were loaded. \n
	So, the drawing zone will start where the CPU starts to "run" things in the video memory.
	---
	In which line starts the drawing is not directly controlled actually! \n
	What it is really controlled is when the VSYNC starts only. \n
	It starts when the port FE is read. This is the zone of the ROM called VSYC Routine. \n
	When that happens a signal is sent to ULA to reset the vertical raster,
	LINECTRL is set to 0 and "locked" temporally. \n
	That routine VSYNC finishes, a OUT port FD is sent to activate (in ULA) the NMI interrupts. \n
	This port also indicates that the VSYNC zone has finishes and LINENCTRL is unlocked to count. \n
	----
	In MCHEmul, raster is thought to be "linked" to the different zones of a screen (border, drawing, etc.),
	And also used to draw the window that content the whole simulation. \n
	The only important value is where the visible parts starts because 
	it has to be aligned with the INT routine that draws the screen. \n
	142 "cycles" is enough to left a 8 width pixels border. \n
	After all being said, the only behaviour that could be use is the second one. */
/** 32 lines border up/down/left/right by default in both PAL & NTSC. */
const MCHEmul::RasterData ZX81::ULA_PAL::_VRASTERDATA
	(0, 30 /** +30 starts visible. */, 30 /** = starts screen. */, 285 /** +32+192+32 (24 char lines * 8) end screen. */,
	 285 /** = end visible part. */, 311 /** +26 retrace. */, 311 /** = end. */, 312 /* total */, 0, 0);
const MCHEmul::RasterData ZX81::ULA_PAL::_HRASTERDATA
	(0, 125 /** +128 starts visible. */, 125 /** = starts screen, */, 413 /** +19+256+14 (32 chars * 8) end screen. */,
	 413 /** = end visible part. */, 413 /** = retrace. */, 413 /** = end. */, 414 /** total. */, 0, 0);
const MCHEmul::RasterData ZX81::ULA_NTSC::_VRASTERDATA (0, 15, 15, 246, 246, 261, 261, 262, 0, 0);
const MCHEmul::RasterData ZX81::ULA_NTSC::_HRASTERDATA (0, 125, 125, 413, 413, 413, 413, 414, 0, 0);

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
	  _showEvents (false),
	  _lastCPUCycles (0),
	  _format (nullptr),
	  _HALTBefore (false),
	  _INTActive (false), _NMIActive (false), _HALTActive (false),
	  _writePort (false), _readPortFE (false),
	  _NMIGeneratorOn (false), _NMIGeneratorOff (false)
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

	// Notice that all attributes related with drawing signal are not inialized
	// to avoid that when restart a new ulaevents instruction must be commanded!

	_ULARegisters -> initialize ();

	_lastCPUCycles = 0;

	// Events null...
	_HALTBefore = false;
	_INTActive = false; _NMIActive = false; _HALTActive = false;
	_writePort = false; _readPortFE = false;
	_NMIGeneratorOn = false; _NMIGeneratorOff = false;
	
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
	for (unsigned int i = 0;
			i < ((cpu -> clockCycles  () - _lastCPUCycles) << 1 /** ULA cycles = 2 * CPU Cycles. */); 
			i++)
	{
		_IFDEBUG debugULACycle (cpu, i);

		// Draws the screen to the display at the beginning of everything...
		if (_raster.vData ().currentPositionAtBase0 () == 0 &&
			_raster.hData ().currentPositionAtBase0 () == 0)
			MCHEmul::GraphicalChip::notify (MCHEmul::Event (_GRAPHICSREADY));

		// Decide whether it is needed to draw or not the events...
		if (_showEvents)
		{
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
			if (cpu -> lastInstruction () -> code () == 0x076)
			{
				if (!_HALTActive.peekValue ())
					_HALTActive = true;
			}
			else
				_HALTBefore = false;
		}

		// Notice that graphics are always tried to be drawn, 
		// but only f there were something to draw because the ULA has info to draw in the shift register...
		drawInVisibleZone (cpu);

		// This block is the one that simulates the HSYNC and VSYNC signals...
		// The HSYNC signal can come from outside the ULA (from the INTack signal) or
		// from the internal raster data (when the NMI interrupts are active) at the end of the line.
		bool eH = _ULARegisters -> INTack (_lastCPUCycles + (i >> 1));
		if (_raster.hData ().add (1) || eH)
		{
			// the HSYNC happens...
			_raster.vData ().add (1);
			// ..and just in case horizontal raster is set back to 0...
			_raster.hData ().initialize ();
			// If there was a signal from outside to change the line (a INT was launched)...
			// ...the LINECTRL register is incremented...
			if (eH)	_ULARegisters -> incLINECTRL ();

			// If the NMI generator is active a interrupt is launched to the CPU...
			// This situation shouldn't ever happen is the previous one (INT interrupt) was launched...
			if (_ULARegisters -> NMIGenerator ()) // The generator has to be active! (@see PortManager)
				cpu -> requestInterrupt (FZ80::NMIInterrupt::_ID, 
					_lastCPUCycles + (i >> 1), this, 1 /** HSYNC reason */);
		}

		// Notice that the VSYNC doesn't happen here as it is the port output the one launching that...
		// (@see ZX81::PortManager class)

		// If the status of the casette signal has changed, it has to be notified...
		if (_ULARegisters -> casetteSignalChanged ())
			notify (MCHEmul::Event (MCHEmul::DatasetteIOPort::_WRITE, 
				_ULARegisters -> casetteSignal () ? 1 : 0));
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
	cP [2]  = SDL_MapRGBA (_format, 0x92, 0x4a, 0x40, 0xff); // Red
	cP [3]  = SDL_MapRGBA (_format, 0x84, 0xc5, 0xcc, 0xff); // Cyan
	cP [4]  = SDL_MapRGBA (_format, 0x93, 0x51, 0xb6, 0xff); // Violet
	cP [5]  = SDL_MapRGBA (_format, 0x72, 0xb1, 0x4b, 0xff); // Green
	cP [6]  = SDL_MapRGBA (_format, 0x48, 0x3a, 0xaa, 0xff); // Blue
	cP [7]  = SDL_MapRGBA (_format, 0xd5, 0xdf, 0x7c, 0xff); // Yellow
	cP [8]  = SDL_MapRGBA (_format, 0x99, 0x69, 0x2d, 0xff); // Brown
	cP [9]  = SDL_MapRGBA (_format, 0x67, 0x52, 0x00, 0xff); // Light Red
	cP [10] = SDL_MapRGBA (_format, 0xc1, 0x81, 0x78, 0xff); // Orange
	cP [11] = SDL_MapRGBA (_format, 0x60, 0x60, 0x60, 0xff); // Dark Grey
	cP [12] = SDL_MapRGBA (_format, 0x8a, 0x8a, 0x8a, 0xff); // Medium Grey
	cP [13] = SDL_MapRGBA (_format, 0xb3, 0xec, 0x91, 0xff); // Light Green
	cP [14] = SDL_MapRGBA (_format, 0x86, 0x7a, 0xde, 0xff); // Light Blue
	cP [15] = SDL_MapRGBA (_format, 0xb3, 0xb3, 0xb3, 0xff); // Light Grey

	return (new MCHEmul::ScreenMemory (numberColumns (), numberRows (), cP));
}

// --
void ZX81::ULA::drawInVisibleZone (MCHEmul::CPU* cpu)
{
	bool d = false;
	bool p = _ULARegisters-> shiftOutData (d);

	if (!_raster.isInVisibleZone ())
		return; // Nothing else can be done...

	unsigned short x = 0, y = 0;
	_raster.currentVisiblePosition (x, y);
	// Draws the pixel..
	_screenMemory -> setPixel (x, y, 1);
	if (p && _ULARegisters -> syncOutputWhite () &&
		(_ULARegisters -> reverseVideo () ^ d)) // normal video and pixel on?, or inverse video and pixels off?
			_screenMemory -> setPixel (x, y, 0); // ...then put it in black...

	// Draws the events if any...
	if (_showEvents)
	{
		if (_HALTActive)
			{ _screenMemory -> setPixel (x, y, !_HALTBefore ? 13 /** Fist HALT. */ : 12); _HALTBefore = true; }
		if (_INTActive)			_screenMemory -> setPixel (x, y, 2);	// Red
		if (_NMIActive)			_screenMemory -> setPixel (x, y, 7);	// Yellow
		// Writting and reading to the specific ports is also detected...
		size_t l = (x <= 5) ? x : 5;
		if (_writePort)			_screenMemory -> setHorizontalLine (x - l, y, l, 5);	// Green
		if (_NMIGeneratorOn)	_screenMemory -> setHorizontalLine (x - l, y, l, 6);	// Blue
		if (_NMIGeneratorOff)	_screenMemory -> setHorizontalLine (x - l, y, l, 8);	// Brown
		if (_readPortFE)		_screenMemory -> setHorizontalLine (x - l, y, l, 3);	// Cyan
	}
}

// ---
bool ZX81::ULA::readCharData (MCHEmul::CPU* cpu, const MCHEmul::UByte& dt)
{
	MCHEmul::MemoryView* oV = memoryRef () -> activeView ();

	// The information is looked for always from the point of view of the ULA!
	// That0s the reason to remember what the origina view was before changing it...
	memoryRef () -> setActiveView (_ULAView);

	bool result = false;

	// Possible or not?
	// It wasn't possible is it beacuse it was reading another value before...
	if ((result = _ULARegisters -> loadSHIFTRegister (
			memoryRef () -> value (MCHEmul::Address (2,
				(unsigned int (static_cast <FZ80::CZ80*> (cpu) -> iRegister ().values ()[0].value ()) << 8) |
				(unsigned int ((dt.value () & 0b00111111) << 3)) |
				(unsigned int) _ULARegisters -> LINECNTRL ())))))
		_ULARegisters -> setReverseVideo (dt.bit (7)); // The reverse video depends on the 
													   // bit 7 of the data received (from video memory)

	// ...then the view is set back to the original one...
	memoryRef () -> setActiveView (oV -> id ());

	return (result);
}

// ---
void ZX81::ULA::debugULACycle (MCHEmul::CPU* cpu, unsigned int i)
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeCompleteLine (className (), _lastCPUCycles + (i >> 1), "Info Cycle",
		{ { "Raster position",
			std::to_string (_raster.currentColumnAtBase0 ()) + "," +
			std::to_string (_raster.currentLineAtBase0 ()) },
		  { "Internal status",
			"NMI=" + std::string ((_ULARegisters -> NMIGenerator () ? "NMI_ON" : "NMI_OFF")) + "," +
			"ZONE=" + std::string ((_ULARegisters -> syncOutputWhite () ? "WHITE" : "BLACK")) + "," + 
			"LNCTRL=" +	std::to_string (_ULARegisters -> LINECNTRL ()) } });
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
