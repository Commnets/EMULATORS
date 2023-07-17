#include <F6500/IRQInterrupt.hpp>
#include <F6500/C6510.hpp>

// ---
bool F6500::IRQInterrupt::isTime (MCHEmul::CPU* c) const
{
	return (!c -> statusRegister ().bitStatus (F6500::C6500::_IRQFLAG));
}

// ---
bool F6500::IRQInterrupt::executeOverImpl (MCHEmul::CPU* c, unsigned int& nC, unsigned int& nCR)
{
	assert (c != nullptr);
	assert (c -> memoryRef () != nullptr);
	assert (c -> memoryRef () -> stack () != nullptr);

	MCHEmul::StatusRegister& st = c -> statusRegister ();
	MCHEmul::ProgramCounter& pc = c -> programCounter ();
	c -> memoryRef () -> stack () -> push ((_exeAddress = pc.asAddress ()).bytes () /** First high, then low byte */);
	c -> memoryRef () -> stack () -> push (st.values ()); // The break flag is not taken into account...
	st.setBitStatus (F6500::C6500::_IRQFLAG, true); // No more interruptions so far...

	pc.setAddress (MCHEmul::Address (c -> memoryRef () -> values 
		(dynamic_cast <F6500::C6510*> (c) -> IRQVectorAddress (), 2), false /** Little - endian. */));

	nC = 7; // 7 ticks has taken...
	nCR = 4; // 4 of which where on reading activities...

	return (!c -> memoryRef () -> stack () -> overflow ());
}

// ---
MCHEmul::InfoStructure F6500::IRQInterrupt::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::CPUInterrupt::getInfoStructure ());

	result.add ("ADDRESS", _exeAddress);

	return (result);
}
