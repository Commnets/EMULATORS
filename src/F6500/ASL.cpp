#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
_INST_IMPL (F6500::ASL_Absolute)
{
	return (executeOn (address_absolute ()));
}

// ---
_INST_IMPL (F6500::ASL_ZeroPage)
{
	return (executeOn (address_zeroPage ()));
}

// ---
_INST_IMPL (F6500::ASL_Accumulator)
{
	assert (parameters ().size () == 1);

	MCHEmul::Register& a = cpu () -> internalRegister (F6500::C6510::_ACCUMULATOR);

	// Set the value...
	MCHEmul::UBytes v = a.values ();
	bool c = v.shiftLeftC (false /** 0 is put into */, 1);
	a.set (v);

	// Time of the status register...
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	st.setBitStatus (F6500::C6500::_NEGATIVEFLAG, v [0][7]);
	st.setBitStatus (F6500::C6500::_ZEROFLAG, v [0] == MCHEmul::UByte::_0);
	st.setBitStatus (F6500::C6500::_CARRYFLAG, c);

	return (true);
}

// ---
_INST_IMPL (F6500::ASL_ZeroPageX)
{
	return (executeOn (address_zeroPageX ()));
}

// ---
_INST_IMPL (F6500::ASL_AbsoluteX)
{
	return (executeOn (address_absoluteX ()));
}
