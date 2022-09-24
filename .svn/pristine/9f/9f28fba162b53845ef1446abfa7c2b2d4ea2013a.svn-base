/** \ingroup CPU */
/*@{*/

/**	
 *	@file	
 *	File: ProgramCounter.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 10/04/2021 \n
 *	Description: Defines the behaviour of a program counter.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_PROGRAMCOUNTER__
#define __MCHEMUL_PROGRAMCOUNTER__

#include <CORE/Register.hpp>

namespace MCHEmul
{
	/** The program counter is an special register within any CPU, 
		keeping an address inside that is incremented or decremented as the program is executed. */
	class ProgramCounter final : protected Register
	{
		public:
		ProgramCounter (size_t sz)
			: Register (-1 /** always */, UBytes (std::vector <UByte> (sz, UByte::_0))) 
							{ }

		ProgramCounter (const ProgramCounter&) = default;
		ProgramCounter& operator = (const ProgramCounter&) = default;

		void initialize ()
							{ Register::initialize (); }

		Address asAddress () const
							{ return (Register::asAddress (true)); }
		void setAddress (const Address& a)
							{ set (a.bytes ()); }

		void increment (size_t n = 1)
							{ setAddress (asAddress ().next (n)); }
		void decrement (size_t n = 1)
							{ setAddress (asAddress ().previous (n)); }

		bool operator == (const Address& a) const
							{ return (asAddress () == a); }
		bool operator != (const Address& a) const
							{ return (asAddress () != a); }
		bool operator > (const Address& a) const
							{ return (asAddress () > a); }
		bool operator >= (const Address& a) const
							{ return (asAddress () >= a); }
		bool operator < (const Address& a) const
							{ return (asAddress () < a); }
		bool operator <= (const Address& a) const
							{ return (asAddress () != a); }

		ProgramCounter operator + (size_t n) const
							{ ProgramCounter r = *this; r.increment (n); return (r); }
		ProgramCounter& operator += (size_t n)
							{ increment (n); return (*this); }
		ProgramCounter operator - (size_t n) const
							{ ProgramCounter r = *this; r.decrement (n); return (r); }
		ProgramCounter& operator -= (size_t n)
							{ decrement (n); return (*this); }

		std::string asString (UByte::OutputFormat oF) const
							{ return (asAddress ().asString (oF)); }

		friend std::ostream& operator << (std::ostream& o, const ProgramCounter& pc);
	};
}

#endif
  
// End of the file
/*@}*/