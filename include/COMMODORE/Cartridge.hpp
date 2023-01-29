/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: Cartridge.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 27/01/2023 \n
 *	Description: To emulate the behaviour of a cartridge.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_CARTRIDGE__
#define __COMMODORE_CARTRIDGE__

#include <CORE/incs.hpp>
#include <COMMODORE/ExpansionPeripherals.hpp>

namespace COMMODORE
{
	class Cartridge final : public ExpansionPeripheral
	{
		public:
		static const int _ID = 200;

		Cartridge ();

		virtual bool initialize () override
							{ delete (_fileData); _fileData = nullptr; return (true); } 

		virtual bool connectData (MCHEmul::FileData* dt) override;

		virtual bool simulate () override
							{ return (true); } // There is nothing to simulate...

		private:
		MCHEmul::FileData* _fileData;
	};
}

#endif
  
// End of the file
/*@}*/
