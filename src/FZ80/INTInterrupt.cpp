#include <FZ80/INTInterrupt.hpp>
#include <FZ80/CZ80.hpp>

// ---
MCHEmul::InfoStructure FZ80::INTInterrupt::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::CPUInterrupt::getInfoStructure ());

	// TODO

	return (result);
}

// ---
bool FZ80::INTInterrupt::isTime (MCHEmul::CPU* c, unsigned int cC) const
{
	// TODO

	return (FZ80::Interrupt::isTime (c, cC));
}

// ---
bool FZ80::INTInterrupt::executeOverImpl (MCHEmul::CPU* c, unsigned int cC)
{
	FZ80::Interrupt::executeOverImpl (c, cC);

	assert (c != nullptr);
	assert (c -> memoryRef () != nullptr);
	assert (c -> memoryRef () -> stack () != nullptr);
	
	// TODO
	
	return (true);
}
