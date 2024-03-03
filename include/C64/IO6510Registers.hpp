/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: IO6510Registers.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 15/05/2021 \n
 *	Description: Memory locations 0 & 1.
 *	Versions: 1.0 Initial
 */

#ifndef __C64_IO6150REGISTERS__
#define __C64_IO6150REGISTERS__

#include <COMMODORE/incs.hpp>

namespace C64
{
	class IO6510Registers final : public MCHEmul::ChipRegisters
	{
		public:
		/** A name when this register is a subset in the main memory. */
		static const int _IO6510REGISTERS_SUBSET = 2040;

		// Some events.
		/** Event when the C64 Port IO bits are actualized. 
			This even is sent when the bits 0,1,2 are modified at the register 0x01. */
		static const unsigned int _C64PORTIOBITSACTUALIZED = 300;

		IO6510Registers (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s);

		virtual size_t numberRegisters () const override
							{ return (2); }

		virtual void initialize () override;

		private:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* ntier) override;

		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;
		virtual const MCHEmul::UByte& readValue (size_t p) const override;

		// Implementation
		void initializeInternalValues ();

		private:
		bool _CHAREN, _HIRAM, _LORAM;
		bool _casetteData, _casetteNoKeyPressed, _casetteMotorStopped;

		// Implementation
		mutable MCHEmul::UByte _lastValueRead;
	};
}

#endif
  
// End of the file
/*@}*/
