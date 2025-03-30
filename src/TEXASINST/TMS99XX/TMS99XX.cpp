#include <TEXASINST/TMS99XX/TMS99XX.hpp>
#include <TEXASINST/TMS99XX/TMS99XXRegisters.hpp>
#include <FZ80/CZ80.hpp>

// ---
const MCHEmul::RasterData TEXASINSTRUMENTS::TMS9929A_PAL::_VRASTERDATA
	(248, 256 /** 8 VSYNC. */, 0 /** 56 top border. */, 191 /** 192 draw. */, 247 /** 56 bottom. */, 247, 247, 312, 0, 0);
const MCHEmul::RasterData TEXASINSTRUMENTS::TMS9929A_PAL::_HRASTERDATA
	(320, 416 /** 96 = Blanking Period + HSYNC (different in 5C, happens before, than in 6C) */, 
	 0 /** 32 left border. */, 255 /** 256 (32 chars * 8 TMS9918 cycles or 4 CPU cycles) draw. */, 
	 287 /** 32 right border. */, 319 /** Other 32 non visible. */, 319, 448, 0, 0);
// NTSC is explained in similar terms than PAL...
const MCHEmul::RasterData TEXASINSTRUMENTS::TMS9928A_NTSC::_VRASTERDATA (216, 224, 0, 191, 215, 215, 215, 264, 0, 0);
const MCHEmul::RasterData TEXASINSTRUMENTS::TMS9928A_NTSC::_HRASTERDATA (320, 416, 0, 255, 287, 319, 319, 448, 0, 0);

// ---
TEXASINSTRUMENTS::TMS99XX::TMS99XX (const MCHEmul::RasterData& vd, const MCHEmul::RasterData& hd,
		unsigned int cF,
		const MCHEmul::Attributes& attrs)
	: MCHEmul::GraphicalChip (_ID, 
		{ { "Name", "TMS99XX" },
		  { "Code", "TMS9918A, TMS9928A, TMS9929A" },
		  { "Manufacturer", "Texas Instruments" },
		  { "Year", "1982-1985" } }),
	  _TMS99XXRegisters (new TEXASINSTRUMENTS::TMS99XXRegisters),
	  _raster (vd, hd, 1 /** The step is 1 pixel. */),
	  _showEvents (false),
	  _lastCPUCycles (0),
	  _format (nullptr),
	  _eventStatus ({ })
{
	setClassName ("TMS9918");

	_format = SDL_AllocFormat (SDL_PIXELFORMAT_ARGB8888);
}

// ---
TEXASINSTRUMENTS::TMS99XX::~TMS99XX ()
{
	SDL_FreeFormat (_format);
}

// ---
bool TEXASINSTRUMENTS::TMS99XX::initialize ()
{
	assert (MCHEmul::GraphicalChip::memoryRef () != nullptr);

	if (!MCHEmul::GraphicalChip::initialize ())
		return (false);

	_raster.initialize ();

	_TMS99XXRegisters -> initialize ();
	
	// TODO

	_lastCPUCycles = 0;

	_eventStatus = { };

	return (true);
}

// ---
bool TEXASINSTRUMENTS::TMS99XX::simulate (MCHEmul::CPU* cpu)
{
	// First time?
	if (_lastCPUCycles == 0)
	{ 
		_lastCPUCycles = cpu -> clockCycles (); // Nothing to do...

		return (true);
	}

	// Simulate the visulization...
	for (unsigned int i = 
			((cpu -> clockCycles  () - _lastCPUCycles) << 1 /** TMS9918 cycles = 2 * CPU Cycles. */); 
			i > 0; i--)
	{
		_IFDEBUG debugTMS99XXCycle (cpu, i);

		// Read the graphics and draw the visible zone, if it is the case...
		// ...Mark also whethe rthe event have to be drawn or not!
		if (_raster.isInVisibleZone ())
		{
			readGraphicInfoAndDrawVisibleZone (cpu);

			if (_showEvents)
				drawEvents ();
		}
	
		// First, moves the raster...
		_raster.moveCycles (1);
		// But, if is starting the screen...
		if (_raster.vData ().currentPositionAtBase0 () == 0 &&
			_raster.hData ().currentPositionAtBase0 () == 0)
		{
			// An interrupt is generated...
			cpu -> requestInterrupt 
				(FZ80::INTInterrupt::_ID, i, this, 0 /** The reason is that the screen is complete. */);
	
			// ...a notification to draw the screen is also generated...
			MCHEmul::GraphicalChip::notify (MCHEmul::Event (_GRAPHICSREADY));
		}
	}

	_lastCPUCycles = cpu -> clockCycles ();

	return (true);
}

// ---
MCHEmul::InfoStructure TEXASINSTRUMENTS::TMS99XX::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::GraphicalChip::getInfoStructure ());

	result.remove ("Memory"); // This info is not neccesary...
	result.add ("TMS9918Registers",	std::move (_TMS99XXRegisters -> getInfoStructure ()));
	result.add ("Raster",			std::move (_raster.getInfoStructure ()));

	return (result);
}

// ---
void TEXASINSTRUMENTS::TMS99XX::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	switch (evnt.id ())
	{
		// TODO
		
		default:
			break;
	}
}

// ---
MCHEmul::ScreenMemory* TEXASINSTRUMENTS::TMS99XX::createScreenMemory ()
{
	unsigned int* cP = new unsigned int [20];
	// The colors are partially transparents to allow the blending...
	cP [0]  = SDL_MapRGBA (_format, 0x00, 0x00, 0x00, 0xe0); // Transparent
	cP [1]  = SDL_MapRGBA (_format, 0x01, 0x01, 0x01, 0xe0); // Black
	cP [2]  = SDL_MapRGBA (_format, 0x3e, 0xb8, 0x49, 0xe0); // Medium Green
	cP [3]  = SDL_MapRGBA (_format, 0x74, 0xd0, 0x7d, 0xe0); // Light Green
	cP [4]  = SDL_MapRGBA (_format, 0x59, 0x55, 0xe0, 0xe0); // Dark Blue
	cP [5]  = SDL_MapRGBA (_format, 0x80, 0x76, 0xf1, 0xe0); // Light Blue
	cP [6]  = SDL_MapRGBA (_format, 0xb9, 0x5e, 0x51, 0xe0); // Dark Red
	cP [7]  = SDL_MapRGBA (_format, 0x65, 0xdb, 0xef, 0xe0); // Cyan
	cP [8]  = SDL_MapRGBA (_format, 0xdb, 0x65, 0x69, 0xe0); // Medium Red
	cP [9]  = SDL_MapRGBA (_format, 0xff, 0x89, 0x7d, 0xe0); // Light Red
	cP [10] = SDL_MapRGBA (_format, 0xcc, 0xc3, 0x5e, 0xe0); // Dark Yellow
	cP [11] = SDL_MapRGBA (_format, 0xde, 0xd0, 0x87, 0xe0); // Light Yellow
	cP [12] = SDL_MapRGBA (_format, 0x3a, 0xa2, 0x42, 0xe0); // Dark Green
	cP [13] = SDL_MapRGBA (_format, 0xb7, 0x66, 0xb5, 0xe0); // Magenta
	cP [14] = SDL_MapRGBA (_format, 0xcc, 0xcc, 0xcc, 0xe0); // Gray
	cP [15] = SDL_MapRGBA (_format, 0xff, 0xff, 0xff, 0xe0); // White

	// Colors used for events!
	cP [16] = SDL_MapRGBA (_format, 0X00, 0Xf5, 0xff, 0xff); // light Cyan
	cP [17] = SDL_MapRGBA (_format, 0Xfc, 0Xe7, 0x00, 0xff); // light Yellow
	cP [18] = SDL_MapRGBA (_format, 0Xff, 0X6d, 0x28, 0xff); // light Orange
	cP [19] = SDL_MapRGBA (_format, 0Xea, 0X04, 0x7e, 0xff); // light Purple

	return (new MCHEmul::ScreenMemory (numberColumns (), numberRows (), cP));
}

// --
void TEXASINSTRUMENTS::TMS99XX::readGraphicInfoAndDrawVisibleZone (MCHEmul::CPU* cpu)
{
	// Here it is sure that the raster in the visible zone...

	// Gets the position with in the visible zone...
	// That position will take into account the birder (top and left)...
	unsigned short x = 0, y = 0;
	_raster.currentVisiblePosition (x, y);
		
	// TODO
}

// ---
void TEXASINSTRUMENTS::TMS99XX::drawEvents ()
{
	// Draw the border events...
	unsigned int cEvent = std::numeric_limits <unsigned int>::max ();
	
	// TODO
	
	if (cEvent != std::numeric_limits <unsigned int>::max ())
		screenMemory () -> setHorizontalLine // Draw at least two pixels when the events has happpened...
			(_raster.hData ().currentVisiblePosition (), _raster.vData ().currentVisiblePosition (), 2, cEvent);
}

// ---
void TEXASINSTRUMENTS::TMS99XX::debugTMS99XXCycle (MCHEmul::CPU* cpu, unsigned int i)
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeCompleteLine (className (), cpu -> clockCycles () - (i >> 1), "Info Cyle", 
		MCHEmul::Attributes ({ 
			{ "Raster",
				std::to_string (_raster.currentColumnAtBase0 ()) + "," +
				std::to_string (_raster.currentLineAtBase0 ()) } }));
}

// ---
TEXASINSTRUMENTS::TMS9929A_PAL::TMS9929A_PAL (unsigned int cF /** Clock frequency. */)
	: TEXASINSTRUMENTS::TMS99XX (_VRASTERDATA, _HRASTERDATA, cF,
		 { { "Name", "TMS9929A" },
		   { "Manufacturer", "Texas Instruments" },
		   { "Year", "1982-1985" } })
{
	// Nothing else...
}

// ---
TEXASINSTRUMENTS::TMS9928A_NTSC::TMS9928A_NTSC (unsigned int cF /** Clock frequency. */)
	: TEXASINSTRUMENTS::TMS99XX (_VRASTERDATA, _HRASTERDATA, cF,
		 { { "Name", "TMS9928A" },
		   { "Manufacturer", "Texas Instruments" },
		   { "Year", "1982-1985" } })
{
	// Nothing else...
}
