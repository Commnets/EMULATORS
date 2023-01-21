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
	/** Representing a set of UByte.
		The class includes static methods to save / load bytes into / from a bin file. */
	class UBytes final
	{
		public:
		static const UBytes _E;

		UBytes ()
			: _values ()
							{ }

		UBytes (const std::vector <UByte>& v, bool bE = true)
			: _values (v)
							{ if (size () > 1 && !bE) std::reverse (_values.begin (), _values.end ()); }

		UBytes (std::vector <UByte>&& v, bool bE = true) noexcept
			: _values (std::move (v))
							{ if (size () > 1 && !bE) std::reverse (_values.begin (), _values.end ()); }

		size_t size () const
							{ return (_values.size ()); }
		size_t sizeBits () const
							{ return (size () * UByte::sizeBits ()); }

		inline void setMinLength (size_t l, 
			bool r = true /** on the right or not? where to introduce the additional byte. */);

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

		inline UBytes complement () const;

		inline bool shiftLeftC (bool c = false /** value to introduce. */, size_t p = 1);
		UBytes& shiftLeft (size_t p = 1)
							{ shiftLeftC (false, p); return (*this); }
		inline bool shiftRightC (bool c = false, size_t p = 1);
		UBytes& shiftRight (size_t p = 1)
							{ shiftRightC (false, p); return (*this); }
		inline bool rotateLeftC (bool c = false, size_t p = 1);
		inline UBytes& rotateLeft (size_t p = 1);
		inline bool rotateRightC (bool c = false, size_t p = 1);
		inline UBytes& rotateRight (size_t p = 1);

		/** 
		  *	Just to do a bit adding.
		  *	But very interesting method.
		  *	@param	u		The bytes to add.
		  * @param	cin		Is there any carry at the beginning to take into account?
		  * @param	cout	It is actualized whether a carry is generated from the adding.
		  * @param	o		Is is actualized whether an overflow is generated, 
		  *					that is when the MSBits of inputs are equal and different from the output. 
		  */
		inline UBytes bitAdding (const UBytes& u, bool cin, bool& cout, bool& o) const;

		inline UBytes reverse () const;

		inline bool operator == (const UBytes& u) const
							{ return (_values == u._values); }
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

		// To save and load binary data...
		/** To load bytes from a file. The variable (parameter) "e" will hold true when error. */
		static std::vector <MCHEmul::UByte> loadBytesFrom (const std::string& fN, bool& e);
		/** To save bytes to a file. Returns true when everything ok. */
		static bool saveBytesTo (const std::string& fN, const std::vector <UByte>& u);
		static bool saveBytesTo (const std::string& fN, const MCHEmul::UBytes& u)
							{ return (saveBytesTo (fN, u.bytes ())); }

		private:
		/** Most significant bit in the first elemnt. */
		std::vector <UByte> _values;
	};

	// ---
	inline void UBytes::setMinLength (size_t l, bool r)
	{
		size_t s = size ();
		if (s < l)
		{
			for (size_t i = 0; i < (l - s); i++)
			{
				if (r) _values.emplace_back (UByte::_0);
				else _values.insert (_values.begin (), UByte::_0);
			}
		}
	}

	// ---
	inline UBytes MCHEmul::UBytes::complement () const
	{
		UBytes result = *this;
		for (auto &i : result._values) 
			i = i.complement ();

		return (result);
	}

	// ---
	inline bool UBytes::shiftLeftC (bool c, size_t p)
	{
		bool nC = c;
		for (size_t i = 0; i < p; i++)
		{
			bool mc = c;
			for (int j = (int) (size () - 1); j >= 0; j--)
				mc = _values [(size_t) j].shiftLeftC (mc, 1);
			nC = mc;
		}

		return (nC);
	}

	// ---
	inline bool UBytes::shiftRightC (bool c, size_t p)
	{
		bool nC = c;
		for (size_t i = 0; i < p; i++)
		{
			bool mc = c;
			for (size_t j = 0; j < size (); j++)
				mc = _values [j].shiftRightC (mc, 1);
			nC = mc;
		}

		return (nC);
	}

	// ---
	inline bool UBytes::rotateLeftC (bool c, size_t p)
	{
		bool nC = c;
		for (size_t i = 0; i < p; i++)
		{
			bool mc = c;
			for (int j = (int) (size () - 1); j >= 0; j--)
				mc = _values [(size_t) j].rotateLeftC (mc, 1);
			nC = mc;
		}

		return (nC);
	}

	// ---
	inline UBytes& MCHEmul::UBytes::rotateLeft (size_t p)
	{
		for (size_t i = 0; i < p; i++)
			rotateLeftC (bit (sizeBits () - 1), 1);

		return (*this);
	}

	// ---
	inline bool UBytes::rotateRightC (bool c, size_t p)
	{
		bool nC = c;
		for (size_t i = 0; i < p; i++)
		{
			bool mc = c;
			for (size_t j = 0; j < size (); j++)
				mc = _values [j].rotateRightC (mc, 1);
			nC = mc;
		}

		return (nC);
	}

	// ---
	inline UBytes& MCHEmul::UBytes::rotateRight (size_t p)
	{
		for (size_t i = 0; i < p; i++)
			rotateRightC (bit (0), 1);

		return (*this);
	}

	// ---
	inline UBytes MCHEmul::UBytes::bitAdding (const UBytes& u, bool cin, bool& cout, bool& o) const
	{
		// If the elements to add already have the same size, no silly copies are done...

		UBytes result = *this;
		cout = cin /** To simplify the loop. */;
		if (size () != u.size ())
		{
			UBytes u2 = u;
			size_t mL = std::max (result.size (), u2.size ());
			// The operation has to be done with UBytes of the same length...
			result.setMinLength (mL, false /** at the beginning the additional 0. */); u2.setMinLength (mL, false);

			for (int i = (int) (result.size () - 1); i >= 0; i--) 
				result [i] = result [i].bitAdding (u2 [i], cout, cout, o);
		}
		else
		{
			for (int i = (int) (result.size () - 1); i >= 0; i--) 
				result [i] = result [i].bitAdding (u [i], cout, cout, o);
		}

		return (result);
	}

	// ---
	inline UBytes UBytes::reverse () const
	{
		std::vector <UByte> dt = _values;
		std::reverse (dt.begin (), dt.end ());
		return (UBytes (std::move (dt) /** no longer used. */));
	}
}

#endif
  
// End of the file
/*@}*/