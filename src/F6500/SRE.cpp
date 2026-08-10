#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
_INST_IMPL (F6500::SRE_Absolute)
{
	return (executeOn (address_absolute ()));
}

// ---
_INST_IMPL (F6500::SRE_ZeroPage)
{
	return (executeOn (address_zeroPage ()));
}

// ---
_INST_IMPL (F6500::SRE_ZeroPageIndirectX)
{
	return (executeOn (address_indirectZeroPageX ()));
}

// ---
_INST_IMPL (F6500::SRE_ZeroPageIndirectY)
{
	return (executeOn (address_indirectZeroPageY (PageCrossingCycle::_IGNORE)));
}

// ---
_INST_IMPL (F6500::SRE_ZeroPageX)
{
	return (executeOn (address_zeroPageX ()));
}

// ---
_INST_IMPL (F6500::SRE_AbsoluteX)
{
	return (executeOn (address_absoluteX (PageCrossingCycle::_IGNORE)));
}

// ---
_INST_IMPL (F6500::SRE_AbsoluteY)
{
	return (executeOn (address_absoluteY (PageCrossingCycle::_IGNORE)));
}
