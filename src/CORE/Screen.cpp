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

	// The render...
	_renderer = SDL_CreateRenderer (_window, -1, SDL_RENDERER_ACCELERATED);
	// The background is black...
	SDL_SetRenderDrawColor (_renderer, 0, 0, 0, 255);
	// Just to maintain always the aspect ratio even when the size of the output window is changed...
	SDL_RenderSetLogicalSize (_renderer, _screenColumns, _screenRows);

	// The texture drawn in the render zone...
	_texture  = SDL_CreateTexture
		(_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, _screenColumns, _screenRows);

	setClassName ("Screen");
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
	// If there is a screen it is mandatory to have a graphical chip attached...
	assert (_graphicalChip != nullptr);

	// The events comming from the graphical chip are observed here to update the screen...
	observe (_graphicalChip);
}

// ---
bool MCHEmul::Screen::initialize ()
{
	_graphicsReady = false;

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
		drawAdditional ();

		SDL_UpdateTexture (_texture, nullptr, 
			_graphicalChip -> screenMemory () -> frameData (), 
			(int) _graphicalChip -> screenMemory () -> columns () * sizeof (unsigned int)); // The link with the chip...
		SDL_RenderClear (_renderer);
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
