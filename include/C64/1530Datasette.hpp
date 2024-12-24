/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: 1530Datasette.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 06/04/2024 \n
 *	Description: To emulate the behaviour of the 1530 Datasette Unit in a C64 (loading memory).
 *	Versions: 1.0 Initial
 */

#ifndef __C64_1530DATASETTE__
#define __C64_1530DATASETTE__

#include <COMMODORE/incs.hpp>
#include <C64/Memory.hpp>

namespace C64
{
	class Datasette1530Injection : public COMMODORE::Datasette1530Injection
	{
		public:
		Datasette1530Injection (const Definition& dt)
			: COMMODORE::Datasette1530Injection (dt)
							{ }

		private:
		/** To load the info just only inthe RAM. */
		virtual void loadDataBlockInRAM (const MCHEmul::DataMemoryBlock& dB, MCHEmul::CPU* cpu) override
							{ static_cast <C64::Memory*> (cpu -> memoryRef ()) -> loadDataBlockInRAM (_data._data [_dataCounter]); }
	};
}

#endif
  
// End of the file
/*@}*/
