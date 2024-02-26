/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: ACIA.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 09/12/2023 \n
 *	Description: Very simple chip that presents the info to read the keyboard (@see @TED).
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_ACIA__
#define __COMMODORE_ACIA__

#include <COMMODORE/ACIA/ACIARegisters.hpp>

namespace COMMODORE
{
	class ACIARegisters;

	/** This chip manages the communciations in computers like C16/116 and CPlus4. */
	class ACIA final : public MCHEmul::Chip
	{
		public:
		static const int _ID = 103;
		
		ACIA ();

		virtual bool initialize () override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		/**
		  *	The name of the fields are: \n
		  * The Chip info plus:
		  * ACIARegisters	= InfoStructure: Info about the registers.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		/** The memory is used also as the set of registers of the chip. */
		ACIARegisters* _ACIARegisters;
	};
}

#endif
  
// End of the file
/*@}*/
