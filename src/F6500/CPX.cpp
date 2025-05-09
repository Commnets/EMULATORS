#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
_INST_IMPL (F6500::CPX_Inmediate)
{
	return (executeWith (value_inmediate ()));
}

// ---
_INST_IMPL (F6500::CPX_Absolute)
{
	return (executeWith (value_absolute ()));
}

// ---
_INST_IMPL (F6500::CPX_ZeroPage)
{
	return (executeWith (value_zeroPage ()));
}
