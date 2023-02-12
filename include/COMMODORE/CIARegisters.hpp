/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: CIA2Registers.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 15/05/2021 \n
 *	Description: CIA Registers Emulation
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_CIAREGISTERS__
#define __COMMODORE_CIAREGISTERS__

#include <CORE/incs.hpp>

namespace COMMODORE
{
	class CIA;
	class CIATimer;
	class CIAClock;

	/** In the CIA Memory, there are a couple of records that behave different
		when they are read that when they are written. */
	class CIARegisters : public MCHEmul::ChipRegisters
	{
		public:
		friend CIA;

		CIARegisters (int id, MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s);

		virtual size_t numberRegisters () const override
							{ return (0x10); }

		virtual void initialize () override;

		/** To know when the flag line is enabled. */
		bool flagLineInterruptRequested () const
							{ return (_flagLineInterruptRequested); }
		void setFlagLineInterruptRequested (bool a)
							{ _flagLineInterruptRequested = a; }

		protected:
		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;
		virtual const MCHEmul::UByte& readValue (size_t p) const override;

		// Implementation
		virtual void initializeInternalValues ();

		// This methods are invoked from CIA chip...
		/** The registers will observe also the timers. */
		void lookAtTimers (CIATimer* tA, CIATimer* tB)
							{ _timerA = tA, _timerB = tB; }
		/** ...and also of the clock. */
		void lookAtClock (CIAClock* c)
							{ _clock = c; }

		protected:
		/** Reference to the timers */
		CIATimer* _timerA;
		CIATimer* _timerB;
		/** Reference to the clock */
		CIAClock* _clock;
		/** To indicate that the flag line has been or not activated. */
		bool _flagLineInterruptRequested;

		// Implementation
		mutable MCHEmul::UByte _lastValueRead;
	};
}

#endif
  
// End of the file
/*@}*/
