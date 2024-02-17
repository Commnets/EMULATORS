#include <COMMODORE/TED/TEDTimer.hpp>

// ---
COMMODORE::TEDTimer::TEDTimer (int id, COMMODORE::TEDTimer::RunMode rM)
	: MCHEmul::InfoClass ("TEDTimer"),
	  _id (id),
	  _runMode (rM),
	  _interruptRequested (false)
{ 
	initialize (); /** To initialize the rest of the values. */ 
}

// ---
void COMMODORE::TEDTimer::initialize ()
{
	_initialValue = 0x0000;

	_interruptEnabled = false;

	// The implementation values...
	_counting = false;
	_currentValue = 0x0000;
	_reaches0 = _reaches0LSB = _reachesHalf = _alreadyReachedHalf = false;
	_interruptRequested = false;
}

// ---
void COMMODORE::TEDTimer::reset ()
{ 
	_currentValue = 
		(_runMode == COMMODORE::TEDTimer::RunMode::_FROMINITIALVALUE) ? _initialValue : 0xffff; 
		
	_alreadyReachedHalf = false; 
		
	_counting = false;
}

// ---
bool COMMODORE::TEDTimer::simulate (MCHEmul::CPU* cpu)
{
	assert (cpu != nullptr);

	if (_counting && 
		countDown (cpu)) // Counting one at a time...
	{
		_interruptRequested = true;

		reset ();

		start ();
	}

	return (true);
}

// ---
MCHEmul::InfoStructure COMMODORE::TEDTimer::getInfoStructure () const
{
	MCHEmul::InfoStructure result = MCHEmul::InfoClass::getInfoStructure ();

	result.add ("RUN",			(unsigned int) _runMode);
	result.add ("IRQ",			_interruptEnabled);
	result.add ("VALUE",		_currentValue);
	result.add ("INITIALVALUE", _initialValue);

	return (result);
}

// ---
bool COMMODORE::TEDTimer::countDown (MCHEmul::CPU* cpu)
{
	--_currentValue;

	// When the current value reaches 0....
	_reaches0 = (_currentValue == 0x0000);
	// ...and this one when the LSB part reaches 0...
	_reaches0LSB = ((_currentValue & 0x00ff) == 0x0000);
	// This variable is set first time the value is below the half...
	if ((_reachesHalf = ((_currentValue >> 1) < _initialValue) && !_alreadyReachedHalf)) 
		_alreadyReachedHalf = true; // Latches, but not follow the value...

	return (_reaches0);
}
