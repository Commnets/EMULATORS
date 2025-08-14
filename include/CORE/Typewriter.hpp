/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: Typewriter.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 28/07/2025 \n
 *	Description: To emulate the behviour of a typewriter, sending tips to the emulation!. \n
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_TYPEWRITER__
#define __MCHEMUL_TYPEWRITER__

#include <CORE/IOPeripheral.hpp>
#include <CORE/OSIOPeripheral.hpp>

namespace MCHEmul
{
	class InputOSSystem;

	/** A typewriter device is a device to inject keystrokes into the simulation. \n
		The simulation is accountable to manage them. \n
		What happens doing so will then relay on the situation that the simulation is in. */
	class Typewriter final : public InputOSSystemPeripheral
	{
		public:
		/** This ID is part of the basic library. */
		static const unsigned int _ID = 10;

		/** The constructor receoved the number of cycles to wait until injecting another element 
			in the simulation, and a reference to the real IO system used in the simnulation.
			This last one can not be nullptr. */
		Typewriter (unsigned int nC, Computer* c,const Attributes& attrs = { });

		/** The keys allowed are: \n
			The FileData receives has to contain every "symbol" in the keyboard (UK one).
			The KeystrokeTypeIO has to convert the "text" into "symbols" of the keyboard. \n
			This method will translate those symbols into SDL_Events. \n
			The special keys of the keyboard are: LSHIFT, RSHIFT, LCTRL, 
			RCTRL, LALT, RALT, HOME, UP, DOWN, LEFT, RIGH, SPACE & RETURN  \n
			Every RETURN will generate a special delay time to treat it. \n
			This is because in manu emulators the kernel takes time to process the line introduce after a RETURN! */
		virtual bool connectData (FileData* dt) override;
		virtual FileData* retrieveData () const override;

		protected:
		/** The original set of keystrokes to be injected into the InputOSSystem. */
		Strings _keystrokes;
	};
}

#endif
  
// End of the file
/*@}*/
