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

	/** This zone of the memory is very related with the matrix keyboard & joystick. \n
		Writting to any of their registers a specific column of the keyboard matrix will be selected. \n
		The value of that specific column must be read in the TED register 08 (@see TED at COMMODORE level). */
	class C6529BRegisters : public MCHEmul::ChipRegisters
	{
		public:
		friend C6529B;

		static const int _C6529BREGS_SUBSET = 1080;

		C6529BRegisters (int id, MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
			: MCHEmul::ChipRegisters (id, ps, pp, a, s)
							{ setClassName ("6529Registers"); }

		/** 16 registers that always behaves in the same way. */
		virtual size_t numberRegisters () const override
							{ return (0x0f); }

		/**
		  *	The name of the fields are: \n
		  * The structure of Chip Registers plus:
		  * LATCH			= Attribute: Value latched. \n
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		virtual void setValue (size_t p, const MCHEmul::UByte& v) override
							{ _latchKeyboard = v; } // All 16 registers behaves in the same way...
		virtual const MCHEmul::UByte& readValue (size_t p) const override
							{ return (_latchKeyboard); } // All 16 registers behaves in the same way...

		private:
		MCHEmul::UByte _latchKeyboard;
	};
}

#endif
  
// End of the file
/*@}*/
