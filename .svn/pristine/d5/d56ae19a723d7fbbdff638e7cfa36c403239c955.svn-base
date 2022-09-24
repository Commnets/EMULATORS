#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>
#include <core/CPU.hpp>

// ---
bool F6500::ROL_General::executeOn (const MCHEmul::Address& a)
{
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	bool c = st.bitStatus ("C"); 

	// Read the value, makes the operation and set it back!
	MCHEmul::UByte v = memory () -> values (a, 1)[0]; // 1 byte long always
	c = v.rotateLeftC (1, c /** The carry is put into. */); // Keeps the status of the last bit to actualize later the carry flag
	memory () -> set (a, { v });

	// Time of the status register...
	st.setBitStatus ("N", v [7]);
	st.setBitStatus ("Z", v == MCHEmul::UByte::_0);
	st.setBitStatus ("C", c);

	return (true);
}

// ---
_INST_IMPL (F6500::ROL_Absolute)
{
	return (executeOn (address_absolute ()));
}

// ---
_INST_IMPL (F6500::ROL_ZeroPage)
{
	return (executeOn (address_zeroPage ()));
}

// ---
_INST_IMPL (F6500::ROL_Accumulator)
{
	assert (parameters ().size () == 1);

	MCHEmul::Register& a = cpu () -> internalRegister (F6500::C6510::_ACCUMULATOR);
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	bool c = st.bitStatus ("C"); 

	// Set the value...
	MCHEmul::UBytes v = a.values ();
	c = v.rotateLeftC (1, c);
	a.set (v);

	// Time of the status register...
	st.setBitStatus ("N", v [0][7]);
	st.setBitStatus ("Z", v [0] == MCHEmul::UByte::_0);
	st.setBitStatus ("C", c);

	return (true);
}

// ---
_INST_IMPL (F6500::ROL_ZeroPageX)
{
	return (executeOn (address_zeroPageX ()));
}

// ---
_INST_IMPL (F6500::ROL_AbsoluteX)
{
	return (executeOn (address_absoluteX ()));
}
