/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: CIA2Registers.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 15/05/2021 \n
 *	Description: C64 CIA 2 Registers Emultation
 *	Versions: 1.0 Initial
 */

#ifndef __C64_CIA2REGISTERS__
#define __C64_CIA2REGISTERS__

#include <COMMODORE/incs.hpp>

namespace C64
{
	class CIA2;

	/** In the CIA2 Memory, there are a couple of records that behave different
		when they are read that when they are written. */
	class CIA2Registers final : public COMMODORE::CIARegisters
	{
		public:
		friend CIA2;

		static const int _CIA2_SUBSET = 1003;

		CIA2Registers (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s);

		unsigned char VICIIBank () const
							{ return (_VICBank); }

		private:
		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;
		virtual const MCHEmul::UByte& readValue (size_t p) const override;

		// Implementation
		virtual void initializeInternalValues () override;

		private:
		// Reference to the timers...
		COMMODORE::CIATimer* _timerA;
		COMMODORE::CIATimer* _timerB;
		// Reference to the clock...
		COMMODORE::CIAClock* _clock;

		/** The bank managed by VICII is controlled from this Chip. */
		unsigned char _VICBank;
	};
}

#endif
  
// End of the file
/*@}*/
