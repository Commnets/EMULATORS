/** \ingroup CPU */
/*@{*/

/**	
 *	@file	
 *	File: StatusRegister.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forni?s (EMULATORS library) \n
 *	Creation Date: 11/04/2021 \n
 *	Description: The status register usd by almost any instruction.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_STATUSREGISTER__
#define __MCHEMUL_STATUSREGISTER__

#include <CORE/Register.hpp>

namespace MCHEmul
{
	/** In the register avery bit could have a different meaning. */
	class StatusRegister : protected Register
	{
		public:
		using BitNames = std::map <std::string, int>;

		StatusRegister (size_t nB, const BitNames& bN)
			: Register (-2 /** always */, "ST", UBytes (std::vector <UByte> (nB, UByte::_0))), 
			  _bitNames (bN)
							{}

		StatusRegister (const StatusRegister&) = default;

		StatusRegister& operator = (const StatusRegister&) = default;

		void initialize ()
							{ Register::initialize (); }

		const BitNames& bitNames () const
							{ return (_bitNames); }
		bool existsBitStatus (const std::string& bN) const
							{ return (_bitNames.find (bN) != _bitNames.end ()); }
		bool bitStatus (const std::string& bN) const
							{ return (existsBitStatus (bN) ? _values.bit ((*_bitNames.find (bN)).second) : false); }
		void setBitStatus (const std::string& bN, bool s)
							{ if (existsBitStatus (bN)) _values.setBit ((*_bitNames.find (bN)).second, s); }

		const UBytes& values () const
							{ return (Register::values ()); }
		const std::vector <UByte>& bytes () const
							{ return (Register::bytes ()); }

		UBytes valuesWithout (const std::vector <std::string>& bN);
		void set (const std::vector <UByte>& v)
							{ Register::set (v); }
		void set (const UBytes& v)
							{ Register::set (v); }

		std::string asString () const;

		friend std::ostream& operator << (std::ostream& o, const StatusRegister& r)
							{ return (o << r.asString ()); }

		private:
		const BitNames _bitNames; 
	};
}

#endif
  
// End of the file
/*@}*/