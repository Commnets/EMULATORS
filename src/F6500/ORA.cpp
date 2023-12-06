#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

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
