/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: Latch.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 18/03/2023 \n
 *	Description: Special type of Registers that behaves like a latch whent accesed.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_LATCH__
#define __MCHEMUL_LATCH__

#include <CORE/global.hpp>
#include <CORE/Address.hpp>
#include <CORE/UBytes.hpp>

namespace MCHEmul
{
	/** A Latch latches the full value when set and release it when read. \n
		It means no other data can be set untless the one already set is read. */
	class Latch
	{
		public:
		Latch () = delete;

		Latch (int id, const std::string& n, const UBytes& v /** variable */)
			: _id (id), _name (n), _values (v),
			  _latched (false)
							{ }

		Latch (int id, const std::string& n, UBytes&& v) noexcept
			: _id (id), _name (n), _values (std::move (v)),
			  _latched (false)
							{ }

		Latch (int id, const std::string& n, const std::vector <UByte>& v /** variable */)
			: _id (id), _name (n), _values (v),
			  _latched (false)
							{ }

		Latch (int id, const std::string& n, std::vector <UByte>&& v) noexcept
			: _id (id), _name (n), _values (std::move (v)),
			  _latched (false)
							{ }

		/** To initialize. */
		void initialize ()
							{ _values.to0 (); _latched = false; }

		int id () const
							{ return (_id); }
		const std::string& name () const
							{ return (_name); }
		size_t size () const
							{ return (_values.size ()); }

		/** When the value is got the latched becomes false, */
		const UBytes& values () const
							{ _latched = false; return (_values); }
		const std::vector <UByte>& bytes () const
							{ _latched = false; return (_values.bytes ()); }

		UInt asUInt (bool bE) const
							{ return (UInt (values (), bE)); }
		Address asAddress (bool bE) const
							{ return (Address (UInt (values (), bE))); }

		/** The values can be accepted only if they have the same length than the register. */
		bool accept (const UBytes& v) const
							{ return (size () == v.size ()); }

		/** Set values only if they can be accepted. */
		void set (const std::vector <UByte>& v)
							{ if (accept (v) && !_latched) { _values = v; _latched = true; } }
		void set (std::vector <UByte>&& v)
							{ if (accept (v) && !_latched) { _values = std::move (v); _latched = true; } }
		void set (const UBytes& v)
							{ if (accept (v) && !_latched) { _values = v; _latched = true; } }
		void set (UBytes&& v)
							{ if (accept (v) && !_latched) { _values = std::move (v); _latched = true; } }

		/** Set the values from another register, only if they can be accepted. */
		void set (const Latch& r)
							{ set (r.values ()); }

		bool operator == (const Latch& r) const
							{ return (_values == r._values); }
		bool operator != (const Latch& r) const
							{ return (_values != r._values); }

		UByte operator [] (size_t p) const
							{ return (values ()[p]); }

		std::string asString () const
							{ return (_name + "(" + std::to_string (_id) + "):$" + 
								_values.asString (UByte::OutputFormat::_HEXA, '\0', 2)); }
			
		friend std::ostream& operator << (std::ostream& o, const Latch& r)
							{ return (o << r.asString ()); }

		protected:
		const int _id;
		const std::string _name;
		UBytes _values;
		
		// Implementation
		mutable bool _latched;
	};
}

#endif
  
// End of the file
/*@}*/
