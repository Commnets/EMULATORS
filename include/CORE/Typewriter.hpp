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
			1.- Any simple key of the keyboard, like A, B, C.... \n
				If a capital letter is requiered, the combination should be "[L|R]SHIFT+[letter]". \n
			2.- The special keys allowed are: LSHIFT, RSHIFT, LCTRL, RCTRL, LALT, and RALT. */
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
