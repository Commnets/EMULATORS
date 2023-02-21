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
		/** This structure is what the cartridge need to be installed in the memory. */
		struct Data
		{
			unsigned short _version;
			unsigned short _type;
			std::string _name;
			MCHEmul::DataMemoryBlocks _memoryBlocks;
		};

		static const int _ID = 200;

		Cartridge ();

		/** To get the info about the cartridge. */
		const Data& cartridgeData () const
							{ return (_cartridgeData); }

		/** See that initialize never initialize the data content,
			because it will be reused whilst the cartridge is connected. */
		virtual bool initialize () override
							{ return (true); }

		/** The connectData is invoked once the computer has started. 
			The data is store in a very internal variable.
			That internal variable is reused once the machine is started back. */
		virtual bool connectData (MCHEmul::FileData* dt) override;

		virtual bool simulate (MCHEmul::CPU* cpu) override
							{ return (true); }

		protected:
		Data _cartridgeData;
	};
}

#endif
  
// End of the file
/*@}*/
