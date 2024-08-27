/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: VICII.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 26/08/2024 \n
 *	Description: The VICII Chip.
 *	Versions: 1.0 Initial.
 *	In the implementation of the VICII for a C64, the fire button events comming from joystick in port 1
 *  could be understood as activations of the light pen = LP signal negative edge....
 */

#ifndef __C64_VICII__
#define __C64_VICII__

#include <COMMODORE/incs.hpp>

namespace C64
{
	/** A "instrumental class" to manage joystick events too. */
	class VICII_JoystickEvent
	{
		public:
		VICII_JoystickEvent ()
			: _VICIIRegisters (nullptr)
							{ }

		inline void linkToVICIIRegisters (COMMODORE::VICIIRegisters* vR);

		void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* nfier);

		private:
		/** It is linked when the final C64 VICII classes are created. @see above. */
		COMMODORE::VICIIRegisters* _VICIIRegisters;
	};

	// ---
	inline void VICII_JoystickEvent::linkToVICIIRegisters (COMMODORE::VICIIRegisters* vR)
	{ 
		assert (vR != nullptr);

		_VICIIRegisters = vR; 
	}
	
	/** The version for PAL systems. */
	class VICII_PAL final : public COMMODORE::VICII_PAL, public VICII_JoystickEvent
	{
		public:
		VICII_PAL (MCHEmul::PhysicalStorageSubset* cR, 
			const MCHEmul::Address& cRA, int vV)
			: COMMODORE::VICII_PAL (cR, cRA, vV),
			  VICII_JoystickEvent ()
							{ }
							
		virtual bool initialize () override;

		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* nfier) override;
	};
	
	/** The version for PAL systems. */
	class VICII_NTSC final : public COMMODORE::VICII_NTSC, public VICII_JoystickEvent
	{
		public:
		VICII_NTSC (MCHEmul::PhysicalStorageSubset* cR, 
			const MCHEmul::Address& cRA, int vV)
			: COMMODORE::VICII_NTSC (cR, cRA, vV)
							{ }
							
		virtual bool initialize () override;
							
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* nfier) override;
	};
}

#endif
  
// End of the file
/*@}*/
