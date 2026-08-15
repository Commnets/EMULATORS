#include <F6500/Interrupt.hpp>

const MCHEmul::CycleStructure F6500::Interrupt::_CYCLESTRUCTURE =
{
	MCHEmul::CPUCycle::_READ,
	MCHEmul::CPUCycle::_READ,
	MCHEmul::CPUCycle::_WRITE,
	MCHEmul::CPUCycle::_WRITE,
	MCHEmul::CPUCycle::_WRITE,
	MCHEmul::CPUCycle::_READ,
	MCHEmul::CPUCycle::_READ
};

// ---
F6500::Interrupt::Interrupt (int id, int pr)
	: MCHEmul::CPUInterrupt (id, 7, pr, _CYCLESTRUCTURE),
	  _exeAddress (),
	  _requestClock (0),
	  _execClock (0),
	  _instChecked (false)
{
	assert (busCycleData ()._numberReadCycles == 4);
	assert (busCycleData ()._numberWriteCycles == 3);
	assert (busCycleData ()._maximumConsecutiveWriteCycles == 3);
}

// ---
void F6500::Interrupt::initialize ()
{ 
	MCHEmul::CPUInterrupt::initialize (); 
							  
	_instChecked = false; 
}

// ---
MCHEmul::InfoStructure F6500::Interrupt::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::CPUInterrupt::getInfoStructure ());

	result.add ("REQCLOCK", _requestClock);
	result.add ("EXECCLOCK", _execClock);
	result.add ("ADDRESS", (_exeAddress == MCHEmul::Address ()) 
		? "-" : _exeAddress.asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0', 0));

	return (result);
}

// ---
unsigned int F6500::Interrupt::isTime (MCHEmul::CPU* c, unsigned int cC) const
{
	// If a previous instruction was checked,
	// the interrupt has to be launched, because even if the last one had only 2 cycles
	// the total accumulated since the previous one will be more than 2 cycles.
	if (_instChecked)
	{ 
		// ...ready for a new consult...
		_instChecked = false;

		_execClock = c -> clockCycles ();

		return (MCHEmul::CPUInterrupt::_EXECUTIONALLOWED);	
	}
	else
	{
		if ((c -> clockCycles () - cC) > 2)
		{
			_requestClock = _execClock = c -> clockCycles ();

			return (MCHEmul::CPUInterrupt::_EXECUTIONALLOWED);
		}
		else
		{ 
			_instChecked = true;

			_requestClock = c -> clockCycles ();

			return (MCHEmul::CPUInterrupt::_EXECUTIONTOWAIT);
		}
	}
}
