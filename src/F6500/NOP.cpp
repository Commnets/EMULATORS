#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
_INST_IMPL (F6500::NOP)
{
	assert (parameters ().size () == 1);

	// Does nothing
	return (true);
}

// ---
_INST_IMPL (F6500::NOP_Inmediate)
{
	return (executeWith (value_inmediate ()));
}

// ---
_INST_IMPL (F6500::NOP_Absolute)
{
	return (executeWith (value_absolute ()));
}

// ---
_INST_IMPL (F6500::NOP_ZeroPage)
{
	return (executeWith (value_zeroPage ()));
}

// ---
_INST_IMPL (F6500::NOP_ZeroPageX)
{
	return (executeWith (value_zeroPageX ()));
}

// ---
unsigned int F6500::NOP_AbsoluteX::clockCyclesToExecute (
	MCHEmul::CPU* c, MCHEmul::Memory* m, const MCHEmul::Address& a) const
{
	return (clockCyclesForAbsoluteIndexed (m, a,
		c -> internalRegister (F6500::C6510::_XREGISTER).values ()[0].value ()));
}

// ---
_INST_IMPL (F6500::NOP_AbsoluteX)
{
	return (executeWith (value_absoluteX ()));
}
