/** \ingroup C264 */
/*@{*/

/**	
 *	@file	
 *	File: C6529B2Registers.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 20/02/2024 \n
 *	Description: C6529B2 Registers Emulation.
 *	Versions: 1.0 Initial
 */

#ifndef __C264_C6529B2REGISTERS__
#define __C264_C6529B2REGISTERS__

#include <COMMODORE/incs.hpp>

namespace C264
{
	class C6529B2;

	class C6529B2Registers final : public COMMODORE::C6529BRegisters
	{
		public:
		friend C6529B2;

		static const int _C6529B2REGS_SUBSET = 2010;

		/** The positions where the C6529B2 chip is reachable are constant in C264 series. */
		C6529B2Registers (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s);

		/** This version of the latch is also reachable in 0x10 (= 16) positions. 
			But all of them behave in the same way...*/
		virtual size_t numberRegisters () const override
							{ return (0x10); }

		// Managing the status of the keys in the datasette...
		bool casetteKeyPressed () const
							{ return (_cassetteKeyPressed); }
		void setCasetteKeyPressed (bool kp)
							{ _cassetteKeyPressed = kp; }

		virtual void initialize () override;

		private:
		/** Just to read whether the is or not a key pressed in the tape. */
		virtual const MCHEmul::UByte& readValue (size_t p) const override;

		// Implementation
		void initializeInternalValues ();

		private:
		/** true when a key is pressed in the tape...
			 ...and false when not. */
		bool _cassetteKeyPressed;
		
		// Implmenetation
		mutable MCHEmul::UByte _lastValueRead;
	};
}

#endif
  
// End of the file
/*@}*/
