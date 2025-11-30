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
		/** In this case the implementation of the trap is a bit differenet 
			than in the case of the Commodore64 and VIC20. \n */
		virtual bool executeTrap (const MCHEmul::Trap& t, MCHEmul::CPU* cpu) override;
	};
}

#endif
  
// End of the file
/*@}*/
