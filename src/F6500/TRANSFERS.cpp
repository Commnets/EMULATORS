#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
_INST_IMPL (F6500::TAX)
{
	assert (parameters ().size () == 1);

	MCHEmul::UBytes u = cpu () -> internalRegister (F6500::C6510::_ACCUMULATOR).values ();
	cpu () -> internalRegister (F6500::C6510::_XREGISTER).set (u);

	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	st.setBitStatus (F6500::C6500::_NEGATIVEFLAG, u [0][7]);
	st.setBitStatus (F6500::C6500::_ZEROFLAG, u [0] == MCHEmul::UByte::_0);

	return (true);
}

// ---
_INST_IMPL (F6500::TAY)
{
	assert (parameters ().size () == 1);

	MCHEmul::UBytes u = cpu () -> internalRegister (F6500::C6510::_ACCUMULATOR).values ();
	cpu () -> internalRegister (F6500::C6510::_YREGISTER).set (u);

	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	st.setBitStatus (F6500::C6500::_NEGATIVEFLAG, u [0][7]);
	st.setBitStatus (F6500::C6500::_ZEROFLAG, u [0] == MCHEmul::UByte::_0);

	return (true);
}

// ---
_INST_IMPL (F6500::TXA)
{
	assert (parameters ().size () == 1);

	MCHEmul::UBytes u = cpu () -> internalRegister (F6500::C6510::_XREGISTER).values ();
	cpu () -> internalRegister (F6500::C6510::_ACCUMULATOR).set (u);

	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	st.setBitStatus (F6500::C6500::_NEGATIVEFLAG, u [0][7]);
	st.setBitStatus (F6500::C6500::_ZEROFLAG, u [0] == MCHEmul::UByte::_0);

	return (true);
}

// ---
_INST_IMPL (F6500::TYA)
{
	assert (parameters ().size () == 1);

	MCHEmul::UBytes u = cpu () -> internalRegister (F6500::C6510::_YREGISTER).values ();
	cpu () -> internalRegister (F6500::C6510::_ACCUMULATOR).set (u);

	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	st.setBitStatus (F6500::C6500::_NEGATIVEFLAG, u [0][7]);
	st.setBitStatus (F6500::C6500::_ZEROFLAG, u [0] == MCHEmul::UByte::_0);

	return (true);
}
