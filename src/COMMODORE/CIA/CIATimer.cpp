#include <COMMODORE/CIA/CIATimer.hpp>

// ---
void COMMODORE::CIATimer::initialize ()
{
	_runMode = RunMode::_RESTART;
	_countMode = CountMode::_PROCESSORCYCLES;

	_affectPortDataB = false;
	_portOutputMode = PortOutputMode::_PULSE;
	_portOutput = false;
	_pulseOutputActive = false;

	_enabled = false;
	_interruptEnabled = false;

	_initialValue = 0x0000;
	_currentValue = 0x0000;

	_interruptRequested = false;
}

// ---
void COMMODORE::CIATimer::setEnabled (bool e)
{
	if (_enabled == e)
		return;

	_enabled = e;

	// In toggle mode the output is set when the timer is started.
	if (

		_affectPortDataB &&
		_portOutputMode == PortOutputMode::_TOGGLE)
		_portOutput = true;
}

// ---
void COMMODORE::CIATimer::setLatchLow (const MCHEmul::UByte& v)
{
	_initialValue =
		(_initialValue & 0xff00) |
		(unsigned short) v.value ();
}

// ---
void COMMODORE::CIATimer::setLatchHigh (const MCHEmul::UByte& v)
{
	_initialValue =
		(_initialValue & 0x00ff) |
		((unsigned short) v.value () << 8);

	// A high-byte write loads the counter only while the timer is stopped.
	if (!_enabled)
		forceLoad ();
}

// ---
COMMODORE::CIATimer::CycleResult COMMODORE::CIATimer::simulate
	(bool processorCycle, bool CNTRisingEdge, bool linkedTimerUnderflow, bool CNTHigh)
{
	CycleResult result;

	// Pulse mode holds the timer output active for exactly one simulated cycle.
	if (_pulseOutputActive)
	{
		_pulseOutputActive = false;
		if (_portOutput)
		{
			_portOutput = false;
			result._portOutputChanged = true;
			result._portOutput = false;
		}
	}

	if (!_enabled ||
		!hasToCount (processorCycle, CNTRisingEdge, linkedTimerUnderflow, CNTHigh))
		return (result);

	// CIA already invokes this method once per recovered CPU cycle.
	if (_currentValue > 1)
		_currentValue--;
	else
		triggerUnderflow (result);

	return (result);
}

// ---
MCHEmul::InfoStructure COMMODORE::CIATimer::getInfoStructure () const
{
	MCHEmul::InfoStructure result = MCHEmul::InfoClass::getInfoStructure ();

	result.add ("RUN",			(unsigned int) _runMode);
	result.add ("COUNT",		(unsigned int) _countMode);
	result.add ("STATUS",		_enabled);
	result.add ("IRQ",			_interruptEnabled);
	result.add ("VALUE",		_currentValue);
	result.add ("INITIALVALUE", _initialValue);

	return (result);
}

// ---
bool COMMODORE::CIATimer::hasToCount
	(bool processorCycle, bool CNTRisingEdge, bool linkedTimerUnderflow, bool CNTHigh) const
{
	bool result = false;

	switch (_countMode)
	{
		case CountMode::_PROCESSORCYCLES:
			result = processorCycle;
			break;

		case CountMode::_SIGNALSONCNTLINE:
			result = CNTRisingEdge;
			break;

		case CountMode::_TIMERCOUNTSDOWNTO0:
			result = linkedTimerUnderflow;
			break;

		case CountMode::_0ONCNTPULSES:
			result = linkedTimerUnderflow && CNTHigh;
			break;

		default:
			break;
	}

	return (result);
}

// ---
void COMMODORE::CIATimer::triggerUnderflow (CycleResult& result)
{
	result._underflow = true;
	_interruptRequested = true;
	_currentValue = _initialValue;

	if (_affectPortDataB)
	{
		bool oldOutput = _portOutput;

		if (_portOutputMode == PortOutputMode::_TOGGLE)
			_portOutput = !_portOutput;
		else
		{
			_portOutput = true;
			_pulseOutputActive = true;
		}

		result._portOutputChanged = oldOutput != _portOutput;
		result._portOutput = _portOutput;
	}

	if (_runMode == RunMode::_ONETIME)
		_enabled = false;
}
