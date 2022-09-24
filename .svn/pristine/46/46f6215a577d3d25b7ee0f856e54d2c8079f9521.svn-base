/** \ingroup CPU */
/*@{*/

/**	
 *	@file	
 *	File: UInt.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 26/04/2021 \n
 *	Description: Defining UInts (unsigned & signed), managed as set of UBytes.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_UINT__
#define __MCHEMUL_UINT__

#include <CORE/UBytes.hpp>

namespace MCHEmul
{
	/** Representing a Integer (with and without sign). 
		The negativ numbers are kept internally in the complement_2 way. */
	class UInt final
	{
		public:
		static const UInt _0;
		static const UInt _1;

		UInt ()
			: _values (),
			  _carry (false), _overflow (false)
							{ }

		/** Always kept in Big-endian format. 
			Negative numbers are complement_2. */
		UInt (const UBytes& u, bool bE = true)
			: _values (bE ? u : u.reverse ()),
			  _carry (false), _overflow (false)
							{ }

		UInt (const std::vector <UByte>& u, bool bE = true)
			: _values (u, bE),
			  _carry (false), _overflow (false)
							{ }

		UInt (const UInt&) = default;
		UInt& operator = (const UInt&) = default;

		size_t size () const
							{ return (_values.size ()); }
		size_t sizeBits () const
							{ return (_values.sizeBits ()); }

		bool negative () const
							{ return (_values [0][UByte::sizeBits () - 1]); }
		bool positive () const
							{ return (!_values [0][UByte::sizeBits () - 1]); }

		/** These 2 flags are set after operations. */
		bool carry () const
							{ return (_carry); }
		void resetCarry ()
							{ _carry = false; }
		bool overflow () const
							{ return (_overflow); }
		void resetOverflow ()
							{ _overflow = false; }

		void setMinLength (size_t l)
							{ return (_values.setMinLength (l, false /** _0 at the left. */)); }

		const UBytes& values () const
							{ return (_values); }
		const std::vector <UByte>& bytes () const
							{ return (_values.bytes ()); }

		UInt LSUInt (size_t p) const
							{ return (UInt (_values.LSUBytes (p))); }
		UInt MSUInt (size_t p) const
							{ return (UInt (_values.MSUBytes (p))); }

		UInt add (const UInt& u, bool iC = false) const;
		UInt complement () const;
		UInt complement_2 () const
							{ return (complement ().add (_1)); }
		UInt substract (const UInt& u) const;
		UInt multiply (const UInt& u) const;

		bool operator == (const UInt& u) const
							{ return (_values == u._values); }
		bool operator != (const UInt& u) const
							{ return (_values != u._values); }

		/** Takes into account the sign. */
		bool operator > (const UInt& u) const;
		bool operator >= (const UInt& u) const
							{ return ((*this > u) || (*this == u)); }
		bool operator < (const UInt& u) const;
		bool operator <= (const UInt& u) const
							{ return ((*this < u) || (*this == u)); }

		UInt operator + (const UInt& u) const
							{ return (add (u)); }
		UInt& operator += (const UInt& u)
							{ *this = *this + u; return (*this); }
		UInt operator - () const
							{ return (complement_2 ()); }
		UInt operator - (const UInt& u) const
							{ return (substract (u)); }
		UInt& operator -= (const UInt& u)
							{ *this = *this - u; return (*this); }
		UInt operator * (const UInt& u) const
							{ return (multiply (u)); }
		UInt& operator *= (const UInt& u)
							{ *this = *this * u; return (*this); }

		UByte operator [] (size_t p) const
							{ return (_values [p]); }
		
		std::string asString (UByte::OutputFormat oF, char s /** separator */, size_t l = 0 /** Minimum length per UByte */) const
							{ return (_values.asString (oF, s, l)); }

		unsigned int asUnsignedInt () const;
		int asInt () const
							{ return (negative () ? -((int) complement_2 ().asUnsignedInt ()) : (int) asUnsignedInt ()); }

		static UInt fromUnsignedInt (unsigned int n);
		static UInt fromInt (int n);
		static UInt fromStr (const std::string& s);

		friend std::ostream& operator << (std::ostream& o, const UInt& u)
							{ return (o << u.asString (UByte::OutputFormat::_HEXA, ' ', 2)); }

		private:
		UBytes _values;

		// Implementation
		mutable bool _carry;
		mutable bool _overflow;
	};

}

#endif
  
// End of the file
/*@}*/