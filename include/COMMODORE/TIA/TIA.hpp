/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: TIA.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 09/12/2023 \n
 *	Description: Very simple chip that presents the info to read the keyboard (@see @TED).
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_TIA__
#define __COMMODORE_TIA__

#include <COMMODORE/TIA/TIARegisters.hpp>

namespace COMMODORE
{
	class TIARegisters;

	/** This chip manages the communciations in computers like C16/116 and CPlus4. */
	class TIA final : public MCHEmul::Chip
	{
		public:
		static const int _ID = 110;
		
		TIA ();

		virtual bool initialize () override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		/**
		  *	The name of the fields are: \n
		  * The Chip info plus:
		  * TIARegisters	= InfoStructure: Info about the registers.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		/** The memory is used also as the set of registers of the chip. */
		TIARegisters* _TIARegisters;
	};
}

#endif
  
// End of the file
/*@}*/
