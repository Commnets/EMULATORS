#include <C264/C6529B2Registers.hpp>

C264::C6529B2Registers::C6529B2Registers 
		(MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: COMMODORE::C6529BRegisters (_C6529B2REGS_SUBSET, ps, pp, a, s)
{
	initializeInternalValues ();
}

// ---
void C264::C6529B2Registers::initialize ()
{
	initializeInternalValues ();
}

// ---
const MCHEmul::UByte& C264::C6529B2Registers::readValue (size_t p) const
{
	MCHEmul::UByte result = MCHEmul::UByte::_FF;

	// TODO...
	// Bit 2 is also used to know whether a key has been pressed in the cassette...
	// This happen in the Plus4 version...
	// ...in the C16 there is no 6529B2 but the position where 
	// the keys of the cassette is detected is the same
	// There is also other documents stating that port 1 (bit 7) is also involved in the detection of the datasette keys...
	// ...however seems that ROM only detects this one...
	result.setBit (2, _cassetteKeyPressed ? false : true);

	return (_lastValueRead = result);
}

// ---
void C264::C6529B2Registers::initializeInternalValues ()
{
	_cassetteKeyPressed = false;
}
