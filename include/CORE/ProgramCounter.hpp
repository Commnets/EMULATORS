/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: ProgramCounter.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 10/04/2021 \n
 *	Description: Defines the behaviour of a program counter.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_PROGRAMCOUNTER__
#define __MCHEMUL_PROGRAMCOUNTER__

namespace MCHEmul
{
	/** The program counter is a special register within any CPU, 
		keeping an address inside that is incremented or decremented as the program is executed. 
		The maximum size is the size of an unsigned int (4 bytes) = 4GBytes of addresable memory.  */
	class ProgramCounter final
	{
		public:
		ProgramCounter (size_t sz)
			: _size (sz),
			  _maxValue ((unsigned int) 1 << (8 * sz)),
			  _internalRepresentation (0)
							{ assert (_size <= sizeof (unsigned int)); }

		void initialize ()
							{ _internalRepresentation = 0; }

		Address asAddress () const
							{ return (Address (_size, _internalRepresentation)); }
		void setAddress (const Address& a)
							{ _internalRepresentation = UInt (a.bytes ()).asUnsignedInt (); adjust (); }

		void increment (unsigned int n = 1)
							{ _internalRepresentation += n; adjust (); }
		void decrement (unsigned int n = 1)
							{ _internalRepresentation -= n; adjust (); }

		bool operator == (const Address& a) const
							{ return (asAddress () == a); }
		bool operator != (const Address& a) const
							{ return (asAddress () != a); }
		bool operator > (const Address& a) const
							{ return (asAddress () > a); }
		bool operator >= (const Address& a) const
							{ return (asAddress () >= a); }
		bool operator < (const Address& a) const
							{ return (asAddress () < a); }
		bool operator <= (const Address& a) const
							{ return (asAddress () != a); }

		ProgramCounter operator + (unsigned int n) const
							{ ProgramCounter r = *this; r.increment (n); return (r); }
		ProgramCounter& operator += (unsigned int n)
							{ increment (n); return (*this); }
		ProgramCounter operator - (unsigned int n) const
							{ ProgramCounter r = *this; r.decrement (n); return (r); }
		ProgramCounter& operator -= (unsigned int n)
							{ decrement (n); return (*this); }

		/** Like an address. */
		std::string asString () const
							{ return ("PC:$" + asAddress ().asString (UByte::OutputFormat::_HEXA, '\0', 2)); }

		friend std::ostream& operator << (std::ostream& o, const ProgramCounter& pc)
							{ return (o << pc.asString ()); }

		/** The very internal values. 
			Very rare used! */
		unsigned int maxValue () const
							{ return (_maxValue); }
		unsigned int internalRepresentation () const
							{ return (_internalRepresentation); }

		private:
		void adjust ()
							{ if (_internalRepresentation >= _maxValue) 
								_internalRepresentation = _internalRepresentation % _maxValue; }

		private:
		size_t _size;

		// Implementation
		unsigned int _maxValue; // Adjusted at construction time...
		unsigned int _internalRepresentation;
	};
}

#endif
  
// End of the file
/*@}*/