#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>
#include <core/CPU.hpp>

// ---
bool F6500::ROR_General::executeOn (const MCHEmul::Address& a)
{
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	bool c = st.bitStatus ("C"); 

	// Set the value...
	MCHEmul::UBytes v = memory () -> values (a, 1);
	c = v.rotateRightC (1, c);
	memory () -> set (a, v);

	// Time of the status register...
	st.setBitStatus ("N", v [0][7]);
	st.setBitStatus ("Z", v [0] == MCHEmul::UByte::_0);
	st.setBitStatus ("C", c);

	return (true);
}

// ---
_INST_IMPL (F6500::ROR_Absolute)
{
	return (executeOn (address_absolute ()));
}

// ---
_INST_IMPL (F6500::ROR_ZeroPage)
{
	return (executeOn (address_zeroPage ()));
}

// ---
_INST_IMPL (F6500::ROR_Accumulator)
{
	assert (parameters ().size () == 1);

	MCHEmul::Register& a = cpu () -> internalRegister (F6500::C6510::_ACCUMULATOR);
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	bool c = st.bitStatus ("C"); 

	// Set the value...
	MCHEmul::UBytes v = a.values ();
	c = v.rotateRightC (1, c);
	a.set (v);

	// Time of the status register...
	st.setBitStatus ("N", v [0][7]);
	st.setBitStatus ("Z", v [0] == MCHEmul::UByte::_0);
	st.setBitStatus ("C", c);

	return (true);
}

// ---
_INST_IMPL (F6500::ROR_ZeroPageX)
{
	return (executeOn (address_zeroPageX ()));
}

// ---
_INST_IMPL (F6500::ROR_AbsoluteX)
{
	return (executeOn (address_absoluteX ()));
}
