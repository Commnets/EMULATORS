/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: global.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 07/08/2021 \n
 *	Description: Global classes & definitions for the C64 implementation
 *	Versions: 1.0 Initial
 */

#ifndef __C64_GLOBAL__
#define __C64_GLOBAL__

#include <chrono>

namespace C64
{
	/** The C64 counts using tenth of seconds only. */
	using Duration = std::chrono::duration <unsigned long, std::deci>;
	using Time = std::chrono::time_point <std::chrono::steady_clock, Duration>;

	/** This variable is initialized when the emulator starts. */
	static const Time _STARTINGTIME = std::chrono::time_point_cast <Duration> (std::chrono::steady_clock::now ());
	/** This variable always keeps the current time in the format used by the C64. */
	static Time _NOW = std::chrono::time_point_cast <Duration> (std::chrono::steady_clock::now ());
	/** This variable always keeps the number of tenths of second between two actualizations of the global time. */
	static Duration _TENTHSSECONDPAST = Duration ();

	/** To actualize the time. */
	void actualizeGlobalTime ();
}

#endif
  
// End of the file
/*@}*/