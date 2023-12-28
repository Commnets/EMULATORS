/** \ingroup VIC20 */
/*@{*/

/**	
 *	@file	
 *	File: VIA1Registers.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 17/12/2023 \n
 *	Description: VIC20 VIA 1 Registers Emulation.
 *	Versions: 1.0 Initial.
 */

#ifndef __VIC20_VIA1REGISTERS__
#define __VIC20_VIA1REGISTERS__

#include <COMMODORE/incs.hpp>

namespace VIC20
{
	class VIA1;

	/** In the VIA1 Memory, there are a couple of records that behave different
		when they are read that when they are written. */
	class VIA1Registers final : public COMMODORE::VIARegisters
	{
		public:
		friend VIA1;

		static const int _VIA1_SUBSET = 2100;

		VIA1Registers (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s);

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
