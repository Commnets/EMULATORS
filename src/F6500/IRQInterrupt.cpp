#include <F6500/IRQInterrupt.hpp>
#include <F6500/C6510.hpp>
#include <core/Memory.hpp>
#include <core/Stack.hpp>

// ---
bool F6500::IRQInterrupt::isTime (MCHEmul::CPU* c) const
{
	return (!c -> statusRegister ().bitStatus ("I"));
}

// ---
void F6500::IRQInterrupt::executeOverImpl (MCHEmul::CPU* c, unsigned int& nC)
{
	assert (c != nullptr);
	assert (c -> memoryRef () != nullptr);
	assert (c -> memoryRef () -> stack () != nullptr);

	MCHEmul::StatusRegister& st = c -> statusRegister ();
	MCHEmul::ProgramCounter& pc = c -> programCounter ();
	c -> memoryRef () -> stack () -> push (pc.asAddress ().bytes ().reverse () /** Little - endian. */);
	c -> memoryRef () -> stack () -> push (st.valuesWithout ({ "B" })); // The break flag is not taken into account...
	st.setBitStatus ("I", true); // No more interruptions so far...

	pc.setAddress (MCHEmul::Address (c -> memoryRef () -> values 
		(dynamic_cast <F6500::C6510*> (c) -> IRQVectorAddress (), 2), false /** Little - endian. */));

	// TODO

	nC = 0;
}
