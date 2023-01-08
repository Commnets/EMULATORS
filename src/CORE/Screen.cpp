#include <CORE/Screen.hpp>
#include <CORE/GraphicalChip.hpp>

// ---
MCHEmul::Screen::Screen (const std::string& n, int id, 
		unsigned int sc, unsigned int sr, unsigned int vF, double hz,
		const Attributes& attrs)
	: MCHEmul::IODevice (MCHEmul::IODevice::Type::_OUTPUT, id, attrs),
	  _screenName (n), 
	  _screenColumns (sc), _screenRows (sr), _visibilityFactor (vF), 
	  _hertzs (hz), _clock ((unsigned int) hz /** integer. */),
	  _graphicalChip (nullptr),
	  _window (nullptr), _renderer (nullptr), _texture (nullptr)
{
	assert (_hertzs > 0);

	_window = SDL_CreateWindow (
		_screenName.c_str (),
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		_screenColumns * _visibilityFactor, 
		// Maybe one point in certain condition can be represented as 2 and it would need more space...
		_screenRows * _visibilityFactor,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
	);

	_renderer = SDL_CreateRenderer (_window, -1, SDL_RENDERER_ACCELERATED);
	_texture  = SDL_CreateTexture	
		(_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, _screenColumns, _screenRows);
}

// ---
MCHEmul::Screen::~Screen ()
{
	SDL_DestroyRenderer (_renderer);
	SDL_DestroyTexture (_texture);
}

// ---
void MCHEmul::Screen::linkToChips (const MCHEmul::Chips& c)
{
	for (const auto& i : c)
		if ((_graphicalChip = dynamic_cast <MCHEmul::GraphicalChip*> (i.second)) != nullptr)
			break;

	// It is mandatory!...
	assert (_graphicalChip != nullptr);
}

// ---
bool MCHEmul::Screen::simulate ()
{
	if (!MCHEmul::IODevice::simulate ())
		return (false);

	if (_clock.realCyclesPerSecond () > _clock.cyclesPerSecond ())
	{
		_clock.countCycles (0);

		return (true); // The cycle was not executed, but everything went ok...
	}

	if (_graphicalChip -> graphicsReady ())
	{
		SDL_UpdateTexture (_texture, nullptr, 
			_graphicalChip -> screenMemory () -> frameData (), 
			(int) _graphicalChip -> screenMemory () -> columns () * sizeof (unsigned int)); // The link with the chip...
		SDL_RenderClear (_renderer);
		SDL_RenderCopy (_renderer, _texture, nullptr, nullptr);
		SDL_RenderPresent (_renderer);

		_graphicalChip -> setGraphicsReady (false);

		_clock.countCycles (1);
	}

	return (true);
}
