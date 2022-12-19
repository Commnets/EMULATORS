/** \ingroup CORE */
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

#include <CORE/global.hpp>

namespace MCHEmul
{
	/** Representing the minimum piece of addressable info in a computer. 
		MSB is the bit 7, LSB is the bit 0.
		Read bits from right to left. */
	class UByte final
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
		static constexpr unsigned char _0		= 0x00;
		static constexpr unsigned char _1		= 0x01;				// Used e.g in adding to get complement_2!
		static constexpr unsigned char _FF		= 0xff;
		static constexpr unsigned char _0F		= 0x0f;				// Used to extract the LS Nibble
		static constexpr unsigned char _F0		= 0xf0;				// Used to extract the MS Nibble
		static constexpr unsigned char _80		= 0x80;				// Used to test the sign...
		static constexpr unsigned char _09		= 0x09;				// Used in BCD limits...
		static constexpr unsigned char _90		= 0x90;				// Used in BCD limits...
		static constexpr unsigned char _09N		= ~_09 + 1;			// Used in BCD limits...
		static constexpr unsigned char _90N		= ~_90 + 1;			// Used in BCD limits...
		static constexpr unsigned char _10		= 0x10;				// Used in BCD operations...
		static constexpr unsigned char _06		= 0x06;				// Used in BCD operations...
		static constexpr unsigned char _60		= 0x60;				// Used in BCD operations...
		static constexpr unsigned char _06N		= ~_06 + _1;		// Used in BCD operations...
		static constexpr unsigned char _60N		= ~_60 + _1;		// Used in BCD operations...

		UByte ()
			: _value (_0)
							{ }

		UByte (unsigned char v)
			: _value (v)
							{ }

		UByte (const UByte&) = default;

		UByte& operator = (const UByte&) = default;

		/** Common to every UByte. */
		static constexpr size_t size () // (static) All the same...
							{ return (sizeof (unsigned char)); }
		static constexpr size_t sizeBits () // (static) All the same...
							{ return (sizeof (unsigned char) << 3); }

		unsigned char value () const
							{ return (_value); }

		/** If you requested for a bit bigger than sizeBits, the crash is guaranteed. 
			No checks are done to increase the speed. */
		bool bit (size_t p) const
							{ return ((_value & (_1 << p)) != _0); }
		void setBit (size_t p, bool s)
							{ unsigned char v = _1 << p; _value = _value & (~v) | (s ? v : _0); }

		UByte LSNibble () const
							{ return (UByte (_value & _0F)); }
		UByte MSNibble () const
							{ return (UByte (_value & _F0)); }

		/** To calculate the complement (bits 0 to 1 and the other way around). */
		UByte complement () const
							{ return (~_value); }

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

		/** 
		  *	Just to do a bit adding.
		  *	But very interesting method.
		  *	@param	u		The byte to add.
		  * @param	cin		Is there any carry at the beginning to take into account?
		  * @param	cout	It is actualized whether a carry is generated from the adding.
		  * @param	o		Is is actualized whether an overflow is generated, 
		  *					that is when the MSBits of inputs are equal and different from the output. 
		  */
		inline UByte bitAdding (const UByte& u, bool cin, bool& cout, bool& o) const;

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
		UByte operator ~ () const
							{ return (complement ()); }

		bool operator [] (size_t p) const
							{ return (bit (p)); }
		UByte& operator << (size_t p)
							{ return (shiftLeft (p)); }
		UByte& operator >> (size_t p)
							{ return (shiftRight (p)); }

		std::string asString (OutputFormat oF, size_t l = 0 /** Minimum length. */) const;

		friend std::ostream& operator << (std::ostream& o, const UByte& u)
							{ return (o << u.asString (UByte::OutputFormat::_HEXA, 2 /** sizeof (unsigned char) * 2 */)); }

		private:
		unsigned char _value; // 8 bits - length (@see static method size)
	};

	// This piece of the code should be inline ever!
	// This is the very basic adder!
	inline UByte UByte::bitAdding (const UByte& u, bool cin, bool& cout, bool& o) const
	{
		unsigned short r = 
			(unsigned short) (_value + u._value + 
				(cin ? 0x01 /** instead UByte::_1 to guaranttee it is online. */ : 0x00));

		// The carry is generated if the result is bigger than a unsigned char long!
		cout = (r & 0xff00 /** unsigned short = 2 bytes long. */) != 0x000; 
		// The overflow is generated when both are negative and the outcome positive...or the other way around!
		o = (bit (7) && u.bit (7) && ((r & 0x0080) == 0x0000)) ||  
				(!bit (7) && !u.bit (7) && ((r & 0x0080) != 0x0000)); 

		return (MCHEmul::UByte ((unsigned char) /** cut it. */ r));
	}
}

#endif
  
// End of the file
/*@}*/