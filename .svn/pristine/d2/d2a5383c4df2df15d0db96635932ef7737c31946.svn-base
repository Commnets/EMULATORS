/** \ingroup core */
/*@{*/

/**	
 *	@file	
 *	File: Address.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 11/04/2021 \n
 *	Description: Define the structure of an address in the emulation.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_ADDRESS__
#define __MCHEMUL_ADDRESS__

#include <global.hpp>
#include <core/UInt.hpp>
#include <core/UBytes.hpp>
#include <core/UByte.hpp>

namespace MCHEmul
{
	/** Representing and address that can vary from 0 to a limit.
		An initial value and the limit is given at construction time. 
		If no limit is given the maximum possible in the framework is taken. */
	class Address
	{
		public:
		Address ()
			: _value (UInt ())
							{ }

		/** Set at the length of the limit. */
		Address (const UInt& a)
			: _value (a)
							{ }

		Address (const MCHEmul::UBytes& a, bool bE = true)
			: _value (a, bE)
							{ }

		Address (const std::vector <UByte>& a, bool bE = true)
			: _value (a, bE)
							{ }

		Address (const Address&) = default;
		Address& operator = (const Address&) = default;

		size_t size () const
							{ return (_value.size ()); }

		const UInt& value () const
							{ return (_value);}
		const UBytes& bytes () const
							{ return (_value.bytes ()); }

		/** Could be negative. */
		int distanceWith (const Address& a) const
							{ return ((int) a.value ().asUnsignedInt () - (int) value ().asUnsignedInt ()); }

		/** When reach the limit start back in th other side. */
		Address next (size_t n) const
							{ return (Address (_value + UInt::fromUnsignedInt ((unsigned int) n))); }		
		Address previous (size_t n) const
							{ return (Address (_value - UInt::fromUnsignedInt ((unsigned int) n))); }

		bool operator == (const Address& a) const
							{ return (_value == a._value); }
		bool operator != (const Address& a) const
							{ return (_value != a._value); }
		bool operator < (const Address& a) const
							{ return (_value < a._value); }
		bool operator <= (const Address& a) const
							{ return (_value <= a._value); }
		bool operator > (const Address& a) const
							{ return (_value > a._value); }
		bool operator >= (const Address& a) const
							{ return (_value >= a._value); }

		Address operator + (size_t n) const
							{ return (Address (*this).next (n)); }
		friend Address operator + (size_t n, const Address& a)
							{ return (a + n); }
		Address& operator += (size_t n)
							{ *this = *this + n; return (*this); }
		size_t operator - (const Address& a) const
							{ return (this -> distanceWith (a)); }
		Address operator - (size_t n) const
							{ return (Address (*this).previous (n)); }
		friend Address operator - (size_t n, const Address& a)
							{ return (a - n); }
		Address& operator -= (size_t n)
							{ *this = *this - n; return (*this); }

		UByte operator [] (size_t p) const
							{ return (_value [p]); }

		std::string asString (UByte::OutputFormat oF) const
							{ return (_value.asString (oF, ' ', 2)); }

		friend std::ostream& operator << (std::ostream& o, const Address& a);

		/** To create an Address from an string.\n
			The string should start with $ if written in hexadecimal, with 0 if it is in octal and with no 0 number in decimal: \n
			e.g $D400 (hexa), 07600 (octal), 53248 (decimal). \n
			If the str is not valid, then an empty address will be given. */
		static Address fromStr (const std::string& str);

		private:
		UInt _value;
	};
}

#endif
  
// End of the file
/*@}*/
