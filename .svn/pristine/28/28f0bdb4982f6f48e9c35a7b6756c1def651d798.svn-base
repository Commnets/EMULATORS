#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>
#include <core/CPU.hpp>

// ---
bool F6500::CMP_General::executeWith (MCHEmul::UByte u)
{
	MCHEmul::UInt r = 
		MCHEmul::UInt (cpu () -> internalRegister (F6500::C6510::_ACCUMULATOR).values () /** 1 byte long */) - 
		MCHEmul::UInt ({ u });  // Never longer that 1 byte, but the result could be negative...

	// Time of the status register...
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	st.setBitStatus ("N", r [0][7]);
	st.setBitStatus ("Z", r [0] == MCHEmul::UByte::_0);
	st.setBitStatus ("C", !r [0][7]); // When the result is positive (a > u)

	return (true);
}

// ---
_INST_IMPL (F6500::CMP_Inmediate)
{
	return (executeWith (value_inmediate ()));
}

_INST_IMPL (F6500::CMP_Absolute)
{
	return (executeWith (value_absolute ()));
}

_INST_IMPL (F6500::CMP_ZeroPage)
{
	return (executeWith (value_zeroPage ()));
}

_INST_IMPL (F6500::CMP_ZeroPageIndirectX)
{
	return (executeWith (value_indirectZeroPageX ()));
}

_INST_IMPL (F6500::CMP_ZeroPageIndirectY)
{
	return (executeWith (value_indirectZeroPageY ()));
}

_INST_IMPL (F6500::CMP_ZeroPageX)
{
	return (executeWith (value_zeroPageX ()));
}

_INST_IMPL (F6500::CMP_AbsoluteX)
{
	return (executeWith (value_absoluteX ()));
}

_INST_IMPL (F6500::CMP_AbsoluteY)
{
	return (executeWith (value_absoluteY ()));
}
