/** \ingroup VIC20 */
/*@{*/

/**	
 *	@file	
 *	File: VIC20.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 11/05/2023 \n
 *	Description: The global structure defining a VIC20 machine.
 *	Versions: 1.0 Initial
 */

#ifndef __VIC20_VIC20__
#define __VIC20_VIC20__

#include <CORE/incs.hpp>
#include <COMMODORE/incs.hpp>
#include <VIC20/Memory.hpp>
#include <VIC20/VIA1.hpp>
#include <VIC20/VIA2.hpp>

namespace VIC20
{
	class CommodoreVIC20 final : public COMMODORE::Computer
	{
		public:
		enum class VisualSystem { _NTSC, _PAL };

		/** Macros for the speed. \n
			In the original computer, 
			the speed if defined by the VIC1 (Video & Sound) Chip. */
		static const unsigned int _PALCLOCK		= 1108405; // 1.108 MHz
		static const unsigned int _NTSCCLOCK	= 1022727; // 1.023 MHz

		CommodoreVIC20 (Memory::Configuration cfg, 
			VisualSystem vS, const std::string& lg);

		virtual bool initialize (bool iM = true) override;

		// To get direct access to the most important VIC20 chips...
		// Other methods can be used at parent level to access other important chips.
		const VIA1* via1 () const
							{ return (dynamic_cast <const VIA1*> ((* _chips.find (VIA1::_ID)).second)); }
		VIA1* via1 ()
							{ return (dynamic_cast <VIA1*> ((* _chips.find (VIA1::_ID)).second)); }
		const VIA2* via2 () const
							{ return (dynamic_cast <const VIA2*> ((*_chips.find (VIA2::_ID)).second)); }
		VIA2* via2 ()
							{ return (dynamic_cast <VIA2*> ((*_chips.find (VIA2::_ID)).second)); }

		private:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		/** Adapt the VIC20 to the memory configuration. */
		void setConfiguration (Memory::Configuration cfg);

		// Implementation
		static MCHEmul::Chips standardChips (CommodoreVIC20::VisualSystem vS, const std::string& sS);
		static MCHEmul::IODevices standardDevices (CommodoreVIC20::VisualSystem vS);

		protected:
		/** The video system used by the commodore 64. */
		VisualSystem _visualSystem;
	};
}

#endif
  
// End of the file
/*@}*/
