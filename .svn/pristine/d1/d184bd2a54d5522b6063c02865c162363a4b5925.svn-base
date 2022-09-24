#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>
#include <core/CPU.hpp>

// ---
bool F6500::DEC_General::executeOn (const MCHEmul::Address& a)
{
	// Sets the value...
	MCHEmul::UInt v (memory () -> values (a, 1));
	v -= MCHEmul::UInt::_1; /** carry could be generated. */
	if (v.size () > 1) v = MCHEmul::UInt ({ v [1] }); // [1] = Number is stored in Big-endian format
	memory () -> set (a, v.bytes ());

	// Time of the status register...
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	st.setBitStatus ("N", v [0][7]);
	st.setBitStatus ("Z", v [0] == MCHEmul::UByte::_0);

	return (true);
}

// ---
_INST_IMPL (F6500::DEC_Absolute)
{
	return (executeOn (address_absolute ()));
}

// ---
_INST_IMPL (F6500::DEC_ZeroPage)
{
	return (executeOn (address_zeroPage ()));
}

// ---
_INST_IMPL (F6500::DEC_ZeroPageX)
{
	return (executeOn (address_zeroPageX ()));
}

// ---
_INST_IMPL (F6500::DEC_AbsoluteX)
{
	return (executeOn (address_absoluteX ()));
}
