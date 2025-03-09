#include <CORE/CPUInterrupt.hpp>
#include <CORE/Computer.hpp>
#include <CORE/CPU.hpp>

bool MCHEmul::CPUInterrupt::_debug = false;
std::string MCHEmul::CPUInterrupt::_debugFileName = "";
bool MCHEmul::CPUInterrupt::_addDebugInfo = true; // Adding info by default...
MCHEmul::Computer* MCHEmul::CPUInterrupt::_computer = nullptr;
bool MCHEmul::CPUInterrupt::_debugOffWhenFinishes = false;
unsigned long MCHEmul::CPUInterruptRequest::_COUNTER = 0;
// Only the first parameter cares...the rest don't!
const MCHEmul::CPUInterruptRequest MCHEmul::CPUInterruptRequest::_NOINTREQUEST = 
	{ -1 /** not use this id ever for a real transaction type */, 0, nullptr, -1 }; 

// ---
void MCHEmul::CPUInterrupt::setInExecution (bool i)
{
	if (i == _inExecution)
		return; // No changed, nothing to do...

	if ((_inExecution = i))
	{
		if (_computer == nullptr)
			return;

		// Debugging...
		// and first time to pass over this point?
		if (_debug && 
			!_computer -> deepDebug ())
		{
			if (_computer -> activateDeepDebug // ...activate the deep debug, and if there is no error...
				(_debugFileName,
					MCHEmul::Address (),
					_computer -> cpu () -> architecture ().longestAddressPossible (),
					true, 
					{ -1 } /* all chips. */, { -1 } /* all io. */, { -1 } /** all memory. */, _addDebugInfo))
				debugStartingToDebugInterrupt ();
		}
	}
	else
	{
		if (_computer == nullptr)
			return;

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

	// Mark that the interrupt is starting...
	_IFDEBUGINELEMENT(c) debugInterruptToStart (c);

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
	result.add ("PRIORITY",		_priority);
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

// ---
void MCHEmul::CPUInterrupt::debugStartingToDebugInterrupt ()
{
	assert (_computer != nullptr);

	_computer -> deepDebugFile () -> writeSimpleLine ("\n****************************************");
	_computer -> deepDebugFile () -> writeSimpleLine ("Starting Interrupt Debugging");
	_computer -> deepDebugFile () -> writeSimpleLine ("CPU STATUS:");
	_computer -> deepDebugFile () -> writeSimpleLine (MCHEmul::removeAll0 (
		MCHEmul::FormatterBuilder::instance () -> formatter ("Computer") -> 
			format (_computer -> getInfoStructure ())));
	_computer -> deepDebugFile () -> writeSimpleLine ("Last instruction executed before Interrupt:");
	_computer -> deepDebugFile () -> writeSimpleLine (
		MCHEmul::removeAll0 (_computer -> cpu () -> programCounter ().asString ()) + ":" +
			_computer -> cpu () -> lastInstruction () -> asString ());
}

// ---
void MCHEmul::CPUInterrupt::debugInterruptToStart (MCHEmul::CPU* c)
{
	assert (c != nullptr);

	c -> deepDebugFile () -> writeSimpleLine ("->Interrupt code about to start:");
}

std::string MCHEmul::CPUInterruptRequest::toString () const
{ 
	return (
		"type " +	std::to_string (_type) + ", " +
		"from " + ((_from == nullptr) ? " - " : std::to_string (_from -> id ())) + ", " +
		"reason " + std::to_string (_reason));
}

// ---
MCHEmul::CPUInterruptSystem::CPUInterruptSystem (const MCHEmul::CPUInterrupts& irr)
	: MCHEmul::InfoClass ("CPUInterruptSystem"),
	  _interrupts (irr),
	  _rowInterrupts ()
{ 
	makeInterruptionRowData (); 
}

// --
MCHEmul::CPUInterruptSystem::~CPUInterruptSystem ()
{
	for (const auto& i : _interrupts)
		delete (i.second);
}

// ---
void MCHEmul::CPUInterruptSystem::addInterrupt (MCHEmul::CPUInterrupt* in)
{
	assert (in != nullptr);

	if (_interrupts.find (in -> id ()) != _interrupts.end ())
		return; // Only one with the the same id...

	_interrupts.insert (MCHEmul::CPUInterrupts::value_type (in -> id (), in));

	makeInterruptionRowData ();
}

// ---
void MCHEmul::CPUInterruptSystem::removeInterrrupt (int id)
{
	MCHEmul::CPUInterrupts::const_iterator i;
	if ((i = _interrupts.find (id)) == _interrupts.end ())
		return;

	_interrupts.erase (i);

	makeInterruptionRowData ();
}

// ---
MCHEmul::InfoStructure MCHEmul::CPUInterruptSystem::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	MCHEmul::InfoStructure intrr;
	for (const auto& i : _interrupts)
		intrr.add (std::to_string (i.second -> id ()), std::move (i.second -> getInfoStructure ()));
	result.add ("INTERRUPTS", intrr);

	return (result);
}

// ---
void MCHEmul::CPUInterruptSystem::makeInterruptionRowData ()
{
	_rowInterrupts.clear ();

	// Put the interruptions in an array to speed up things later...
	_rowInterrupts = MCHEmul::CPUListOfInterrupts
		((*_interrupts.rbegin ()).second -> id () + 1 /** the last code plus 1. */, nullptr);
	for (const auto& i : _interrupts)
	{	
		// Just to be sure that the boundaries of the list are overlapped...
		assert ((size_t) i.second -> id () < _rowInterrupts.size ());
		_rowInterrupts [i.second -> id ()] = i.second;
	}
}

// ---
bool MCHEmul::StandardCPUInterruptSystem::requestInterrupt (const MCHEmul::CPUInterruptRequest& iR)
{ 
	// Only one interrupt at the same time...
	// or with more priority that the one that could be already pending to be processed...
	bool result = 
		(_interruptsRequested.empty () || 
		 (!_interruptsRequested.empty () && 
			 _rowInterrupts [iR.type ()] -> priority () >
			 _rowInterrupts [(*_interruptsRequested.begin ()).type ()] -> priority ())) &&
	// ...and also wether to admit a new interrupt of the same type is possible...
		_rowInterrupts [iR.type ()] -> admitNewInterruptRequest ();

	if (result)
	{ 
		// Insert the new one at the beginning of the vector always...
		// But if any other interrupt were already in with lower priority it would be maintained...
		_interruptsRequested.insert (_interruptsRequested.begin (), iR);
		// The new interrupt has been admittted...
		_rowInterrupts [iR.type ()] -> setNewInterruptRequestAdmitted (true);
	}

	return (result);
}
