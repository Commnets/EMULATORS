#include <F6500/NMIInterrupt.hpp>
#include <F6500/C6510.hpp>

// ---
void F6500::NMIInterrupt::executeOverImpl (MCHEmul::CPU* c, unsigned int& nC)
{
	assert (c != nullptr);
	assert (c -> memoryRef () != nullptr);
	assert (c -> memoryRef () -> stack () != nullptr);

	MCHEmul::ProgramCounter& pc = c -> programCounter ();
	MCHEmul::StatusRegister& st = c -> statusRegister ();
	c -> memoryRef () -> stack () -> push (pc.asAddress ().bytes () /** First high, then low byte */);
	c -> memoryRef () -> stack () -> push (st.valuesWithout ({ "B" })); // The break flag is not taken into account...

	pc.setAddress (MCHEmul::Address (c -> memoryRef () -> values 
		(dynamic_cast <F6500::C6510*> (c) -> NMIVectorAddress (), 2), false /** Little - endian. */));

	nC = 7; // It took 7 ticks to run it...
}
