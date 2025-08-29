#include <CORE/CPU.hpp>
#include <CORE/FmterBuilder.hpp>
#include <CORE/Formatter.hpp>
#include <CORE/Stack.hpp>
#include <CORE/LogChannel.hpp>

// ---
MCHEmul::Attributes MCHEmul::CPU::StopStatusData::attributes () const
{
	return (MCHEmul::Attributes (
		{
			{ "When", std::to_string (_cyclesAtStop) },
			{ "CPU", std::to_string (_cyclesAtCPU) },
			{ "Type", (_typeCyclesStopped == MCHEmul::InstructionDefined::_CYCLEALL) 
				? "All" 
				: std::to_string (_typeCyclesStopped) },
			{ "Duration", (_cyclesStopped == -1) 
				? "Forever" 
				: std::to_string (_cyclesStopped) + "(" + std::to_string (_cyclesStopped - _counterCyclesStopped) + " pending)" },
			{ "LI Exec", std::to_string (_cyclesLastInstructionExecutedStopRequest) },
			{ "LI Overlap", std::to_string (_cyclesLastInstructionOverlappedStopRequest) }
		}));
}

// ---
std::string MCHEmul::CPU::StopStatusData::asString () const
{ 
	std::string result;

	bool f = true;
	MCHEmul::Attributes attrs = std::move (attributes ());
	for (const auto& i : attrs)
	{
		result += (!f ? "," : "") + i.first + ("=") + i.second;

		f = false;
	}

	return (result);
}

// ---
std::string MCHEmul::CPU::StopStatusData::asStringCore () const
{
	MCHEmul::Attributes attrs = std::move (attributes ());
	return ("Duration " + (*attrs.find ("Duration")).second + "," +
			"Cycles type " + (*attrs.find ("Type")).second);
}

// ---
MCHEmul::CPU::CPU (int id, const MCHEmul::CPUArchitecture& a, 
	const MCHEmul::Registers& r, const MCHEmul::StatusRegister& sR, const MCHEmul::Instructions& ins,
	const MCHEmul::Attributes& attrs)
	: MCHEmul::MotherboardElement (id, "CPU", attrs),
	  // Basic definition of the CPU...
	  _architecture (a), 
	  _registers (r), 
	  _statusRegister (sR), 
	  _instructions (ins),
	  _programCounter (a.numberBytes ()), 
	  _memory (nullptr), 
	  _interruptSystem (nullptr),
	  // When neither buses nor wires are used, these variable must be set from instruction execution!
	  _lastINOUTAddress (), _lastINOUTData (),
	  // Info while running!
	  _state (MCHEmul::CPU::_EXECUTINGINSTRUCTION),
	  _clockCycles (0), _lastCPUClockCycles (0),
	  _lastState (MCHEmul::CPU::_EXECUTINGINSTRUCTION),
	  _debugLimitsInit (a.numberBytes (), 0), _debugLimitsEnd (a.longestAddressPossible ()), // To debug...
	  _lastInstruction (nullptr),
	  _error (_NOERROR),
	  // Only when executing a instruction/interrupt per cycle...
	  _currentInstruction (nullptr),
	  _currentInterruptRequest (CPUInterruptRequest::_NOINTREQUEST), 
	  _currentInterrupt (nullptr), 
	  _cyclesPendingExecution (0),
	  _stopStatusData (), // When the CPU is stopped. Then same object is reused!
	  // For performance reasons...
	  _rowInstructions () // It will be fulfilled below!
{ 
	assert (_registers.size () > 0 && _instructions.size () > 0); 

	// Put the instrucctions in an array to speed up things later...
	_rowInstructions = MCHEmul::ListOfInstructions  
		((*_instructions.rbegin ()).first + 1 /** the last code plus 1. */, nullptr);
	for (const auto& i : _instructions)
		_rowInstructions [i.first] = i.second;
}

// ---
MCHEmul::CPU::~CPU ()
{
	for (const auto& i : _instructions)
		delete (i.second);

	delete (_interruptSystem);
}

// ---
void MCHEmul::CPU::setStop (bool s, unsigned int tC, unsigned int cC, int nC)
{ 
	// Stop requested...
	if (s)
	{
		// ...the requested cycles to stop should then be 
		// positive or -1 to indicate infinite...
		if (nC == -1 || nC > 0)
		{
			// If the new request were over a still valid previous request...
			// ...it would be admitted, but the situation would be also debugged
			// as it not so common, no?
			if (_state == MCHEmul::CPU::_STOPPED)
			{
				_IFDEBUG debugAlreadyStopped ();
			}
			// Any other case the last state...
			else
				_lastState = _state; // ...is kept to be recovered later...

			_stopStatusData.reset (tC, cC, nC, _clockCycles);

			// If the CPU was running before...
			// it is needed to calculate information about what may cycles of the last 
			// instruction (if any, what should be the normal cincunstance) were executed before receiving this notification
			// and how many are overlapped in it....
			if (_lastState == MCHEmul::CPU::_EXECUTINGINSTRUCTION &&
				_state == MCHEmul::CPU::_EXECUTINGINSTRUCTION)
				_stopStatusData.calcOverlapInfo ( // ...and the status data is updated...
					(_lastInstruction == nullptr) ? 0 : _lastInstruction -> totalClockCyclesExecuted ());

			// The new state is admitted...
			// It could be the same than before, remember!
			_state = MCHEmul::CPU::_STOPPED; 

			_IFDEBUG debugStopRequest ();
		}
	}
	// Run back requested...
	else
	{
		// ...valid only when stopeed previously...
		if (_state == CPU::_STOPPED)
		{ 
			unsigned int iS = _state;	
			
			_state = _lastState; 
			
			_lastState = iS;

			// No longer valid...
			_stopStatusData.reset ();
		}
	}
}

// ---
bool MCHEmul::CPU::initialize ()
{
	if (! MCHEmul::MotherboardElement::initialize ())
		return (false);

	for (auto& i : _registers)
		i.initialize ();

	_statusRegister.initialize ();

	_programCounter.initialize ();

	// The interrupts are loaded here if they didn't before...
	interruptSystem () -> initialize ();

	_lastINOUTAddress = MCHEmul::Address ();
	
	_lastINOUTData = { };

	_state = _lastState = MCHEmul::CPU::_EXECUTINGINSTRUCTION;

	_clockCycles = _lastCPUClockCycles = 0;

	_error = _NOERROR;

	// When executed per cycle...
	_currentInterrupt = nullptr;
	_currentInterruptRequest = MCHEmul::CPUInterruptRequest::_NOINTREQUEST;
	_currentInstruction = nullptr;
	_cyclesPendingExecution = 0;

	// Related with the internal states...
	_lastInstruction = nullptr;

	_stopStatusData.reset ();

	return (true);
}

// ---
bool MCHEmul::CPU::executeNextCycle ()
{
	memoryRef () -> setCPUView (); // Always...

	// If the memory access were buffered... 
	// ...this instruction would free all accesses...
	// but only in the case it was possible...
	// something that depends on the type of CPU!
	if (unbufferCommands ())
		MCHEmul::Memory::configuration ().executeMemorySetCommandsBuffered ();

	bool result;
	switch (_state)
	{
		case MCHEmul::CPU::_EXECUTINGINSTRUCTION:
			result = when_ExecutingInstruction ();
			break;
		
		case MCHEmul::CPU::_STOPPED:
			result = when_Stopped ();
			break;

		default:
			{ 
				// It shouldn't be here. 
				// It would mean that there are states with no treatment...
				_LOG ("CPU State not allowed");
				assert (false); 
			}

			break;
	}

	return (result);
}

// ---
MCHEmul::InfoStructure MCHEmul::CPU::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::MotherboardElement::getInfoStructure ());

	result.add ("CPUInterruptSystem", std::move (interruptSystem () -> getInfoStructure ()));
	result.add ("Architecture", std::move (_architecture.getInfoStructure ()));

	MCHEmul::InfoStructure regs;
	for (const auto& i : internalRegisters ())
		regs.add (i.name (), std::move (i.asString ()));
	result.add ("REGS",	std::move (regs));

	result.add ("PC",		std::move (programCounter ().asString ()));
	result.add ("SR",		std::move (statusRegister ().asString ()));
	result.add ("CLK",		std::move (std::to_string (_clockCycles)));
	result.add ("STATE",	std::to_string ((int) _state));
	result.add ("CLKSTOP",	std::to_string (cyclesRemainStopped ()));

	return (result);
}

// ---
bool MCHEmul::CPU::when_ExecutingInstruction ()
{
	assert (_state == MCHEmul::CPU::_EXECUTINGINSTRUCTION);

	_IFDEBUG debugLastExecutionData ();

	auto execFunction = [&](bool f) -> bool
		{
			_lastCPUClockCycles = f ? 0 /** the full execution will decide. */ : 1 /** minimu step. */;
	
			bool result = false;
			// The activation of the interruptions has priority over the execution of the instructions...
			if (f ? executeNextInterruptRequest_Full (_error) 
				  : executeNextInterruptRequest_PerCycle (_error)) // ...if any!
				result = (_error != MCHEmul::_NOERROR)
					? false /** Meaning a mistake in the execution. */ : true /** all right. */;
			else
			{ 
				// If no interrupt was executed...
				// ...it is time for the instructions, just if there was no previous error (even not executing)...
				if (_error != MCHEmul::_NOERROR)
					result = false;
				else
				{
					if (f ? executeNextInstruction_Full (_error) 
						  : executeNextInstruction_PerCycle (_error))
						result = (_error != MCHEmul::_NOERROR) 
							? false /** Meaning a error in the "no" execution. */ : true /** all right. */;
					// But if no instruction could be executed either...the CPU has a big issue!
					// Because at this moment there wouldn't be neither instruction not interrupt to execute
					// and the CPU is the mode EXECUTING!...
					else
					{
						_error = MCHEmul::_CPU_ERROR;

						result = false; 
					}
				}
			}

			// The total clock is also actualized...
			// according with what happend within the methods...
			_clockCycles += _lastCPUClockCycles;

			return (result);
		};

	bool result = false;
	if (ticksCounter () == nullptr)
		result = execFunction (true); // Full...
	else
		for (unsigned int i = ticksCounter () -> elapsedTicks (); i > 0; i--)
			result = execFunction (false); // Per cycle...

	return (result);
}

// ---
bool MCHEmul::CPU::when_Stopped ()
{
	assert (_state == MCHEmul::CPU::_STOPPED);

	// _stopStatusData.run () returns false when no more cycles valid...
	// Important: This instruction actualize the number of cycles pending...
	// This is the reason to do the code in this way: 
	// to reflect in the debug file the number of cycles pending!
	bool f = !_stopStatusData.run ();
	_IFDEBUG debugStopSituation ();
	if (f) 
	{
		unsigned int iS = _state;

		_state = _lastState;

		_lastState = iS;

		// No longer valid...
		_stopStatusData.reset ();
	}

	// No other value of this status is changed, because everything has to continue 
	// at the situation it had when stopped, 
	// either running a instruction or launching a interruption...

	_clockCycles++;

	return (true);
}

// ---
bool MCHEmul::CPU::executeNextInterruptRequest_PerCycle (unsigned int& e)
{
	if (_currentInstruction != nullptr)
		return (false); // It is the the turn of the instructions instead!
						// Why is this here?...

	// ...processed if nothing against were indicated later...
	bool result = true; 
	// Is there any interrupt already being processed?
	if (_currentInterrupt != nullptr)
	{
		_lastCPUClockCycles++;
		if (--_cyclesPendingExecution == 0) // Time to launch it?
		{
			_IFDEBUG debugInterruptLaunched ();

			if (_currentInterrupt ->
				executeOver (this, _currentInterruptRequest.cycles () /** at the moment if was requested. */))
			{
				_lastCPUClockCycles += _currentInterrupt -> cycledAfterLaunch (); // Most of the times = 0

				removeInterruptRequest (_currentInterruptRequest); // Done...
				_currentInterruptRequest = MCHEmul::CPUInterruptRequest::_NOINTREQUEST; // ..no longer valid...

				_currentInterrupt -> initialize (); // Get ready for the next interrupt...
				_currentInterrupt = nullptr; // No longer valid...
			}
			else
			{
				e = MCHEmul::_INTERRUPT_ERROR; // ...error although it's been executed (result = true)...

				_IFDEBUG debugInterruptFails ();
			}
		}
	}
	// There is no any interrupt being processed...
	// so it is time to select a new one...
	else
	{
		const MCHEmul::CPUInterruptRequest& iR = getNextInterruptRequest ();
		if (iR.type () == CPUInterruptRequest::_NOINTREQUEST.type ())
			result = false; // not processed, 
							// but with no errors, maybe time for the instructions...
		else
		{
			CPUInterrupt* interr = getInterruptForRequest (iR);
			switch (interr -> canBeExecutedOver (this, iR.cycles ()))
			{
				case CPUInterrupt::_EXECUTIONNOTALLOWED:
					{
						_IFDEBUG debugInterruptRequestNotAllowed (iR);

						removeInterruptRequest (iR);

						result = false; // not executed, but with no errors...
					}
			
					break;

				case CPUInterrupt::_EXECUTIONTOWAIT:
					{
						_IFDEBUG debugInterruptRequestToWait (iR);

						// It is not removed from the pending list...
						// Because it is valid but not still the time to be executed!
						// So in the next loop the request will be still there...

						result = false; // not executed, but with no errors...
					}

					break;

				case CPUInterrupt::_EXECUTIONALLOWED:
					{
						// The acknowledge has to be issued!
						aknowledgeInterrupt ();

						_currentInterruptRequest = iR;
						_currentInterrupt = interr;

						_IFDEBUG debugInterruptLaunched ();

						_cyclesPendingExecution = _currentInterrupt -> cyclesToLaunch ();
		
						// The request will be deleted once the interrupt was executed (see above)...

						//... to start the execution with no errors...
						// ...they might come later!! (when the real execution happens)
					}

					break;

				// It shouldn't be here, just in case...
				default:
					{
						_LOG ("CPU Interrupt status not supported:" + 
							std::to_string (iR.type ()));
						assert (false);
					}

					break;
			}
		}
	}

	return (result);
}

// ---
bool MCHEmul::CPU::executeNextInterruptRequest_Full (unsigned int& e)
{
	const MCHEmul::CPUInterruptRequest& iR = getNextInterruptRequest ();
	if (iR.type () == CPUInterruptRequest::_NOINTREQUEST.type ())
		return (false); // not executed, it might by time for the instruction...

	// To indicate whether the transaction is executed or not...
	// Nothing to do with the value of the variable e (error), received as a reference!
	bool result = true; // ...executed if nothing is said against later...
	_currentInterrupt = getInterruptForRequest (iR);
	switch (_currentInterrupt -> canBeExecutedOver (this, iR.cycles ()))
	{
		// The interrupt can not be executed...
		case CPUInterrupt::_EXECUTIONNOTALLOWED:
			{
				_IFDEBUG debugInterruptRequestNotAllowed (iR);

				removeInterruptRequest (iR); // No longer valid...

				result = false; // ...not executed, but with no errors...
			}

			break;

		// The interrupt is valid but has to wait to another interation
		// This can happen when the CPU takes certain time to recognize the interrupt.
		// For example in 6500 family the no interrupt can be launched in the last instruction took less than 2 cycles
		case CPUInterrupt::_EXECUTIONTOWAIT:
			{
				_IFDEBUG debugInterruptRequestToWait (iR);

				// The request is not removed as it is still valid...
				// But not in this moment, in the next loop the CPU will get it back!

				result = false; // no errors, no executed...
			}

			break;

		// Time to execute the interrupt...
		case CPUInterrupt::_EXECUTIONALLOWED:
			{
				_IFDEBUG debugInterruptLaunched ();

				// The acknowledge has to be issued!
				aknowledgeInterrupt ();

				_lastCPUClockCycles = _currentInterrupt -> cyclesToLaunch ();
				if (_currentInterrupt -> executeOver (this, iR.cycles ())) // Thismethod returns true when ok, and false with errors...
					_lastCPUClockCycles += _currentInterrupt -> cycledAfterLaunch ();
				else
				{ 
					_error = MCHEmul::_INTERRUPT_ERROR; // and error happended...

					_IFDEBUG debugInterruptFails ();
				}

				_currentInterrupt -> initialize (); // Get ready for the next interrupt...
				_currentInterrupt = nullptr;

				removeInterruptRequest (iR); // Done!

				// executed (return = true)...but with errors? (see above)
			}

			break;

		default:
			{
				_LOG ("CPU Interrupt status not supported");
				assert (false);
			}

			break;
	}

	return (result);
}

// ---
bool MCHEmul::CPU::executeNextInstruction_PerCycle (unsigned int& e)
{
	if (_currentInterrupt != nullptr)
		return (false); // I guess this is going to generate a big issue!! 
						// (becuase this situation shouldn't happen)

	assert (_memory != nullptr);

	// By default, 
	// the instruction cycle should be processed always at this point!
	// ...but with errors? (see below)

	if (_currentInstruction != nullptr)
	{
		// ...but only when the cycles pending are 0!
		if (--_cyclesPendingExecution == 0)
		{ 
			std::string sdd = "";
			_IFDEBUG { 
				sdd = MCHEmul::removeAll0 (_programCounter.asString ()) + "(Stack "
					+ std::to_string (memoryRef () -> stack () -> position ()) + ")"; }

			// The execution of the instruction is notified
			// just in case any other part of the computer needed to prepare something...
			notify (MCHEmul::Event (_CPUTOEXECUTEINSTRUCTION, 0 /** No sense. */,
				std::shared_ptr <MCHEmul::Event::Data> (new MCHEmul::CPU::EventData (_currentInstruction))));

			// Finally executed the instruction...
			// This method returns true when everything ok and false if not...
			if (!_currentInstruction -> 
				execute (this, _memory, _memory -> stack (), &_programCounter))
			{
				_error = MCHEmul::_INSTRUCTION_ERROR; // ...error executing...(return = true)

				_IFDEBUG debugInstructionFails ();
			}
			// when ther eis no errors executing the transaction
			// ..the rest of the calculus are done...
			else
			{
				_lastCPUClockCycles += _currentInstruction -> additionalClockCyclesExecuted (); // Just in case...

				_lastInstruction = _currentInstruction; // save the last instruction executed...

				_currentInstruction = nullptr; // another one is needed...

				_lastState = _state; // After one instruction executed, the last state was also running...

				_IFDEBUG debugInstructionExecuted (sdd);
			}
		}
		else
			_IFDEBUG debugInstructionWaiting ();
	}
	else
	{
		// ...look for the next instruction to be executed...
		unsigned int nInst = 
			MCHEmul::UInt (
				_memory -> values (
					programCounter ().asAddress (), architecture ().instructionLength ()), 
				architecture ().bigEndian ()).asUnsignedInt ();
		if (nInst < _rowInstructions.size () && 
			(_currentInstruction = _rowInstructions [nInst]) != nullptr)
			// The initial ones, 
			// but there might be some additional cycles after execution...
			// The cycles are decided according with the right instruction...
			_cyclesPendingExecution = 
				_currentInstruction -> clockCycles (_memory, _programCounter.asAddress ()); // virtual!
		else
		{
			_error = MCHEmul::_INSTRUCTION_ERROR; // executed but with an error, the instruction is not found...

			_IFDEBUG debugInstructionNoExists (nInst);
		}
	}

	return (true);
}

// ---
bool MCHEmul::CPU::executeNextInstruction_Full (unsigned int &e)
{
	assert (_memory != nullptr);

	// By default, 
	// the instruction cycle should be processed always at this point!
	// ...but with errors? (see below)

	// Access the next instruction...
	// Using the row description of the instructions!
	unsigned int nInst = 
		MCHEmul::UInt (
			_memory -> values (
				programCounter ().asAddress (), architecture ().instructionLength ()), 
			architecture ().bigEndian ()).asUnsignedInt ();
	// If the instruction doesn't exist according with what is indicated in the memory of 
	// the computer, and an error is generated...
	MCHEmul::Instruction* inst = nullptr;
	if (nInst >= _rowInstructions.size () || 
		(inst = _rowInstructions [nInst]) == nullptr)
	{
		e = MCHEmul::_INSTRUCTION_ERROR;

		_IFDEBUG debugInstructionNoExists (nInst);

		return (true); // ...executed but with errors!
	}

	std::string sdd = "";
	_IFDEBUG {
		sdd = MCHEmul::removeAll0 (_programCounter.asString ()) + "(Stack "
			+ std::to_string (memoryRef () -> stack () -> position ()) + ")"; }

	// The execution of the instruction is notified
	// just in case any other part of the computer needed to prepare something...
	notify (MCHEmul::Event (_CPUTOEXECUTEINSTRUCTION, 0 /** No sense. */,
		std::shared_ptr <MCHEmul::Event::Data> (new MCHEmul::CPU::EventData (inst))));

	// Finally executed the instruction...
	// This method returns true when everything ok and false if not...
	if ((_currentInstruction = inst) -> 
			execute (this, _memory, _memory -> stack (), &_programCounter))
	{
		// Actualize the clock cycles used by the last instruction...
		_lastCPUClockCycles = inst -> clockCyclesExecuted () + 
							  inst -> additionalClockCyclesExecuted ();
		// ...and finally saves the instruction executed...
		_lastInstruction = inst;

		// After one instruction executed, the last state was also running...
		_lastState = _state; 

		_IFDEBUG debugInstructionExecuted (sdd);
	}
	// ...otherwise again an error is generated...
	else
	{
		e = MCHEmul::_INSTRUCTION_ERROR;

		_IFDEBUG debugInstructionFails ();
	}

	// No longer valid...
	// The use of _currentInstruction was only for the debug info...
	_currentInstruction = nullptr;

	return (true);
}

// ---
MCHEmul::Instruction* MCHEmul::CPU::instructionAt 
	(MCHEmul::Memory* m, const MCHEmul::Address& addr, MCHEmul::CPU* c)
{
	assert (c != nullptr &&
			m != nullptr);

	MCHEmul::Instruction* inst = nullptr;
	unsigned int nInst = 
		MCHEmul::UInt (
			m -> values (
				addr, c -> architecture ().instructionLength ()), 
			c -> architecture ().bigEndian ()).asUnsignedInt ();
	if (nInst >= c -> _rowInstructions.size () ||
		(inst = c -> _rowInstructions [nInst]) == nullptr)
		return (nullptr); // Not possible to define the instruction...

	return ((inst -> defineInstructionFrom (m, addr)) ? inst : nullptr);
}

// ---
void MCHEmul::CPU::debugStopRequest () const
{
	assert (_deepDebugFile != nullptr);

	if (_programCounter < _debugLimitsInit ||
		_programCounter > _debugLimitsEnd)
		return; // Nothing to do...

	_deepDebugFile -> writeLineData 
		("New stop request. " + _stopStatusData.asStringCore ());
}

// ---
void MCHEmul::CPU::debugInterruptRequest (const MCHEmul::CPUInterruptRequest& iR) const
{ 
	assert (_deepDebugFile != nullptr);

	if (_programCounter < _debugLimitsInit ||
		_programCounter > _debugLimitsEnd)
		return; // Nothing to do...

	_deepDebugFile -> writeLineData ("Interrupt CPU requested " + iR.toString ()); 
}

// ---
void MCHEmul::CPU::debugLastExecutionData () const
{ 
	assert (_deepDebugFile != nullptr &&
			_lastInstruction != nullptr);

	if (_programCounter < _debugLimitsInit ||
		_programCounter > _debugLimitsEnd)
		return; // Nothing to do...

	_deepDebugFile -> writeCompleteLine ("CPU", _clockCycles, "Info Cycle", 
		{ "Last " + ((_lastInstruction == nullptr)
			? "0" 
			: std::to_string (_lastInstruction -> clockCyclesExecuted () + 
			  _lastInstruction -> additionalClockCyclesExecuted ())) + " cycles" }); 
}

// ---
void MCHEmul::CPU::debugStopSituation () const
{
	assert (_deepDebugFile != nullptr);

	if (_programCounter < _debugLimitsInit ||
		_programCounter > _debugLimitsEnd)
		return; // Nothing to do...

	_deepDebugFile -> writeCompleteLine ("CPU", _clockCycles, "Stopped", 
		_stopStatusData.attributes ());
	_deepDebugFile -> writeLineData	("State:" + std::to_string (_state) + 
		", Last State:" + std::to_string (_lastState));
}

// ---
void MCHEmul::CPU::debugAlreadyStopped () const
{
	assert (_deepDebugFile != nullptr);

	if (_programCounter < _debugLimitsInit ||
		_programCounter > _debugLimitsEnd)
		return; // Nothing to do...

	_deepDebugFile -> writeLineData (
		"Already stopped. " + std::to_string (_stopStatusData.cyclesStillValid ()) + " cycles pending");
}

// ---
void MCHEmul::CPU::debugInterruptLaunched () const
{
	assert (_deepDebugFile != nullptr &&
			_currentInterrupt != nullptr);

	if (_programCounter < _debugLimitsInit ||
		_programCounter > _debugLimitsEnd)
		return; // Nothing to do...

	_deepDebugFile -> writeLineData ("Interrupt launched type " + 
		std::to_string (_currentInterrupt -> id ()));
}

// ---
void MCHEmul::CPU::debugInterruptFails () const
{
	assert (_deepDebugFile != nullptr &&
			_currentInterrupt != nullptr);

	if (_programCounter < _debugLimitsInit ||
		_programCounter > _debugLimitsEnd)
		return; // Nothing to do...

	_deepDebugFile -> writeLineData ("Error launching Interrupt type " + 
		std::to_string (_currentInterrupt -> id ()));
}

// ---
void MCHEmul::CPU::debugInterruptRequestNotAllowed (const MCHEmul::CPUInterruptRequest& iR) const
{
	assert (_deepDebugFile != nullptr);

	if (_programCounter < _debugLimitsInit ||
		_programCounter > _debugLimitsEnd)
		return; // Nothing to do...

	_deepDebugFile -> writeLineData ("Interrupt request not allowed " + iR.toString ());
}

// ---
void MCHEmul::CPU::debugInterruptRequestToWait (const MCHEmul::CPUInterruptRequest& iR) const
{
	assert (_deepDebugFile != nullptr);

	if (_programCounter < _debugLimitsInit ||
		_programCounter > _debugLimitsEnd)
		return; // Nothing to do...

	_deepDebugFile -> writeLineData ("Interrupt request to wait " + iR.toString ());
}

// ---
void MCHEmul::CPU::debugInstructionFails () const
{
	assert (_deepDebugFile != nullptr &&
			_currentInstruction != nullptr);

	if (_programCounter < _debugLimitsInit ||
		_programCounter > _debugLimitsEnd)
		return; // Nothing to do...

	_deepDebugFile -> writeLineData ("Error executing instruction " + 
		_currentInstruction -> asString ());
}

// ---
void MCHEmul::CPU::debugInstructionExecuted (const std::string& sdd) const
{
	assert (_deepDebugFile != nullptr &&
			_lastInstruction != nullptr);

	if (_programCounter < _debugLimitsInit ||
		_programCounter > _debugLimitsEnd)
		return; // Nothing to do...

	std::string lSt = _lastInstruction -> asString (); 
	std::string lStA = 
		((_lastInstruction -> INOUTAddress ().value () != 0)
			? "$" + MCHEmul::removeAll0 // Was there address?
				(_lastInstruction -> INOUTAddress ().asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2))
			: "");
	std::string lStB = 
		((_lastInstruction -> INOUTData ().size () != 0)
			? "$" + MCHEmul::removeAll0 // Was there data used?
				(_lastInstruction -> INOUTData ().asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2))
			: "");
	std::string lStAB = lStA + ((lStA == "") ? "" : ((lStB == "") ? "" : ",")) + lStB;
	lStAB = ((lStAB != "") ? "(" : "") + lStAB + ((lStAB != "") ? ")" : "");
	std::string rInfo; 
	for (const auto& i : _registers) 
		rInfo += i.asString () + " "; // The last space is lost...

	_deepDebugFile -> writeLineData (sdd + " " + lSt + " " + lStAB);
	// ...The program counter and the stack position + The last in(struction + Address bus and Data bus ...if used!
	_deepDebugFile -> writeLineData	(MCHEmul::Strings
		({ "State:" + std::to_string (_state) + ", Last State:" + std::to_string (_lastState),
		   _statusRegister.asString (), // ...The status register...
		   rInfo })); // The register info...
}

// ---
void MCHEmul::CPU::debugInstructionWaiting () const
{
	assert (_deepDebugFile != nullptr);

	if (_programCounter < _debugLimitsInit &&
		_programCounter > _debugLimitsEnd)
		return; // Nothing to do...

	_deepDebugFile -> writeCompleteLine ("CPU", _clockCycles, "Info Cycle", 
		{ "Instruction in " + std::to_string (_cyclesPendingExecution) + " cycles" });
}

// ---
void MCHEmul::CPU::debugInstructionNoExists (unsigned int nI)
{
	assert (_deepDebugFile != nullptr);

	if (_programCounter < _debugLimitsInit &&
		_programCounter > _debugLimitsEnd)
		return; // Nothing to do...

	_deepDebugFile -> writeLineData ("Instruction doesn't exist " + std::to_string (nI));
}
