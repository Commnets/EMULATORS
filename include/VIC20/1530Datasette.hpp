/** \ingroup VIC20 */
/*@{*/

/**	
 *	@file	
 *	File: 1530Datasette.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 06/04/2024 \n
 *	Description: To emulate the behaviour of the 1530 Datasette Unit in a VIC20 (loading memory).
 *	Versions: 1.0 Initial
 */

#ifndef __VIC20_1530DATASETTE__
#define __VIC20_1530DATASETTE__

#include <COMMODORE/incs.hpp>

namespace VIC20
{
	class Datasette1530Injection : public COMMODORE::Datasette1530Injection
	{
		public:
		Datasette1530Injection ()
			: COMMODORE::Datasette1530Injection (_DEFINITION)
							{ }

		private:
		/** The default definition of the traps and so for this device. */
		static const Definition _DEFINITION;
	};
}

#endif
  
// End of the file
/*@}*/
