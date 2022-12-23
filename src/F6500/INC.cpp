#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
bool F6500::INC_General::executeOn (const MCHEmul::Address& a)
{
	// Read the value, makes the operation and sets it back...
	MCHEmul::UInt v = MCHEmul::UInt ((memory () -> values (a, 1)[0])) + MCHEmul::UInt::_1;
	// A carry could be generated, but it will be ignored...
	memory () -> set (a, v.bytes ()); // 1 byte long always

	// Time of the status register...
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	st.setBitStatus (F6500::C6500::_NEGATIVEFLAG, v.negative ());
	st.setBitStatus (F6500::C6500::_ZEROFLAG, v == MCHEmul::UInt::_0);

	return (true);
}

// ---
_INST_IMPL (F6500::INC_Absolute)
{
	return (executeOn (address_absolute ()));
}

// ---
_INST_IMPL (F6500::INC_ZeroPage)
{
	return (executeOn (address_zeroPage ()));
}

// ---
_INST_IMPL (F6500::INC_ZeroPageX)
{
	return (executeOn (address_zeroPageX ()));
}

// ---
_INST_IMPL (F6500::INC_AbsoluteX)
{
	return (executeOn (address_absoluteX ()));
}
