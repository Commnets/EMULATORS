/** \ingroup CORE */
/*@{*/

/**
 *	@file
 *	File: SDLGfx.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 15/11/2026 \n
 *	Description: Selects the SDL_gfx headers used by the framework.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_SDLGFX__
#define __MCHEMUL_SDLGFX__

#include <CORE/SDL.hpp>

#if MCHEMUL_SDL_VERSION == 2
#include <SDL2_gfxPrimitives.h>
#elif MCHEMUL_SDL_VERSION == 3
#include <SDL3_gfxPrimitives.h>
#endif

#endif

// End of the file
/*@}*/
