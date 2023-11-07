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

namespace VIC20
{
	class VIC20 final : public COMMODORE::Computer
	{
		public:
		enum class VisualSystem { _NTSC, _PAL };

		/** Macros for the speed. */
		static const unsigned int _PALCLOCK		= 985248; // 0.985 MHz
		static const unsigned int _NTSCCLOCK	= 1022727; // 1.023 MHz

		VIC20 (VisualSystem vS);

		virtual bool initialize (bool iM = true) override;

		private:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		// Implementation
		static MCHEmul::Chips standardChips ();
		static MCHEmul::IODevices standardDevices ();

		protected:
		/** The video system used by the commodore 64. */
		VisualSystem _visualSystem;
	};
}

#endif
  
// End of the file
/*@}*/
