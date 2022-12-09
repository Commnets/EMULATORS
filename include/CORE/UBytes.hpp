/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: UBytes.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 26/04/2021 \n
 *	Description: Definition of a set of UByte. \n
 *				 Useful to define Words and Long Words used in advanced architectures.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_UBYTES__
#define __MCHEMUL_UBYTES__

#include <CORE/UByte.hpp>

namespace MCHEmul
{
	/** Representing a set of UByte. */
	class UBytes final
	{
		public:
		static const UBytes _E;

		UBytes ()
			: _values ()
							{ }

		UBytes (const std::vector <UByte>& v, bool bE = true)
			: _values (v)
							{ if (size () > 1 && !bE) _values = reverse ().bytes (); }

		UBytes (const UBytes&) = default;
		UBytes& operator = (const UBytes&) = default;

		size_t size () const
							{ return (_values.size ()); }
		size_t sizeBits () const
							{ return (size () * UByte::sizeBits ()); }

		void setMinLength (size_t l, bool r = true /** right or not? where to introduce the additional byte. */);

		const UByte& value (size_t p) const
							{ return (_values [p]); }
		UByte& value (size_t p)
							{ return (_values [p]); }

		const std::vector <UByte>& bytes () const
							{ return (_values); }

		UBytes LSUBytes (size_t p) const
							{ return (UBytes ((p >= size ()) ? bytes ()
								: std::vector <UByte> (bytes ().begin () + (bytes ().size () - p), bytes ().end ()))); }
		UBytes MSUBytes (size_t p) const
							{ return (UBytes (((p >= size ()) ? bytes () 
								: std::vector <UByte> (bytes ().begin (), bytes ().begin () + p)))); }

		/** If you requested for a bit bigger than sizeBits, the crash is guaranteed.
			No checks are done to increase the speed. */
		bool bit (size_t p) const 
							{ return (_values [size () - (p / UByte::sizeBits ()) - 1][p % UByte::sizeBits ()]); }
		void setBit (size_t p, bool s)
							{ _values [size () - (p / UByte::sizeBits ()) - 1].setBit (p % UByte::sizeBits (), s); }
		
		void to0 ()
							{ for (auto& i : _values) i = UByte::_0; }
		void toFF ()
							{ for (auto& i : _values) i = UByte::_FF; }

		UBytes complement () const;

		bool shiftLeftC (bool c = false /** value to introduce. */, size_t p = 1);
		UBytes& shiftLeft (size_t p = 1)
							{ shiftLeftC (false, p); return (*this); }
		bool shiftRightC (bool c = false, size_t p = 1);
		UBytes& shiftRight (size_t p = 1)
							{ shiftRightC (false, p); return (*this); }
		bool rotateLeftC (bool c = false, size_t p = 1);
		UBytes& rotateLeft (size_t p = 1);
		bool rotateRightC (bool c = false, size_t p = 1);
		UBytes& rotateRight (size_t p = 1);

		/** 
		  *	Just to do a bit adding.
		  *	But very interesting method.
		  *	@param	u		The bytes to add.
		  * @param	cin		Is there any carry at the beginning to take into account?
		  * @param	cout	It is actualized whether a carry is generated from the adding.
		  * @param	o		Is is actualized whether an overflow is generated, 
		  *					that is when the MSBits of inputs are equal and different from the output. 
		  */
		UBytes bitAdding (const UBytes& u, bool cin, bool& cout, bool& o) const;

		UBytes reverse () const;

		bool operator == (const UBytes& u) const;
		bool operator != (const UBytes& u) const
							{ return (!(*this == u)); }

		UByte operator [] (size_t p) const
							{ return (value (p)); }
		UByte& operator [] (size_t p) 
							{ return (value (p)); }
		UBytes& operator << (size_t p)
							{ return (shiftLeft (p)); }
		UBytes& operator >> (size_t p)
							{ return (shiftRight (p)); }

		std::string asString (UByte::OutputFormat oF, char s /** separator */, 
			size_t l = 0 /** Minimum length per UByte */, size_t sb = 0xffff /** size of the block, then end of line. */) const;

		friend std::ostream& operator << (std::ostream& o, const UBytes& u)
							{ return (o << u.asString (UByte::OutputFormat::_HEXA, '\0', 2 /** sizeof (unsigned char) * 2 */)); }

		static std::vector <MCHEmul::UByte> loadBytesFrom (const std::string& fN, bool& e);
		static bool saveBytesTo (const std::string& fN, const std::vector <UByte>& u);
		static bool saveBytesTo (const std::string& fN, const MCHEmul::UBytes& u)
							{ return (saveBytesTo (fN, u.bytes ())); }

		private:
		/** Most significant bit in the first elemnt. */
		std::vector <UByte> _values;
	};
}

#endif
  
// End of the file
/*@}*/