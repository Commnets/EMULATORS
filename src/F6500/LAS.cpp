#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
unsigned int F6500::LAS_AbsoluteY::clockCyclesToExecute (
	MCHEmul::CPU* c, MCHEmul::Memory* m, const MCHEmul::Address& a) const
{
	return (clockCyclesForAbsoluteIndexed (m, a,
		c -> internalRegister (F6500::C6510::_YREGISTER).values ()[0].value ()));
}

// ---
_INST_IMPL (F6500::LAS_AbsoluteY)
{
	return (executeOn (address_absoluteY (PageCrossingCycle::_ADD)));
}
