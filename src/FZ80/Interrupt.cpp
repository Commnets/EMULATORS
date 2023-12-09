#include <FZ80/Interrupt.hpp>

// ---
MCHEmul::InfoStructure FZ80::Interrupt::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::CPUInterrupt::getInfoStructure ());

	result.add ("ADDRESS", _exeAddress);

	return (result);
}
