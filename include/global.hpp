/** \ingroup core */
/*@{*/

/**	
 *	@file	
 *	File: global.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 07/04/2021 \n
 *	Description: Global classes & definitions.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_GLOBAL__
#define __MCHEMUL_GLOBAL__

#include <string>
#include <map>
#include <ostream>

namespace MCHEmul
{
	/** Attributes ar used in many places. */
	using Attributes = std::map <std::string, std::string>;
	/** When a no defined attributes is requested, this is the value returned. */
	const std::string AttributedNotDefined = "";
	/** The list of attributes can be printed out. */
	std::ostream& operator << (std::ostream& o, const Attributes& attrs);

	/** Constants for errors. */
	const unsigned int _NOERROR		= 0;
	const unsigned int _INIT_ERROR	= 1;
	const unsigned int _CPU_ERROR	= 2;
	const unsigned int _CHIP_ERROR	= 3;

	/** The max number of bytes managed in this emulator. */
	const unsigned int _MAXBYTESMANAGED = 2;
}

#endif
  
// End of the file
/*@}*/