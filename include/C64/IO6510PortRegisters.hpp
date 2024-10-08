/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: IO6510PortRegisters.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 01/03/2024 \n
 *	Description: The 6510 with in the C64, that includss some more detail. \n
 *	Versions: 1.0 Initial
 */

#ifndef __C64_IO6150PORTREGISTERS__
#define __C64_IO6150PORTREGISTERS__

#include <F6500/incs.hpp>

namespace C64
{
	/** In the C64 implementation,
		the bits of the port are distributed as follows:
		bit 0	= _LORAM, \n
		bit 1	= _HIRAM, \n
		bit 2	= _CHAREN, \n
		bit 3	= caseette output data, \n
		bit 4	= casette key pressed (0 when something has been pressed), \n
		bit 5	= casette motor status (0 when running). */
	class IO6510PortRegisters final : public F6500::IO6510PortRegisters
	{
		public:
		static const int _IO6510REGISTERS_SUBSET = 2040;

		// Some events.
		/** Event when the C64 Port IO bits are actualized. 
			This even is sent when the bits 0,1,2 are modified at the register 0x01. */
		static const unsigned int _C64PORTIOBITSACTUALIZED = 300;

		/** The set commands are not buffered in this case. \n
			The IO6510Ports are connected to the PLA. \n
			When a change happens it is notified to the PLA and 
			the chip is ready to change the configuration of the memory. \n
			But it doesn't happen actually until the its simulated method is invoked. \n
			That PLA simulation happens just after CPU simulation, so the change in the configuration is almost inmediate. \n
			So if the next CPU instruction (after writting in the port) used the new memory configuration, everything would go ok.
			But if the set command were buffered instead, this wouldn't happen until the next simulation cycle
			and a crash could be issued. For this reason the set commands are not buffered. \n
			If the memory were not configured to buffer set commands alld the explanation above is useless. */
		IO6510PortRegisters (MCHEmul::PhysicalStorage* ps);
							
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
