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

		/** Macros for the speed. */
		static const unsigned int _PALCLOCK		= 985248; // 0.985 MHz
		static const unsigned int _NTSCCLOCK	= 1022727; // 1.023 MHz

		Commodore64 (
			VisualSystem vS = VisualSystem::_PAL /** Europe initially */,
			const std::string& sS = "SID", /** Sound Emulation Library used is initially SID. */
			const std::string& lang = MCHEmul::_DEFAULTLANGUAGE /** English language used behind. */);

		virtual bool initialize (bool iM = true) override;

		// To get direct access to the most important C64 chips...
		// Other methods can be used at parent level to access other important chips.
		const CIA1* cia1 () const
							{ return (dynamic_cast <const CIA1*> ((* _chips.find (CIA1::_ID)).second)); }
		CIA1* cia1 ()
							{ return (dynamic_cast <CIA1*> ((* _chips.find (CIA1::_ID)).second)); }
		const CIA2* cia2 () const
							{ return (dynamic_cast <const CIA2*> ((*_chips.find (CIA2::_ID)).second)); }
		CIA2* cia2 ()
							{ return (dynamic_cast <CIA2*> ((*_chips.find (CIA2::_ID)).second)); }

		private:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		// Implementation
		static MCHEmul::Chips standardChips (VisualSystem vS, const std::string& sS /** The sound emulation library used. */);
		static MCHEmul::IODevices standardDevices (VisualSystem vS);

		protected:
		/** The video system used by the commodore 64. */
		VisualSystem _visualSystem;
	};
}

#endif
  
// End of the file
/*@}*/
