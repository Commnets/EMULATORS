#include <C64/Screen.hpp>
#include <C64/VICII.hpp>
#include <SDL.h>

// ---
bool C64::Screen::initialize ()
{
	if (!MCHEmul::Screen::initialize ())
		return (false);

	// Try to localize the VICii chip in the list of chips...
	for (MCHEmul::Chips::const_iterator i = _chips.begin (); 
		i != _chips.end () && _vicII == nullptr; i++)
		_vicII = dynamic_cast <C64::VICII*> ((*i).second);

	return ((_vicII == nullptr) ? false : true);
}

// ---
void C64::Screen::initializeColorPalette ()
{
	_colorPalette [0]   = SDL_MapRGB (_format, 0x00, 0x00, 0x00);
	_colorPalette [1]   = SDL_MapRGB (_format, 0xff, 0xff, 0xff);
	_colorPalette [2]   = SDL_MapRGB (_format, 0xab, 0x31, 0x26);
	_colorPalette [3]   = SDL_MapRGB (_format, 0x66, 0xda, 0xff);
	_colorPalette [4]   = SDL_MapRGB (_format, 0xbb, 0x3f, 0xb8);
	_colorPalette [5]   = SDL_MapRGB (_format, 0x55, 0xce, 0x58);
	_colorPalette [6]   = SDL_MapRGB (_format, 0x1d, 0x0e, 0x97);
	_colorPalette [7]   = SDL_MapRGB (_format, 0xea, 0xf5, 0x7c);
	_colorPalette [8]   = SDL_MapRGB (_format, 0xb9, 0x74, 0x18);
	_colorPalette [9]   = SDL_MapRGB (_format, 0x78, 0x53, 0x00);
	_colorPalette [10]  = SDL_MapRGB (_format, 0xdd, 0x93, 0x87);
	_colorPalette [11]  = SDL_MapRGB (_format, 0x5b, 0x5b, 0x5b);
	_colorPalette [12]  = SDL_MapRGB (_format, 0x8b, 0x8b, 0x8b);
	_colorPalette [13]  = SDL_MapRGB (_format, 0xb0, 0xf4, 0xac);
	_colorPalette [14]  = SDL_MapRGB (_format, 0xaa, 0x9d, 0xef);
	_colorPalette [15]  = SDL_MapRGB (_format, 0xb8, 0xb8, 0xb8);
}
