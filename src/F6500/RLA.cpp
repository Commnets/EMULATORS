#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
_INST_IMPL (F6500::RLA_Absolute)
{
	return (executeOn (address_absolute ()));
}

// ---
_INST_IMPL (F6500::RLA_ZeroPage)
{
	return (executeOn (address_zeroPage ()));
}

// ---
_INST_IMPL (F6500::RLA_ZeroPageIndirectX)
{
	return (executeOn (address_indirectZeroPageX ()));
}

// ---
_INST_IMPL (F6500::RLA_ZeroPageIndirectY)
{
	return (executeOn (address_indirectZeroPageY ()));
}

// ---
_INST_IMPL (F6500::RLA_ZeroPageX)
{
	return (executeOn (address_zeroPageX ()));
}

// ---
_INST_IMPL (F6500::RLA_AbsoluteX)
{
	return (executeOn (address_absoluteX ()));
}

// ---
_INST_IMPL (F6500::RLA_AbsoluteY)
{
	return (executeOn (address_absoluteY ()));
}
