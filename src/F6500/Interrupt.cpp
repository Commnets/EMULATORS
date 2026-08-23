#include <F6500/Interrupt.hpp>
#include <F6500/C6500.hpp>

const MCHEmul::CycleStructure F6500::Interrupt::_CYCLESTRUCTURE =
{
	MCHEmul::CPUCycle::_READ,
	MCHEmul::CPUCycle::_READ,
	MCHEmul::CPUCycle::_WRITE,
	MCHEmul::CPUCycle::_WRITE,
	MCHEmul::CPUCycle::_WRITE,
	MCHEmul::CPUCycle::_READ,
	MCHEmul::CPUCycle::_READ
};

// ---
F6500::Interrupt::Interrupt (int id, int pr)
	: MCHEmul::CPUInterrupt (id, 7, pr, _CYCLESTRUCTURE),
	  _exeAddress (),
	  _requestClock (0),
	  _execClock (0)
{
	assert (busCycleData ()._numberReadCycles == 4);
	assert (busCycleData ()._numberWriteCycles == 3);
	assert (busCycleData ()._maximumConsecutiveWriteCycles == 3);
}

// ---
void F6500::Interrupt::initialize ()
{ 
	MCHEmul::CPUInterrupt::initialize (); 
}

// ---
MCHEmul::InfoStructure F6500::Interrupt::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::CPUInterrupt::getInfoStructure ());

	result.add ("REQCLOCK", _requestClock);
	result.add ("EXECCLOCK", _execClock);
	result.add ("ADDRESS", (_exeAddress == MCHEmul::Address ()) 
		? "-" : _exeAddress.asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0', 0));

	return (result);
}

// ---
unsigned int F6500::Interrupt::isTime (MCHEmul::CPU* c, unsigned int cC) const
{
	assert (c != nullptr);

	_requestClock = cC;

	if (!static_cast <const F6500::C6500*> (c) -> interruptRequestSampled (cC))
		return (MCHEmul::CPUInterrupt::_EXECUTIONTOWAIT);

	_execClock = c -> clockCycles ();

	return (MCHEmul::CPUInterrupt::_EXECUTIONALLOWED);
}
