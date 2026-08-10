#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
_INST_IMPL (F6500::STA_Absolute)
{
	return (executeOn (address_absolute ()));
}

// ---
_INST_IMPL (F6500::STA_ZeroPage)
{
	return (executeOn (address_zeroPage ()));
}

// ---
_INST_IMPL (F6500::STA_ZeroPageIndirectX)
{
	return (executeOn (address_indirectZeroPageX ()));
}

// ---
_INST_IMPL (F6500::STA_ZeroPageIndirectY)
{
	return (executeOn (address_indirectZeroPageY (PageCrossingCycle::_IGNORE)));
}

// ---
_INST_IMPL (F6500::STA_ZeroPageX)
{
	return (executeOn (address_zeroPageX ()));
}

// ---
_INST_IMPL (F6500::STA_AbsoluteX)
{
	return (executeOn (address_absoluteX (PageCrossingCycle::_IGNORE)));
}

// ---
_INST_IMPL (F6500::STA_AbsoluteY)
{
	return (executeOn (address_absoluteY (PageCrossingCycle::_IGNORE)));
}
