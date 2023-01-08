/** \ingroup CORE */
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
#include <map>

namespace MCHEmul
{
	 /** 
	  *	Representing a Integer, with and without sign. \n
	  *	It might by said that this class represents actually CPU's ALU (Arithmetic Logic Unit) as well. \n
	  * A number is represented by several UBytes. Those UBytes can be given either in big-endian format or low-endian one. \n
	  * That number can be represented in many different formats. \n
	  * The _BINARY one is the default and the most commonly used. \n
	  * The type of format used in many computers at 80's was the packaged BCD. \n
	  * The class is prepared to support additional formats further than those two ones. It can be extended somehow. See later. \n
	  * The negative numbers are always kept and "understood" using complement 2. \n
	  * The basic operation in an UInt is "adding". The rest are always refer to this one. \n
	  * When the UInt is transformed into/from a standard type it has to be a unsigned int. \n
	  * It means that the maximum number than can be got is (positive): 4.294.967.296.
	  */
	class UInt final
	{
		public:
		/** This class is accountable for managing aspects 
			specifically related with the format of a number. \n */
		class FormatManager
		{
			public:
			/** The way the add is executed depends on the format. */
			virtual UInt add (const UInt&, const UInt&, bool) const = 0;
			/** The same for substracting. */
			virtual UInt substract (const UInt&, const UInt&, bool) const = 0;
			/** To return the value of the UInt as an unsigned int. 
				The int like will be the unsigned int taking into account the sign. */
			virtual unsigned int asUnsignedInt (const UInt&) const = 0;
			/** To create the UInt from an unsigned int. */
			virtual UInt fromUnsignedInt (unsigned int n) = 0;
			/** The same but from a in. */
			virtual UInt fromInt (int n) = 0;
		};

		/** For _BINARY format. \n
			The very common format. */
		class BinaryFormatManager final : public FormatManager
		{
			public:
			/** For _BINARY numbers no adjustment is needed at all after adding operation. */
			virtual UInt add (const UInt& u1, const UInt& u2, bool cIn) const;
			virtual UInt substract (const UInt& u1, const UInt& u2, bool cIn) const;
			virtual unsigned int asUnsignedInt (const UInt& u) const override;
			virtual UInt fromUnsignedInt (unsigned int n) override;
			virtual UInt fromInt (int n) override;
		};

		/** For _PACKAGEDBCD format. */
		class PackagedBCDFormatManager final : public FormatManager
		{
			public:
			virtual UInt add (const UInt& u1, const UInt& u2, bool cIn) const;
			virtual UInt substract (const UInt& u1, const UInt& u2, bool cIn) const;
			virtual unsigned int asUnsignedInt (const UInt& u) const override;
			virtual UInt fromUnsignedInt (unsigned int n) override;
			virtual UInt fromInt (int n) override;
		};

		/** Another type of format could be created extendinf the FormatManger class. */

		// The different types of formats defined by default
		/** The very basic one and the default in most of the methods. */
		static const unsigned char _BINARY = 0x00;
		/** ...and also the Packaged BCD very common in 80's computers. */
		static const unsigned char _PACKAGEDBCD = 0x01;

		/** To create and destroy the format managers. 
			There can only be onky one instance of this class!. */
		class FormatManagers final
		{
			public:
			/** Get the formates used, Only one instance can exists. */
			FormatManagers (const std::map <unsigned char, FormatManager*>& fM)
				: _formatManagers (fM)
							{ if (_instance != nullptr) exit (0); }

			~FormatManagers ()
							{ _instance = nullptr; for (const auto& i : _formatManagers) delete (i.second);}

			public:
			/** It can be accessed from anyplace. \n
				So other formaters can be added later if needed. */
			std::map <unsigned char, FormatManager*> _formatManagers;

			private:
			// Implementation
			/** A very basic class with the two type of basic formats will be create. */
			static FormatManagers* _instance;			
		};

		/** The different format managers are kept in a public static map. \n
			The map is loaded with the basic formaters (BINARY and BCD), 
			buy others can be added if needed later by the user. */
		static std::map <unsigned char, FormatManager*> _formatManagers;

		/** The very basic numbers, represented in binary. */
		static const UInt _0;
		static const UInt _1;

		UInt ()
			: _values (),
			  _carry (false), _overflow (false),
			  _format (_BINARY)
							{ }

		UInt (const UByte& u, unsigned char f = _BINARY)
			: _values ({ u }),
			  _carry (false), _overflow (false),
			  _format (f)
							{ }

		UInt (UByte&& u, unsigned char f = _BINARY)
			: _values ({ std::move (u) }),
			  _carry (false), _overflow (false),
			  _format (f)
							{ }

		/** Always kept in Big-endian format. */
		UInt (const UBytes& u, bool bE = true, unsigned char f = _BINARY)
			: _values ((u.size () > 1 && !bE) ? u.reverse () : u),
			  _carry (false), _overflow (false),
			  _format (f)
							{ }

		UInt (UBytes&& u, bool bE = true, unsigned char f = _BINARY)
			: _values ((u.size () > 1 && !bE) ? std::move (u.reverse ()) : std::move (u)),
			  _carry (false), _overflow (false),
			  _format (f)
							{ }

		/** Always kept in Big-endian format. */
		UInt (const std::vector <UByte>& u, bool bE = true, unsigned char f = _BINARY)
			: _values (u, bE),
			  _carry (false), _overflow (false),
			  _format (f)
							{ }

		UInt (std::vector <UByte>&& u, bool bE = true, unsigned char f = _BINARY)
			: _values (std::move (u), bE),
			  _carry (false), _overflow (false),
			  _format (f)
							{ }

		/** From an unsigned int. */
		UInt (unsigned int n)
			: _values (fromUnsignedInt (n).bytes () /** Already in big-endian. */),
			  _carry (false), _overflow (false),
			  _format (_BINARY)
							{ }

		/** From an int. */
		UInt (int n)
			: _values (fromInt (n).bytes () /** Already in big-endian. */),
			  _carry (false), _overflow (false),
			  _format (_BINARY)
							{ }

		UInt (const UInt&) = default;

		UInt& operator = (const UInt&) = default;

		unsigned char format () const
							{ return (_format); }

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
		UInt complement () const
							{ return (MCHEmul::UInt (_values.complement (), true, _format)); }
		UInt complement_2 () const
							{ unsigned char f = _format; 
							  UInt r = complement (); r._format = _BINARY; r += _1; r._format = f; return (r); }
		UInt substract (const UInt& u, bool iC = true) const;
		UInt multiply (const UInt& u) const;
		UInt divide (const UInt& u) const;

		bool operator == (const UInt& u) const
							{ return (_values == u._values); }
		bool operator != (const UInt& u) const
							{ return (_values != u._values); }

		/** It is bigger when the difference between two quantities is positive 
			and then the carry generated is 1. */
		bool operator > (const UInt& u) const
							{ return ((*this >= u) && (*this != u)); }
		bool operator >= (const UInt& u) const
							{ return ((*this - u)._carry); }
		bool operator < (const UInt& u) const
							{ return ((*this <= u) && (*this != u)); }
		bool operator <= (const UInt& u) const
							{ return ((u - *this)._carry); }

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
		UInt operator / (const UInt& u) const
							{ return (divide (u)); }
		UInt& operator /= (const UInt & u)
							{ *this = *this / u; return (*this); }

		UByte operator [] (size_t p) const
							{ return (_values [p]); }
		UByte& operator [] (size_t p)
							{ return (_values [p]); }
		
		std::string asString (UByte::OutputFormat oF, char s /** separator */, size_t l = 0 /** Minimum length per UByte */) const
							{ return (_values.asString (oF, s, l /** No more than 16 bytes per line. */)); }

		unsigned int asUnsignedInt () const
							{ return (_formaters._formatManagers [_format] -> asUnsignedInt (*this)); }
		int asInt () const
							{ return (negative () ? -((int) complement_2 ().asUnsignedInt ()) : (int) asUnsignedInt ()); }

		static UInt fromUnsignedInt (unsigned int n, unsigned char f = _BINARY)
							{ return (_formaters._formatManagers [f] -> fromUnsignedInt (n)); }
		static UInt fromInt (int n, unsigned char f = _BINARY)
							{ return (_formaters._formatManagers [f] -> fromInt (n)); }
		/** When the str is written either in hexadecimal or binary the length of the UInt result 
			could be what ever. \n
			When the str is written in decimal or octal the equivalent UInt 
			can not have more bytes than be longer than "size_of (unsigned int)". */
		static UInt fromStr (const std::string& s, unsigned char f = _BINARY);

		/** To eliminate the 0 at the left. */
		static UInt adjust (const UInt& n)
							{ return (UInt (std::vector <UByte> (std::find_if (n.bytes ().begin (), 
								n.bytes ().end (), [](const UByte& u) -> bool { return (u.value () != 0); }), n.bytes ().end ()))); }

		friend std::ostream& operator << (std::ostream& o, const UInt& u)
							{ return (o << u.asString (UByte::OutputFormat::_HEXA, '\0', 2 /** sizeof (unsigned char) * 2 */)); }

		private:
		UBytes _values;
		/** The bytes can represent a number in BCD format. */
		unsigned char _format;

		// Implementation
		mutable bool _carry;
		mutable bool _overflow;
		static FormatManagers _formaters;
	};

}

#endif
  
// End of the file
/*@}*/