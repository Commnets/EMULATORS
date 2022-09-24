/** \ingroup core */
/*@{*/

/**	
 *	@file	
 *	File: UInt.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 26/04/2021 \n
 *	Description: Defining UInts, managd as set of UBytes.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_UINT__
#define __MCHEMUL_UINT__

#include <string>
#include <ostream>
#include <core/UBytes.hpp>

namespace MCHEmul
{
	/** Representing a Integer. */
	class UInt
	{
		public:
		static const UInt _0;
		static const UInt _1;

		UInt ()
			: _bytes ()
							{ }

		/** Always kept in Big-endian format. */
		UInt (const UBytes& u, bool bE = true)
			: _bytes (bE ? u : u.reverse ())
							{ }

		UInt (const std::vector <UByte>& u, bool bE = true)
			: _bytes (u, bE)
							{ }

		UInt (const UInt&) = default;
		UInt& operator = (const UInt&) = default;

		size_t size () const
							{ return (_bytes.size ()); }

		void setMinLength (size_t l)
							{ return (_bytes.setMinLength (l, false /** _0 at the left. */)); }

		const UBytes& bytes () const
							{ return (_bytes); }

		UInt LSUInt (size_t p) const
							{ return (UInt (_bytes.LSUBytes (p))); }
		UInt MSUInt (size_t p) const
							{ return (UInt (_bytes.MSUBytes (p))); }

		UInt add (const UInt& u) const;
		UInt complement () const;
		UInt complement_2 () const
							{ return (complement ().add (_1)); }
		UInt substract (const UInt& u) const;

		bool operator == (const UInt& u) const
							{ return (_bytes == u._bytes); }
		bool operator != (const UInt& u) const
							{ return (_bytes != u._bytes); }
		bool operator > (const UInt& u) const;
		bool operator >= (const UInt& u) const
							{ return (*this == u || *this > u); }
		bool operator < (const UInt& u) const;
		bool operator <= (const UInt& u) const
							{ return (*this == u || *this < u); }

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

		UByte operator [] (size_t p) const
							{ return (_bytes [p]); }
		
		std::string asString (UByte::OutputFormat oF, char s /** separator */, size_t l = 0 /** Minimum length per UByte */) const
							{ return (_bytes.asString (oF, s, l)); }
		unsigned int asUnsignedInt () const;
		int asInt () const;

		static UInt fromUnsignedInt (unsigned int n);
		static UInt fromInt (int n);

		friend std::ostream& operator << (std::ostream& o, const UInt& u)
							{ return (o << u.asString (UByte::OutputFormat::_HEXA, ' ', 2)); }

		private:
		UBytes _bytes;
	};
}

#endif
  
// End of the file
/*@}*/