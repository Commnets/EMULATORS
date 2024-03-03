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

		C6529BRegisters (int id, MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
			: MCHEmul::ChipRegisters (id, ps, pp, a, s),
			  _latchValue (MCHEmul::UByte::_0),
			  _latchChanged (false)
							{ setClassName ("6529Registers"); }

		// This method puts back _latchValue to false...
		bool latchChanged () const
							{ return (_latchChanged); }

		/**
		  *	The name of the fields are: \n
		  * The structure of Chip Registers plus:
		  * LATCHVALUE			= Attribute: Value latched. \n
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		virtual void setValue (size_t p, const MCHEmul::UByte& v) override
							{ _latchValue = v; _latchChanged = true; }
		virtual const MCHEmul::UByte& readValue (size_t p) const override
							{ return (_latchValue); }

		protected:
		MCHEmul::UByte _latchValue;
		// Set when the latched valued chages...
		MCHEmul::OBool _latchChanged;
	};
}

#endif
  
// End of the file
/*@}*/
