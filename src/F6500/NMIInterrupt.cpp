#include <F6500/NMIInterrupt.hpp>
#include <F6500/C6510.hpp>

// ---
bool F6500::NMIInterrupt::executeOverImpl (MCHEmul::CPU* c, unsigned int cC)
{
	if (!F6500::Interrupt::executeOverImpl (c, cC))
		return (false);

	assert (c != nullptr);
	assert (c -> memoryRef () != nullptr);
	assert (c -> memoryRef () -> stack () != nullptr);

	MCHEmul::ProgramCounter& pc = c -> programCounter ();
	MCHEmul::StatusRegister& st = c -> statusRegister ();
	c -> memoryRef () -> stack () -> push ((_exeAddress = pc.asAddress ()).bytes () /** First high, then low byte */);
	c -> memoryRef () -> stack () -> push (st.values ()); // The break flag is not taken into account...
	st.setBitStatus (F6500::C6500::_IRQFLAG, true); // No more interruptions so far...

	pc.setAddress (MCHEmul::Address (c -> memoryRef () -> values 
		(static_cast <F6500::C6500*> (c) -> NMIVectorAddress (), 2), false /** Little - endian. */));

	return (!c -> memoryRef () -> stack () -> overflow ());
}

