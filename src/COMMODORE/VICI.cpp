#include <COMMODORE/VICI.hpp>

// ---
const MCHEmul::RasterData COMMODORE::VICI_NTSC::_VRASTERDATA (27, 41, 51, 250, 12, 26, 262, 4, 4);
const MCHEmul::RasterData COMMODORE::VICI_NTSC::_HRASTERDATA (412, 488, 24, 343, 388, 411, 512, 7, 9);
const MCHEmul::RasterData COMMODORE::VICI_PAL::_VRASTERDATA (0, 16, 51, 250, 299, 311, 312, 4, 4);
const MCHEmul::RasterData COMMODORE::VICI_PAL::_HRASTERDATA (404, 480, 24, 343, 380, 403, 504, 7, 9);

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
	  _incCyclesPerRasterLine (cRL - COMMODORE::VICI_PAL::_CYCLESPERRASTERLINE),
	  _raster (vd, hd, 4 /** The step is here 4 pixels */),
	  _lastCPUCycles (0),
	  _format (nullptr),
	  _cycleInRasterLine (0),
	  _lastVBlankEntered (false)
{
	assert (_cyclesPerRasterLine >= 65);

	MCHEmul::GraphicalChip::setClassName ("VICI");

	_format = SDL_AllocFormat (SDL_PIXELFORMAT_ARGB8888);
}

// ---
COMMODORE::VICI::~VICI ()
{
	delete (_soundFunction);

	SDL_FreeFormat (_format);
}

// ---
bool COMMODORE::VICI::initialize ()
{
	assert (MCHEmul::GraphicalChip::memoryRef () != nullptr);

	if (!MCHEmul::GraphicalChip::initialize () ||
		!_soundFunction -> initialize ())
		return (false);

	// Gets the memory block dedicated to the VICI
	if (!(_VICIRegisters = 
		dynamic_cast <COMMODORE::VICIRegisters*> 
			(MCHEmul::GraphicalChip::memoryRef () -> subset (COMMODORE::VICIRegisters::_VICREGS_SUBSET))))
	{
		MCHEmul::GraphicalChip::_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	_raster.initialize ();

	_VICIRegisters -> setSoundLibWrapper (_soundFunction -> soundWrapper ());
	_VICIRegisters -> initialize ();

	_lastCPUCycles = 0;
	
	_cycleInRasterLine = 1;

	_lastVBlankEntered = false;

	return (true);
}

// ---
bool COMMODORE::VICI::simulate (MCHEmul::CPU* cpu)
{
	// First time?
	if (_lastCPUCycles == 0)
	{ 
		_lastCPUCycles = cpu -> clockCycles (); // Nothing to do...

		return (true);
	}

	// Simulate the visulization...
	for (unsigned int i = (cpu -> clockCycles  () - _lastCPUCycles); i > 0; i--)
	{
		if (MCHEmul::GraphicalChip::deepDebugActive ())
		{
			*MCHEmul::GraphicalChip::_deepDebugFile
				// Where
				<< "VICI\t" 
				// When
				<< std::to_string (cpu -> clockCycles () - i) << "\t" // clock cycles at that point
				// What
				<< "Info cycle\t\t"
				// Data
				<< "Raster:["
				<< std::to_string (_raster.currentColumnAtBase0 ()) << "," 
				<< std::to_string (_raster.currentLineAtBase0 ()) << ","
				<< std::to_string (_cycleInRasterLine)
				<< "], Memory:["
				<< "SM=$" << MCHEmul::removeAll0 (_VICIRegisters -> screenMemory ().asString
					(MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) << ","
				<< "CM=$" << MCHEmul::removeAll0 (_VICIRegisters -> charDataMemory ().asString
					(MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) << ","
				<< "BM=$" << MCHEmul::removeAll0 (_VICIRegisters -> colourMemory ().asString
					(MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) << "]\n";
		}

		// Treat the right cycle...
		unsigned int cS = 0;
		if ((cS = treatRasterCycle ()) > 0)
			cpu -> setStop (true, MCHEmul::Instruction::_CYCLEALL /** fully stopped. */, cpu -> clockCycles () - i, (int) cS);

		// Draws the graphics & border if it has to do so...
		if (_raster.isInVisibleZone ())
			drawVisibleZone (cpu);

		// Move to the next cycle...
		_cycleInRasterLine++;

		// Move 4 pixels right in the raster line and jump to other line is possible...
		// Notice that the variable _isNewRasterLine becomes true when a new line comes...
		if (_raster.moveCycles (1))
			_cycleInRasterLine = 1;
	}

	// When the raster enters the non visible part of the screen,
	// a notification is sent (to the Screen class usually) 
	// just to draw the screen...
	if (_raster.isInLastVBlank ())
	{
		if (!_lastVBlankEntered)
		{
			_lastVBlankEntered = true;

			MCHEmul::GraphicalChip::notify (MCHEmul::Event (_GRAPHICSREADY)); 
		}
	}
	else
		_lastVBlankEntered = false;

	_VICIRegisters -> setCurrentRasterLine (_raster.currentLine ()); 

	_lastCPUCycles = cpu -> clockCycles ();

	return (_soundFunction -> simulate (cpu));
}

// ---
MCHEmul::InfoStructure COMMODORE::VICI::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::GraphicalChip::getInfoStructure ());

	result.remove ("Memory"); // This info is not neccesary...
	result.add ("VICIRegisters",	std::move (_VICIRegisters -> getInfoStructure ()));
	result.add ("Raster",			std::move (_raster.getInfoStructure ()));
	result.add ("Sound",			std::move (_soundFunction -> getInfoStructure ()));

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
				unsigned short x = (unsigned short) 
					std::dynamic_pointer_cast <MCHEmul::InputOSSystem::MouseMovementEvent> (evnt.data ()) -> _x;
				unsigned short y = (unsigned short) 
					std::dynamic_pointer_cast <MCHEmul::InputOSSystem::MouseMovementEvent> (evnt.data ()) -> _y;
				setLightPenPosition ((x >= _raster.hData ().firstDisplayPosition () && 
									  y <= _raster.hData ().lastDisplayPosition ()) 
										? (x - _raster.hData ().firstDisplayPosition ()) : 0, 
									 (y >= _raster.vData ().firstDisplayPosition () && 
									  y <= _raster.vData ().lastDisplayPosition ()) 
										? (y - _raster.vData ().firstDisplayPosition ()) : 0);
			}

			break;

		// The lightpen actives when the right button of the mouse is pressed...
		case MCHEmul::InputOSSystem::_MOUSEBUTTONPRESSED:
			{
				if (std::dynamic_pointer_cast <MCHEmul::InputOSSystem::MouseButtonEvent> 
					(evnt.data ()) -> _buttonId == 0 /** Left. */)
					setLightPenActive (true);
			}
			
			break;

		// ...and stop being active when released...
		case MCHEmul::InputOSSystem::_MOUSEBUTTONRELEASED:
			{
				if (std::dynamic_pointer_cast <MCHEmul::InputOSSystem::MouseButtonEvent> 
					(evnt.data ()) -> _buttonId == 0 /** Left. */)
					setLightPenActive (false);
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
	// The colors are partially transparents to allow the blending...
	cP [0]  = SDL_MapRGBA (_format, 0x00, 0x00, 0x00, 0xe0); // Black
	cP [1]  = SDL_MapRGBA (_format, 0xff, 0xff, 0xff, 0xe0); // White
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

// ---
unsigned int COMMODORE::VICI::treatRasterCycle ()
{
	// TODO

	return (0);
}

// ---
void COMMODORE::VICI::drawVisibleZone (MCHEmul::CPU* cpu)
{
	// TODO
}

// ---
COMMODORE::VICI::DrawResult COMMODORE::VICI::drawGraphics (const COMMODORE::VICI::DrawContext& dC)
{
	// TODO

	return (COMMODORE::VICI::DrawResult ());
}

// ---
void COMMODORE::VICI::drawResultToScreen (const COMMODORE::VICI::DrawResult& cT, const COMMODORE::VICI::DrawContext& dC)
{
	// TODO
}

// ---
COMMODORE::VICI_NTSC::VICI_NTSC (int vV, MCHEmul::SoundLibWrapper* wS)
	: COMMODORE::VICI (
		 _VRASTERDATA, _HRASTERDATA, vV, 71, wS,
		 { { "Name", "VIC-II (NTSC) Video Chip Interface II" },
		   { "Code", "6560" },
		   { "Manufacturer", "MOS Technology INC/Commodore Semiconductor Group (CBM)"},
		   { "Year", "1977" } })
{
	// Nothing else to do...
}

// ---
unsigned int COMMODORE::VICI_NTSC::treatRasterCycle ()
{
	unsigned int result = COMMODORE::VICI::treatRasterCycle ();

	// TODO

	return (result);
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
unsigned int COMMODORE::VICI_PAL::treatRasterCycle ()
{
	unsigned int result = COMMODORE::VICI::treatRasterCycle ();

	// TODO

	return (result);
}

// ---
COMMODORE::VICI::SoundFunction::SoundFunction (MCHEmul::SoundLibWrapper* sW)
	: MCHEmul::SoundChip (_ID, 
		{ { "Name", "SoundFunction" },
		  { "Code", "Inside 6560 (NTSC), 6561/6561E/6561-101 (PAL)" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Year", "1977" } },
		sW)
{ 
	// Take care that the sound emulation library was not null at all...
	// ...and also belonging to the right type..
	assert (dynamic_cast <COMMODORE::VICISoundLibWrapper*> (soundWrapper ()) != nullptr);

	setClassName ("SoundFunction");
}

// ---
bool COMMODORE::VICI::SoundFunction::initialize ()
{
	if (!MCHEmul::SoundChip::initialize ())
		return (false);

	_lastCPUCycles = 0;

	return (true);
}

// ---
bool COMMODORE::VICI::SoundFunction::simulate (MCHEmul::CPU* cpu)
{
	// First time?
	if (_lastCPUCycles == 0)
	{ 
		_lastCPUCycles = cpu -> clockCycles (); // Nothing to do...

		return (true);
	}

	if (soundWrapper () != nullptr)
	{
		for (unsigned int i = (cpu -> clockCycles () - _lastCPUCycles); i > 0 ; i--)
		{
			// The number of bytes that can come from the wrapper might not be fixed...
			MCHEmul::UBytes data;
			if (soundWrapper () -> getData (cpu, data))
			{
				for (size_t j = 0; j < data.size (); j++)
				{
					char dt = data [j].value ();
					if (soundMemory () -> addSampleData (&dt, sizeof (char)))
						MCHEmul::SoundChip::notify (MCHEmul::Event (_SOUNDREADY));
				}
			}
		}
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
