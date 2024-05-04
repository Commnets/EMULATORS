#include <FZ80/NMIInterrupt.hpp>
#include <FZ80/CZ80.hpp>

// ---
bool FZ80::NMIInterrupt::executeOverImpl (MCHEmul::CPU* c, unsigned int cC)
{
	assert (c != nullptr);
	assert (c -> memoryRef () != nullptr);
	assert (c -> memoryRef () -> stack () != nullptr);
	
	FZ80::CZ80* c80 = static_cast <FZ80::CZ80*> (c);
	assert (c80 != nullptr); // Just in case...

	resetHalt (c80);
	c80 -> setIFF2 (c80-> IFF1 ()); // save IFF2
	c80 -> setIFF1 (false); // For not other interrupts to happen...
	c -> memoryRef () -> stack () -> push 
		((_exeAddress = c -> programCounter ().asAddress ()).bytes ());
	c -> programCounter ().setAddress (c80 -> NMIVectorAddress ());
	
	return (true);
}

