#include <CORE/Screen.hpp>
#include <CORE/GraphicalChip.hpp>
#include <CORE/SDLImage.hpp>
#include <limits>

// ---
MCHEmul::Screen::Screen (const std::string& n, int id, 
		unsigned int sc, unsigned int sr, double vFX, double vFY, double hz,
		const Attributes& attrs)
	: MCHEmul::IODevice (MCHEmul::IODevice::Type::_OUTPUT, id, attrs),
	  _CRTActive (false), // It is not active by default...
	  _drawGrid (false), _gridColor (0),
	  _screenName (n), 
	  _screenColumns (sc), _screenRows (sr), _visibilityFactorX (vFX), _visibilityFactorY (vFY),
	  _hertzs (hz),
	  _clock ((unsigned int) hz,
		hz / (double) (unsigned int) hz),
	  _graphicalChip (nullptr),
	  _window (nullptr), _renderer (nullptr), _texture (nullptr),
	  _graphicsReady (false),
	  _graphicsDef (), _defaultGraphicDef () // Must be defined in every screen emulation is needed...
{
	assert (_hertzs > 0);

	setClassName ("Screen");

#if MCHEMUL_SDL_VERSION == 2
	_window = SDL_CreateWindow (
		_screenName.c_str (), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		(int) ((double) _screenColumns * _visibilityFactorX),
		(int) ((double) _screenRows * _visibilityFactorY),
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
#elif MCHEMUL_SDL_VERSION == 3
	_window = SDL_CreateWindow (
		_screenName.c_str (),
		(int) ((double) _screenColumns * _visibilityFactorX),
		(int) ((double) _screenRows * _visibilityFactorY), SDL_WINDOW_RESIZABLE);
#endif
	if (_window == nullptr)
	{
		_error = MCHEmul::_INIT_ERROR;
		return;
	}

#if MCHEMUL_SDL_VERSION == 2
	_renderer = SDL_CreateRenderer (_window, -1, SDL_RENDERER_ACCELERATED);
#elif MCHEMUL_SDL_VERSION == 3
	_renderer = SDL_CreateRenderer (_window, nullptr);
#endif
	if (_renderer == nullptr)
	{
		_error = MCHEmul::_INIT_ERROR;
		return;
	}

	// Keep the framebuffer aspect ratio when resizing the output window.
#if MCHEMUL_SDL_VERSION == 2
	bool result = SDL_RenderSetLogicalSize (_renderer, (int) _screenColumns, (int) _screenRows) == 0 &&
		SDL_SetRenderDrawBlendMode (_renderer, SDL_BLENDMODE_BLEND) == 0;
	SDL_SetHint (SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
#elif MCHEMUL_SDL_VERSION == 3
	bool result = SDL_SetRenderLogicalPresentation (_renderer,
		(int) _screenColumns, (int) _screenRows, SDL_LOGICAL_PRESENTATION_LETTERBOX) &&
		SDL_SetRenderDrawBlendMode (_renderer, SDL_BLENDMODE_BLEND);
#endif

	if (!result)
	{
		_error = MCHEmul::_INIT_ERROR;

		return;
	}

	_texture = SDL_CreateTexture (_renderer, SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING, (int) _screenColumns, (int) _screenRows);
	if (_texture == nullptr)
	{
		_error = MCHEmul::_INIT_ERROR;

		return;
	}

#if MCHEMUL_SDL_VERSION == 2
	result = SDL_SetTextureBlendMode (_texture, SDL_BLENDMODE_BLEND) == 0;
#elif MCHEMUL_SDL_VERSION == 3
	result = SDL_SetTextureBlendMode (_texture, SDL_BLENDMODE_BLEND) &&
		SDL_SetTextureScaleMode (_texture, SDL_SCALEMODE_NEAREST);
#endif

	if (!result)
		_error = MCHEmul::_INIT_ERROR;
}

// ---
MCHEmul::Screen::~Screen ()
{
	SDL_DestroyTexture (_texture);

	SDL_DestroyRenderer (_renderer);

	SDL_DestroyWindow (_window);
}

// ---
void MCHEmul::Screen::setCRTEffect (bool a)
{
	if (_texture == nullptr)
		return; // Construction failed; initialize will report the error.

#if MCHEMUL_SDL_VERSION == 2
	SDL_SetHintWithPriority (SDL_HINT_RENDER_SCALE_QUALITY,
		a ? "linear" : "nearest", SDL_HINT_OVERRIDE);

	// SDL2 applies the hint when creating the texture. Keep the old one until ready.
	SDL_Texture* texture = SDL_CreateTexture (_renderer, SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING, (int) _screenColumns, (int) _screenRows);
	if (texture == nullptr)
	{
		_error = MCHEmul::_DEVICE_ERROR;

		return;
	}

	if (SDL_SetTextureBlendMode (texture, SDL_BLENDMODE_BLEND) != 0)
	{
		SDL_DestroyTexture (texture);

		_error = MCHEmul::_DEVICE_ERROR;

		return;
	}

	SDL_DestroyTexture (_texture);
	_texture = texture;
#elif MCHEMUL_SDL_VERSION == 3
	if (!SDL_SetTextureScaleMode (_texture, a ? SDL_SCALEMODE_LINEAR : SDL_SCALEMODE_NEAREST))
	{
		_error = MCHEmul::_DEVICE_ERROR;

		return;
	}
#endif

	_CRTActive = a;
}

// ---
bool MCHEmul::Screen::takePicture (const std::string& fN) const
{
	size_t extensionPosition = fN.find_last_of (".");
	if (extensionPosition == std::string::npos)
		return (false);
	std::string ext = MCHEmul::upper (fN.substr (extensionPosition + 1));
	if (ext != "BMP" && ext != "PNG" && ext != "JPG" && ext != "JPEG")
		return (false);

#if MCHEMUL_SDL_VERSION == 2
	SDL_Surface* surface = capturePictureSDL2 ();
#elif MCHEMUL_SDL_VERSION == 3
	SDL_Surface* surface = capturePictureSDL3 ();
#endif
	if (surface == nullptr)
		return (false);

	bool result = false;
#if MCHEMUL_SDL_VERSION == 2
	if (ext == "BMP")
		result = SDL_SaveBMP (surface, fN.c_str ()) == 0;
	else if (ext == "PNG")
		result = IMG_SavePNG (surface, fN.c_str ()) == 0;
	else
		result = IMG_SaveJPG (surface, fN.c_str (), 95) == 0;

	SDL_FreeSurface (surface);
#elif MCHEMUL_SDL_VERSION == 3
	if (ext == "BMP")
		result = SDL_SaveBMP (surface, fN.c_str ());
	else if (ext == "PNG")
		result = IMG_SavePNG (surface, fN.c_str ());
	else
		result = IMG_SaveJPG (surface, fN.c_str (), 95);

	SDL_DestroySurface (surface);
#endif
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
	if (_error != MCHEmul::_NOERROR)
		return (false);

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
		if (!adaptToFrameBuffer ())
			return (false);

		// At this point the screen memory has the "computer screen" drawn
		// it is needed to move to the real screen later using the raster...
		// Additional things could be drawn on top of the "computer screen"...
		drawAdditional ();
		// That "computer screen" is then moved to the texture 
		// that will be moved into the screen eventually...
#if MCHEMUL_SDL_VERSION == 2
		bool result = SDL_UpdateTexture (_texture, nullptr,
			_graphicalChip -> screenMemory () -> frameData (),
			(int) (_graphicalChip -> screenMemory () -> columns () * sizeof (unsigned int))) == 0;
#elif MCHEMUL_SDL_VERSION == 3
		bool result = SDL_UpdateTexture (_texture, nullptr,
			_graphicalChip -> screenMemory () -> frameData (),
			(int) (_graphicalChip -> screenMemory () -> columns () * sizeof (unsigned int)));
#endif

		// Draw the CRT lines first, then blend the partially transparent framebuffer.
#if MCHEMUL_SDL_VERSION == 2
		result = result && SDL_SetRenderDrawColor (_renderer, 0x00, 0x00, 0x00, 0x00) == 0 &&
			SDL_RenderClear (_renderer) == 0;
#elif MCHEMUL_SDL_VERSION == 3
		result = result && SDL_SetRenderDrawColor (_renderer, 0x00, 0x00, 0x00, 0x00) &&
			SDL_RenderClear (_renderer);
#endif
		if (result && _CRTActive)
		{
#if MCHEMUL_SDL_VERSION == 2
			result = SDL_SetRenderDrawColor (_renderer, 0x30, 0x30, 0x30, 0xe0) == 0;
#elif MCHEMUL_SDL_VERSION == 3
			result = SDL_SetRenderDrawColor (_renderer, 0x30, 0x30, 0x30, 0xe0);
#endif
			for (int i = 0; result && i < (int) _screenRows; i += 2)
#if MCHEMUL_SDL_VERSION == 2
				result = SDL_RenderDrawLine (_renderer, 0, i, (int) _screenColumns, i) == 0;
#elif MCHEMUL_SDL_VERSION == 3
				result = SDL_RenderLine (_renderer, 0.0f, (float) i, (float) _screenColumns, (float) i);
#endif
		}

#if MCHEMUL_SDL_VERSION == 2
		result = result && SDL_RenderCopy (_renderer, _texture, nullptr, nullptr) == 0;
		if (result)
			SDL_RenderPresent (_renderer);
#elif MCHEMUL_SDL_VERSION == 3
		result = result && SDL_RenderTexture (_renderer, _texture, nullptr, nullptr) &&
			SDL_RenderPresent (_renderer);
#endif

		if (!result)
		{
			_error = MCHEmul::_DEVICE_ERROR;

			return (false);
		}

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
void MCHEmul::Screen::setPresentationFrequency (double hz)
{
	assert (hz > 0);

	if (_hertzs == hz)
		return;

	_hertzs = hz;
	_clock.setFactor (hz / (double) _clock.cyclesPerSecond ());
}

// ---
void MCHEmul::Screen::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	if (evnt.id () == MCHEmul::GraphicalChip::_GRAPHICSREADY)
		_graphicsReady = true; // To be processed in simulation...
}

// ---
bool MCHEmul::Screen::adaptToFrameBuffer ()
{
	const MCHEmul::ScreenMemory* screenMemory = _graphicalChip -> screenMemory ();
	const unsigned int columns = (unsigned int) screenMemory -> columns ();
	const unsigned int rows = (unsigned int) screenMemory -> rows ();
	if (columns == _screenColumns && rows == _screenRows)
		return (true);

	// Replace the texture only when a frame in the new geometry is ready.
	SDL_Texture* texture = SDL_CreateTexture
		(_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
		 (int) columns, (int) rows);
	if (texture == nullptr)
	{
		_error = MCHEmul::_DEVICE_ERROR;
		return (false);
	}

#if MCHEMUL_SDL_VERSION == 2
	bool result = SDL_SetTextureBlendMode (texture, SDL_BLENDMODE_BLEND) == 0 &&
		SDL_RenderSetLogicalSize (_renderer, (int) columns, (int) rows) == 0;
#elif MCHEMUL_SDL_VERSION == 3
	bool result = SDL_SetTextureBlendMode (texture, SDL_BLENDMODE_BLEND) &&
		SDL_SetTextureScaleMode (texture, _CRTActive ? SDL_SCALEMODE_LINEAR : SDL_SCALEMODE_NEAREST) &&
		SDL_SetRenderLogicalPresentation (_renderer,
			(int) columns, (int) rows, SDL_LOGICAL_PRESENTATION_LETTERBOX);
#endif

	if (!result)
	{
		SDL_DestroyTexture (texture);

		_error = MCHEmul::_DEVICE_ERROR;

		return (false);
	}

	SDL_DestroyTexture (_texture);
	_texture = texture;
	_screenColumns = columns;
	_screenRows = rows;

	return (true);
}

#if MCHEMUL_SDL_VERSION == 2
// ---
SDL_Surface* MCHEmul::Screen::capturePictureSDL2 () const
{
	if (_renderer == nullptr || _texture == nullptr)
		return (nullptr);

	int w, h;
	unsigned int format;
	if (SDL_QueryTexture (_texture, &format, nullptr, &w, &h) != 0 ||
		w <= 0 || h <= 0 || w > std::numeric_limits <int>::max () / 4 ||
		h > std::numeric_limits <int>::max () / 4)
		return (nullptr);
	w *= 4; h *= 4;

	unsigned char r, g, b, a;
	if (SDL_GetRenderDrawColor (_renderer, &r, &g, &b, &a) != 0)
		return (nullptr);
	SDL_Texture* previousTarget = SDL_GetRenderTarget (_renderer);
	SDL_Texture* texture = SDL_CreateTexture (_renderer, format, SDL_TEXTUREACCESS_TARGET, w, h);
	if (texture == nullptr)
		return (nullptr);
	if (SDL_SetRenderTarget (_renderer, texture) != 0)
	{
		SDL_DestroyTexture (texture);

		return (nullptr);
	}

	// Blend onto known transparent pixels rather than uninitialized target contents.
	SDL_Surface* surface = nullptr;
	bool result = SDL_SetRenderDrawColor (_renderer, 0, 0, 0, 0) == 0 &&
		SDL_RenderClear (_renderer) == 0 &&
		SDL_RenderCopy (_renderer, _texture, nullptr, nullptr) == 0;
	if (result)
	{
		surface = SDL_CreateRGBSurfaceWithFormat (0, w, h, 32, format);
		result = surface != nullptr && SDL_RenderReadPixels (_renderer, nullptr,
			format, surface -> pixels, surface -> pitch) == 0;
	}

	// Restore the renderer before releasing the temporary target, even after capture failure.
	if (SDL_SetRenderTarget (_renderer, previousTarget) != 0)
		result = false;
	if (SDL_SetRenderDrawColor (_renderer, r, g, b, a) != 0)
		result = false;
	SDL_DestroyTexture (texture);
	if (!result && surface != nullptr)
	{
		SDL_FreeSurface (surface);
		surface = nullptr;
	}

	return (surface);
}
#elif MCHEMUL_SDL_VERSION == 3
// ---
SDL_Surface* MCHEmul::Screen::capturePictureSDL3 () const
{
	if (_renderer == nullptr || _texture == nullptr)
		return (nullptr);

	float width, height;
	if (!SDL_GetTextureSize (_texture, &width, &height) ||
		width <= 0.0f || height <= 0.0f ||
		(double) width > (double) (std::numeric_limits <int>::max () / 4) ||
		(double) height > (double) (std::numeric_limits <int>::max () / 4))
		return (nullptr);
	int w = (int) width * 4, h = (int) height * 4;
	SDL_PropertiesID properties = SDL_GetTextureProperties (_texture);
	if (properties == 0)
		return (nullptr);
	SDL_PixelFormat format = static_cast <SDL_PixelFormat>
		(SDL_GetNumberProperty (properties, SDL_PROP_TEXTURE_FORMAT_NUMBER, SDL_PIXELFORMAT_UNKNOWN));
	if (format == SDL_PIXELFORMAT_UNKNOWN)
		return (nullptr);

	unsigned char r, g, b, a;
	if (!SDL_GetRenderDrawColor (_renderer, &r, &g, &b, &a))
		return (nullptr);
	SDL_Texture* previousTarget = SDL_GetRenderTarget (_renderer);
	SDL_Texture* texture = SDL_CreateTexture (_renderer, format, SDL_TEXTUREACCESS_TARGET, w, h);
	if (texture == nullptr)
		return (nullptr);
	if (!SDL_SetRenderTarget (_renderer, texture))
	{
		SDL_DestroyTexture (texture);

		return (nullptr);
	}

	// Logical presentation belongs to each target in SDL3. Capture the entire enlarged texture.
	SDL_Surface* surface = nullptr;
	bool result = SDL_SetRenderLogicalPresentation (_renderer, w, h, SDL_LOGICAL_PRESENTATION_DISABLED) &&
		SDL_SetRenderDrawColor (_renderer, 0, 0, 0, 0) && SDL_RenderClear (_renderer) &&
		SDL_RenderTexture (_renderer, _texture, nullptr, nullptr);
	if (result)
	{
		surface = SDL_RenderReadPixels (_renderer, nullptr);
		result = surface != nullptr;
	}

	// Restore the renderer before releasing the temporary target, even after capture failure.
	if (!SDL_SetRenderTarget (_renderer, previousTarget))
		result = false;
	if (!SDL_SetRenderDrawColor (_renderer, r, g, b, a))
		result = false;
	SDL_DestroyTexture (texture);
	if (!result && surface != nullptr)
	{
		SDL_DestroySurface (surface);
		surface = nullptr;
	}

	return (surface);
}
#endif
