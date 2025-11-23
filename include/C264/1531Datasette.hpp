/** \ingroup C264 */
/*@{*/

/**	
 *	@file	
 *	File: 1531Datasette.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 22/11/2025 \n
 *	Description: To emulate the behaviour of the 1531 Datasette Unit in a C264 (loading memory).
 *	Versions: 1.0 Initial
 */

#ifndef __C264_1531DATASETTE__
#define __C264_1531DATASETTE__

#include <COMMODORE/incs.hpp>
#include <C264/Memory.hpp>

namespace C264
{
	class Datasette1531Injection : public COMMODORE::Datasette1530Injection
	{
		public:
		Datasette1531Injection (const Definition& dt)
			: COMMODORE::Datasette1530Injection (dt)
							{ }

		private:
		/** To load the info just only inthe RAM. */
		virtual void loadDataBlockInRAM (const MCHEmul::DataMemoryBlock& dB, MCHEmul::CPU* cpu) override
							{ /** TODO. */ }
	};
}

#endif
  
// End of the file
/*@}*/
