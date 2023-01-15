/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: C64.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 11/04/2021 \n
 *	Description: The global structure defining a c64 machine.
 *	Versions: 1.0 Initial
 */

#ifndef __C64_C64__
#define __C64_C64__

#include <CORE/incs.hpp>
#include <COMMODORE/incs.hpp>
#include <C64/Screen.hpp>
#include <C64/UserPeripherals.hpp>
#include <C64/CIA1.hpp>
#include <C64/CIA2.hpp>

namespace C64
{
	/** 
	  * There were two main lines of versions for the C64, atenfing to the standard of video system: NTSC and PAL.
	  *	However the constructor receives attributes comming from the emulator class (createComputer method)
	  *	and within those there might be attributes to define specific devices connected.
	  */
	class Commodore64 final : public COMMODORE::Computer
	{
		public:
		enum class VisualSystem { _NTSC, _PAL };

		Commodore64 (VisualSystem vS = VisualSystem::_PAL /** Europe initially */);

		virtual bool connect (MCHEmul::IOPeripheral* p, MCHEmul::IODevice* d) override;

		virtual bool initialize (bool iM = true) override;

		// To get direct access to the most important C64 chips...
		const COMMODORE::VICII* vicII () const
							{ return (dynamic_cast <const COMMODORE::VICII*> 
								((* _chips.find (COMMODORE::VICII::_ID)).second)); }
		COMMODORE::VICII* vicII ()
							{ return (dynamic_cast <COMMODORE::VICII*> 
								((* _chips.find (COMMODORE::VICII::_ID)).second)); }
		const CIA1* cia1 () const
							{ return (dynamic_cast <const CIA1*> ((* _chips.find (CIA1::_ID)).second)); }
		CIA1* cia1 ()
							{ return (dynamic_cast <CIA1*> ((* _chips.find (CIA1::_ID)).second)); }
		const CIA2* cia2 () const
							{ return (dynamic_cast <const CIA2*> ((*_chips.find (CIA2::_ID)).second)); }
		CIA2* cia2 ()
							{ return (dynamic_cast <CIA2*> ((*_chips.find (CIA2::_ID)).second)); }
		const COMMODORE::SID* sid () const
							{ return (dynamic_cast <const COMMODORE::SID*> 
								((*_chips.find (COMMODORE::SID::_ID)).second)); }
		COMMODORE::SID* sid ()
							{ return (dynamic_cast <COMMODORE::SID*> 
								((*_chips.find (COMMODORE::SID::_ID)).second)); }

		private:
		// Implementation
		static MCHEmul::Chips standardChips (VisualSystem vS);
		static MCHEmul::IODevices standardDevices (VisualSystem vS);

		protected:
		/** The video system used by the commodore 64. */
		VisualSystem _visualSystem;
		/** Other parameters used when initialized. */
		MCHEmul::Attributes _parameters;
	};
}

#endif
  
// End of the file
/*@}*/
