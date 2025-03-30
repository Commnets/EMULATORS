/** \ingroup TEXASINSTRUMENTS */
/*@{*/

/**	
 *	@file	
 *	File: TMS9918Registers.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 28/03/2024 \n
 *	Description: TMS9918 Registers Emulation.
 *	Versions: 1.0 Initial
 */

#ifndef __TEXASINSTRUMENTS_TMS9918REGISTERS__
#define __TEXASINSTRUMENTS_TMS9918REGISTERS__

#include <CORE/incs.hpp>

namespace TEXASINSTRUMENTS
{
	class TMS99XXRegisters final : public MCHEmul::InfoClass
	{
		public:
		TMS99XXRegisters ();
		
		// TODO

		void initialize ();

		/**
		  *	The name of the fields are: \n
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		void initializeInternalValues ();

		private:
		// TODO
	};
}

#endif
  
// End of the file
/*@}*/
