/** \ingroup C264 */
/*@{*/

/**	
 *	@file	
 *	File: C6529B1Registers.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 20/02/2024 \n
 *	Description: C6529B1 Registers Emulation.
 *	Versions: 1.0 Initial
 */

#ifndef __C264_C6529B1REGISTERS__
#define __C264_C6529B1REGISTERS__

#include <COMMODORE/incs.hpp>

namespace C264
{
	class C6529B1;

	class C6529B1Registers final : public COMMODORE::C6529BRegisters
	{
		public:
		friend C6529B1; // To set the vakue when an event is received...

		static const int _C6529B1REGS_SUBSET = 2000;

		/** The positions where the C6529B1 chip is reachable are constant. */
		C6529B1Registers (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s);

		/** Resolves current keyboard columns from the retained row selection. \n */
		inline MCHEmul::UByte keyboardPins () const;

		/** In the C264, this zone of the memory covers 0x10 (= 16) positions. */
		virtual size_t numberRegisters () const override
							{ return (0x10); }

		virtual void initialize () override;

		private:
		/** The information about the keyboard has to be transfered to the port 
			in the right way when a value is set into the data bus. */
		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;

		/** Updates the published columns without clearing an already pending change. \n */
		inline void refreshKeyboardPins ();

		// Implementation
		/** and these ones are to manage the matrix of keys pressed...
			...and also the inputs comming from the joystick 1... */
		bool keyboardStatusMatrix (size_t r, size_t c) const
							{ return (_keyboardStatusMatrix [r].bit (c)); }
		const MCHEmul::UByte& keyboardStatusMatrix (size_t r) const
							{ return (_keyboardStatusMatrix [r]); }
		inline void setKeyboardStatusMatrix (size_t r, size_t c, bool s);

		/** To initilize the keyboard matrix. */
		inline void initializeKeyboard ();

		private:
		// Implementation
		/** Last row-selection value written to the keyboard port. \n */
		MCHEmul::UByte _selectedRows;
		/** A set bit represents a pressed key in the keyboard matrix. \n
			In this case the matrix is always 8 * 8 = 64 positions. */
		MCHEmul::UByte _keyboardStatusMatrix [8];
		MCHEmul::UByte _rev_keyboardStatusMatrix [8];
	};

	// ---
	inline MCHEmul::UByte C6529B1Registers::keyboardPins () const
	{
		MCHEmul::UByte pins = MCHEmul::UByte::_FF;

		for (size_t i = 0; i < 8; i++)
			if (!_selectedRows.bit (i))
				pins &= ~_rev_keyboardStatusMatrix [i];

		return (pins);
	}

	// ---
	inline void C6529B1Registers::refreshKeyboardPins ()
	{
		MCHEmul::UByte pins = keyboardPins ();

		// Repeated resolutions must not erase a change awaiting publication.
		if (pins != portValue ())
			setPortValue (pins);
	}

	// ---
	inline void C6529B1Registers::setKeyboardStatusMatrix (size_t r, size_t c, bool s)
	{
		_keyboardStatusMatrix [r].setBit (c, s);
		_rev_keyboardStatusMatrix [c].setBit (r, s);

		refreshKeyboardPins ();
	}

	// ---
	inline void C6529B1Registers::initializeKeyboard ()
	{
		_selectedRows = MCHEmul::UByte::_FF;

		for (size_t i = 0; i < 8; i++)
			_keyboardStatusMatrix [i] =
				_rev_keyboardStatusMatrix [i] = MCHEmul::UByte::_0;

		refreshKeyboardPins ();
	}
}

#endif
  
// End of the file
/*@}*/
