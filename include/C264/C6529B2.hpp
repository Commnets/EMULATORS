/** \ingroup C264 */
/*@{*/

/**	
 *	@file	
 *	File: C6529B2.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 25/02/2024 \n
 *	Description: Specific implementation of the 6529 chip in C264. \n
 *				 It only exists in CPlus4 version, and takes care of the
 *	Versions: 1.0 Initial
 */

#ifndef __C264_C6529B2__
#define __C264_C6529B2__

#include <COMMODORE/incs.hpp>

namespace C264
{
	class C6529B2Registers;

	class C6529B2 final : public COMMODORE::C6529B
	{
		public:
		static const int _ID = 201;
		
		C6529B2 ()
			: COMMODORE::C6529B (_ID),
			  _C6529B2Registers (nullptr)
							{ }
		
		virtual bool initialize () override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		private:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* ntfer) override;

		private:
		// Implementation
		/** The memory is used also as the set of registers of the chip. */
		C6529B2Registers* _C6529B2Registers;
	};
}

#endif
  
// End of the file
/*@}*/
