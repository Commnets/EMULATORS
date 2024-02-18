/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: C6529B.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 09/12/2023 \n
 *	Description: Very simple chip that presents the info to read the keyboard (@see @TED).
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_C6529B__
#define __COMMODORE_C6529B__

#include <COMMODORE/TED/C6529BRegisters.hpp>

namespace COMMODORE
{
	class C6529BRegisters;

	/** The important thing of this chip is just their registers (@see C6529Registers). */
	class C6529B final : public MCHEmul::Chip
	{
		public:
		static const int _ID = 200;
		
		C6529B ();

		/** Any register is valid. */
		const MCHEmul::UByte& latchValue () const
							{ return (_C6529BRegisters -> readValue (0)); }

		virtual bool initialize () override;

		virtual bool simulate (MCHEmul::CPU* cpu) override
							{ return (true); }

		/**
		  *	The name of the fields are: \n
		  * The Chip info plus:
		  * C6529Registers	= InfoStructure: Info about the registers.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		/** The memory is used also as the set of registers of the chip. */
		C6529BRegisters* _C6529BRegisters;
	};
}

#endif
  
// End of the file
/*@}*/
