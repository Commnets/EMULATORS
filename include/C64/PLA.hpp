/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: PLA.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 04/03/2023 \n
 *	Description: To emulate the PLA chip. \n
 *				 This chip is not well know and documented, but is critical controling things
 *				 like the access to the memory from the CPY/VICII. \n
 *				 The lines comming from the C64 IO Port (Byte 0x01, bits 0,1,2) and the 
 *				 _GAME and _EXROM lines from the expansion port are
 *				 inputs to this chip which determines the zones of the memory active to be accessed.
 *	Versions: 1.0 Initial
 */

#ifndef __C64_PLA__
#define __C64_PLA__

#include <COMMODORE/incs.hpp>

namespace C64
{	
	/* More information at: \n
	   http://skoe.de/docs/c64-dissected/pla/
	   http://www.zimmers.net/cbmpics/cbm/c64/pla.txt */
	class PLA final : public MCHEmul::Chip
	{
		public:
		/** The simulation of the chips are excecuted according with their ids. 
			It is important ths chip is executed the last one to adjust the memory 
			once the information from the rest has been received. */
		static const unsigned int _ID = 203;
		
		PLA ();

		virtual bool initialize () override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier*) override;

		// Implementation
		/** To know whether the status of the entry lines have been affected and 
			then it is necessary to adjust the distribution of the memory. \n
			Onece it is read, the internal variable is set back to false. */
		bool statusAffected () const
							{ bool r = _statusAffected; _statusAffected = false; return (r); }

		// -----
		// Different debug methods to simplify the internal code
		// and to make simplier the modification in case it is needed...
		/** Debug special situations...
			Take care using this instructions _deepDebugFile could be == nullptr... */
		void debugPLACycle (MCHEmul::CPU* cpu);
		// -----

		private:
		// Inputs to the PLA...
		/** From the C64 IO Port (Byte 0x011m bites 0,1,2). */
		bool _LORAM, _HIRAM, _CHAREN;
		bool _EXROM, _GAME;

		// Implementation
		mutable bool _statusAffected;
		bool _noneOrC8k, _c16k, _c8Or16k, _ultimax;
		bool _basicActive, _kernelActive, _charromActive_cpu, _charromActive_vicII, _ioActive_cpu;
		bool _romlActive, _romh1Active_cpu, _romh2Active_cpu, _romh2Active_vicII;
	};
}

#endif
  
// End of the file
/*@}*/
