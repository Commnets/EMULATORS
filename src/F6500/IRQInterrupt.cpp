#include <F6500/IRQInterrupt.hpp>
#include <F6500/C6500.hpp>

// ---
unsigned int F6500::IRQInterrupt::isTime (MCHEmul::CPU* c, unsigned int cC) const
{
	const unsigned int result = F6500::Interrupt::isTime (c, cC);
	if (result != MCHEmul::CPUInterrupt::_EXECUTIONALLOWED)
		return (result);

	return (static_cast <F6500::C6500*> (c) -> IRQDisabledAtInterruptSampling ()
		? MCHEmul::CPUInterrupt::_EXECUTIONNOTALLOWED
		: MCHEmul::CPUInterrupt::_EXECUTIONALLOWED);
}

// ---
bool F6500::IRQInterrupt::executeOverImpl (MCHEmul::CPU* c, unsigned int cC)
{
	if (!F6500::Interrupt::executeOverImpl (c, cC))
		return (false);

	assert (c != nullptr);
	assert (c -> memoryRef () != nullptr);
	assert (c -> memoryRef () -> stack () != nullptr);

	MCHEmul::StatusRegister& st = c -> statusRegister ();
	MCHEmul::ProgramCounter& pc = c -> programCounter ();
	c -> memoryRef () -> stack () -> push ((_exeAddress = pc.asAddress ()).bytes () /** First high, then low byte */);
	// The BREAK flag is always set to false when saving...
	// To distinguish this execution from the BRK one!
	MCHEmul::UByte stV = st.values ()[0];
	stV.setBit (F6500::C6500::_BREAKFLAG, false);
	c -> memoryRef () -> stack () -> push (MCHEmul::UBytes ({ stV }));
	st.setBitStatus (F6500::C6500::_IRQFLAG, true); // No more interruptions so far...

	// Jump to the IRQ vector...
	pc.setAddress (MCHEmul::Address (c -> memoryRef () -> values 
		(static_cast <F6500::C6500*> (c) -> IRQVectorAddress (), 2), false /** Little - endian. */));

	return (!c -> memoryRef () -> stack () -> overflow ());
}
