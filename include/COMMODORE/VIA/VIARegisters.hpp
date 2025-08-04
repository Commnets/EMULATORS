/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: VIARegisters.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 16/12/2023 \n
 *	Description: VIA Registers Emulation
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_VIAREGISTERS__
#define __COMMODORE_VIAREGISTERS__

#include <CORE/incs.hpp>
#include <COMMODORE/VIA/VIAControlLines.hpp>
#include <COMMODORE/VIA/VIATimer.hpp>
#include <COMMODORE/VIA/VIAShiftRegister.hpp>

namespace COMMODORE
{
	class VIA;
	class VIAControlLine;
	class VIATimer;
	class VIAShiftRegister;
	class VIAPortA;
	class VIAPortB;

	/** In the VIA Memory, there are a couple of records that behave different
		when they are read that when they are written. \n
		Read this document for better understanding: \n
		https://web.archive.org/web/20160108173129if_/http://archive.6502.org/datasheets/mos_6522_preliminary_nov_1977.pdf */
	class VIARegisters : public MCHEmul::ChipRegisters
	{
		public:
		friend VIA;

		VIARegisters (int id, MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s);

		virtual size_t numberRegisters () const override
							{ return (0x10); }

		virtual void initialize () override;

		/** To know whether the interrupt has or not to be launched,
			becasue any of the elements integrating the VIA has generated one allowed. */
		inline bool launchInterrupt () const;
		/** Just to have a code with the reason of the interrupt when happens. */
		inline unsigned int reasonIntCode () const;

		protected:
		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;
		virtual const MCHEmul::UByte& readValue (size_t p) const override;
		virtual const MCHEmul::UByte& peekValue (size_t p) const override;

		// Implementation
		virtual void initializeInternalValues ();

		// This methods are invoked from CIA chip...
		/** Link to the "ControlLines". */
		void lookAtControlLines (VIAControlLineType1* ca1, VIAControlLineType2* ca2, 
			VIAControlLineType1* cb1, VIAControlLineType2* cb2)
							{ _CA1 = ca1; _CA2 = ca2; _CB1 = cb1; _CB2 = cb2; }
		/** Link to "Timers". */
		void lookAtTimers (VIATimer* tA, VIATimer* tB)
							{ _T1 = tA, _T2 = tB; }
		/** Link to "ShiftRegister". */
		void lookAtShiftRegister (VIAShiftRegister* s)
							{ _SR = s; }
		/** Link to "Ports". */
		void lookAtPorts (VIAPortA* pA, VIAPortB* pB)
							{ _PA = pA; _PB = pB; }

		protected:
		// Elements related with this one...
		/** The control lines: _CA1, _CA2, CB1, _CB2 */
		VIAControlLineType1 *_CA1, *_CB1;
		VIAControlLineType2 *_CA2, *_CB2;
		/** The timers = T1 & T2 */
		VIATimer *_T1, *_T2;
		/** The shift register = SR */
		VIAShiftRegister* _SR;
		/** The ports = PA & PB */
		VIAPortA *_PA; 
		VIAPortB *_PB;

		// Implementation
		/** Keeps always the last value read. */
		mutable MCHEmul::UByte _lastValueRead;
	};

	// ---
	inline bool VIARegisters::launchInterrupt () const
	{
		return (_T1	 -> launchInterrupt () ||
				_T2  -> launchInterrupt () ||
			    _CB1 ->	launchInterrupt () ||
				_CB2 ->	launchInterrupt () ||
				_SR  -> launchInterrupt () ||
				_CA1 ->	launchInterrupt () ||
				_CA2 ->	launchInterrupt ());
	}

	// ---
	inline unsigned int VIARegisters::reasonIntCode () const
	{
		return ((_T1  -> launchInterrupt () ? 1 : 0) +
				(_T2  -> launchInterrupt () ? 2 : 0) +
				(_CB1 -> launchInterrupt () ? 4 : 0) +
				(_CB2 -> launchInterrupt () ? 16 : 0) +
				(_SR  -> launchInterrupt () ? 32 : 0) +
				(_CA1 -> launchInterrupt () ? 64 : 0) + 
				(_CA2 -> launchInterrupt () ? 128 : 0));
	}
}

#endif
  
// End of the file
/*@}*/
