/** \ingroup CORE */
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
#include <iostream>

namespace MCHEmul
{
	// Important macros used many times
	static const std::string _YES = "YES";
	static const std::string _NO = "NO";

	/** A vector of strings. */
	using Strings = std::vector <std::string>;
	/** Attributes ar used in many places. */
	using Attributes = std::map <std::string, std::string>;
	/** When a no defined attributes is requested, this is the value returned. */
	static const std::string AttributedNotDefined = "";
	/** The list of attributes can be printed out. */
	std::ostream& operator << (std::ostream& o, const Attributes& attrs);

	/** Constants for errors. */
	static const unsigned int _NOERROR						= 0;
	static const unsigned int _INIT_ERROR					= 1;
	static const unsigned int _CPU_ERROR					= 2;
	static const unsigned int _CHIP_ERROR					= 3;
	static const unsigned int _DEVICE_ERROR					= 4;
	static const unsigned int _COMMSNOTOPENED_ERROR			= 5;
	static const unsigned int _COMMSCHANNELNOTOPENED_ERROR	= 6;
	static const unsigned int _COMMSINTNOTCREATED_ERROR		= 7;
	static const unsigned int _CHANNELREADERROR_ERROR		= 8;
	static const unsigned int _CHANNELWRITEERROR_ERROR		= 9;
	static const unsigned int _FORMATTERNOTVALID_ERROR		= 10;

	/** The max number of bytes managed in this emulator. */
	static const unsigned int _MAXBYTESMANAGED				= 2;

	/** Levels of debugging */
	static const unsigned int _DEBUGNOTHING					= 0;
	static const unsigned int _DEBUGERRORS					= 1;
	static const unsigned int _DEBUGERRORANDWARNINGS		= 2;
	static const unsigned int _DEBUGALL						= 3;
	static const unsigned int _DEBUGTRACEINTERNALS			= 4;

	/** Generic strings. */
	static const std::string _SPACES (20, ' ');
	static const std::string _CEROS (20, '0');
	static const std::string _TABS (20, '\t');

	/** General functions to manage strings */
	std::string ltrim (const std::string& s);
	std::string rtrim (const std::string& s);
	std::string trim (const std::string& s);
	std::string upper (const std::string& s);
	std::string lower (const std::string& s);
	std::string noSpaces (const std::string& s);
	std::string onlyAlphanumeric (const std::string& s);
	std::string noneOf (const std::string& s, const std::string& chrs);
	std::string removeAllFrom (const std::string& s, const Strings& strs);
	std::string removeAll0 (const std::string& s);
	std::string replaceAll (const std::string& s, const std::string& o, const std::string& d);
	/** Every element got is trim-ed. */
	Strings getElementsFrom (const std::string& txt, unsigned char ch, 
		size_t nE = std::numeric_limits <size_t>::max ());
	/** To setup the list of strings like a table. */
	std::string tableFormat (const Strings& s, const std::string& sp, size_t l /** minimun length per element. */,
		size_t sb = std::numeric_limits <size_t>::max () /** size per line. */);
	/** Used in the main function, mainly! */
	Strings convertIntoStrings (int n, char** dt);
	Strings convertIntoStrings (int n, wchar_t** dt); // Unicode...

	/** General functions to determine whether a string is or not valid
		from a spcific perspective. */
	bool validLabel (const std::string& s); 
	bool validBytesOctal (const std::string& s); 
	bool validBytesHexadecimal (const std::string& s); 
	bool validBytesDecimal (const std::string& s); 
	bool validBytes (const std::string& s);

	/** To determine whether a class is or not a subclass of other. */
	template <typename Base, typename Type>
	bool instanceOf (const Type* ptr)
							{ return (dynamic_cast <const Base*> (ptr) != nullptr); }
}

#endif
  
// End of the file
/*@}*/