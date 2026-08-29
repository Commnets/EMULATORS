#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
_INST_IMPL (F6500::LDA_Inmediate)
{
	return (executeWith (value_inmediate ()));
}

// ---
_INST_IMPL (F6500::LDA_Absolute)
{
	return (executeWith (value_absolute ()));
}

// ---
_INST_IMPL (F6500::LDA_ZeroPage)
{
	return (executeWith (value_zeroPage ()));
}

// ---
_INST_IMPL (F6500::LDA_ZeroPageIndirectX)
{
	return (executeWith (value_indirectZeroPageX ()));
}

// ---
unsigned int F6500::LDA_ZeroPageIndirectY::clockCyclesToExecute (
	MCHEmul::CPU* c, MCHEmul::Memory* m, const MCHEmul::Address& a) const
{
	return (clockCyclesForIndirectZeroPageY (m, a,
		c -> internalRegister (F6500::C6510::_YREGISTER).values ()[0].value ()));
}

// ---
_INST_IMPL (F6500::LDA_ZeroPageIndirectY)
{
	return (executeWith (value_indirectZeroPageY ()));
}

// ---
_INST_IMPL (F6500::LDA_ZeroPageX)
{
	return (executeWith (value_zeroPageX ()));
}

// ---
unsigned int F6500::LDA_AbsoluteX::clockCyclesToExecute (
	MCHEmul::CPU* c, MCHEmul::Memory* m, const MCHEmul::Address& a) const
{
	return (clockCyclesForAbsoluteIndexed (m, a,
		c -> internalRegister (F6500::C6510::_XREGISTER).values ()[0].value ()));
}

// ---
_INST_IMPL (F6500::LDA_AbsoluteX)
{
	return (executeWith (value_absoluteX ()));
}

// ---
unsigned int F6500::LDA_AbsoluteY::clockCyclesToExecute (
	MCHEmul::CPU* c, MCHEmul::Memory* m, const MCHEmul::Address& a) const
{
	return (clockCyclesForAbsoluteIndexed (m, a,
		c -> internalRegister (F6500::C6510::_YREGISTER).values ()[0].value ()));
}

// ---
_INST_IMPL (F6500::LDA_AbsoluteY)
{
	return (executeWith (value_absoluteY ()));
}
