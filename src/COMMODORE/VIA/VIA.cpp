#include <COMMODORE/VIA/VIA.hpp>
#include <F6500/IRQInterrupt.hpp>

COMMODORE::VIA::VIA (int id, int rId, unsigned int intId)
	: MCHEmul::Chip (id,
		{ { "Name", "VIA" },
		  { "Code", "6522" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Year", "1980" } }),
	  _registersId (rId),
	  _VIARegisters (nullptr),
	  _interruptId (intId),
	  _CA1 (0 /** id. */), _CA2 (1), _CB1 (2), _CB2 (3),
	  _T1 (0), _T2 (1),
	  _SR (0),
	  _PA (0), _PB (1),
	  _lastClockCycles (0),
	  _interruptOutputActive (false),
	  _timedWriteCPU (nullptr),
	  _writeBusCycleData (nullptr),
	  _writeStartCycle (0),
	  _pendingRegisterWrites (),
	  _cyclesSimulatedInAdvance (0),
	  _advancedCyclePending (false)
{ 
	setClassName ("VIA"); 
}

// ---
bool COMMODORE::VIA::enableCPUTimedWrites (MCHEmul::CPU* cpu)
{
	assert (cpu != nullptr);
	assert (_VIARegisters != nullptr);
	assert (_timedWriteCPU == nullptr);
	assert (_pendingRegisterWrites.empty ());

	if (cpu -> ticksCounter () != nullptr)
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	_timedWriteCPU = cpu;
	// Establish the origin before the first transaction, including cycle zero.
	_lastClockCycles = cpu -> clockCycles ();

	return (true);
}

// ---
void COMMODORE::VIA::CPUAboutToExecute (const MCHEmul::InstructionContextEventData* dt)
{
	assert (dt != nullptr);
	if (_timedWriteCPU == nullptr || _error != MCHEmul::_NOERROR)
		return;

	const MCHEmul::InstructionDefined* instruction =
		static_cast <const MCHEmul::InstructionDefined*> (dt -> _instruction);
	const size_t cycleStructure =
		(size_t) (dt -> _clockCycles - instruction -> clockCycles ());
	const MCHEmul::BusCycleData& busData = instruction -> busCycleData (cycleStructure);
	prepareCPUTransaction (dt -> _cpu, &busData);
	if (_error != MCHEmul::_NOERROR)
		return;

	unsigned int cycles = 0;
	if (busData._lastReadCycle != MCHEmul::BusCycleData::_NOCYCLE)
		cycles = (unsigned int) (busData._lastReadCycle + 1);
	// Commands do not exist yet: never advance through an earlier write cycle.
	if (busData._numberWriteCycles != 0 && cycles > busData._firstWriteCycle)
		cycles = (unsigned int) (busData._firstWriteCycle);

	if (!simulate (dt -> _cpu, cycles))
		_error = MCHEmul::_CHIP_ERROR;
}

// ---
void COMMODORE::VIA::CPUAboutToExecute (const MCHEmul::InterruptContextEventData* dt)
{
	assert (dt != nullptr);
	if (_timedWriteCPU == nullptr)
		return;

	prepareCPUTransaction (dt -> _cpu, &dt -> _interrupt -> busCycleData ());
}

// ---
bool COMMODORE::VIA::initialize ()
{
	assert (memoryRef () != nullptr);

	// Gets the memory block dedicated to the VIA
	if (!(_VIARegisters = 
		dynamic_cast <COMMODORE::VIARegisters*> (memoryRef () -> subset (_registersId))))
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	// Initialize the control lines...
	_CA1.linkToPort (&_PA); // Each active CA1 edge captures PA; ACR selects captured input for reads.
	_CA1.initialize ();
	_CA2.linkToControlLine (&_CA1); // In handshake mode, the active CA1 edge restores CA2 high.
	_CA2.initialize ();
	_CB1.linkToPort (&_PB); // @see _CA1
	_CB1.initialize ();
	_CB2.linkToControlLine (&_CB1); // @see _CA2
	_CB2.initialize ();

	// ...then the timers...
	/** Timer 1 owns PB7 in signal modes: one-shot sets it at the first timeout,
		continuous mode toggles it at underflow; reload follows on the next cycle. */
	_T1.lookAtPort (&_PB); 
	_T1.initialize ();
	// Timer 2 samples negative-going transitions on PB6 in pulse-count mode.
	_T2.lookAtPort (&_PB);
	_T2.initialize (); 

	// ...now the shift register...
	/** And also the wires, to know the signal or to output a new one. */
	_SR.lookAtControlLines (&_CB1, &_CB2);
	/** Timer-driven serial modes use the low Timer 2 latch to reload their private divider,
		independently of the Timer 2 counter. */
	_SR.lookAtTimer (&_T2);
	_SR.initialize ();

	// ..now the ports...
	/** Read / write action might clear interrupt flag of the control lines linked (@see VIAControlLine)... */
	_PA.linkAtControlLines (&_CA1, &_CA2); 
	_PA.initialize ();
	/** PB7 will impact in port output 
		when T1 run modes = _ONESHOOTSIGNAL or _CONTINUOSSHOOTSIGNAL... */
	_PB.linkAtTimer (&_T1); 
	/** Read/write action might clear interrupt flag of the control lines linked (@see VIAControlLine)
		It will depend on the configuration of the control lines. */
	_PB.linkAtControlLines (&_CB1, &_CB2); 
	_PB.initialize ();

	// ...and finally all registers...
	/** The registers need to access control lines, timers, shift register and ports. */
	_VIARegisters -> lookAtControlLines (&_CA1, &_CA2, &_CB1, &_CB2);
	_VIARegisters -> lookAtTimers (&_T1, &_T2);
	_VIARegisters -> lookAtShiftRegister (&_SR);
	_VIARegisters -> lookAtPorts (&_PA, &_PB);
	_VIARegisters -> initialize ();

	_lastClockCycles = 0;
	_interruptOutputActive = false;
	_timedWriteCPU = nullptr;
	_writeBusCycleData = nullptr;
	_writeStartCycle = 0;
	_pendingRegisterWrites.clear ();
	_cyclesSimulatedInAdvance = 0;
	_advancedCyclePending = false;
	_error = MCHEmul::_NOERROR;

	return (true);
}

// ---
bool COMMODORE::VIA::simulate (MCHEmul::CPU* cpu)
{
	assert (cpu != nullptr);
	unsigned int remainingCycles = 0;

	if (_timedWriteCPU != nullptr)
	{
		assert (cpu == _timedWriteCPU);
		if (_error != MCHEmul::_NOERROR || cpu -> ticksCounter () != nullptr)
		{
			_error = MCHEmul::_CHIP_ERROR;
			return (false);
		}

		if (_writeBusCycleData != nullptr)
		{
			unsigned int elapsedCycles = cpu -> clockCycles () - _writeStartCycle;
			// Simulated time cannot be reversed after an inaccurate prediction.
			if (elapsedCycles < _cyclesSimulatedInAdvance ||
				_lastClockCycles != _writeStartCycle + _cyclesSimulatedInAdvance)
			{
				_error = MCHEmul::_CHIP_ERROR;
				return (false);
			}
			remainingCycles = elapsedCycles - _cyclesSimulatedInAdvance;
		}
		else
		{
			assert (_cyclesSimulatedInAdvance == 0 && !_advancedCyclePending);
			// Stopped CPU cycles can arrive without a new instruction context.
			remainingCycles = cpu -> clockCycles () - _lastClockCycles;
		}

		if (!extractPendingRegisterWrites ())
			return (false);

		if (_advancedCyclePending)
		{
			// Close the read point after its normal register read and side effects.
			if (!finishVIACycle (cpu, _lastClockCycles - 1, true))
				return (false);
			_advancedCyclePending = false;
		}
	}
	else
	{
		if (_lastClockCycles == 0)
		{
			_lastClockCycles = cpu -> clockCycles ();
			return (true);
		}

		remainingCycles = cpu -> clockCycles () - _lastClockCycles;
	}

	for (unsigned int i = 0; i < remainingCycles; ++i)
	{
		_IFDEBUG debugVIACycle (_lastClockCycles);
		if (!simulateVIACycle (cpu, _lastClockCycles, true))
			return (false);
		++_lastClockCycles;
	}

	if (_timedWriteCPU != nullptr)
	{
		if (!_pendingRegisterWrites.empty ())
		{
			_error = MCHEmul::_CHIP_ERROR;
			return (false);
		}

		_writeBusCycleData = nullptr;
		_cyclesSimulatedInAdvance = 0;
		_advancedCyclePending = false;
	}

	return (true);
}

// ---
MCHEmul::InfoStructure COMMODORE::VIA::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::Chip::getInfoStructure ());

	result.remove ("Memory"); // This is not neccesary...
	result.add ("Registers",	std::move (_VIARegisters -> getInfoStructure ()));
	result.add ("VIATimer1",	std::move (_T1.getInfoStructure ()));
	result.add ("VIATimer2",	std::move (_T2.getInfoStructure ()));
	result.add ("VIAShiftReg",	std::move (_SR.getInfoStructure ()));
	result.add ("PortA",		std::move (_PA.getInfoStructure ()));
	result.add ("PortB",		std::move (_PB.getInfoStructure ()));
	result.add ("VIACtrlA1",	std::move (_CA1.getInfoStructure ()));
	result.add ("VIACtrlA2",	std::move (_CA2.getInfoStructure ()));
	result.add ("VIACtrlB1",	std::move (_CB1.getInfoStructure ()));
	result.add ("VIACtrlB2",	std::move (_CB2.getInfoStructure ()));

	return (result);
}

// ---
void COMMODORE::VIA::prepareCPUTransaction
	(MCHEmul::CPU* cpu, const MCHEmul::BusCycleData* busData)
{
	assert (cpu != nullptr && busData != nullptr);
	assert (cpu == _timedWriteCPU);

	// One pending context requires CPU/chip interleaving at transaction boundaries.
	if (cpu -> ticksCounter () != nullptr || _writeBusCycleData != nullptr ||
		!_pendingRegisterWrites.empty () || _cyclesSimulatedInAdvance != 0 ||
		_advancedCyclePending || _lastClockCycles != cpu -> clockCycles ())
	{
		_error = MCHEmul::_CHIP_ERROR;
		return;
	}

	_writeBusCycleData = busData;
	_writeStartCycle = cpu -> clockCycles ();
	_cyclesSimulatedInAdvance = 0;
	_advancedCyclePending = false;
}

// ---
bool COMMODORE::VIA::extractPendingRegisterWrites ()
{
	assert (_pendingRegisterWrites.empty ());
	_pendingRegisterWrites = MCHEmul::Memory::configuration ().
		extractMemorySetCommandsBuffered (_VIARegisters);
	if (_pendingRegisterWrites.empty ())
		return (true);

	// VIC20 6502 data writes, including both RMW writes, target one effective address.
	if (_writeBusCycleData == nullptr ||
		_pendingRegisterWrites.size () != _writeBusCycleData -> _numberWriteCycles)
	{
		_error = MCHEmul::_CHIP_ERROR;
		return (false);
	}

	return (true);
}

// ---
bool COMMODORE::VIA::executePendingRegisterWriteAt (unsigned int cycle)
{
	if (_pendingRegisterWrites.empty ())
		return (false);

	const size_t writeIndex =
		_writeBusCycleData -> _numberWriteCycles - _pendingRegisterWrites.size ();
	if (cycle != _writeStartCycle +
		(unsigned int) (_writeBusCycleData -> writeCycle (writeIndex)))
		return (false);

	_pendingRegisterWrites.front ().execute ();
	_pendingRegisterWrites.erase (_pendingRegisterWrites.begin ());

	return (true);
}

// ---
bool COMMODORE::VIA::simulate (MCHEmul::CPU* cpu, unsigned int cycles)
{
	assert (cpu != nullptr && cpu == _timedWriteCPU);
	assert (_writeBusCycleData != nullptr);
	assert (_cyclesSimulatedInAdvance == 0 && !_advancedCyclePending);
	assert (_pendingRegisterWrites.empty ());

	for (unsigned int i = 0; i < cycles; ++i)
	{
		_IFDEBUG debugVIACycle (_lastClockCycles);
		// Only the last advanced cycle waits for the instruction's data access.
		if (!simulateVIACycle (cpu, _lastClockCycles, i + 1 != cycles))
			return (false);
		++_lastClockCycles;
		++_cyclesSimulatedInAdvance;
	}

	_advancedCyclePending = (cycles != 0);
	
	return (true);
}

// ---
bool COMMODORE::VIA::simulateVIACycle
	(MCHEmul::CPU* cpu, unsigned int cycle, bool completeCycle)
{
	bool result = true;

	result &= _CA1.simulate (cpu);
	result &= _CA2.simulate (cpu);
	result &= _CB2.simulate (cpu);
	// Resolve PB before T2 samples a possible negative edge on PB6.
	result &= _PB.simulate (cpu);

	VIATimer::CycleResult timer1Result = _T1.simulate (cpu);
	VIATimer::CycleResult timer2Result = _T2.simulate (cpu);
	(void) timer1Result;
	(void) timer2Result;

	result &= _SR.simulate (cpu);
	result &= _CB1.simulate (cpu);
	result &= _PA.simulate (cpu);
	result &= _PB.simulate (cpu);

	// Consume internal transitions before the CPU access can create new ones.
	_CA1.positiveEdge (); _CA1.negativeEdge ();
	_CA2.positiveEdge (); _CA2.negativeEdge ();
	_CB1.positiveEdge (); _CB1.negativeEdge ();
	_CB2.positiveEdge (); _CB2.negativeEdge ();
	_PB.p6Pulse ();

	if (!result)
		return (false);
	if (completeCycle)
		return (finishVIACycle (cpu, cycle, false));
	
	return (true);
}

// ---
bool COMMODORE::VIA::finishVIACycle
	(MCHEmul::CPU* cpu, unsigned int cycle, bool afterCPURead)
{
	bool result = true;
	bool writeApplied = executePendingRegisterWriteAt (cycle);
	if (writeApplied || afterCPURead)
	{
		// The access follows internal simulation. Preserve the pin level while
		// allowing a new peripheral pulse to finish on the following cycle.
		_CA2._justMovedToFalse = false;
		_CB2._justMovedToFalse = false;

		// Publish access effects without repeating timers or control-line work.
		// A new PB6 edge remains available to the following T2 sample.
		result &= _PA.simulate (cpu);
		result &= _PB.simulate (cpu);
	}

	unsigned int interruptReason = _VIARegisters -> reasonIntCode ();
	bool interruptOutputActive = (interruptReason != 0);
	if (interruptOutputActive)
		cpu -> requestInterrupt (_interruptId, cycle, this, (int) interruptReason);
	else if (_interruptOutputActive)
		cpu -> interrupt (_interruptId) -> setNewInterruptRequestAdmitted (false);
	_interruptOutputActive = interruptOutputActive;

	return (result);
}

// ---
void COMMODORE::VIA::debugVIACycle (unsigned int cycle)
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeCompleteLine (className (), cycle, "Info Cycle",
		{ { "PortA", _PA.asString () },
		  { "PortB", _PB.asString () } });
}
