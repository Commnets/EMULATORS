#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
_INST_IMPL (F6500::SAX_Absolute)
{
	return (executeOn (address_absolute ()));
}

// ---
_INST_IMPL (F6500::SAX_ZeroPage)
{
	return (executeOn (address_zeroPage ()));
}

// ---
_INST_IMPL (F6500::SAX_ZeroPageY)
{
	return (executeOn (address_zeroPageY ()));
}

// ---
_INST_IMPL (F6500::SAX_ZeroPageIndirectX)
{
	return (executeOn (address_indirectZeroPageX ()));
}
