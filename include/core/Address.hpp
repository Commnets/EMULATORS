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

#include <assert.h>
#include <ostream>

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
			: _value (UInt ()), _limit (UInt ())
							{ }

		/** Set at the length of the limit. */
		Address (const UInt& a, const UInt& l)
			: _value (a), _limit (l)
							{ _value.setMinLength (_limit.size ()); }

		Address (const UInt& a)
			: _value (a), _limit (UInt (std::vector <UByte> (a.size (), UByte::_F)))
							{ _value.setMinLength (_limit.size ()); }

		Address (const MCHEmul::UBytes& a, const MCHEmul::UBytes& l, bool bE = true)
			: _value (a, bE), _limit (l, bE)
							{ }

		Address (const MCHEmul::UBytes& a, bool bE = true)
			: _value (a, bE), _limit (std::vector <UByte> (a.size (), UByte::_F), bE)
							{ }

		Address (const std::vector <UByte>& a, const std::vector <UByte>& l, bool bE = true)
			: _value (a, bE), _limit (l, bE)
							{ _value.setMinLength (_limit.size ()); }

		Address (const std::vector <UByte>& a, bool bE = true)
			: _value (a, bE), _limit (std::vector <UByte> (a.size (), UByte::_F), bE)
							{ }

		Address (const Address&) = default;
		Address& operator = (const Address&) = default;

		size_t size () const
							{ return (_value.size ()); }

		const UInt& value () const
							{ return (_value);}
		const UInt& limit () const
							{ return (_limit); }

		/** Always positive. */
		int distanceWith (const Address& a) const
							{ return ((int) a.value ().asUnsignedInt () - (int) value ().asUnsignedInt ()); }

		/** When reach the limit start back in th other side. */
		Address next (size_t n) const;
		Address previous (size_t n) const;

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

		private:
		UInt _value;
		UInt _limit;
	};
}

#endif
  
// End of the file
/*@}*/
