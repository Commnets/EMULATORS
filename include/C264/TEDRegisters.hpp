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

		// Related with the access to the memory...
		bool hiromSelected () const
							{ return (_HIROMSelected); }
		// Any time the value above changes, this variable is set to true
		// and when checked it goes back to false...
		bool romAccessChanged () const
							{ return (_ROMAccessChanged); }
		bool peekROMAccessChanged () const
							{ return (_ROMAccessChanged.peekValue ()); }

		void setJoystickPins (size_t nj, const MCHEmul::UByte& v)
							{ _joystickPins [nj] = v; }

		void initialize ();

		private:
		void setValue (size_t p, const MCHEmul::UByte& v);
		const MCHEmul::UByte& readValue (size_t p) const;
		
		void initializeInternalValues ();

		private:
		/** Whether the access to the HIROM has or not been selected. */
		bool _HIROMSelected;
		MCHEmul::OBool _ROMAccessChanged;
		MCHEmul::UByte _joystickPins [2];
	};
}

#endif
  
// End of the file
/*@}*/
