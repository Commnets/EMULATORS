#include <FZ80/NMIInterrupt.hpp>
#include <FZ80/CZ80.hpp>

// ---
bool FZ80::NMIInterrupt::executeOverImpl (MCHEmul::CPU* c, unsigned int cC)
{
	assert (c != nullptr);
	assert (c -> memoryRef () != nullptr);
	assert (c -> memoryRef () -> stack () != nullptr);

	c -> memoryRef () -> stack () -> push ((_exeAddress = c -> programCounter ().asAddress ()).bytes ());
	static_cast <FZ80::CZ80*> (c) -> setIFF2 (static_cast <FZ80::CZ80*> (c) -> IFF1 ()); // save IFF2
	static_cast <FZ80::CZ80*> (c) -> setIFF1 (false);
	c -> programCounter ().setAddress (static_cast <FZ80::CZ80*> (c) -> NMIVectorAddress ());
	
	return (true);
}

