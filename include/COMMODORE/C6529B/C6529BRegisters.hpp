/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: C6529BRegisters.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 09/02/2024 \n
 *	Description: C6529B Registers Emulation.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_C6529BREGISTERS__
#define __COMMODORE_C6529BREGISTERS__

#include <CORE/incs.hpp>

namespace COMMODORE
{
	class C6529B;

	/** This zone of the memory can be related with many things. \n
		As an example in the C264 series one C6529B chip latches half of the value matrix. */
	class C6529BRegisters : public MCHEmul::ChipRegisters
	{
		public:
		friend C6529B;

		static const int _C6529BREGS_SUBSET = 1080;

		C6529BRegisters (int id, MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s);

		// Managing directly the value of the ports...
		// These methods are not usually managed...
		/** Set the data to the port. */
		inline void setPortValue (const MCHEmul::UByte& v);
		/** Get the data from the port. */
		const MCHEmul::UByte& portValue () const
							{ return (_portValue); }
		/** Has the por t value changed. Becomes false after being tested. */
		bool portValueChanged () const
							{ return (_portValueChanged); }

		/** Port pulled up by default. */
		virtual void initialize () override;

		/**
		  *	The name of the fields are: \n
		  * The structure of Chip Registers plus:
		  * PORT			= Attribute: Value Port. \n
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		virtual void setValue (size_t p, const MCHEmul::UByte& v) override
							{ setPortValue (v); }
		virtual const MCHEmul::UByte& readValue (size_t p) const override
							{ return (portValue ()); }

		protected:
		/** The value of the port. */
		MCHEmul::UByte _portValue; // pulled up (= 0xff) by default...

		// Implementation
		/** Becomes true when the value port is changed. \n
			Once it is checked becomes back false. */
		MCHEmul::OBool _portValueChanged = false;
	};

	// ---
	inline void C6529BRegisters::setPortValue (const MCHEmul::UByte& v)
	{ 
		_portValueChanged = (v != _portValue);

		_portValue = v;
	}
}

#endif
  
// End of the file
/*@}*/
