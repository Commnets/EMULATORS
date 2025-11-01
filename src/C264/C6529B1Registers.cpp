#include <C264/C6529B1Registers.hpp>

// ---
C264::C6529B1Registers::C6529B1Registers 
		(MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: COMMODORE::C6529BRegisters (_C6529B1REGS_SUBSET, ps, pp, a, s)
{ 
	initializeKeyboard ();
}

// ---
void C264::C6529B1Registers::initialize ()
{
	COMMODORE::C6529BRegisters::initialize ();

	initializeKeyboard ();
}

// ---
void C264::C6529B1Registers::setValue (size_t p, const MCHEmul::UByte& v)
{
	// Depending on the value put here...
	// ...the value transmited to the port will be one or another...
	MCHEmul::UByte pV = MCHEmul::UByte::_FF; // Nothing pressed by default...
	for (size_t i = 0; i < 8; i++)
		if (!v.bit (i)) // When the bit is down...
			pV &= ~_rev_keyboardStatusMatrix [i]; // All values to 1 will be transmitted to the outcome as 0...
	setPortValue (pV); // ...and the changes notified...
}
