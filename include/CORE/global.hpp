/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: global.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
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
#include <chrono>

namespace MCHEmul
{
	// Important macros used many times
	static const std::string _YES = "YES";
	static const std::string _NO = "NO";
	static const std::string _DEFAULTLANGUAGE = "ENG";
	// Used in simulations, sometimes...
	static const unsigned int _U0 = (unsigned int) ~0;
	static const size_t _S0 = (size_t) ~0;

	/** A vector of strings. */
	using Strings = std::vector <std::string>;
	/** The list of Strigs can be printed out. */
	std::ostream& operator << (std::ostream& o, const Strings& strs);
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
	static const unsigned int _INTERRUPT_ERROR				= 3;
	static const unsigned int _INSTRUCTION_ERROR			= 4;
	static const unsigned int _CHIP_ERROR					= 5;
	static const unsigned int _DEVICE_ERROR					= 6;
	static const unsigned int _PERIPHERAL_ERROR				= 7;
	static const unsigned int _COMMSNOTOPENED_ERROR			= 8;
	static const unsigned int _COMMSCHANNELNOTOPENED_ERROR	= 9;
	static const unsigned int _COMMSINTNOTCREATED_ERROR		= 10;
	static const unsigned int _CHANNELREADERROR_ERROR		= 11;
	static const unsigned int _CHANNELWRITEERROR_ERROR		= 12;
	static const unsigned int _FORMATTERNOTVALID_ERROR		= 13;

	/** The max number of bytes managed in this emulator. */
	static const unsigned int _MAXBYTESMANAGED				= 2;

	/** Levels of debugging */
	static const unsigned int _DEBUGNOTHING					= 0;
	static const unsigned int _DEBUGERRORS					= 1;
	static const unsigned int _DUMPATEXIT					= 2;
	static const unsigned int _DEBUGERRORANDWARNINGS		= 3;
	static const unsigned int _DEBUGALL						= 4;
	static const unsigned int _DEBUGTRACEINTERNALS			= 5;

	/** Generic strings. */
	static const std::string _SPACES (100, ' ');
	static const std::string _CEROS (100, '0');
	static const std::string _TABS (100, '\t');
	static const std::string _BACKS (100, '\b');

	/** General functions to manage strings */
	size_t firstSpaceIn (const std::string& s);
	size_t firstOf (const std::string& s, const std::string& c);
	size_t firstOf (const std::string& s, const Strings& strs, size_t& r); // r = the string, and return the position in that...
	std::string ltrim (const std::string& s);
	std::string rtrim (const std::string& s);
	std::string trim (const std::string& s);
	std::string upper (const std::string& s);
	std::string upperExcept (const std::string& s); // All to upper except things between quotes! (qhen quote starts upper stops)
	std::string lower (const std::string& s);
	std::string lowerExcept (const std::string& s); // All to lower except things between quotes! (when quote starts lower stops)
	std::string noSpaces (const std::string& s);
	std::string replaceAllSpacesPerEquivalent (const std::string& s); // in comms usually...
	std::string restoreSpacesFromEquivalent (const std::string& s); // in comms usually...
	std::string onlyAlphanumeric (const std::string& s);
	std::string noneOf (const std::string& s, const std::string& chrs);
	std::string removeAllFrom (const std::string& s, const Strings& strs);
	std::string removeAll0 (const std::string& s);
	std::string replaceAll (const std::string& s, const std::string& o, const std::string& d);
	std::string fixLenStr (const std::string& s, size_t sz, 
		bool l, const std::string& as = _SPACES); // To return a fix in a fix length...
	/** Every element got is trim-ed. */
	Strings getElementsFrom (const std::string& txt, unsigned char ch, 
		size_t nE = std::numeric_limits <size_t>::max ());
	/** To setup the list of strings like a table. */
	std::string tableFormat (const Strings& s, const std::string& sp, size_t l /** minimun length per element. */,
		size_t sb = std::numeric_limits <size_t>::max () /** size per line. */, 
		int ne = -1 /** number of elements to comvert in table. -1 = all. */);
	/** Used in the main function, mainly! */
	Strings convertIntoStrings (int n, char** dt);
	Strings convertIntoStrings (int n, wchar_t** dt); // Unicode...
	std::string concatenateStrings (const Strings& strs, const std::string& c);

	/** General functions to determine whether a string is or not valid
		from a specific perspective. */
	bool validLabel (const std::string& s); // alphanumeric characters and no _
	bool validBytesChar (const std::string& s); // any alphanumeric character in ''
	bool validBytesBinary (const std::string& s); // 0 or 1 and must start with a z
	bool validBytesOctal (const std::string& s); // from 0 to 8 and must start with a 0
	bool validBytesHexadecimal (const std::string& s); // from 0 to f and must start with a $
	bool validBytesDecimal (const std::string& s); // only numbers and not starting with 0
	bool validBytes (const std::string& s); // if type valid...
	bool validOperation (const std::string& s); // very generic validation...

	/** To calculate the point in a curve. */
	double linearInterpolation (double minx, double miny, double maxx, double maxy, double x);

	/** To determine whether a class is or not a subclass of other. */
	template <typename Base, typename Type>
	bool instanceOf (const Type* ptr)
							{ return (dynamic_cast <const Base*> (ptr) != nullptr); }

	/** The computer emulation using milliseconds only. */
	using ClockType = std::chrono::steady_clock;
	using ClockDurationType = std::chrono::duration <long long, std::milli>;
	using ClockTime = std::chrono::time_point <ClockType, ClockDurationType>;
	// The time in the emulation is managed always in milliseconds.
	// It is decalred as extern because it will be used all over the application...
	/** This variable is initialized when the emulator starts. \n
		It keeps the global time. Some chips and structures can benefit from it. */
	extern const ClockTime _STARTINGTIME; 
	/** This variable always keeps the current time in the format used by the C64. \n
		This variable has to be periodically updated. */
	extern ClockTime _NOW;
	/** This variable always keeps the number of milliseconds between two actualizations of the global time. */
	extern ClockDurationType _MILLISECONDSPAST;

	/** To actualize the time. */
	void actualizeGlobalTime ();
}

#endif
  
// End of the file
/*@}*/
