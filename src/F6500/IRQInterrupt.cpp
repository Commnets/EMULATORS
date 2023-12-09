#include <F6500/IRQInterrupt.hpp>
#include <F6500/C6510.hpp>

// ---
MCHEmul::InfoStructure F6500::IRQInterrupt::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::CPUInterrupt::getInfoStructure ());

	result.add ("ADDRESS", _exeAddress);

	return (result);
}

// ---
bool F6500::IRQInterrupt::isTime (MCHEmul::CPU* c, unsigned int cC) const
{
	return (!c -> statusRegister ().bitStatus (F6500::C6500::_IRQFLAG) &&
			F6500::Interrupt::isTime (c, cC));
}

// ---
bool F6500::IRQInterrupt::executeOverImpl (MCHEmul::CPU* c, unsigned int cC)
{
	F6500::Interrupt::executeOverImpl (c, cC);

	assert (c != nullptr);
	assert (c -> memoryRef () != nullptr);
	assert (c -> memoryRef () -> stack () != nullptr);

	MCHEmul::StatusRegister& st = c -> statusRegister ();
	MCHEmul::ProgramCounter& pc = c -> programCounter ();
	c -> memoryRef () -> stack () -> push ((_exeAddress = pc.asAddress ()).bytes () /** First high, then low byte */);
	c -> memoryRef () -> stack () -> push (st.values ()); // The break flag is not taken into account...
	st.setBitStatus (F6500::C6500::_IRQFLAG, true); // No more interruptions so far...

	pc.setAddress (MCHEmul::Address (c -> memoryRef () -> values 
		(static_cast <F6500::C6500*> (c) -> IRQVectorAddress (), 2), false /** Little - endian. */));

	return (!c -> memoryRef () -> stack () -> overflow ());
}
