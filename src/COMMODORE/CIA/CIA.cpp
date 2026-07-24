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
	  _CNTPin (true),
	  _pendingCNTRisingEdges (0), _pendingCNTFallingEdges (0)
{ 
	setClassName ("CIA");

	observe (&_serialPort);
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

	_CNTPin = true;
	_pendingCNTRisingEdges = _pendingCNTFallingEdges = 0;

	_lastClockCycles = 0;

	return (true);
}

// ---
bool COMMODORE::CIA::simulate (MCHEmul::CPU* cpu)
{
	if (_lastClockCycles == 0)
	{
		_lastClockCycles = cpu -> clockCycles ();
		return (true);
	}

	if (_CIARegisters -> interruptsEnabledBack ())
		cpu -> interrupt (_interruptId) -> setNewInterruptRequestAdmitted (false);

	for (unsigned int i = cpu -> clockCycles () - _lastClockCycles; i > 0; i--)
	{
		_IFDEBUG debugCIACycle (cpu, i);

		bool CNTRisingEdge = consumeCNTRisingEdge ();
		bool CNTFallingEdge = consumeCNTFallingEdge ();
		CIATimer::CycleResult timerAResult = 
			_timerA.simulate (true, CNTRisingEdge, false, _CNTPin);
		CIATimer::CycleResult timerBResult = _timerB
			.simulate (true, CNTRisingEdge, timerAResult._underflow, _CNTPin);

		_CIARegisters -> setReflectTimerAAtPortDataB 
			(_timerA.affectPortDataB (), _timerA.portOutput ());
		_CIARegisters -> setReflectTimerBAtPortDataB 
			(_timerB.affectPortDataB (), _timerB.portOutput ());

		_clock.simulate (cpu);
		_serialPort.simulate (CNTRisingEdge, CNTFallingEdge, timerAResult._underflow,
			_timerA.runMode () == CIATimer::RunMode::_RESTART);

		int cI = -1;
		if ((cI = (int) _CIARegisters -> reasonIRQCode ()) != 0)
			cpu -> requestInterrupt (_interruptId, cpu -> clockCycles () - i, this, cI);

		_lastClockCycles++;
	}

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
		bool newValue = evnt.value () == 1;
		if (!_CNTPin && newValue)
			_pendingCNTRisingEdges++;
		else if (_CNTPin && !newValue)
			_pendingCNTFallingEdges++;
		_CNTPin = newValue;
	}
	else if (evnt.id () == _SPSIGNAL)
		_serialPort.setSPSignal (evnt.value () == 1);

	if (n == &_serialPort)
		notify (evnt);
}

// ---
bool COMMODORE::CIA::consumeCNTRisingEdge ()
{
	if (_pendingCNTRisingEdges == 0)
		return (false);
	_pendingCNTRisingEdges--;
	return (true);
}

// ---
bool COMMODORE::CIA::consumeCNTFallingEdge ()
{
	if (_pendingCNTFallingEdges == 0)
		return (false);
	_pendingCNTFallingEdges--;
	return (true);
}

// ---
void COMMODORE::CIA::debugCIACycle (MCHEmul::CPU* cpu, unsigned int i)
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeCompleteLine (className (), cpu -> clockCycles () - i, "Info Cycle",
		{ { "Port A status",
			std::to_string (_CIARegisters -> outputRegisterA ()) + "," +
			std::to_string (_CIARegisters -> dataPortADir ()) + "," +
			_CIARegisters -> portA ().asString (MCHEmul::UByte::OutputFormat::_HEXA, 0) },
		  { "Port B status",
			std::to_string (_CIARegisters -> outputRegisterB ()) + "," +
			std::to_string (_CIARegisters -> dataPortBDir ()) + "," +
			_CIARegisters -> portB ().asString (MCHEmul::UByte::OutputFormat::_HEXA, 0) },
		  { "Timer A status",
			std::string (_CIARegisters -> _timerA -> enabled () ? "ON" : "OFF") + "," +
			std::to_string ((unsigned int) _CIARegisters -> _timerA -> countMode ()) + "," +
			std::to_string (_CIARegisters -> _timerA -> initialValue ()) + "," +
			std::to_string (_CIARegisters -> _timerA -> currentValue ()) },
		  { "Timer B status",
			std::string (_CIARegisters -> _timerB -> enabled () ? "ON" : "OFF") + "," +
			std::to_string ((unsigned int) _CIARegisters -> _timerB -> countMode ()) + "," +
			std::to_string (_CIARegisters -> _timerB -> initialValue ()) + "," +
			std::to_string (_CIARegisters -> _timerB -> currentValue ()) } });
}
