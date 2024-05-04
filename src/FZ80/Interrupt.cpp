#include <FZ80/Interrupt.hpp>
#include <FZ80/CZ80.hpp>

// ---
MCHEmul::InfoStructure FZ80::Interrupt::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::CPUInterrupt::getInfoStructure ());

	result.add ("ADDRESS", (_exeAddress == MCHEmul::Address ()) 
		? "-" : _exeAddress.asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0', 0));

	return (result);
}

// ---
void FZ80::Interrupt::resetHalt (FZ80::CZ80* c)
{
	assert (c != nullptr);

	if (c -> haltActive ())
	{
		c -> setHaltActive (false);

		c -> programCounter ().increment ();
	}
}
