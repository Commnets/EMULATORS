#include <F6500/NMIInterrupt.hpp>
#include <F6500/C6510.hpp>

// ---
MCHEmul::InfoStructure F6500::NMIInterrupt::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::CPUInterrupt::getInfoStructure ());

	result.add ("ADDRESS", _exeAddress);

	return (result);
}

// ---
bool F6500::NMIInterrupt::executeOverImpl (MCHEmul::CPU* c, unsigned int cC)
{
	F6500::Interrupt::executeOverImpl (c, cC);

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

