#include <CORE/CPUInterrupt.hpp>
#include <CORE/Computer.hpp>
#include <CORE/CPU.hpp>

bool MCHEmul::CPUInterrupt::_debug = false;
std::string MCHEmul::CPUInterrupt::_debugFileName = "";
bool MCHEmul::CPUInterrupt::_addDebugInfo = true; // Adding info by default...
MCHEmul::Computer* MCHEmul::CPUInterrupt::_computer = nullptr;
bool MCHEmul::CPUInterrupt::_debugOffWhenFinishes = false;

// ---
void MCHEmul::CPUInterrupt::setInExecution (bool i)
{
	if (i == _inExecution)
		return; // No changed, nothing to do...

	if ((_inExecution = i))
	{
		// Debugging...and first time to pass over this point?
		if (_debug && 
			!_computer -> deepDebug ())
		{
			if (_computer -> activateDeepDebug 
				(_debugFileName, true, { -1 }, { -1 }, _addDebugInfo)) // ...activate the deep debug, and if there is no error...
			{ 
				*_computer -> deepDebugFile () <<
					"\n" << "****************************************" << "\n" <<
					"Starting Interrupt Debugging" << "\n" <<
					"CPU STATUS" << "\n\n";
				*_computer -> deepDebugFile () <<
					MCHEmul::removeAll0 (
						MCHEmul::FormatterBuilder::instance () -> formatter ("Computer") -> 
						format (_computer -> getInfoStructure ())) << "\n"; // ...and prints out the status of the computer...
				*_computer -> deepDebugFile () << "\n";
			}
		}
	}
	else
	{
		// Debugging...and first time to pass over this point?
		if (_debug && 
			_computer -> deepDebug ())
		{
			_computer -> desactivateDeepDebug (); // ...deactivate the deep debug...

			if (_debugOffWhenFinishes)
			{ 
				_debug = false;

				_debugOffWhenFinishes = false;

				_debugFileName = "";

				_computer = nullptr;
			}
		}
	}
}

// ---
bool MCHEmul::CPUInterrupt::executeOver (MCHEmul::CPU* c, unsigned int cC)
{
	assert (c != nullptr);

	setInExecution (true);

	_lastClockCyclesExecuted = c -> clockCycles ();

	return (executeOverImpl (c, cC));

	// The sentence (depends of the CPU instruction set)
	// finishing the execution of the interruption will invoke setInExecution (false)
	// and if there were any debug for interruption active, it would be closed!
}

// ---
MCHEmul::InfoStructure MCHEmul::CPUInterrupt::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	result.add ("ID",			_id);
	result.add ("ACTIVE",		_active);
	result.add ("EXECUTION",	std::string (_inExecution ? "YES" : "NO"));

	return (result);
}

// ---
bool MCHEmul::CPUInterrupt::activateDebug (MCHEmul::Computer* c, const std::string& nF, bool a)
{ 
	assert (c != nullptr);

	// If it is already debugging or 
	// the deepDebug at CPU level is activated...
	if (_debug || 
		c -> deepDebug ())
		return (false); // ...no activation will be possible...

	_debug = true;

	_debugOffWhenFinishes = false;

	_debugFileName = nF;

	_addDebugInfo = a;

	_computer = c;

	return (true);
}

// ---
bool MCHEmul::CPUInterrupt::desactivateDebug (MCHEmul::Computer* c)
{ 
	assert (c != nullptr);

	// If it is already debugging...
	if (!_debug)
		return (false); // ...no activation will be possible...

	if (_computer -> deepDebug ())
		_debugOffWhenFinishes = true;
	else
	{
		_computer -> desactivateDeepDebug (); // ...deactivate the deep debug...

		_debug = false;

		_debugOffWhenFinishes = false;

		_debugFileName = "";

		_computer = nullptr;
	}

	return (true);
}
