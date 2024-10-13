#include <CORE/CPU.hpp>
#include <CORE/FmterBuilder.hpp>
#include <CORE/Formatter.hpp>
#include <CORE/Stack.hpp>
#include <iostream>

// Only the first parameter cares...the rest don't!
const MCHEmul::CPU::InterruptRequest MCHEmul::CPU::_NOINTREQUEST = 
	{ -1 /** not use this id ever for a real transaction type */, 0, nullptr, -1 }; 

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
	  _interrupts (),
	  _hooks (),
	  // When neither buses nor wires are used, these variable must be set from instruction execution!
	  _lastINOUTAddress (), _lastINOUTData (),
	  // Info while running!
	  _state (MCHEmul::CPU::_EXECUTINGINSTRUCTION),
	  _clockCycles (0), _lastCPUClockCycles (0),
	  _lastState (MCHEmul::CPU::_EXECUTINGINSTRUCTION),
	  _lastInstruction (nullptr),
	  _deepDebugFile (nullptr),
	  _error (_NOERROR),
	  // Only when executing a instruction/interrupt per cycle...
	  _currentInstruction (nullptr), _currentInterruptRequest (_NOINTREQUEST), 
	  _currentInterrupt (nullptr), _cyclesPendingExecution (0), 
	  // When the CPU is stopped...
	  _typeCycleStopped (0 /** nothing. */), _cyclesStopped (0), _cyclesAtStop (0), _counterCyclesStopped (0),
	  // For performance reasons...
	  _rowInstructions (), // It will be fulfilled below!
	  _rowInterrupts () // Just to be completed using @see makeInterruptionRowData after filling up _interrupts
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

	for (const auto& i : _interrupts)
		delete (i.second);

	for (const auto& i : _hooks)
		delete (i.second);
}

// ---
void MCHEmul::CPU::setStop (bool s, unsigned int tC, unsigned int cC, int nC)
{ 
	// Stop requested...
	if (s)
	{
		if (deepDebugActive ())
			*_deepDebugFile 
				<< "\t\t\t\t\tStop CPU requested:" << std::to_string (nC) 
				<< " cycles(" << ((tC == std::numeric_limits <unsigned int>::max ()) ? "-" : std::to_string (tC)) << " type)\n";

		// If the CPU was already stopped...
		// the counter starts back, but neither the state changes
		// nor the point where the stop started...
		if (_state == CPU::_STOPPED)
		{ 
			_typeCycleStopped = tC;

			_cyclesStopped = nC; 
			_counterCyclesStopped = 0; 
		}
		else
		{
			// If not already stopped, 
			// the requested cycles to stop should be positive or -1 to indicate infinite...
			if (nC == -1 || nC > 0)
			{
				_lastState = _state;

				_state = CPU::_STOPPED; 

				_typeCycleStopped = tC;
			
				_cyclesAtStop = cC;

				_cyclesStopped = nC; 
				_counterCyclesStopped = 0; 
			}
			// ...otherwise the CPU doesn't stop.
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

	for (auto& i : _interrupts)
		i.second -> initialize ();

	_lastINOUTAddress = MCHEmul::Address ();
	
	_lastINOUTData = { };

	_state = _lastState = MCHEmul::CPU::_EXECUTINGINSTRUCTION;

	_clockCycles = _lastCPUClockCycles = 0;

	_error = _NOERROR;

	// When executed per cycle...
	_currentInterrupt = nullptr;
	_currentInterruptRequest = MCHEmul::CPU::_NOINTREQUEST;
	_currentInstruction = nullptr;
	_cyclesPendingExecution = 0;

	// Related with the internal states...
	_lastInstruction = nullptr;
	_typeCycleStopped = 0; // Meaning none
	_cyclesStopped = 0;
	_cyclesAtStop = _counterCyclesStopped = 0;

	_interruptsRequested = { };

	return (true);
}

// ---
void MCHEmul::CPU::addInterrupt (MCHEmul::CPUInterrupt* in)
{
	assert (in != nullptr);

	if (_interrupts.find (in -> id ()) != _interrupts.end ())
		return; // Only one with the the same id...

	_interrupts.insert (MCHEmul::CPUInterrupts::value_type (in -> id (), in));

	makeInterruptionRowData ();
}

// ---
void MCHEmul::CPU::removeInterrrupt (int id)
{
	MCHEmul::CPUInterrupts::const_iterator i;
	if ((i = _interrupts.find (id)) == _interrupts.end ())
		return;

	_interrupts.erase (i);

	makeInterruptionRowData ();
}

// ---
void MCHEmul::CPU::requestInterrupt (const MCHEmul::CPU::InterruptRequest& iR)
{ 
	// Only one interrupt at the same time...
	// or with more priority that the one that could be already pending to be processed...
	if ((_interruptsRequested.empty () || 
		 (!_interruptsRequested.empty () && 
			 _rowInterrupts [iR._type] -> priority () > 
			 _rowInterrupts [(*_interruptsRequested.begin ())._type] -> priority ())) &&
	// ...and also wether to admit a new interrupt of the same type is possible...
		_rowInterrupts [iR._type] -> admitNewInterruptRequest ()) 
	{ 
		// Insert the new one at the beginning of the vector always...
		// But if any other interrupt were already in with lower priority it would be maintained...
		_interruptsRequested.insert (_interruptsRequested.begin (), iR);

		// The new interrupt has been admittted...
		_rowInterrupts [iR._type] -> setNewInterruptRequestAdmitted (true);

		if (deepDebugActive ())
			*_deepDebugFile << "\t\t\t\t\tInterrupt CPU requested:" 
							<< iR.toString () << "\n";
	} 
}

// ---
bool MCHEmul::CPU::executeNextCycle ()
{
	memoryRef () -> setCPUView (); // Always...

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
			// It shouldn't be here. 
			// It would mean that there are states with no treatment...
			assert (0); 
			break;
	}

	return (result);
}

// ---
MCHEmul::InfoStructure MCHEmul::CPU::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::MotherboardElement::getInfoStructure ());

	MCHEmul::InfoStructure intr;
	for (const auto& i : _interrupts)
		intr.add (std::to_string (i.second -> id ()), std::move (i.second -> getInfoStructure ()));
	result.add ("INTERRUPTS", std::move (intr));

	result.add ("Architecture", std::move (_architecture.getInfoStructure ()));

	MCHEmul::InfoStructure regs;
	for (const auto& i : internalRegisters ())
		regs.add (i.name (), std::move (i.asString ()));
	result.add ("REGS",	std::move (regs));

	result.add ("PC",  std::move (programCounter ().asString ()));
	result.add ("SR",  std::move (statusRegister ().asString ()));
	result.add ("CLK", std::move (std::to_string (_clockCycles)));

	return (result);
}

// ---
void MCHEmul::CPU::makeInterruptionRowData ()
{
	_rowInterrupts.clear ();

	// Put the interruptions in an array to speed up things later...
	_rowInterrupts = MCHEmul::CPUListOfInterrupts
		((*_interrupts.rbegin ()).second -> id () + 1 /** the last code plus 1. */, nullptr);
	for (const auto& i : _interrupts)
		_rowInterrupts [i.second -> id ()] = i.second;
}

// ---
bool MCHEmul::CPU::when_ExecutingInstruction ()
{
	assert (_state == MCHEmul::CPU::_EXECUTINGINSTRUCTION);

	if (deepDebugActive ())
		*_deepDebugFile
		// Where
		<< "CPU\t\t"
		// When
		<< MCHEmul::fixLenStr (std::to_string (_clockCycles), 12, false)
		// What
		<< "Info Cycle\t\t"
		// Data
		<< "Last:" + ((_lastInstruction == nullptr)
			? "0" : std::to_string 
				(_lastInstruction -> clockCyclesExecuted () + 
				 _lastInstruction -> additionalClockCyclesExecuted ())) + " cycles\n";

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

	if (deepDebugActive ())
		*_deepDebugFile
		// Where
		<< "CPU\t\t"
		// When
		<< MCHEmul::fixLenStr (std::to_string (_clockCycles), 12, false)
		// What
		<< "Stopped\t\t"
		// Data
		<< ((_cyclesStopped == -1) ? "-" : std::to_string (_counterCyclesStopped)) << " cycle\n";

	if (_cyclesStopped != -1 &&
		(++_counterCyclesStopped == (unsigned int) _cyclesStopped))
	{
		unsigned int iS = _state;

		_state = _lastState;

		_lastState = iS;
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
			if (deepDebugActive ())
				*_deepDebugFile << "\t\t\t\t\tInterrupt launched:"
				<< std::to_string (_currentInterrupt -> id ()) << "\n";

			if (_currentInterrupt ->
				executeOver (this, _currentInterruptRequest._cycles /** at the moment if was requested. */))
			{
				_lastCPUClockCycles += _currentInterrupt -> cycledAfterLaunch (); // Most of the times = 0
	
				removeInterruptRequest (_currentInterruptRequest); // Done...

				_currentInterruptRequest = MCHEmul::CPU::_NOINTREQUEST; // ..no longer valid...

				_currentInterrupt -> initialize (); // Get ready for the next interrupt...

				_currentInterrupt = nullptr; // No longer valid...
			}
			else
				e = MCHEmul::_INTERRUPT_ERROR; // ...error although it's been executed...
		}
	}
	else
	{
		const MCHEmul::CPU::InterruptRequest& iR = getNextInterruptRequest ();
		assert (iR._type == -1 || (size_t) iR._type < _rowInterrupts.size ());
		if (iR._type == -1)
			result = false; // not processed, 
							// but with no errors, maybe time for the instructions...
		else
		{
			switch (iR._type)
			{
				case CPUInterrupt::_EXECUTIONNOTALLOWED:
				{
					if (deepDebugActive ())
						*_deepDebugFile << "\t\t\t\t\tInterrupt not allowed:" 
										<< std::to_string (_currentInterrupt -> id ()) << "\n";

					removeInterruptRequest (iR);

					result = false; // not executed, but with no errors...
				}
			
				break;

				case CPUInterrupt::_EXECUTIONTOWAIT:
				{
					if (deepDebugActive ())
						*_deepDebugFile << "\t\t\t\t\tInterrupt to wait:" 
										<< std::to_string (_currentInterrupt -> id ()) << "\n";

					// It is not removed from the pending list...
					// Because it is valid but not still the time to be executed!
					// So in the next loop the request will be still there...

					result = false; // not executed, but with no errors...
				}

				break;

				case CPUInterrupt::_EXECUTIONALLOWED:
				{
					if (deepDebugActive ())
						*_deepDebugFile << "\t\t\t\t\tInterrupt launched:" 
										<< std::to_string (_currentInterrupt -> id ()) << "\n";

					// The acknowledge has to be issued!
					aknowledgeInterrupt ();

					_currentInterruptRequest = iR;

					_currentInterrupt = _rowInterrupts [(size_t) _currentInterruptRequest._type];

					_cyclesPendingExecution = _currentInterrupt -> cyclesToLaunch ();
		
					// The request will be deleted once the interrupt was executed (see above)...

					//... to start the execution with no errors...
					// ...they might come later!! (when the real execution happens)
				}

				break;

				// It shouldn't be here, just in case...
				default:
					assert (false);
					break;
			}
		}
	}

	return (result);
}

// ---
bool MCHEmul::CPU::executeNextInterruptRequest_Full (unsigned int& e)
{
	const MCHEmul::CPU::InterruptRequest& iR = getNextInterruptRequest ();
	assert (iR._type == -1 || (size_t) iR._type < _rowInterrupts.size ());
	if (iR._type == -1)
		return (false); // not executed, it might by time for the instruction...

	// Notice that _currentInterrupt is not used but i instead (on purpose!)...
	// ...as there is no needed to maintain the status between cpu loops...

	// To indicate whether the transaction is executed or not...
	// Nothing to do with the value of the variable e (error), received as a reference!
	bool result = true; // ...executed if nothing is said against later...
	MCHEmul::CPUInterrupt* i = _rowInterrupts [iR._type];
	switch (i -> canBeExecutedOver (this, iR._cycles))
	{
		// The interrupt can not be executed...
		case CPUInterrupt::_EXECUTIONNOTALLOWED:
		{
			if (deepDebugActive ())
				*_deepDebugFile << "\t\t\t\t\tInterrupt not allowed:" 
								<< std::to_string (i -> id ()) << "\n";

			removeInterruptRequest (iR); // No longer valid...

			result = false; // ...not executed, but with no errors...
		}

		break;

		// The interrupt is valid but has to wait to another interation
		// This can happen when the CPU takes certain time to recognize the interrupt.
		// For example in 6500 family the no interrupt can be launched in the last instruction took less than 2 cycles
		case CPUInterrupt::_EXECUTIONTOWAIT:
		{
			if (deepDebugActive ())
				*_deepDebugFile << "\t\t\t\t\tInterrupt to wait:" 
								<< std::to_string (i -> id ()) << "\n";

			// The request is not removed as it is still valid...
			// But not in this moment, in the next loop the CPU will get it back!

			result = false; // no errors, no executed...
		}

		break;

		// Time to execute the transaction...
		case CPUInterrupt::_EXECUTIONALLOWED:
		{
			if (deepDebugActive ())
				*_deepDebugFile << "\t\t\t\t\tInterrupt launched:" 
								<< std::to_string (i -> id ()) << "\n";

			// The acknowledge has to be issued!
			aknowledgeInterrupt ();

			_lastCPUClockCycles = i -> cyclesToLaunch ();
			if (i -> executeOver (this, iR._cycles)) // Thismethod returns true when ok, and false with errors...
				_lastCPUClockCycles += i -> cycledAfterLaunch ();
			else
				_error = MCHEmul::_INTERRUPT_ERROR; // and error happended...

			i -> initialize (); // Get ready for the next interrupt...

			removeInterruptRequest (iR); // Done!

			// executed (return = true)...but with errors? (see above)
		}

		break;

		default:
			// It shouldn't be here, just in case...
			assert (false);
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
			if (deepDebugActive ())
				sdd = MCHEmul::removeAll0 (_programCounter.asString ()) + "(Stack "
						+ std::to_string (memoryRef () -> stack () -> position ()) + ") ";

			// If the memory access were buffered, this instruction would free all accesses...
			MCHEmul::Memory::configuration ().executeMemorySetCommandsBuffered ();

			// The execution of the instruction is notified
			// just in case any other part of the computer needed to prepare something...
			notify (MCHEmul::Event (_CPUTOEXECUTEINSTRUCTION, 0 /** No sense. */,
				std::shared_ptr <MCHEmul::Event::Data> (new MCHEmul::CPU::EventData (_currentInstruction))));

			// Finally executed the instruction...
			// This method returns true when everything ok and false if not...
			if (_currentInstruction -> 
				execute (this, _memory, _memory -> stack (), &_programCounter)) 
				_error = MCHEmul::_INSTRUCTION_ERROR; // ...error executing...

			_lastCPUClockCycles += _currentInstruction -> additionalClockCyclesExecuted (); // Just in case...

			_lastInstruction = _currentInstruction; // save the last instruction executed...

			_currentInstruction = nullptr; // another one is needed...

			// executed (return = true)...but with error? (see above)

			if (deepDebugActive ())
			{
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
				*_deepDebugFile
						<< "\t\t\t\t\t"
						<< sdd // ...The program counter and the stack position...
						<< lSt << MCHEmul::_SPACES.substr (0, 15 - lSt.length ()) << "\t" // The last instruction...
						<< lStAB << MCHEmul::_SPACES.substr (0, 15 - lStAB.length ()) << "\t" // Address bus and Data bus ...if used!
						<< _statusRegister.asString () << "\t"; // ...The status register...
				for (const auto& i : _registers) 
					*_deepDebugFile 
						<< " " << i.asString () << " "; // ...and the registers info.
				*_deepDebugFile << "\n";
			}
		}
		else
		{
			if (deepDebugActive ())
				*_deepDebugFile
					// Where
					<< "CPU\t\t"
					// When
					<< MCHEmul::fixLenStr (std::to_string (_clockCycles), 12, false)
					// What
					<< "Info Cycle\t\t"
					// Data
					<< "Instruction in:" + std::to_string (_cyclesPendingExecution) + " cycles\n";
		}
	}
	else
	{
		// Any hook at this position?
		MCHEmul::CPUHook* hk = executeHookIfAny ();
		if (hk != nullptr && deepDebugActive ())
			*_deepDebugFile
					<< "\t\t\t\t\t" << hk -> asString () << "\n";

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
			_error = MCHEmul::_INSTRUCTION_ERROR; // executed but with an error, the instruction is not found...
	}

	return (true);
}

// ---
bool MCHEmul::CPU::executeNextInstruction_Full (unsigned int &e)
{
	if (_currentInterrupt != nullptr)
		return (false); // The interrupt has priority...

	assert (_memory != nullptr);

	// By default, 
	// the instruction cycle should be processed always at this point!
	// ...but with errors? (see below)

	// Any hook at this position?
	MCHEmul::CPUHook* hk = executeHookIfAny ();
	if (hk != nullptr && deepDebugActive ()) // if debug print it out!
		*_deepDebugFile
				<< "\t\t\t\t\t" << hk -> asString () << "\n";

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

		return (true); // ...executed but with errors!
	}

	std::string sdd = "";
	if (deepDebugActive ())
		sdd = MCHEmul::removeAll0 (_programCounter.asString ()) + "(Stack "
				+ std::to_string (memoryRef () -> stack () -> position ()) + ") ";

	// If the memory access were buffered, this instruction would free all accesses...
	MCHEmul::Memory::configuration ().executeMemorySetCommandsBuffered ();

	// The execution of the instruction is notified
	// just in case any other part of the computer needed to prepare something...
	notify (MCHEmul::Event (_CPUTOEXECUTEINSTRUCTION, 0 /** No sense. */,
		std::shared_ptr <MCHEmul::Event::Data> (new MCHEmul::CPU::EventData (inst))));

	// Finally executed the instruction...
	// This method returns true when everything ok and false if not...
	if (inst -> execute (this, _memory, _memory -> stack (), &_programCounter))
	{
		// Actualize the clock cycles used by the last instruction...
		_lastCPUClockCycles = inst -> clockCyclesExecuted () + 
							  inst -> additionalClockCyclesExecuted ();
		// ...and finally saves the instruction executed...
		_lastInstruction = inst;

		if (deepDebugActive ())
		{
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
			*_deepDebugFile
					<< "\t\t\t\t\t"
					<< sdd // ...The program counter and the stack position...
					<< lSt << MCHEmul::_SPACES.substr (0, 15 - lSt.length ()) << "\t" // The last instruction...
					<< lStAB << MCHEmul::_SPACES.substr (0, 15 - lStAB.length ()) << "\t" // Address bus and Data bus ...if used!
					<< _statusRegister.asString () << "\t"; // ...The status register...
			for (const auto& i : _registers) 
				*_deepDebugFile 
					<< " " << i.asString () << " "; // ...and the registers info.
			*_deepDebugFile << "\n";
		}
	}
	// ...otherwise again an error is generated...
	else
		e = MCHEmul::_INSTRUCTION_ERROR;

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
