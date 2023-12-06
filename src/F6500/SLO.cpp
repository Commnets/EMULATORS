#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
_INST_IMPL (F6500::SLO_Absolute)
{
	return (executeOn (address_absolute ()));
}

// ---
_INST_IMPL (F6500::SLO_ZeroPage)
{
	return (executeOn (address_zeroPage ()));
}

// ---
_INST_IMPL (F6500::SLO_ZeroPageIndirectX)
{
	return (executeOn (address_indirectZeroPageX ()));
}

// ---
_INST_IMPL (F6500::SLO_ZeroPageIndirectY)
{
	return (executeOn (address_indirectZeroPageY ()));
}

// ---
_INST_IMPL (F6500::SLO_ZeroPageX)
{
	return (executeOn (address_zeroPageX ()));
}

// ---
_INST_IMPL (F6500::SLO_AbsoluteX)
{
	return (executeOn (address_absoluteX ()));
}

// ---
_INST_IMPL (F6500::SLO_AbsoluteY)
{
	return (executeOn (address_absoluteY ()));
}
