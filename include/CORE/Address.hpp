/** \ingroup CORE */
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

#include <CORE/global.hpp>
#include <CORE/UInt.hpp>
#include <CORE/UBytes.hpp>
#include <CORE/UByte.hpp>

namespace MCHEmul
{
	/** Representing an address that can vary from 0 to 
		1 << (8 * sizeof (unsigned int)) = 4.294.967.296 = 4GB of addressable memory. \n
		An initial value is given at construction time. \n
		The address is represented internally by an unsigned int. */
	class Address final
	{
		public:
		/** Represents a no valid Address... \n
			When size = 0, the _internalValue is not important at all. */
		Address ()
			: _size (0),
			  _internalRepresentation (0)
							{ }

		/** The address is always undertood as positive. 
			The size could be adjusted depending on the value. */
		Address (size_t s, unsigned int a)
			: _size (s),
			  _internalRepresentation (a)
							{ if (_size > 4) _size = 4; }

		Address (const UInt& a)
			: _size (a.size ()),
			  _internalRepresentation (valueFromUBytes (a.bytes ()))
							{ if (_size > 4) _size = 4; }

		Address (const UBytes& a, bool bE = true)
			: _size (a.size ()),
			  _internalRepresentation (valueFromUBytes (a.bytes (), bE))
							{ if (_size > 4) _size = 4; }

		Address (const std::vector <UByte>& a, bool bE = true)
			: _size (a.size ()),
			  _internalRepresentation (valueFromUBytes (a, bE))
							{ if (_size > 4) _size = 4; }

		size_t size () const
							{ return (_size); }

		unsigned int value () const
							{ return (_internalRepresentation);}
		UBytes values () const
							{ return (UBytes (bytes ())); }
		inline std::vector <UByte> bytes () const;

		/** Could be negative. */
		int distanceWith (const Address& a) const
							{ return ((int) ((long long) a._internalRepresentation - (long long) _internalRepresentation)); }

		/** When reach the limit starts back from 0 (as it is a unsigned int). */
		Address next (size_t n) const
							{ return (Address (_size, _internalRepresentation + (unsigned int) n /** Can overflow. */)); }
		Address previous (size_t n) const
							{ return (Address (_size, _internalRepresentation - (unsigned int) n /** Can overflow. */)); }

		bool operator == (const Address& a) const
							{ return (_internalRepresentation == a._internalRepresentation); }
		bool operator != (const Address& a) const
							{ return (_internalRepresentation != a._internalRepresentation); }
		bool operator < (const Address& a) const
							{ return (_internalRepresentation < a._internalRepresentation); }
		bool operator <= (const Address& a) const
							{ return (_internalRepresentation <= a._internalRepresentation); }
		bool operator > (const Address& a) const
							{ return (_internalRepresentation > a._internalRepresentation); }
		bool operator >= (const Address& a) const
							{ return (_internalRepresentation >= a._internalRepresentation); }

		Address operator + (size_t n) const
							{ return (Address (*this).next (n)); }
		friend Address operator + (size_t n, const Address& a)
							{ return (a + n); }
		Address& operator += (size_t n)
							{ *this = *this + n; return (*this); }
		int operator - (const Address& a) const
							{ return (a.distanceWith (*this)); }
		Address operator - (size_t n) const
							{ return (Address (*this).previous (n)); }
		friend Address operator - (size_t n, const Address& a)
							{ return (a - n); }
		Address& operator -= (size_t n)
							{ *this = *this - n; return (*this); }
		Address& operator -- ()
							{ return (*this -= 1); }

		UByte operator [] (size_t p) const
							{ return (bytes ()[p]); }

		/** Like a UByte in Big-Endian format. */
		std::string asString (UByte::OutputFormat oF, 
			char s /** separator */, size_t l = 0 /** Minimum length per UByte */) const
							{ return (values ().asString (oF, s, l)); }

		friend std::ostream& operator << (std::ostream& o, const Address& a)
							{ return (o << a.asString (UByte::OutputFormat::_HEXA, '\0', 2 /** sizeof (unsigned char) * 2 */)); }

		/** To create an Address from an string.\n
			The string should start with $ if written in hexadecimal, with 0 if it is in octal and with no 0 number in decimal: \n
			e.g $D400 (hexa), 07600 (octal), 53248 (decimal). \n
			If the str is not valid, then an empty address will be given. */
		inline static Address fromStr (const std::string& str);

		private:
		/** To get an address from a set of UByte. */
		static inline unsigned int valueFromUBytes (const std::vector <UByte>& a, bool bE = true);

		private:
		size_t _size;
		unsigned int _internalRepresentation;
	};

	// To simplify the management of a list of addresses...
	using Addresses = std::vector <Address>;

	// ---
	inline std::vector <UByte> Address::bytes () const
	{
		std::vector <UByte> result;

		for (size_t i = 0; i < _size; i++)
		{
			size_t bS = (_size - 1 - i) * UByte::sizeBits ();
			result.emplace_back 
				(UByte ((unsigned char) ((_internalRepresentation & (0xff << bS)) >> bS)));
		}

		return (result);
	}

	// ---
	inline Address Address::fromStr (const std::string& str)
	{ 
		UInt tmp = UInt::fromStr (str); 
		
		return (tmp.size () == 0 ? Address () : Address (tmp.size (), tmp.asUnsignedInt ())); 
	}

	// ---
	inline unsigned int Address::valueFromUBytes (const std::vector <UByte>& a, bool bE)
	{
		unsigned int result = 0;

		for (size_t i = 0; i < a.size (); i++)
			result += a [i].value () * (1 << ((bE ? (a.size () - i - 1) : i) * UByte::sizeBits ()));

		return (result);
	}
}

#endif
  
// End of the file
/*@}*/
