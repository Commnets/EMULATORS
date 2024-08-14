#include <ZXSpectrum/ULA.hpp>
#include <ZXSpectrum/ULARegisters.hpp>
#include <ZXSpectrum/ZXSpectrum.hpp>
#include <ZXSpectrum/OSIO.hpp>
#include <FZ80/NMIInterrupt.hpp>

// ---
const MCHEmul::RasterData ZXSPECTRUM::ULA_PAL::_VRASTERDATA
	(248, 256 /** 8 VSYNC. */, 312 /** 56 top border. */, 191 /** 192 draw. */, 247 /** 56 bottom. */, 247, 312, 0, 0);
const MCHEmul::RasterData ZXSPECTRUM::ULA_PAL::_HRASTERDATA
	(320, 416 /** 96 = Blanking Period + HSYNC (different in 5C, happens before, than in 6C) */, 
	 448 /** 32 left border. */, 255 /** 256 (32 chars * 8 ULA cycles or 4 CPU cycles) draw. */, 
	 319 /** 64 right border. */, 319, 448, 0, 0);
// NTSC is explained in similar terms than PAL...
const MCHEmul::RasterData ZXSPECTRUM::ULA_NTSC::_VRASTERDATA (216, 224, 264, 191, 215, 215, 264, 0, 0);
const MCHEmul::RasterData ZXSPECTRUM::ULA_NTSC::_HRASTERDATA (320, 416, 448, 255, 319, 319, 448, 0, 0);

// ---
ZXSPECTRUM::ULA::ULA (const MCHEmul::RasterData& vd, const MCHEmul::RasterData& hd, 
		int vV, const MCHEmul::Attributes& attrs)
	: MCHEmul::GraphicalChip (_ID, 
		{ { "Name", "ULA" },
		  // https://www.spectrumforeveryone.com/technical/zx-spectrum-ula-types/
		  // There were many types of ULA during Spectrum life period....
		  // The differences are not, in principle, interesting to be applicable to this simulation...
		  // However they are highlighted in futher descriptions...
		  { "Code", "5C102E, 5C112E, 5C112E-2, 5C112E-3, 6C001E-5, 6C001E-6, 6C001E-7, 7K010E-5 ULA / Amstrad 40056, +2A/+3 Gate Array" },
		  { "Manufacturer", "Ferranti" },
		  { "Year", "1982-1985" } }),
	  _ULARegisters (new ZXSPECTRUM::ULARegisters),
	  _ULAView (vV),
	  _raster (vd, hd, 1 /** The step is 1 pixel. */),
	  _showEvents (false),
	  _videoSignalData (),
	  _lastCPUCycles (0),
	  _format (nullptr),
	  _firstVBlankEntered (false)
{
	setClassName ("ULA");

	_format = SDL_AllocFormat (SDL_PIXELFORMAT_ARGB8888);
}

// ---
ZXSPECTRUM::ULA::~ULA ()
{
	SDL_FreeFormat (_format);
}

// ---
bool ZXSPECTRUM::ULA::initialize ()
{
	assert (MCHEmul::GraphicalChip::memoryRef () != nullptr);

	if (!MCHEmul::GraphicalChip::initialize ())
		return (false);

	_raster.initialize ();

	_showEvents = false;

	_ULARegisters -> initialize ();

	_videoSignalData = ZXSPECTRUM::ULA::VideoSignalData ();

	_lastCPUCycles = 0;

	_firstVBlankEntered = false;

	return (true);
}

// ---
bool ZXSPECTRUM::ULA::simulate (MCHEmul::CPU* cpu)
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
				<< "]\n";
		}

		if (_showEvents)
		{
			// TODO
		}

		// Read the graphics and draw the visible zone, 
		// if it is the case...
		if (_raster.isInVisibleZone ())
			readGraphicInfoAndDrawVisibleZone (cpu);

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

		// Moves the internal cycles counter 1...
		if (_raster.moveCycles (1))
		{
			// The flash attribute has to be updated!
			if (++_videoSignalData._flashCounter == 50)
			{
				_videoSignalData._flashCounter = 0;

				_videoSignalData._flash = !_videoSignalData._flash;
			}

			// An interrupt is generated any time the 
			cpu -> requestInterrupt 
				(FZ80::NMIInterrupt::_ID, i, this, 0 /** The reason is that the Screen is complete. */);
		}
	}

	_lastCPUCycles = cpu -> clockCycles ();

	return (true);
}

// ---
MCHEmul::InfoStructure ZXSPECTRUM::ULA::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::GraphicalChip::getInfoStructure ());

	result.remove ("Memory"); // This info is not neccesary...
	result.add ("ULARegisters",	std::move (_ULARegisters -> getInfoStructure ()));
	result.add ("Raster",		std::move (_raster.getInfoStructure ()));

	return (result);
}

// ---
void ZXSPECTRUM::ULA::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	switch (evnt.id ())
	{
		case MCHEmul::InputOSSystem::_KEYBOARDKEYPRESSED:
			{
				const ZXSPECTRUM::InputOSSystem::Keystrokes& ks = ((ZXSPECTRUM::InputOSSystem*) n) -> keystrokesFor
					(std::static_pointer_cast <MCHEmul::InputOSSystem::KeyboardEvent> (evnt.data ()) -> _key);
				if (!ks.empty ()) // The key has to be defined...
					for (const auto& j : ks)
						_ULARegisters -> setKeyboardStatus (j.first, j.second, true);
			}

			break;

		case MCHEmul::InputOSSystem::_KEYBOARDKEYRELEASED:
			{
				const ZXSPECTRUM::InputOSSystem::Keystrokes& ks = ((ZXSPECTRUM::InputOSSystem*) n) -> keystrokesFor
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
MCHEmul::ScreenMemory* ZXSPECTRUM::ULA::createScreenMemory ()
{
	unsigned int* cP = new unsigned int [16];
	// The colors are partially transparents to allow the blending...
	cP [0]  = SDL_MapRGBA (_format, 0x00, 0x00, 0x00, 0xe0); // Black
	cP [1]  = SDL_MapRGBA (_format, 0x01, 0x00, 0xce, 0xe0); // Blue
	cP [2]  = SDL_MapRGBA (_format, 0xcf, 0x01, 0x00, 0xe0); // Red
	cP [3]  = SDL_MapRGBA (_format, 0xcf, 0x01, 0xce, 0xe0); // Magenta
	cP [4]  = SDL_MapRGBA (_format, 0x00, 0xcf, 0x15, 0xe0); // Green
	cP [5]  = SDL_MapRGBA (_format, 0x01, 0xcf, 0xcf, 0xe0); // Cyan
	cP [6]  = SDL_MapRGBA (_format, 0xcf, 0xcf, 0x15, 0xe0); // Yellow
	cP [7]  = SDL_MapRGBA (_format, 0xcf, 0xcf, 0xcf, 0xe0); // White
	cP [8]  = SDL_MapRGBA (_format, 0x00, 0x00, 0x00, 0xe0); // Bright Black
	cP [9]  = SDL_MapRGBA (_format, 0x02, 0x00, 0xfd, 0xe0); // Bright Blue
	cP [10] = SDL_MapRGBA (_format, 0xff, 0x02, 0x01, 0xe0); // Bright Red
	cP [11] = SDL_MapRGBA (_format, 0xff, 0x02, 0xfd, 0xe0); // Bright Magenta
	cP [12] = SDL_MapRGBA (_format, 0x00, 0xff, 0x1c, 0xe0); // Bright Green
	cP [13] = SDL_MapRGBA (_format, 0x02, 0xff, 0xff, 0xe0); // Bright Cyan
	cP [14] = SDL_MapRGBA (_format, 0xff, 0xff, 0x1d, 0xe0); // Bright Yellow
	cP [15] = SDL_MapRGBA (_format, 0xff, 0xff, 0xff, 0xe0); // Bright White

	return (new MCHEmul::ScreenMemory (numberColumns (), numberRows (), cP));
}

// --
void ZXSPECTRUM::ULA::readGraphicInfoAndDrawVisibleZone (MCHEmul::CPU* cpu)
{
	// Gets the position with in the visible zone...
	// That position will take into account the birder (top and left)...
	unsigned short x = 0, y = 0;
	_raster.currentVisiblePosition (x, y);

	// Draws a point with the background first
	// Unless the raster were in a display zone, it will be the only thing to be drawn...
	_screenMemory -> setPixel (x, y, _ULARegisters -> borderColor () & 0x07); // The bright has no effect in the border...

	// When the raster enters the display zone
	// all the info to control it, is reinitilized...
	if (_raster.vData ().currentPosition () == 0 && 
		_raster.hData ().currentPosition () == 0)
		_videoSignalData.initializeDisplayZone (); // ..everything except the flash info...

	// When the raster enters vertically the display zone...
	if (_raster.vData ().currentPosition () >= 0 && 
		_raster.vData ().currentPosition () < 192)
	{
		// This varianle only is true when the position is between 0 and 255...
		_videoSignalData._vidEN = 
			((_raster.hData ().currentPosition () >> 8) & 0x00ff) == 0x0000;
		// ...and in that case...
		if (_videoSignalData._vidEN.value ())
		{
			// load graphical and attribute data when it is the frist time
			// that variable comes from 0 to 1 or every 8 shifts...
			if ((_videoSignalData._vidEN.positiveEdge () | _videoSignalData.clock ()) == true)
			{
				_memory -> setActiveView (_ULAView);

				_videoSignalData._dataLatch = 
					_memory -> value (MCHEmul::Address 
						(2, (unsigned int) ((_raster.vData ().currentPosition () << 5) | (_raster.hData ().currentPosition () >> 3))));
				_videoSignalData._attributeLatch =
					_memory -> value (MCHEmul::Address ({ 0x00, 0x18 }, false) + 
						(_raster.vData ().currentPosition () >> 3) + (_raster.hData ().currentPosition () >> 3));

				_memory -> setCPUView ();
			}

			// But always draw the content of the poixels shifted...
			// but taking into account the flash attribute (1 every 32 frames)....
			// In this case for both PAPER and INK the bright attribute is taking into account...
			unsigned char brightVal = (_videoSignalData._attribute.value () & 0x40) ? 0x80 : 0x00;
			_screenMemory -> setPixel (x, y, unsigned int // To be adapted to the right estructure...
				((_videoSignalData._lastBitShifted ^ _videoSignalData._flash)
					? (_videoSignalData._attribute.value () & 0x07 | brightVal)
					: ((_videoSignalData._attribute.value () & 0x38) >> 3) | brightVal));
		}
	}

	if (_showEvents)
	{
		// TODO
	}
}

// ---
ZXSPECTRUM::ULA_PAL::ULA_PAL (int vV)
	: ZXSPECTRUM::ULA (_VRASTERDATA, _HRASTERDATA, vV,
		 { { "Name", "ULA" },
		  { "Code", "5C102E, 5C112E, 5C112E-2, 5C112E-3, 6C001E-5, 6C001E-6, 6C001E-7, 7K010E-5 ULA / Amstrad 40056, +2A/+3 Gate Array" },
		  { "Manufacturer", "Ferranti" },
		  { "Year", "1982-1985" } })
{
	// Nothing else...
}

// ---
ZXSPECTRUM::ULA_NTSC::ULA_NTSC (int vV)
	: ZXSPECTRUM::ULA (_VRASTERDATA, _HRASTERDATA, vV,
		 { { "Name", "ULA" },
		  { "Code", "5C102E, 5C112E, 5C112E-2, 5C112E-3, 6C001E-5, 6C001E-6, 6C001E-7, 7K010E-5 ULA / Amstrad 40056, +2A/+3 Gate Array" },
		  { "Manufacturer", "Ferranti" },
		  { "Year", "1982-1985" } })
{
	// Nothing else...
}
