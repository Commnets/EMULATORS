/** \ingroup CORE */
/*@{*/

/**
 *	@file
 *	File: SDLImage.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 14/11/2026 \n
 *	Description: Selects the SDL_image headers used by the framework.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_SDLIMAGE__
#define __MCHEMUL_SDLIMAGE__

#include <CORE/SDL.hpp>

#if MCHEMUL_SDL_VERSION == 2
#include <SDL_image.h>
#elif MCHEMUL_SDL_VERSION == 3
#include <SDL3_image/SDL_image.h>
#endif

#endif

// End of the file
/*@}*/
