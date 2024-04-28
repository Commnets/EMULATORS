#include <FZ80/Interrupt.hpp>

// ---
MCHEmul::InfoStructure FZ80::Interrupt::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::CPUInterrupt::getInfoStructure ());

	result.add ("ADDRESS", (_exeAddress == MCHEmul::Address ()) 
		? "-" : _exeAddress.asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0', 0));

	return (result);
}
