/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: StatusRegister.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 11/04/2021 \n
 *	Description: The status register usd by almost any instruction.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_STATUSREGISTER__
#define __MCHEMUL_STATUSREGISTER__

#include <CORE/Register.hpp>

namespace MCHEmul
{
	/** In the register very bit could have a different meaning. 
		But at the end of the day it is a Register!. */
	class StatusRegister : public Register
	{
		public:
		using BitNames = std::map <std::string, size_t>;

		StatusRegister (size_t nB, const BitNames& bN);

		// To get info (name & position) about the different bit status...
		const BitNames& bitNames () const
							{ return (_bitNames); }
		const std::string& bitName (size_t p) const
							{ return (p < (size () * MCHEmul::UByte::sizeBits ())
								? (*_bitPosName.find (p)).second : AttributedNotDefined); }
		bool existsBitStatus (const std::string& bN) const
							{ return (_bitNames.find (bN) != _bitNames.end ()); }

		// To get/set the status of the different bits...
		/** Take care using these methods, no check about the name used are done for speed reasons. */
		bool bitStatus (const std::string& bN) const
							{ return (_values.bit ((*_bitNames.find (bN)).second)); }
		bool bitStatus (size_t bP) const
							{ return (_values.bit (bP)); }
		void setBitStatus (const std::string& bN, bool s)
							{ _values.setBit ((*_bitNames.find (bN)).second, s); }
		void setBitStatus (size_t bP, bool s)
							{ _values.setBit (bP, s); }

		UBytes valuesWithout (const Strings& bN);

		/** The status of every bit is show next to its name. */
		std::string asString () const;

		friend std::ostream& operator << (std::ostream& o, const StatusRegister& r)
							{ return (o << r.asString ()); }

		private:
		const BitNames _bitNames;

		// Implementation
		/** Created at construction time to simplify 
			later the way the info is printed out. */
		std::map <size_t, std::string> _bitPosName;
	};
}

#endif
  
// End of the file
/*@}*/