#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

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
