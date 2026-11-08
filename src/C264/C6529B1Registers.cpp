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
	_selectedRows = v;

	refreshKeyboardPins ();
}
