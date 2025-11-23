/** \ingroup C264 */
/*@{*/

/**	
 *	@file	
 *	File: IO7510PortRegisters.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 23/11/2025 \n
 *	Description: The 7501/8501 ports within the C264, that includss some more detail. \n
 *	Versions: 1.0 Initial
 */

#ifndef __C264_IO7150PORTREGISTERS__
#define __C264_IO7150PORTREGISTERS__

#include <F6500/incs.hpp>

namespace C264
{
	/** In the C264 implementation,
		the bits of the port are distributed as follows:
		bit 0	= ATN Serie, \n
		bit 1	= DATA Serie, \n
		bit 2	= CLK Serie, \n
		bit 3	= WREN: Write allowed, \n
		bit 4	= WRITE: Written signal, \n
		bit 5 	= MOTOR: Moton on (0 when it is running), \n
		bit 6	= SENSE: Play key on (0 when something is pressed), \n
		bit 7	= READ: Signal read */
	class IO7501PortRegisters final : public F6500::IO7501PortRegisters
	{
		public:
		static const int _IO7501REGISTERS_SUBSET = 2040;

		// Some events.
		/** Event when the C264 Port IO bits are actualized. 
			This even is sent when any bit is modified at the register 0x01. */
		static const unsigned int _C264PORTIOBITSACTUALIZED = 300;

		/** The set commands are not buffered in this case. */
		IO7501PortRegisters (MCHEmul::PhysicalStorage* ps);
							
		private:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* ntier) override;
	
		virtual void notifyPortChanges (const MCHEmul::UByte& c, const MCHEmul::UByte& v) override;
	
		// Implementation
		void initializeInternalValues ();
	};
}

#endif
  
// End of the file
/*@}*/
