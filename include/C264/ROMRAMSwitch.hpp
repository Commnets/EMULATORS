/** \ingroup C264 */
/*@{*/

/**	
 *	@file	
 *	File: ROMRAMSwitch.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 10/03/2024 \n
 *	Description: In the C264 series memory, 
 *				 there is a couple of registers (from $fdd0 to fddf) that are used
 *				 to change the configuration of LOROM ($8000 - $bfff) and HIROM ($c000 - $fbff). \n
 *				 We have tried to emulate that behaviour with this cirtual Chip.
 *	Versions: 1.0 Initial
 */

#ifndef __C264_ROMRAMSWITCH__
#define __C264_ROMRAMSWITCH__

#include <COMMODORE/incs.hpp>

namespace C264
{
	class ROMRAMSwitchRegisters;

	class ROMRAMSwitch final : public MCHEmul::Chip
	{
		public:
		static const int _ID = 202;
		
		ROMRAMSwitch ();
		
		// Sets the default ROM access configuration...
		void setBasicROMAccessConfiguration ();

		virtual bool initialize () override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		private:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		private:
		// Implementation
		/** The memory is used also as the set of registers of the chip. */
		ROMRAMSwitchRegisters* _ROMRAMSwitchRegisters;
	};
}

#endif
  
// End of the file
/*@}*/
