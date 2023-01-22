/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: SID.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 13/01/2023 \n
 *	Description: SID Chip Emulation.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_SID__
#define __COMMODORE_SID__

#include <CORE/incs.hpp>
#include <COMMODORE/SIDRegisters.hpp>

namespace COMMODORE
{
	/** The chip is to produce the sound. */
	class SID : public MCHEmul::Chip
	{
		public:
		static const unsigned int _ID = 105;

		SID ();

		virtual bool initialize () override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		SIDRegisters* _SIDRegisters;
	};
}

#endif
  
// End of the file
/*@}*/
