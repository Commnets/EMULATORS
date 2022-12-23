#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
bool F6500::LSR_General::executeOn (const MCHEmul::Address& a)
{
	// Read the value, makes the operation and set it back!
	MCHEmul::UByte v = memory () -> values (a, 1)[0]; // 1 byte long always
	bool c = v.shiftRightC (false /** 0 is put into */, 1); // Keeps the status of the last bit to actualize later the carry flag
	memory () -> set (a, v);

	// Time of the status register...
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	st.setBitStatus (F6500::C6500::_NEGATIVEFLAG, v [7]);
	st.setBitStatus (F6500::C6500::_ZEROFLAG, v == MCHEmul::UByte::_0);
	st.setBitStatus (F6500::C6500::_CARRYFLAG, c);

	return (true);
}

// ---
_INST_IMPL (F6500::LSR_Absolute)
{
	return (executeOn (address_absolute ()));
}

// ---
_INST_IMPL (F6500::LSR_ZeroPage)
{
	return (executeOn (address_zeroPage ()));
}

// ---
_INST_IMPL (F6500::LSR_Accumulator)
{
	assert (parameters ().size () == 1);

	MCHEmul::Register& a = cpu () -> internalRegister (F6500::C6510::_ACCUMULATOR);

	// Set the value...
	MCHEmul::UBytes v = a.values ();
	bool c = v.shiftRightC (false /** 0 is put into */, 1);
	a.set (v);

	// Time of the status register...
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	st.setBitStatus (F6500::C6500::_NEGATIVEFLAG, v [0][7]);
	st.setBitStatus (F6500::C6500::_ZEROFLAG, v [0] == MCHEmul::UByte::_0);
	st.setBitStatus (F6500::C6500::_CARRYFLAG, c);

	return (true);
}

// ---
_INST_IMPL (F6500::LSR_ZeroPageX)
{
	return (executeOn (address_zeroPageX ()));
}

// ---
_INST_IMPL (F6500::LSR_AbsoluteX)
{
	return (executeOn (address_absoluteX ()));
}
