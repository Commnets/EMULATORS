#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
bool F6500::LDA_General::executeWith (MCHEmul::UByte u)
{
	// Set the value...
	cpu () -> internalRegister (F6500::C6510::_ACCUMULATOR).set ({ u });

	// Time of the status register...
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	st.setBitStatus (F6500::C6500::_NEGATIVEFLAG, u [7]);
	st.setBitStatus (F6500::C6500::_ZEROFLAG, u == MCHEmul::UByte::_0);

	return (true);
}

// ---
_INST_IMPL (F6500::LDA_Inmediate)
{
	return (executeWith (value_inmediate ()));
}

// ---
_INST_IMPL (F6500::LDA_Absolute)
{
	return (executeWith (value_absolute ()));
}

// ---
_INST_IMPL (F6500::LDA_ZeroPage)
{
	return (executeWith (value_zeroPage ()));
}

// ---
_INST_IMPL (F6500::LDA_ZeroPageIndirectX)
{
	return (executeWith (value_indirectZeroPageX ()));
}

// ---
_INST_IMPL (F6500::LDA_ZeroPageIndirectY)
{
	return (executeWith (value_indirectZeroPageY ()));
}

// ---
_INST_IMPL (F6500::LDA_ZeroPageX)
{
	return (executeWith (value_zeroPageX ()));
}

// ---
_INST_IMPL (F6500::LDA_AbsoluteX)
{
	return (executeWith (value_absoluteX ()));
}

// ---
_INST_IMPL (F6500::LDA_AbsoluteY)
{
	return (executeWith (value_absoluteY ()));
}
