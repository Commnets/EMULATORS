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
			!_computer -> cpu () -> deepDebug ())
		{
			if (_computer -> cpu () -> activateDeepDebug (_debugFileName, _addDebugInfo)) // ...activate the deep debug, and if there is no error...
			{ 
				_computer -> cpu () -> deepDebugFile () <<
					"---------" << std::endl <<
					"Starting Interrupt Debugging" << std::endl << std::endl;
				_computer -> cpu () -> deepDebugFile () <<
					MCHEmul::removeAll0 (
						MCHEmul::FormatterBuilder::instance () -> formatter ("Computer") -> 
							format (_computer -> getInfoStructure ())) << std::endl; // ...and prints out the status of the computer...
			}
		}
	}
	else
	{
		// Debugging...and first time to pass over this point?
		if (_debug && _computer -> cpu () -> deepDebug ())
		{
			_computer -> cpu () -> desactivateDeepDebug (); // ...deactivate the deep debug...

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
bool MCHEmul::CPUInterrupt::executeOver (MCHEmul::CPU* c, unsigned int& nC)
{
	assert (c != nullptr);

	bool result = false;

	if (active ()) // Launched?
	{
		setActive (false); //...to the next time...

		if (isTime (c)) // But allowed?
		{ 
			setInExecution (true);

			_lastClockCyclesExecuted = c -> clockCycles ();

			result = executeOverImpl (c, nC);

			// The sentence (dependen of the CPU instruction set)
			// finishing the execution of the interruption will invoke setInExecution (false)
			// and if there were any debug for interruption active, it would be closed!
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

// ---
bool MCHEmul::CPUInterrupt::activateDebug (MCHEmul::Computer* c, const std::string& nF, bool a)
{ 
	assert (c != nullptr);

	// If it is already debugging or 
	// the deepDebug at CPU level is activated...
	if (_debug || 
		c -> cpu () -> deepDebug ())
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

	if (_computer -> cpu () -> deepDebug ())
		_debugOffWhenFinishes = true;
	else
	{
		_debug = false;

		_debugOffWhenFinishes = false;

		_debugFileName = "";

		_computer = nullptr;
			
		_computer -> cpu () -> desactivateDeepDebug (); // ...deactivate the deep debug...
	}

	return (true);
}
