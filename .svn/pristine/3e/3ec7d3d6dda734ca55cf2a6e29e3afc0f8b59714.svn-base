/** \ingroup CPU */
/*@{*/

/**	
 *	@file	
 *	File: OSOI.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 11/06/2021 \n
 *	Description: gathering the generation of events in a windows system where the emulation happens.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_OSIO__
#define __MCHEMUL_OSIO__

#include <CORE/IO.hpp>
#include <SDL_events.h>

namespace MCHEmul
{
	/** Represents the generation of inputs from the OS. */
	class InputOSSystem : public IODevice
	{
		public:
		InputOSSystem (int id, const Attributes& attrs = { })
			: IODevice (Type::_OUTPUT, id, attrs),
			  _quitRequested (false)
							{ }

		bool quitRequested () const
							{ return (_quitRequested); }

		virtual bool initialize () override
							{ _quitRequested = false; return (true); }

		virtual bool refresh () override;

		protected:
		/** What to do when a key has been pressed. */
		virtual void whenKeyPressed (SDL_Keycode) = 0;
		/** What to do when a key has been released. */
		virtual void whenKeyReleased (SDL_Keycode) = 0;

		protected:
		bool _quitRequested;
	};
}

#endif
  
// End of the file
/*@}*/
