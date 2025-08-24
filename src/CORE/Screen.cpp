#include <CORE/Screen.hpp>
#include <CORE/GraphicalChip.hpp>
#include <SDL_image.h>

// ---
MCHEmul::Screen::Screen (const std::string& n, int id, 
		unsigned int sc, unsigned int sr, double vFX, double vFY, double hz,
		const Attributes& attrs)
	: MCHEmul::IODevice (MCHEmul::IODevice::Type::_OUTPUT, id, attrs),
	  _CRTActive (false), // It is not active by default...
	  _drawGrid (false), _gridColor (0),
	  _screenName (n), 
	  _screenColumns (sc), _screenRows (sr), _visibilityFactorX (vFX), _visibilityFactorY (vFY),
	  _hertzs (hz), _clock ((unsigned int) hz /** integer. */),
	  _graphicalChip (nullptr),
	  _window (nullptr), _renderer (nullptr), _texture (nullptr),
	  _graphicsReady (false),
	  _graphicsDef (), _defaultGraphicDef () // Must be defined in every screen emulation is needed...
{
	assert (_hertzs > 0);

	_window = SDL_CreateWindow (
		_screenName.c_str (),
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		(unsigned int) ((double) _screenColumns * _visibilityFactorX), 
		// Maybe one point in certain condition can be represented as 2 and it would need more space...
		(unsigned int) ((double) _screenRows * _visibilityFactorY),
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
	);

	// The render...
	_renderer = SDL_CreateRenderer (_window, -1, SDL_RENDERER_ACCELERATED);
	// Just to maintain always the aspect ratio even when the size of the output window is changed...
	SDL_RenderSetLogicalSize (_renderer, _screenColumns, _screenRows);
	// For the draw lines...
	SDL_SetRenderDrawBlendMode (_renderer, SDL_BLENDMODE_BLEND);
	// CRT effect... No blur by default...
	SDL_SetHint (SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

	// The texture drawn in the render zone...
	_texture  = SDL_CreateTexture
		(_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, (int) _screenColumns, (int) _screenRows);
	SDL_SetTextureBlendMode (_texture, SDL_BLENDMODE_BLEND);

	setClassName ("Screen");
}

// ---
MCHEmul::Screen::~Screen ()
{
	SDL_DestroyRenderer (_renderer);

	SDL_DestroyTexture (_texture);
}

// ---
void MCHEmul::Screen::setCRTEffect (bool a)
{ 
	_CRTActive = a;

	SDL_SetHintWithPriority (SDL_HINT_RENDER_SCALE_QUALITY, 
		_CRTActive ? "linear" : "nearest", SDL_HINT_OVERRIDE);
	
	// When the render system is changed
	// The texture has to be recrestaed for the change to take place...
	SDL_DestroyTexture (_texture);
	_texture  = SDL_CreateTexture
		(_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, (int) _screenColumns, (int) _screenRows);
	SDL_SetTextureBlendMode (_texture, SDL_BLENDMODE_BLEND);
}

// ---
bool MCHEmul::Screen::takePicture (const std::string& fN) const
{
    // Get Texture dimensions...
	int w, h;
    Uint32 format;
	int access;
	if (SDL_QueryTexture (_texture, &format, &access, &w, &h) != 0)
		return (false);
	// Usually the emulators managed a "small" screen...
	w *= 4; h *= 4;

	// Save the previous render target...
    SDL_Texture* prevTarget = SDL_GetRenderTarget (_renderer);

	// PHASE 1: Creates a copy of the current texture...
	// ...but to make it accesible later!
    SDL_Texture* newTexture = SDL_CreateTexture 
		(_renderer, format /** Same format than the original. */, 
		 SDL_TEXTUREACCESS_TARGET /** Accesible later for RenderReadPizels. */, w, h);
	if (newTexture == nullptr)
		return (false); // Nothing has changed so far...

	// The new target of the render is the new texture...
	// ...but if it can not be created...
	if (SDL_SetRenderTarget (_renderer, newTexture) != 0)
	{ 
		SDL_DestroyTexture (newTexture); // ...destroy the texture before leaving...
		return (false); 
	}

	// Now render the the original texture into the new one...
	// But, again, it it is not possible, everything has to be destroyed...
    if (SDL_RenderCopy (_renderer, _texture, nullptr, nullptr) != 0)
	{ 
		SDL_DestroyTexture (newTexture); 
		SDL_SetRenderTarget (_renderer, prevTarget); // ...and restore the previous target...
		return (false); 
	}

	// PHASE 2: Now makes a copy of the texture in a surface...
	// ..because it is the only way to save it later...
	SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat 
		(0, w, h, 32, format /** Same format than the original. */);
	// If the surface can not be created...
	if (surface == nullptr)
	{ 
		SDL_DestroyTexture (newTexture); // ...destroy the texture before leaving...
		SDL_SetRenderTarget (_renderer, prevTarget); // ...and restore the previous target...
		return (false); 
	}

	// Render the pixels of the texture into the surface...
	// If is not possible...
    if (SDL_RenderReadPixels (_renderer, nullptr, 
			surface -> format -> format,
			surface -> pixels,
			surface -> pitch) != 0) 
	{ 
		SDL_DestroyTexture (newTexture); 
		SDL_FreeSurface (surface); // ...destroy also the surface before leaving...
		SDL_SetRenderTarget (_renderer, prevTarget); 
		return (false); 
	}

	// The texture copied is no longer needed...
	SDL_DestroyTexture (newTexture);
    // And also rstore the render target...
	SDL_SetRenderTarget (_renderer, prevTarget);

	// PHASE 3: Now it is time to save the picture...
	// Which is the format to save?
	bool result = true;
	std::string ext = MCHEmul::upper (fN.substr (fN.find_last_of (".") + 1));
	if (ext == "PNG" || ext == "JPG" || ext == "JPEG" || ext == "BMP")
	{
		// Save the picture, in the format requested...
		if (ext == "BMP")
			result = SDL_SaveBMP (surface, fN.c_str ()) ? false : true;
		else if (ext == "JPG" || ext == "JPEG")
			result = IMG_SaveJPG (surface, fN.c_str (), 95) ? false : true; // Quality 95%
		else if (ext == "PNG") // Just in case...
			result = IMG_SavePNG (surface, fN.c_str ()) ? false : true;
	}

	// The surface is no longer used...
    SDL_FreeSurface (surface);

	return (result);
}

// ---
void MCHEmul::Screen::linkToChips (const MCHEmul::Chips& c)
{
	for (MCHEmul::Chips::const_iterator i = c.begin (); i != c.end () && _graphicalChip == nullptr; 
		_graphicalChip = dynamic_cast <MCHEmul::GraphicalChip*> ((*i++).second));
	// If there is a screen it is mandatory to have a graphical chip attached...
	assert (_graphicalChip != nullptr);

	// The events comming from the graphical chip are observed here to update the screen...
	observe (_graphicalChip);
}

// ---
bool MCHEmul::Screen::initialize ()
{
	_graphicsReady = false;

	_clock.start ();

	return (MCHEmul::IODevice::initialize ());
}

// ---
bool MCHEmul::Screen::simulate (MCHEmul::CPU* cpu)
{
	if (!MCHEmul::IODevice::simulate (cpu))
		return (false);

	if (_clock.tooQuick ())
	{
		_clock.countCycles (0);

		return (true); // The cycle was not executed, but everything went ok...
	}

	/** This is when the graphics have already been drawn,
		so they are ready to be pull out to the screen. */
	if (_graphicsReady)
	{
		// At this point the screen memory has the "computer screen" drawn
		// it is needed to move to the real screen later using the raster...
		// Additional things could be drawn on top of the "computer screen"...
		drawAdditional ();
		// That "computer screen" is then moved to the texture 
		// that will be moved into the screen eventually...
		SDL_UpdateTexture (_texture, nullptr, 
			_graphicalChip -> screenMemory () -> frameData (), 
			(int) _graphicalChip -> screenMemory () -> columns () * sizeof (unsigned int)); // The link with the chip...

		// Time to render...
		// First the background color (with Alpha in totally transparent to allow blending)
		SDL_SetRenderDrawColor (_renderer, 0x00, 0x00, 0x00, 0x00);
		SDL_RenderClear (_renderer);
		// The effect is redered first f any...
		if (_CRTActive)
		{
			SDL_SetRenderDrawColor (_renderer, 0x30, 0x30, 0x30, 0xe0);
			for (int i = 0; i < (int) _screenRows; i += 2)
				SDL_RenderDrawLine (_renderer, 0, i, (int) _screenColumns, i);
		}
		// ...and then the textur blended...
		// The colors of the texture are partialy transparents....
		SDL_RenderCopy (_renderer, _texture, nullptr, nullptr);
		SDL_RenderPresent (_renderer);

		_graphicsReady = false;

		_clock.countCycles (1);
	}
	else
		_clock.countCycles (0);

	return (true);
}

// ---
MCHEmul::InfoStructure MCHEmul::Screen::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::IODevice::getInfoStructure ());

	result.add ("NAME",		_screenName);
	result.add ("HERTZS",	std::to_string (_clock.realCyclesPerSecond ()));
	result.add ("COLUMNS",	std::to_string (_screenColumns));
	result.add ("ROWS",		std::to_string (_screenRows));

	return (result);
}

// ---
void MCHEmul::Screen::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	if (evnt.id () == MCHEmul::GraphicalChip::_GRAPHICSREADY)
		_graphicsReady = true; // To be processed in simulation...
}
