/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: Cartridge.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 27/01/2023 \n
 *	Description: To emulate the behaviour of a cartridge in the C64 computer
 *	Versions: 1.0 Initial
 */

#ifndef __C64_CARTRIDGE__
#define __C64_CARTRIDGE__

#include <COMMODORE/incs.hpp>

namespace C64
{
	class Cartridge : public COMMODORE::ExpansionPeripheral
	{
		public:
		static const int _ID = 200;

		Cartridge ();

		virtual bool _GAME () const override;
		virtual bool _EXROM () const override;

		virtual bool connectData (MCHEmul::FileData* dt) override;

		virtual bool initialize () override
							{ return (true); }

		virtual bool simulate (MCHEmul::CPU* cpu) override
							{ return (true); }
	};
}

#endif
  
// End of the file
/*@}*/
