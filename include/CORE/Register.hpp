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
	/** A register is an internal storage area within any CPU. \n
		Depending on the CPU's architecture, the length of a register can vary. \n
		The register can be divided in parts as happens in some Chips like Z80 or 8086. */
	class Register
	{
		public:
		/** Every part (portencial) within a register. */
		struct Part
		{
			/** Every part is determined by an id, a name and
				the first and the last byte within the main register. */
			Part (const std::string& n, size_t fB, size_t lB)
				: _name (n), _firstByte (fB), _lastByte (lB)
							{ assert (_lastByte >= _firstByte); }

			size_t size () const
							{ return (_lastByte - _firstByte + 1); }

			std::string _name;
			size_t _firstByte;
			size_t _lastByte;
		};

		using Parts = std::vector <Part>;

		Register () = delete;

		/** The parts of a register can not be out of the size of the register itself. */
		Register (int id, const std::string& n, const UBytes& v /** variable */, const Parts& p = { })
			: _id (id), _name (n), _values (v),
			  _parts (p)
							{ for (const auto& i : _parts) assert (i._lastByte < _values.size ()); }

		Register (int id, const std::string& n, UBytes&& v, Parts&& p = { }) noexcept
			: _id (id), _name (n), _values (std::move (v)),
			  _parts (std::move (p))
							{ for (const auto& i : _parts) assert (i._lastByte < _values.size ()); }

		Register (int id, const std::string& n, const std::vector <UByte>& v /** variable */, const Parts& p = { })
			: _id (id), _name (n), _values (v),
			  _parts (p)
							{ for (const auto& i : _parts) assert (i._lastByte < _values.size ()); }

		Register (int id, const std::string& n, std::vector <UByte>&& v, Parts&& p = { }) noexcept
			: _id (id), _name (n), _values (std::move (v)),
			  _parts (std::move (p))
							{ for (const auto& i : _parts) assert (i._lastByte < _values.size ()); }

		/** To initialize. */
		void initialize ()
							{ _values.to0 (); }

		int id () const
							{ return (_id); }
		const std::string& name () const
							{ return (_name); }
		size_t size () const
							{ return (_values.size ()); }
		const Parts& parts () const // Parts cannot be modified...
							{ return (_parts); }
		const Part& part (size_t i) const // The access to the parts is secuential...
							{ return (_parts [i]); }

		const UBytes& values () const
							{ return (_values); }
		UBytes valuesPart (size_t i) const 
							{ return (UBytes (bytesPart (i))); }
		const std::vector <UByte>& bytes () const
							{ return (_values.bytes ()); }
		std::vector <UByte> bytesPart (size_t i) const 
							{ return (std::vector <UByte> 
								(bytes ().begin () + _parts [i]._firstByte, bytes ().begin () + _parts [i]._lastByte)); }

		UInt asUInt (bool bE) const
							{ return (UInt (_values, bE)); }
		UInt asUIntPart (size_t i, bool bE) const
							{ return (UInt (valuesPart (i), bE)); }
		Address asAddress (bool bE) const
							{ return (Address (UInt (_values, bE))); }
		Address asAddressPart (size_t i, bool bE) const
							{ return (Address (UInt (valuesPart (i), bE))); }

		/** The values can be accepted only if they have the same length than the register. */
		bool accept (const UBytes& v) const
							{ return (size () == v.size ()); }
		bool acceptPart (size_t i, const UBytes& v) const
							{ return (_parts [i].size () == v.size ()); }

		/** Set values only if they can be accepted. */
		void set (const std::vector <UByte>& v)
							{ if (accept (v)) _values = v; }
		void setPart (size_t i, const std::vector <UByte>& v)
							{ if (acceptPart (i, v)) 
								for (size_t j = _parts [i]._firstByte; j <= _parts [i]._lastByte; j++) 
									_values [j] = v [j - _parts [i]._firstByte]; }
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
		Parts _parts;
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
