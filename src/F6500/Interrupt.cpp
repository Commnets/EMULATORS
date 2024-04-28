#include <F6500/Interrupt.hpp>

// ---
MCHEmul::InfoStructure F6500::Interrupt::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::CPUInterrupt::getInfoStructure ());

	result.add ("ADDRESS", (_exeAddress == MCHEmul::Address ()) 
		? "-" : _exeAddress.asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0', 0));

	return (result);
}

// ---
bool F6500::Interrupt::isTime (MCHEmul::CPU* c, unsigned int cC) const
{
	// If a previous instruction was checked,
	// the interrupt has to be launched, because even if the last one had only 2 cycles
	// the total accumulated since the previous one will be more than 2 cycles.
	if (_instChecked)
	{ 
		// ...ready for a new consult...
		_instChecked = false;

		return (true);
	}
	else
	{
		if ((c -> clockCycles () - cC) > 2)
			return (true);
		else
		{ 
			_instChecked = true;

			return (false);
		}
	}
}
