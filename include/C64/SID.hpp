/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: SID.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 13/01/2023 \n
 *	Description: The SID Chip.
 *	Versions: 1.0 Initial
 */

#ifndef __C64_SID__
#define __C64_SID__

#include <CORE/incs.hpp>
#include <C64/SIDRegisters.hpp>

namespace C64
{
	/** The chip is to produce the sound. */
	class SID : public MCHEmul::Chip
	{
		public:
		static const unsigned int _ID = 5;

		SID ()
			: MCHEmul::Chip (_ID,
				{ { "Name", "SID" },
				  { "Code", "6581/8580" },
				  { "Manufacturer", "Commodore Business Machines CBM" },
				  { "Year", "1981" } }),
			  _SIDRegisters (nullptr)
							{ setClassName ("SID"); }

		virtual bool initialize () override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		C64::SIDRegisters* _SIDRegisters;
	};
}

#endif
  
// End of the file
/*@}*/
