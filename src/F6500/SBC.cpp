#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
_INST_IMPL (F6500::SBC_Inmediate)
{
	return (executeWith (value_inmediate ()));
}

// ---
_INST_IMPL (F6500::SBC_Absolute)
{
	return (executeWith (value_absolute ()));
}

// ---
_INST_IMPL (F6500::SBC_ZeroPage)
{
	return (executeWith (value_zeroPage ()));
}

// ---
_INST_IMPL (F6500::SBC_ZeroPageIndirectX)
{
	return (executeWith (value_indirectZeroPageX ()));
}

// ---
_INST_IMPL (F6500::SBC_ZeroPageIndirectY)
{
	return (executeWith (value_indirectZeroPageY ()));
}

// ---
_INST_IMPL (F6500::SBC_ZeroPageX)
{
	return (executeWith (value_zeroPageX ()));
}

// ---
_INST_IMPL (F6500::SBC_AbsoluteX)
{
	return (executeWith (value_absoluteX ()));
}

// ---
_INST_IMPL (F6500::SBC_AbsoluteY)
{
	return (executeWith (value_absoluteY ()));
}
