/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: C6529B.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 09/12/2023 \n
 *	Description: Very simple chip that acts like a lacth.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_C6529B__
#define __COMMODORE_C6529B__

#include <COMMODORE/C6529B/C6529BRegisters.hpp>

namespace COMMODORE
{
	class C6529B : public MCHEmul::Chip
	{
		public:
		static const int _ID = 108;

		/** Some event than can be used when it is needed to notify the latched value. */
		static const unsigned int _LACTCHEDVALUECHANGED = 230;
		
		C6529B (int id);
		
		virtual bool initialize () override;

		/** Any register is valid. */
		const MCHEmul::UByte& latchValue () const
							{ return (_C6529BRegisters -> readValue (0)); }

		// This method puts back _latchValue to false...
		bool latchChanged () const
							{ return (_C6529BRegisters -> latchChanged ()); }

		/**
		  *	The name of the fields are: \n
		  * The Chip info plus:
		  * C6529Registers	= InfoStructure: Info about the registers.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		/** The memory is used also as the set of registers of the chip. */
		C6529BRegisters* _C6529BRegisters;
	};
}

#endif
  
// End of the file
/*@}*/
