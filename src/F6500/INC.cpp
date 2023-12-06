#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
_INST_IMPL (F6500::INC_Absolute)
{
	return (executeOn (address_absolute ()));
}

// ---
_INST_IMPL (F6500::INC_ZeroPage)
{
	return (executeOn (address_zeroPage ()));
}

// ---
_INST_IMPL (F6500::INC_ZeroPageX)
{
	return (executeOn (address_zeroPageX ()));
}

// ---
_INST_IMPL (F6500::INC_AbsoluteX)
{
	return (executeOn (address_absoluteX ()));
}
