#include <CORE/Screen.hpp>

// ---
MCHEmul::Screen::Screen (const std::string& n, int id, 
		unsigned int sc, unsigned int sr, unsigned int vF, unsigned int nC, double hz,
		const Attributes& attrs)
	: MCHEmul::IODevice (MCHEmul::IODevice::Type::_OUTPUT, id, attrs),
	  _screenName (n), 
	  _screenCols (sc), _screenRows (sr), _visibilityFactor (vF), 
	  _numberColors (nC), _colorPalette (nullptr),
	  _hertzs (hz),
	  _window (nullptr), _renderer (nullptr), _texture (nullptr), _format (nullptr), _frame (nullptr),
	  _refreshRate (0)
{
	assert (_numberColors > 0);
	assert (_hertzs > 0);

	_colorPalette = new unsigned int [_numberColors];

	SDL_Init (SDL_INIT_VIDEO);

	_window = SDL_CreateWindow (
		_screenName.c_str (),
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		_screenCols * _visibilityFactor, 
		// Maybe one point in certain condition can be represented as 2 and it would need more space...
		_screenRows * _visibilityFactor,
		SDL_WINDOW_OPENGL
	);

	_renderer = SDL_CreateRenderer (_window, -1, SDL_RENDERER_ACCELERATED);
	_texture  = SDL_CreateTexture	
		(_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, _screenCols, _screenRows);
	_format = SDL_AllocFormat (SDL_PIXELFORMAT_ARGB8888);

	_frame  = new uint32_t [_screenCols * _screenRows]();

	_refreshRate = 1 / _hertzs;
}

// ---
MCHEmul::Screen::~Screen ()
{
	delete (_colorPalette);

	delete [] (_frame);

	SDL_DestroyRenderer (_renderer);
	SDL_DestroyTexture (_texture);
	SDL_FreeFormat (_format);

	SDL_Quit ();
}

// ---
bool MCHEmul::Screen::initialize ()
{
	initializeColorPalette ();

	return (true);
}

// ---
bool MCHEmul::Screen::refresh ()
{
  SDL_UpdateTexture (_texture, nullptr, _frame, _screenCols * sizeof (uint32_t));
  SDL_RenderClear (_renderer);
  SDL_RenderCopy (_renderer, _texture, nullptr, nullptr);
  SDL_RenderPresent (_renderer);

  return (true);
}
