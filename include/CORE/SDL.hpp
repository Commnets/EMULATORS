/** \ingroup CORE */
/*@{*/

/**
 *	@file
 *	File: SDL.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 14/11/2026 \n
 *	Description: Selects the SDL headers used by the framework.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_SDL__
#define __MCHEMUL_SDL__

#ifndef MCHEMUL_SDL_VERSION
#error MCHEMUL_SDL_VERSION must be defined by the SDL property sheet.
#elif MCHEMUL_SDL_VERSION == 2
#include <SDL.h>
#elif MCHEMUL_SDL_VERSION == 3
#include <SDL3/SDL.h>
#else
#error Unsupported MCHEMUL_SDL_VERSION.
#endif

#endif

// End of the file
/*@}*/
