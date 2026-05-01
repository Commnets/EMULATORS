/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: Emulator.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 19/04/2026 \n
 *	Description: The root class to define any COMMODORE emulator.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_C64EMULATOR__
#define __COMMODORE_C64EMULATOR__

#include <EMULATORS/incs.hpp>
#include <COMMODORE/ASCIIToCodeConverter.hpp>

namespace COMMODORE
{
	/** The COMMODORE emulator is just to create the right ASCIIToCodeConverter. */
	class Emulator : public MCHEmul::Emulator
	{
		public:
		Emulator (const MCHEmul::CommandLineArguments& args, MCHEmul::CommunicationSystem* cS = nullptr)
			: MCHEmul::Emulator (args, cS)
							{ }

		protected:
		virtual MCHEmul::ASCIIToCodeConverter* createASCIIToCodeConverter () const override
							{ return (new ASCIIToCodeConverter); } // The COMMODORE machines uses all the same (PETSCII)
	};
}

#endif
  
// End of the file
/*@}*/
