#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>
#include <core/CPU.hpp>

// ---
bool F6500::LSR_General::executeOn (const MCHEmul::Address& a)
{
	// Set the value...
	MCHEmul::UBytes v = memory () -> values (a, 1);
	bool c = v.shiftRightC (1, false /** 0 is put into */);
	memory () -> set (a, v);

	// Time of the status register...
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	st.setBitStatus ("N", v [0][7]);
	st.setBitStatus ("Z", v [0] == MCHEmul::UByte::_0);
	st.setBitStatus ("C", c);

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
	bool c = v.shiftRightC (1, false /** 0 is put into */);
	a.set (v);

	// Time of the status register...
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	st.setBitStatus ("N", v [0][7]);
	st.setBitStatus ("Z", v [0] == MCHEmul::UByte::_0);
	st.setBitStatus ("C", c);

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
