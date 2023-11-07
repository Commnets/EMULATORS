/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: Register.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 10/04/2021 \n
 *	Description: Defines common elements to any Register within a CPU.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_REGISTER__
#define __MCHEMUL_REGISTER__

#include <CORE/global.hpp>
#include <CORE/Address.hpp>
#include <CORE/UBytes.hpp>

namespace MCHEmul
{
	/** A register is an internal storage area within any CPU. 
		Depending on the CPU's architecture, the length of a register can vary. */
	class Register
	{
		public:
		Register () = delete;

		Register (int id, const std::string& n, const UBytes& v /** variable */)
			: _id (id), _name (n), _values (v) 
							{ }

		Register (int id, const std::string& n, UBytes&& v) noexcept
			: _id (id), _name (n), _values (std::move (v)) 
							{ }

		Register (int id, const std::string& n, const std::vector <UByte>& v /** variable */)
			: _id (id), _name (n), _values (v) 
							{ }

		Register (int id, const std::string& n, std::vector <UByte>&& v) noexcept
			: _id (id), _name (n), _values (std::move (v)) 
							{ }

		/** To initialize. */
		void initialize ()
							{ _values.to0 (); }

		int id () const
							{ return (_id); }
		const std::string& name () const
							{ return (_name); }
		size_t size () const
							{ return (_values.size ()); }

		const UBytes& values () const
							{ return (_values); }
		const std::vector <UByte>& bytes () const
							{ return (_values.bytes ()); }

		UInt asUInt (bool bE) const
							{ return (UInt (_values, bE)); }
		Address asAddress (bool bE) const
							{ return (Address (UInt (_values, bE))); }

		/** The values can be accepted only if they have the same length than the register. */
		bool accept (const UBytes& v) const
							{ return (size () == v.size ()); }

		/** Set values only if they can be accepted. */
		void set (const std::vector <UByte>& v)
							{ if (accept (v)) _values = v; }
		void set (std::vector <UByte>&& v)
							{ if (accept (v)) _values = std::move (v); }
		void set (const UBytes& v)
							{ if (accept (v)) _values = v; }
		void set (UBytes&& v)
							{ if (accept (v)) _values = std::move (v); }

		/** Set the values from another register, only if they can be accepted. */
		void set (const Register& r)
							{ set (r.values ()); }

		bool operator == (const Register& r) const
							{ return (_values == r._values); }
		bool operator != (const Register& r) const
							{ return (_values != r._values); }

		const UByte& operator [] (size_t p) const
							{ return (_values [p]); }
		UByte& operator [] (size_t p)
							{ return (_values [p]); }

		std::string asString () const
							{ return (_name + "(" + std::to_string (_id) + "):$" + 
								_values.asString (UByte::OutputFormat::_HEXA, '\0', 2)); }
			
		friend std::ostream& operator << (std::ostream& o, const Register& r)
							{ return (o << r.asString ()); }

		protected:
		const int _id;
		const std::string _name;
		UBytes _values;
	};

	using Registers = std::vector <Register>;
	// Good when an access to a union of registers is needed...
	using RefRegisters = std::vector <Register*>;

	/** It can not be modified. */
	static const Register NoRegister = Register (0, "-", UBytes ());

	// Implementation
	/** Used in no const methods. It could be modified, but it is not as it is 0-length. */
	static Register TrashRegister = Register (0, "-", UBytes ());
}

#endif
  
// End of the file
/*@}*/
