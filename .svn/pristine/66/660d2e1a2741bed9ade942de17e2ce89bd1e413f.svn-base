#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>
#include <core/CPU.hpp>

// ---
bool F6500::ORA_General::executeWith (MCHEmul::UByte u)
{
	MCHEmul::Register& a = cpu () -> internalRegister (F6500::C6510::_ACCUMULATOR);
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	
	// Read the value, makes the operation and sets it back...
	MCHEmul::UByte r = a.values ()[0] | u;
	a.set ({ r });

	// Time of the status register...
	st.setBitStatus ("N", r [7]);
	st.setBitStatus ("Z", r == MCHEmul::UByte::_0);

	return (true);
}

// ---
_INST_IMPL (F6500::ORA_Inmediate)
{
	return (executeWith (value_inmediate ()));
}

// ---
_INST_IMPL (F6500::ORA_Absolute)
{
	return (executeWith (value_absolute ()));
}

// ---
_INST_IMPL (F6500::ORA_ZeroPage)
{
	return (executeWith (value_zeroPage ()));
}

// ---
_INST_IMPL (F6500::ORA_ZeroPageIndirectX)
{
	return (executeWith (value_indirectZeroPageX ()));
}

// ---
_INST_IMPL (F6500::ORA_ZeroPageIndirectY)
{
	return (executeWith (value_indirectZeroPageY ()));
}

// ---
_INST_IMPL (F6500::ORA_ZeroPageX)
{
	return (executeWith (value_zeroPageX ()));
}

// ---
_INST_IMPL (F6500::ORA_AbsoluteX)
{
	return (executeWith (value_absoluteX ()));
}

// ---
_INST_IMPL (F6500::ORA_AbsoluteY)
{
	return (executeWith (value_absoluteY ()));
}
