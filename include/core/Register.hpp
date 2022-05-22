/** \ingroup core */
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

#include <global.hpp>
#include <core/Address.hpp>
#include <core/UBytes.hpp>

namespace MCHEmul
{
	/** A register is an internal storage area within any CPU. 
		Depending on the CPU's architecture, the length of a register can vary. */
	class Register
	{
		public:
		Register () = delete;

		Register (int id, const UBytes& v /** variable */)
			: _id (id), _values (v) 
							{ }

		Register (int id, const std::vector <UByte>&v /** variable */)
			: _id (id), _values (v) 
							{ }

		Register (const Register&) = default;

		Register& operator = (const Register&) = default;

		/** To initialize. */
		void initialize ()
							{ _values.to0 (); }

		int id () const
							{ return (_id); }
		size_t size () const
							{ return (_values.size ()); }
		const UBytes& values () const
							{ return (_values); }

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
		void set (const UBytes& v)
							{ if (accept (v)) _values = v; }
		/** Set the values from another register, only if they can be accepted. */
		void set (const Register& r)
							{ set (r.values ()); }

		bool operator == (const Register& r) const
							{ return (_values == r._values); }
		bool operator != (const Register& r) const
							{ return (_values != r._values); }

		UByte operator [] (size_t p) const
							{ return (_values [p]); }

		std::string asString (UByte::OutputFormat oF, char s /** separator */, size_t l = 0 /** Minimum length per UByte */) const
							{ return (_values.asString (oF, s, l)); }
			
		friend std::ostream& operator << (std::ostream& o, const Register& r);

		protected:
		int _id;
		UBytes _values;
	};

	using Registers = std::vector <Register>;

	/** It can not be modified. */
	static const Register NoRegister = Register (0, UBytes ());

	// Implementation
	/** Used in no const methods. It could be modified, but it is not as it is 0-length. */
	static Register TrashRegister = Register (0, UBytes ());
}

#endif
  
// End of the file
/*@}*/
