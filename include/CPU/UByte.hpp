/** \ingroup CPU */
/*@{*/

/**	
 *	@file	
 *	File: UByte.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 26/04/2021 \n
 *	Description: Definition of UByte, represeting the minimum addressable element in a computer.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_UBYTE__
#define __MCHEMUL_UBYTE__

#include <CPU/global.hpp>

namespace MCHEmul
{
	/** Representing the minimum piece of addressable info in a computer. 
		MSB is the bit 7, LSB is the bit 0.
		Read bits from right to left. */
	class UByte
	{
		public:
		enum class OutputFormat
		{
			_DECIMAL,
			_BINARY,
			_OCTAL,
			_HEXA
		};

		// Most common values
		static const unsigned char _0 = 0x00;
		static const unsigned char _1 = 0x01;
		static const unsigned char _F = 0xff;

		UByte ()
			: _value (_0)
							{ }

		UByte (unsigned char v)
			: _value (v)
							{ }

		UByte (const UByte&) = default;
		UByte& operator = (const UByte&) = default;

		/** COmmon to every UByt. */
		static size_t size () // (static) Alol the same...
							{ return (sizeof (unsigned char)); }
		static size_t sizeBits () // (static) All the same...
							{ return (sizeof (unsigned char) << 3); }

		unsigned char value () const
							{ return (_value); }

		bool bit (size_t p) const
							{ return ((p < sizeBits ()) ? (_value & (_1 << p)) != _0 : false); }
		void setBit (size_t p, bool s)
							{ if (p < sizeBits ()) 
								{ unsigned char v = _1 << p; _value = _value & (_F - v) | (s ? v : _0); } }

		UByte complement () const
							{ return (_F - _value); }

		bool shiftLeftC (bool c = false /** carry flag */, size_t p = 1);
		UByte& shiftLeft (size_t p = 1)
						{ shiftLeftC (false, p); return (*this); }
		bool shiftRightC (bool c = false, size_t p = 1);
		UByte& shiftRight (size_t p = 1)
						{ shiftRightC (false, p); return (*this); }

		bool rotateLeftC (bool c = false /** carry flag */, size_t p = 1);
		UByte& rotateLeft (size_t p = 1 /** without carry flag */);
		bool rotateRightC (bool c = false, size_t p = 1);
		UByte& rotateRight (size_t p = 1);

		bool operator == (const UByte& u) const
							{ return (_value == u._value); }
		bool operator != (const UByte& u) const
							{ return (_value != u._value); }
		bool operator > (const UByte& u) const
							{ return (_value > u._value); }
		bool operator >= (const UByte& u) const
							{ return (_value >= u._value); }
		bool operator < (const UByte& u) const
							{ return (_value == u._value); }
		bool operator <= (const UByte& u) const
							{ return (_value <= u._value); }

		UByte operator & (const UByte& u) const
							{ return (_value & u._value); }
		UByte& operator &= (const UByte& u)
							{ *this = *this & u; return (*this); }
		UByte operator | (const UByte& u) const
							{ return (_value | u._value); }
		UByte& operator |= (const UByte& u)
							{ *this = *this | u; return (*this); }
		UByte operator ^ (const UByte& u) const
							{ return (_value ^ u._value); }
		UByte& operator ^= (const UByte& u)
							{ *this = *this ^ u; return (*this); }

		bool operator [] (size_t p) const
							{ return (bit (p)); }
		UByte& operator << (size_t p)
							{ return (shiftLeft (p)); }
		UByte& operator >> (size_t p)
							{ return (shiftRight (p)); }

		std::string asString (OutputFormat oF, size_t l = 0 /** Minimum length. */) const;

		friend std::ostream& operator << (std::ostream& o, const UByte& u)
							{ return (o << u.asString (UByte::OutputFormat::_HEXA, 2)); }

		private:
		unsigned char _value; // 8 bits - length (@see static method size)
	};
}

#endif
  
// End of the file
/*@}*/