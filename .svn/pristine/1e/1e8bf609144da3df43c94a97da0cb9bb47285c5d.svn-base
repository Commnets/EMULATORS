/** \ingroup CPU */
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

#include <assert.h>
#include <string>
#include <vector>
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
	const unsigned int _NOERROR			= 0;
	const unsigned int _INIT_ERROR		= 1;
	const unsigned int _CPU_ERROR		= 2;
	const unsigned int _CHIP_ERROR		= 3;
	const unsigned int _DEVICE_ERROR	= 4;
	const unsigned int _COMMSNOTOPENED	= 5;
	const unsigned int _COMMSNOTCREATED	= 6;
	const unsigned int _COMMSREADERROR	= 7;

	/** The max number of bytes managed in this emulator. */
	const unsigned int _MAXBYTESMANAGED = 2;

	/** General functions to manage strings */
	std::string ltrim (const std::string& s);
	std::string rtrim (const std::string& s);
	std::string trim (const std::string& s);
	std::string upper (const std::string& s);
	std::string lower (const std::string& s);
	std::string noSpaces (const std::string& s);
	std::string onlyAlphanumeric (const std::string& s);
	std::string noneOf (const std::string& s, const std::string& chrs);
	std::string removeAllFrom (const std::string& s, std::vector <std::string>& strs);
	std::vector <std::string> getElementsFrom (const std::string& txt, unsigned char ch, 
		size_t nE = std::numeric_limits <size_t>::max ());

	/** General functions to determine whether a string is or not valid
		from a spcific perspective. */
	bool validLabel (const std::string& s); 
	bool validBytesOctal (const std::string& s); 
	bool validBytesHexadecimal (const std::string& s); 
	bool validBytesDecimal (const std::string& s); 
	bool validBytes (const std::string& s); 
}

#endif
  
// End of the file
/*@}*/