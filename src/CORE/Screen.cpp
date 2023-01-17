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
	  _window (nullptr), _renderer (nullptr), _texture (nullptr),
	  _graphicsReady (false)
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
	for (MCHEmul::Chips::const_iterator i = c.begin (); i != c.end () && _graphicalChip == nullptr; 
		_graphicalChip = dynamic_cast <MCHEmul::GraphicalChip*> ((*i++).second));
	// It is mandatory!...
	assert (_graphicalChip != nullptr);

	// The events comming from the graphical chip are observed here!
	observe (_graphicalChip);
}

// ---
bool MCHEmul::Screen::initialize ()
{
	_graphicsReady = false;

	return (MCHEmul::IODevice::initialize ());
}

// ---
bool MCHEmul::Screen::simulate ()
{
	if (!MCHEmul::IODevice::simulate ())
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
		SDL_UpdateTexture (_texture, nullptr, 
			_graphicalChip -> screenMemory () -> frameData (), 
			(int) _graphicalChip -> screenMemory () -> columns () * sizeof (unsigned int)); // The link with the chip...
		SDL_RenderClear (_renderer);
		SDL_RenderCopy (_renderer, _texture, nullptr, nullptr);
		SDL_RenderPresent (_renderer);

		_graphicsReady = false;
	}

	_clock.countCycles (1);

	return (true);
}

// ---
void MCHEmul::Screen::processEvent (MCHEmul::Event&& evnt, MCHEmul::Notifier* n)
{
	if (evnt.id () == MCHEmul::Screen::_GRAPHICSREADY)
		_graphicsReady = true; // To be processed in simulation...
}
