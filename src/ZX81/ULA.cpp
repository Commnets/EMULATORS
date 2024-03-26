#include <ZX81/ULA.hpp>
#include <ZX81/ZX81.hpp>
#include <ZX81/OSIO.hpp>

// ---
/*	Based on https://8bit-museum.de/heimcomputer-2/sinclair/sinclair-scans/scans-zx81-video-display-system/ */
const MCHEmul::RasterData ZX81::ULA_PAL::_VRASTERDATA	(0, 28, 28, 311, 311, 311, 312, 0, 0);
/** Really the movement is half the vakues show, but they have been multiplied by two to maitain the proportion inthe screen. */
const MCHEmul::RasterData ZX81::ULA_PAL::_HRASTERDATA	
	(0, 40, 40, 504, 504, 567, 568 /** For everything to run well, it has to be a multiple of 8. */, 0, 0);
const MCHEmul::RasterData ZX81::ULA_NTSC::_VRASTERDATA (0, 28, 28, 260, 260, 260, 261, 0, 0);
/** Really the movement is half the vakues show, but they have been multiplied by two to maitain the proportion inthe screen. */
const MCHEmul::RasterData ZX81::ULA_NTSC::_HRASTERDATA 
	(0, 0, 0, 415, 415, 519, 520 /** For everything to run well, it has to be a multiple of 8. */, 0, 0);

// ---
ZX81::ULA::ULA (const MCHEmul::RasterData& vd, const MCHEmul::RasterData& hd, 
		int vV, unsigned short cRL, const MCHEmul::Attributes& attrs)
	: MCHEmul::GraphicalChip (_ID, 
		{ { "Name", "ULA" },
		  { "Code", "2C184E" },
		  { "Manufacturer", "Ferranti" },
		  { "Year", "1980" } }),
	  _portFE (nullptr), // Assigned later, before starting to use it!
	  _ULAView (vV),
	  _cyclesPerRasterLine (cRL),
	  _raster (vd, hd, 8 /** The step is here 8 pixels */),
	  _lastCPUCycles (0),
	  _format (nullptr)
{
	assert (_portFE != nullptr);

	assert (_cyclesPerRasterLine >= 65);

	MCHEmul::GraphicalChip::setClassName ("ULA");

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

	// The port is initialized when the CPU is initialized...

	_lastCPUCycles = 0;

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
				<< std::to_string (_raster.currentLineAtBase0 ()) << "]\n";
		}

		// TODO
	}

	_lastCPUCycles = cpu -> clockCycles ();

	return (true);
}

// ---
MCHEmul::InfoStructure ZX81::ULA::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::GraphicalChip::getInfoStructure ());

	result.remove ("Memory"); // This info is not neccesary...
	result.add ("Raster", std::move (_raster.getInfoStructure ()));

	return (result);
}

// ---
void ZX81::ULA::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	assert (_portFE != nullptr);

	switch (evnt.id ())
	{
		case MCHEmul::InputOSSystem::_KEYBOARDKEYPRESSED:
			{
				const ZX81::InputOSSystem::Keystrokes& ks = ((ZX81::InputOSSystem*) n) -> keystrokesFor
					(std::static_pointer_cast <MCHEmul::InputOSSystem::KeyboardEvent> (evnt.data ()) -> _key);
				if (!ks.empty ()) // The key has to be defined...
					for (const auto& j : ks)
						_portFE -> setKeyboardStatus (j.first, j.second, true);
			}

			break;

		case MCHEmul::InputOSSystem::_KEYBOARDKEYRELEASED:
			{
				const ZX81::InputOSSystem::Keystrokes& ks = ((ZX81::InputOSSystem*) n) -> keystrokesFor
				(std::static_pointer_cast <MCHEmul::InputOSSystem::KeyboardEvent> (evnt.data ()) -> _key);
				if (!ks.empty ()) // The key has to be defined...
					for (const auto& j : ks)
						_portFE -> setKeyboardStatus (j.first, j.second, false);
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
	cP [2]  = SDL_MapRGBA (_format, 0x48, 0x3a, 0xaa, 0xe0); // Blue
	cP [3]  = SDL_MapRGBA (_format, 0xd5, 0xdf, 0x7c, 0xe0); // Yellow

	return (new MCHEmul::ScreenMemory (numberColumns (), numberRows (), cP));
}

// ---
ZX81::ULA_PAL::ULA_PAL ()
	: ZX81::ULA (_VRASTERDATA, _HRASTERDATA, 0, ZX81::ULA_PAL::_CYCLESPERRASTERLINE,
		 { { "Name", "ULA" },
		   { "Code", "2C184E (PAL)" },
		   { "Manufacturer", "Ferranti"},
		   { "Year", "1980" } })
{
	// Nothing else to do...
}

// ---
ZX81::ULA_NTSC::ULA_NTSC ()
	: ZX81::ULA (_VRASTERDATA, _HRASTERDATA, 0, ZX81::ULA_PAL::_CYCLESPERRASTERLINE,
		 { { "Name", "ULA" },
		   { "Code", "2C184E (PAL)" },
		   { "Manufacturer", "Ferranti"},
		   { "Year", "1980" } })
{
	// Nothing else to do...
}
