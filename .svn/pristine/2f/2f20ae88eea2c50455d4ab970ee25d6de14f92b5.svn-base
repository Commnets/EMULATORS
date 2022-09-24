/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: Memory.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 15/05/2021 \n
 *	Description: C64 CIA Registers Emultation
 *	Versions: 1.0 Initial
 */

#ifndef __C64_CIAREGISTERS__
#define __C64_CIAREGISTERS__

#include <CORE/incs.hpp>

namespace C64
{
	/** In the CIA1 Memory, there are a couple of records that behave different
		when they are read that when they are written. */
	class CIA1Registers : public MCHEmul::Memory
	{
		public:
		CIA1Registers ()
			: MCHEmul::Memory (MCHEmul::Address ({ 0x00, 0xdc }, false), 0x0100)
							{ }

		protected:
		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;
		virtual MCHEmul::UByte readValue (size_t p) const override;
	};

	/** In the CIA2 Memory, there are a couple of records that behave different
		when they are read that when they are written. */
	class CIA2Registers : public MCHEmul::Memory
	{
		public:
		CIA2Registers ()
			: MCHEmul::Memory (MCHEmul::Address ({ 0x00, 0xdd }, false), 0x0100)
							{ }

		protected:
		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;
		virtual MCHEmul::UByte readValue (size_t p) const override;
	};
}

#endif
  
// End of the file
/*@}*/
