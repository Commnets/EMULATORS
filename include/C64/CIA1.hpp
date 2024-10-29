/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: CIA1.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 22/05/2021 \n
 *	Description: The CIA1 Chip to communicate C64.
 *	Versions: 1.0 Initial
 */

#ifndef __C64_CIA1__
#define __C64_CIA1__

#include <CORE/incs.hpp>
#include <COMMODORE/incs.hpp>
#include <C64/CIA1Registers.hpp>

namespace C64
{
	class InputOSSystem;

	/** CIA1 is mainly to communicate C64 with external devices.
		Additionaly it is also used to read the keyboard and the joysticks. */
	class CIA1 final : public COMMODORE::CIA
	{
		public:
		friend InputOSSystem;

		static const unsigned int _ID = 200;

		CIA1 ();

		virtual bool initialize () override;

		/** Before using this method the chip has to be initialized,
			otherwise the variable _CIARegisters will be nullpr and erything will crash. */
		void linkToSID (COMMODORE::SID* sid)
							{ _sid = sid; _CIA1Registers -> linkToSID (sid); }

		// Managing the paddles/jiysticks...
		bool isPaddleConnectedAtPort (unsigned char p) const
							{ return (_CIA1Registers -> isPaddleConnectedAtPort (p)); }
		bool arePaddlesConnected () const
							{ return (_CIA1Registers -> arePaddlesConnected ()); }
		void connectPaddleAtPort (size_t p)
							{ _CIA1Registers -> connectPaddleAtPort (p); }
		void connectAllPaddles ()
							{ _CIA1Registers -> connectAllPaddles (); }
		void disconnectPaddleAtPort (size_t p)
							{ _CIA1Registers -> disconnectPaddle (p); }
		void disconnectAllPaddles ()
							{ _CIA1Registers -> disconnectAllPaddles (); }

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		private:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		// -----
		// Different debug methods to simplify the internal code
		// and to make simplier the modification in case it is needed...
		/** Debug special situations...
			Take care using this instructions _deepDebugFile could be == nullptr... */
		void debugCIA1Cycle ();
		// -----

		private:
		C64::CIA1Registers* _CIA1Registers;
		/** The SID linked. */
		COMMODORE::SID* _sid;

		// Implementation
		unsigned int _lastDatasetteRead;
		/** The last key pressed/released. \n
			used when debugging. */
		std::string _lastKeyPressed, _lastKeyReleased;
	};
}

#endif
  
// End of the file
/*@}*/
