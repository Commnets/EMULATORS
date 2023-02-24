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
	class Cartridge : public ExpansionPeripheral
	{
		public:
		static const int _ID = 200;

		Cartridge ();

		/** See that initialize never initialize the data content,
			because it will be reused whilst the cartridge is connected. */
		virtual bool initialize () override
							{ return (true); }

		virtual bool simulate (MCHEmul::CPU* cpu) override
							{ return (true); }

		protected:
		/** Type and version of the data cartridge. */
		unsigned short _version;
		unsigned short _type;
	};
}

#endif
  
// End of the file
/*@}*/
