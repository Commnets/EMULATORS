#include <F6500/Instructions.hpp>

// ---
_INST_IMPL (F6500::BIT_Absolute)
{
	MCHEmul::UByte v = value_absolute ();

	bool z = (v & cpu () -> internalRegister (F6500::C6510::_ACCUMULATOR).values ()[0] /** 1 byte long */) == MCHEmul::UByte::_0;

	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	st.setBitStatus ("N", v [7]);
	st.setBitStatus ("V", v [6]);
	st.setBitStatus ("Z", z);

	return (true);
}

// ---
_INST_IMPL (F6500::BIT_ZeroPage)
{
	MCHEmul::UByte v = value_zeroPage ();

	bool z = (v & cpu () -> internalRegister (F6500::C6510::_ACCUMULATOR).values ()[0] /** 1 byte long */) == MCHEmul::UByte::_0;

	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	st.setBitStatus ("N", v [7]);
	st.setBitStatus ("V", v [6]);
	st.setBitStatus ("Z", z);

	return (true);
}
