#include <ZXSpectrum/OSIO.hpp>
#include <ZXSpectrum/ULA.hpp>

// ---
const ZXSPECTRUM::InputOSSystem::KeystrockesMap ZXSPECTRUM::InputOSSystem::_ZX81KEYS
	(
		{
			// Row 0
			{ SDL_SCANCODE_LSHIFT,			{ std::make_pair (0, 0) } },
			{ SDL_SCANCODE_RSHIFT,			{ std::make_pair (0, 0) } },	// The same...
			{ SDL_SCANCODE_Z,				{ std::make_pair (0, 1) } },
			{ SDL_SCANCODE_X,				{ std::make_pair (0, 2) } },
			{ SDL_SCANCODE_C,				{ std::make_pair (0, 3) } },
			{ SDL_SCANCODE_V,				{ std::make_pair (0, 4) } },

			// Row 1
			{ SDL_SCANCODE_A,				{ std::make_pair (1, 0) } },
			{ SDL_SCANCODE_S,				{ std::make_pair (1, 1) } },
			{ SDL_SCANCODE_D,				{ std::make_pair (1, 2) } },
			{ SDL_SCANCODE_F,				{ std::make_pair (1, 3) } },
			{ SDL_SCANCODE_G,				{ std::make_pair (1, 4) } },

			// Row 2
			{ SDL_SCANCODE_Q,				{ std::make_pair (2, 0) } },
			{ SDL_SCANCODE_W,				{ std::make_pair (2, 1) } },
			{ SDL_SCANCODE_E,				{ std::make_pair (2, 2) } },
			{ SDL_SCANCODE_R,				{ std::make_pair (2, 3) } },
			{ SDL_SCANCODE_T,				{ std::make_pair (2, 4) } },

			// Row 3
			{ SDL_SCANCODE_1,				{ std::make_pair (3, 0) } },
			{ SDL_SCANCODE_2,				{ std::make_pair (3, 1) } },
			{ SDL_SCANCODE_3,				{ std::make_pair (3, 2) } },
			{ SDL_SCANCODE_4,				{ std::make_pair (3, 3) } },
			{ SDL_SCANCODE_5,				{ std::make_pair (3, 4) } },

			// Row 4
			{ SDL_SCANCODE_0,				{ std::make_pair (4, 0) } },
			{ SDL_SCANCODE_9,				{ std::make_pair (4, 1) } },
			{ SDL_SCANCODE_8,				{ std::make_pair (4, 2) } },
			{ SDL_SCANCODE_7,				{ std::make_pair (4, 3) } },
			{ SDL_SCANCODE_6,				{ std::make_pair (4, 4) } },

			// Row 5
			{ SDL_SCANCODE_P,				{ std::make_pair (5, 0) } },
			{ SDL_SCANCODE_O,				{ std::make_pair (5, 1) } },
			{ SDL_SCANCODE_I,				{ std::make_pair (5, 2) } },
			{ SDL_SCANCODE_U,				{ std::make_pair (5, 3) } },
			{ SDL_SCANCODE_Y,				{ std::make_pair (5, 4) } },

			// Row 6
			{ SDL_SCANCODE_RETURN,			{ std::make_pair (6, 0) } },
			{ SDL_SCANCODE_L,				{ std::make_pair (6, 1) } },
			{ SDL_SCANCODE_K,				{ std::make_pair (6, 2) } },
			{ SDL_SCANCODE_J,				{ std::make_pair (6, 3) } },
			{ SDL_SCANCODE_H,				{ std::make_pair (6, 4) } },

			// Row 7
			{ SDL_SCANCODE_SPACE,			{ std::make_pair (7, 0) } },
			{ SDL_SCANCODE_LALT,			{ std::make_pair (7, 1) } }, // Sym = Left ALT key
			{ SDL_SCANCODE_M,				{ std::make_pair (7, 2) } },
			{ SDL_SCANCODE_N,				{ std::make_pair (7, 3) } },
			{ SDL_SCANCODE_B,				{ std::make_pair (7, 4) } }
		}
	);

const ZXSPECTRUM::InputOSSystem::Keystrokes ZXSPECTRUM::InputOSSystem::_NOKEYSTROKES = { };

// ---
ZXSPECTRUM::InputOSSystem::InputOSSystem ()
	: MCHEmul::InputOSSystem (_ID, 
		{ { "Name", "IOSystem" },
		  { "Type", "Input" },
		  { "Frequency", "50.0Hz" } }),
	  _ula (nullptr)
{ 
	// Nothing else to do...
}

// ---
void ZXSPECTRUM::InputOSSystem::linkToChips (const MCHEmul::Chips& c)
{
	for (MCHEmul::Chips::const_iterator i = c.begin (); i != c.end (); i++)
		if (dynamic_cast <ZXSPECTRUM::ULA*> ((*i).second) != nullptr)
			_ula = dynamic_cast <ZXSPECTRUM::ULA*> ((*i).second);

	// Can't be null after this method...
	assert (_ula != nullptr);

	// The ULA has to observe what happen hire!
	_ula -> observe (this);
}
