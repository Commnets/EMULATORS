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
	_reloadValue = 0x0000;

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
		(_runMode == COMMODORE::TEDTimer::RunMode::_FROMRELOADVALUE) ? _reloadValue : 0xffff;
		
	_reaches0 = _reaches0LSB = _reachesHalf = false;
	_alreadyReachedHalf = false;
		
	_counting = false;
}

// ---
void COMMODORE::TEDTimer::writeLowByte (unsigned char v)
{
	_reloadValue = (_reloadValue & 0xff00) | (unsigned short) v;

	// Loading the low byte inhibits counting until the high byte is written.
	_currentValue = _reloadValue;
	_counting = false;

	_reaches0 = _reaches0LSB = _reachesHalf = false;
	_alreadyReachedHalf = false;
}

// ---
void COMMODORE::TEDTimer::writeHighByte (unsigned char v)
{
	_reloadValue = (_reloadValue & 0x00ff) | ((unsigned short) v << 8);
	_currentValue = _reloadValue;
	_counting = true;

	_reaches0 = _reaches0LSB = _reachesHalf = false;
	_alreadyReachedHalf = false;
}

// ---
void COMMODORE::TEDTimer::clock ()
{
	// Event indicators are pulses lasting one timer clock.
	_reaches0 = _reaches0LSB = _reachesHalf = false;

	if (_counting && 
		countDown ()) // Counting one at a time...
	{
		_interruptRequested = true;

		// The current implementation reloads in the same timer clock.
		// This preserves the existing underflow cadence until it is verified against hardware.
		_currentValue =
			(_runMode == COMMODORE::TEDTimer::RunMode::_FROMRELOADVALUE) ? _reloadValue : 0xffff;
		_alreadyReachedHalf = false;
	}
}

// ---
MCHEmul::InfoStructure COMMODORE::TEDTimer::getInfoStructure () const
{
	MCHEmul::InfoStructure result = MCHEmul::InfoClass::getInfoStructure ();

	result.add ("RUN",			(unsigned int) _runMode);
	result.add ("IRQ",			_interruptEnabled);
	result.add ("VALUE",		_currentValue);
	result.add ("RELOADVALUE",	_reloadValue);

	return (result);
}

// ---
bool COMMODORE::TEDTimer::countDown ()
{
	--_currentValue;

	// When the current value reaches 0....
	_reaches0 = (_currentValue == 0x0000);
	// ...and this one when the LSB part reaches 0...
	_reaches0LSB = ((_currentValue & 0x00ff) == 0x0000);
	// This variable is set the first time the value reaches half of the programmed value.
	if (!_alreadyReachedHalf &&
		_currentValue <= (_reloadValue >> 1))
	{
		_reachesHalf = true;
		_alreadyReachedHalf = true;
	}

	return (_reaches0);
}
