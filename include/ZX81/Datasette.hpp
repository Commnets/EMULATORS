/** \ingroup ZX81 */
/*@{*/

/**	
 *	@file	
 *	File: Datasette.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 14/06/2024 \n
 *	Description: To emulate the behaviour of the Datasette Unit connected to the ZX81.
 *	Versions: 1.0 Initial
 */

#ifndef __ZX81_DATASETTE__
#define __ZX81_DATASETTE__

#include <SINCLAIR/incs.hpp>

namespace ZX81
{
	/** With the io activities running "linked" with the CPU. */
	class Datasette final : public MCHEmul::BasicDatasette
	{
		public:
		static const int _ID = 100;

		Datasette (unsigned int rS);
	};

	/** With the io activities running "in paralell" with the CPU. */
	class DatasetteP final : public MCHEmul::BasicDatasetteP
	{
		public:
		static const int _ID = 1000;

		DatasetteP (unsigned int rS);
	};
}

#endif
  
// End of the file
/*@}*/
