#include <COMMODORE/CIA/CIA.hpp>
#include <F6500/IRQInterrupt.hpp>

COMMODORE::CIA::CIA (int id, int rId, unsigned int intId)
	: MCHEmul::Chip (id,
		{ { "Name", "CIA" },
		  { "Code", "6526/6526A/8521" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Year", "1980" } }),
	  _registersId (rId),
	  _CIARegisters (nullptr),
	  _interruptId (intId),
	  _timerA (0, intId /** they have to know the interruption id. */), _timerB (1, intId), 
	  _clock (0, intId),
	  _serialPort (0, intId),
	  _lastClockCycles (0),
	  _pulseTimerASentToPortB (false), _pulseTimerBSentToPortB (false)
{ 
	setClassName ("CIA"); 
}

// ---
bool COMMODORE::CIA::initialize ()
{
	assert (memoryRef () != nullptr);

	// Gets the memory block dedicated to the CIA
	if (!(_CIARegisters = 
		dynamic_cast <COMMODORE::CIARegisters*> (memoryRef () -> subset (_registersId))))
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	_CIARegisters -> initializeInternalValues ();

	_timerA.initialize ();

	_timerB.initialize ();

	_clock.initialize ();

	_serialPort.initialize ();

	_CIARegisters -> lookAtTimers (&_timerA, &_timerB);

	_CIARegisters -> lookAtClock (&_clock);

	_CIARegisters -> lookAtSerialPort (&_serialPort);

	_CIARegisters -> initialize ();

	_pulseTimerASentToPortB = _pulseTimerBSentToPortB = false;

	_lastClockCycles = 0;

	return (true);
}

// ---
bool COMMODORE::CIA::simulate (MCHEmul::CPU* cpu)
{
	// First time?
	if (_lastClockCycles == 0)
	{ 
		_lastClockCycles = cpu -> clockCycles (); // Nothing to do...

		return (true);
	}

	for (unsigned int i = cpu -> clockCycles () - _lastClockCycles; i > 0; i--)
	{
		// Simulate the Timers...
		// After that the timer can reach 0
		// If so (apart of launching a interrupt if configured) the result can be reflected
		// in the port B of the Chip...

		// Timer A
		_timerA.simulate (cpu);
		// Does timer A outcome affect the port B?
		if (_timerA.affectPortDataB ())
		{
			// if it affects and the timer A has reached 0...
			if (_timerA.reaches0 ())
				// Sets the way this is reflected
				_CIARegisters -> setReflectTimerAAtPortDataB (true, 
					(_pulseTimerASentToPortB = _timerA.pulseAtPortDataB ()) 
						? true // if it is a pulse, the pulse is reflected as true but marked hee to set it off inthe next cycle...
						: _CIARegisters -> readValue (0x01).bit (6 /** Timer A affects bit 6. */) ? false : true); // Toggle the bit!
			else
			// If it affects, it hasn't reached 0, and the pulse is active...
			if (_pulseTimerASentToPortB)
				_CIARegisters -> setReflectTimerAAtPortDataB (true, _pulseTimerASentToPortB = false); // Switch it off...
			// Otherwise, do not change anything...
			else
				_CIARegisters -> setReflectTimerAAtPortDataB (false); // No longer affects...
		}
		// Not affect...
		else
			_CIARegisters -> setReflectTimerAAtPortDataB (false); // Nothing...

		// Same but with timer B
		// The timer B has to take into account the timer A...
		_timerB.simulate (cpu, &_timerA);
		if (_timerB.affectPortDataB ())
		{
			if (_timerB.reaches0 ())
				_CIARegisters -> setReflectTimerBAtPortDataB (true, 
					(_pulseTimerBSentToPortB = _timerB.pulseAtPortDataB ()) 
						? true
						: _CIARegisters -> readValue (0x01).bit (7 /** timer B affects bit 7. */) ? false : true);
			else
			if (_pulseTimerBSentToPortB)
				_CIARegisters -> setReflectTimerBAtPortDataB (true, _pulseTimerBSentToPortB = false);
			else
				_CIARegisters -> setReflectTimerBAtPortDataB (false);
		}
		else
			_CIARegisters -> setReflectTimerBAtPortDataB (false); // Nothing...

		_clock.simulate (cpu);

		_serialPort.simulate (cpu, &_timerA);

		// Any reason to launch an interruption?...
		int cI = -1;
		if ((cI = (int) _CIARegisters -> reasonIRQCode ()) != 0)
			cpu -> requestInterrupt (
				_interruptId, 
				cpu -> clockCycles  () - i, 
				this,
				cI);
	}

	_lastClockCycles = cpu -> clockCycles ();

	return (true);
}

// ---
MCHEmul::InfoStructure COMMODORE::CIA::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::Chip::getInfoStructure ());

	result.remove ("Memory"); // This is not neccesary...
	result.add ("Registers",		std::move (_CIARegisters -> getInfoStructure ()));
	result.add ("CIATimerA",		std::move (_timerA.getInfoStructure ())); // the info origin is no longer needed...
	result.add ("CIATimerB",		std::move (_timerB.getInfoStructure ()));
	result.add ("CIAClock",			std::move (_clock.getInfoStructure ()));
	result.add ("CIASerialPort",	std::move (_serialPort.getInfoStructure ()));
	result.add ("PortA",			_CIARegisters -> portA ());
	result.add ("PortB",			_CIARegisters -> portB ()); // This is already a Ubyte...
	result.add ("CIDDRA",			MCHEmul::UByte (_CIARegisters -> dataPortADir ())); // To write doen it in hexadecimal form...
	result.add ("CIDDRB",			MCHEmul::UByte (_CIARegisters -> dataPortBDir ()));

	return (result);
}

//---
void COMMODORE::CIA::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	if (evnt.id () == _CNTSIGNAL)
	{
		_serialPort.setCNTSignal (evnt.value () == 1);
		
		_timerA.setCNTSignal (evnt.value () == 1);

		_timerB.setCNTSignal (evnt.value () == 1);
	}
	else
	if (evnt.id () == _SPSIGNAL)
		_serialPort.setSPSignal (evnt.value () == 1);
}
