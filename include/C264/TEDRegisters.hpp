/** \ingroup C264 */
/*@{*/

/**	
 *	@file	
 *	File: TEDRegisters.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 09/02/2024 \n
 *	Description: TED 264 Registers Emulation.
 *	Versions: 1.0 Initial
 */

#ifndef __C264_TEDREGISTERS__
#define __C264_TEDREGISTERS__

#include <COMMODORE/incs.hpp>

namespace C264
{
	class C6529B1;

	/** NOTICE:
		https://www.pagetable.com/docs/ted/TED%207360R0%20Preliminary%20Data%20Sheet.pdf 
		at page 18 defines that positions 0x3e and 0x3f are not real registers of the TED chip
		but a mechanism to chage the way the ROM/RAM is undestood in the computer. \n
		The positions between 0x20 and 0x3d behaves like RAM positions any case. */
	class TEDRegisters final : public COMMODORE::TEDRegisters
	{
		public:
		TEDRegisters (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s);

		/** In C264 access to 0x40 registers. \n
			0x20 additional but many of them are managed as RAM. */
		virtual size_t numberRegisters () const override
							{ return (0x40); }

		/** To set the value comming from the keyboard matrix. */
		void setKeyboardEntry (const MCHEmul::UByte& k)
							{ _keyboardEntry = k; }

		/** The joysticks are connected directly to the keyboard pines of the TED. \n
			These methods are not usually manged directly but just in case. */
		const MCHEmul::UByte& joystickStatus (size_t nj) const
							{ return (_joystickStatus [nj]); }
		void setJoystickStatus (size_t nj, const MCHEmul::UByte& s)
							{ _joystickStatus [nj] = s; }
		void setJoystickStatusBit (size_t nj, size_t b, bool s)
							{ _joystickStatus [nj].setBit (b, s); }

		virtual void initialize () override;

		private:
		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;
		virtual const MCHEmul::UByte& readValue (size_t p) const override;
		
		void initializeInternalValues ();

		private:
		/** The value coming from the chip C6529B. */
		MCHEmul::UByte _keyboardEntry;
		/** At the bit representing the direction/fire of the joystick pressed. \n
			Remember that in the Commodore264 series 
			the joysticks are connected directly to the keyboard data port of the TED,
			so the messages should come to here. */
		MCHEmul::UByte _joystickStatus [2]; // two joysticks...

		// Implementation
		/** The value set in the register 8 that will be used later
			when the value of this register was checked. */
		MCHEmul::UByte _register8;
	};
}

#endif
  
// End of the file
/*@}*/
