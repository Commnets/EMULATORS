/** \ingroup VIC20 */
/*@{*/

/**	
 *	@file	
 *	File: VIA2Registers.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 17/12/2023 \n
 *	Description: VIC20 VIA 2 Registers Emulation
 *	Versions: 1.0 Initial
 */

#ifndef __VIC20_VIA2REGISTERS__
#define __VIC20_VIA2REGISTERS__

#include <COMMODORE/incs.hpp>

namespace VIC20
{
	class CIA2;

	/** In the CIA2 Memory, there are a couple of records that behave different
		when they are read that when they are written. */
	class VIA2Registers final : public COMMODORE::VIARegisters
	{
		public:
		friend CIA2;

		static const int _VIA2_SUBSET = 2010;

		VIA2Registers (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s);

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
