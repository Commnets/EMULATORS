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
		https://plus4world.powweb.com/plus4encyclopedia/500290
		The port is for serial data and casette management. \n
		There is no way to detect whether any key is pressed in the datsette as happens in the C64. \n
		bit 0	= Serial Data Out, \n
		bit 1	= Serial Clock Out/Casette Write, <= \n
		bit 2	= Serial ATN Out, \n
		bit 3	= Datasette motor out (0 = on, 1 = off), \n
		bit 4	= Datsette Read in, <= \n
		bit 5 	= Not implemented, \n
		bit 6	= Serial Clock In, \n
		bit 7	= Serial Data In */
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
