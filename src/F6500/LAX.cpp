#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
_INST_IMPL (F6500::LAX_Absolute)
{
	return (executeOn (address_absolute ()));
}

// ---
_INST_IMPL (F6500::LAX_ZeroPage)
{
	return (executeOn (address_zeroPage ()));
}

// ---
_INST_IMPL (F6500::LAX_ZeroPageIndirectX)
{
	return (executeOn (address_indirectZeroPageX ()));
}

// ---
unsigned int F6500::LAX_ZeroPageIndirectY::clockCyclesToExecute (
	MCHEmul::CPU* c, MCHEmul::Memory* m, const MCHEmul::Address& a) const
{
	return (clockCyclesForIndirectZeroPageY (m, a,
		c -> internalRegister (F6500::C6510::_YREGISTER).values ()[0].value ()));
}

// ---
_INST_IMPL (F6500::LAX_ZeroPageIndirectY)
{
	return (executeOn (address_indirectZeroPageY (PageCrossingCycle::_ADD)));
}

// ---
_INST_IMPL (F6500::LAX_ZeroPageY)
{
	return (executeOn (address_zeroPageY ()));
}

// ---
unsigned int F6500::LAX_AbsoluteY::clockCyclesToExecute (
	MCHEmul::CPU* c, MCHEmul::Memory* m, const MCHEmul::Address& a) const
{
	return (clockCyclesForAbsoluteIndexed (m, a,
		c -> internalRegister (F6500::C6510::_YREGISTER).values ()[0].value ()));
}

// ---
_INST_IMPL (F6500::LAX_AbsoluteY)
{
	return (executeOn (address_absoluteY (PageCrossingCycle::_ADD)));
}
