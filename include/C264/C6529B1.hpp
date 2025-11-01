/** \ingroup C264 */
/*@{*/

/**	
 *	@file	
 *	File: C6529B1.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 25/02/2024 \n
 *	Description: Specific implementation of the 6529 chip in C264.
 *				 In C264 series, this chip acts as a latch of the input to the keyboard matrix. \n
 *				 The joysticks have nothing to do with this register,
 *				 but to simplify the code that behaviour has also been grouped into this class. \n
 *	Versions: 1.0 Initial
 */

#ifndef __C264_C6529B1__
#define __C264_C6529B1__

#include <COMMODORE/incs.hpp>
#include <C264/C6529B1Registers.hpp>

namespace C264
{
	/** The important thing of this chip is just their registers (@see C6529Registers). */
	class C6529B1 final : public COMMODORE::C6529B
	{
		public:
		static const int _ID = 200;
		
		C6529B1 ()
			: C6529B (_ID),
			  _C6529B1Registers (nullptr)
							{ }
		
		virtual bool initialize () override;
		
		private:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;
	
		private:
		// Implementation
		/** The memory is used also as the set of registers of the chip. */
		C6529B1Registers* _C6529B1Registers;
	};
}

#endif
  
// End of the file
/*@}*/
