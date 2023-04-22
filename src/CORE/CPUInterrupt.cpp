#include <CORE/CPUInterrupt.hpp>
#include <CORE/CPU.hpp>

// ---
bool MCHEmul::CPUInterrupt::executeOver (MCHEmul::CPU* c, unsigned int& nC)
{
	assert (c != nullptr);

	bool result = false;

	if (active ()) // Launched?
	{
		setActive (false);

		if (isTime (c)) // But allowed?
		{ 
			_inExecution = true;

			_lastClockCyclesExecuted = c -> clockCycles ();

			result = executeOverImpl (c, nC);
		}
	}

	return (result);
}

// ---
MCHEmul::InfoStructure MCHEmul::CPUInterrupt::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	result.add ("ID",			_id);
	result.add ("EXECUTION",	std::string (_inExecution ? "YES" : "NO"));

	return (result);
}
