/** \ingroup CORE */
/*@{*/

/**
 *	@file
 *	File: SDLTtf.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 15/11/2026 \n
 *	Description: Selects the SDL_ttf headers used by the framework.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_SDLTTF__
#define __MCHEMUL_SDLTTF__

#include <CORE/SDL.hpp>

#if MCHEMUL_SDL_VERSION == 2
#include <SDL_ttf.h>
#elif MCHEMUL_SDL_VERSION == 3
#include <SDL3_ttf/SDL_ttf.h>
#endif

#endif

// End of the file
/*@}*/
