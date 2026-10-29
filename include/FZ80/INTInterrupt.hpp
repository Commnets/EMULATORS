/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: INTInterrupt.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 11/11/2023 \n
 *	Description: The INT type of interrupt in FZ80.
 *	Versions: 1.0 Initial
 */

#ifndef __FZ80_INTINTERRUPT__
#define __FZ80_INTINTERRUPT__

#include <CORE/incs.hpp>
#include <FZ80/Interrupt.hpp>

namespace FZ80
{
	/** A Maskarable Interrupt. It could be avoided using the bit I of the status flag. */
	class INTInterrupt final : public Interrupt
	{
		public:
		static const int _ID = 0;

		INTInterrupt ();

		/** To manage the mode of the interruption. */
		unsigned char INTMode () const
							{ return (_INTMode); }
		inline void setINTMode (unsigned char iM); // To set also the number of cycles to lunch...

		const MCHEmul::UByte& dataBusValue () const
							{ return (_dataBusValue); }
		void setDataBusValue (const MCHEmul::UByte& v)
							{ _dataBusValue = v; }

		/** Selects immutable response timing from the mode and acknowledge byte. \n
			IM0 is described only for RST opcodes; other injected instructions remain unsupported. */
		virtual const FZ80::MCyclesData* memoryAccessTiming () const override;

		virtual void initialize () override;

		/**
		  *	The name of the fields are: \n
		  *	The ones from the FZ80::Interrupt +
		  *	MODE			= The mode of execution of the interruption. \n
		  *	DATABUS			= The byte read during the interrupt acknowledge.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		virtual unsigned int isTime (MCHEmul::CPU* c, unsigned int cC) const override;
		virtual bool executeOverImpl (MCHEmul::CPU* c, unsigned int cC) override;

		private:
		/** Complete nominal RST-like response, shared by IM1 and IM0 RST. */
		const FZ80::MCyclesStructure _im1MCycles;
		/** Complete nominal vectored response, including both table reads. */
		const FZ80::MCyclesStructure _im2MCycles;

		/** The type of interruption:
			0:	The interruption executes as instruction the next value in the data bus.
			1:	The interruption executes the routine in a specific memory address. 
			2:	The interruption executes the routine through out a table. */
		unsigned char _INTMode;

		// Implementation
		/** The byte read from the data bus during the interrupt acknowledge. */
		MCHEmul::UByte _dataBusValue;
		/** Immutable timing indexes derived from the owned response descriptions. */
		const FZ80::MCyclesData _im1Timing, _im2Timing;
	};

	// ---
	inline void INTInterrupt::setINTMode (unsigned char iM)
	{
		_INTMode = iM;
		if (_INTMode == 0)
			setCyclesToLaunch (2); // + the ones that the instruction will take...
								 // This mode is very dynamic! (@see _cyclesAfterLaunch)
		else
		if (_INTMode == 1)
			setCyclesToLaunch (13); // like restart (11) + 2
		else
		if (_INTMode == 2)
			setCyclesToLaunch (19); // 7 to fetch the lower address + 6 to save the program counter + 6 to make the jump
		else
			assert (false); // If should be here...
	}
}

#endif
  
// End of the file
/*@}*/
