#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
_INST_IMPL (F6500::LDX_Inmediate)
{
	return (executeWith (value_inmediate ()));
}

// ---
_INST_IMPL (F6500::LDX_Absolute)
{
	return (executeWith (value_absolute ()));
}

// ---
_INST_IMPL (F6500::LDX_ZeroPage)
{
	return (executeWith (value_zeroPage ()));
}

// ---
unsigned int F6500::LDX_AbsoluteY::clockCyclesToExecute (
	MCHEmul::CPU* c, MCHEmul::Memory* m, const MCHEmul::Address& a) const
{
	return (clockCyclesForAbsoluteIndexed (m, a,
		c -> internalRegister (F6500::C6510::_YREGISTER).values ()[0].value ()));
}

// ---
_INST_IMPL (F6500::LDX_AbsoluteY)
{
	return (executeWith (value_absoluteY ()));
}

// ---
_INST_IMPL (F6500::LDX_ZeroPageY)
{
	return (executeWith (value_zeroPageY ()));
}
