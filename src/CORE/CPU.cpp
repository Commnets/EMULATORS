#include <CORE/CPU.hpp>
#include <CORE/FmterBuilder.hpp>
#include <CORE/Formatter.hpp>
#include <CORE/Stack.hpp>
#include <CORE/ProgramCounter.hpp>
#include <iostream>

// ---
MCHEmul::CPU::CPU (int id, const MCHEmul::CPUArchitecture& a, 
	const MCHEmul::Registers& r, const MCHEmul::StatusRegister& sR, const MCHEmul::Instructions& ins,
	const MCHEmul::Attributes& attrs)
	: MCHEmul::MotherboardElement (id, "CPU", attrs),
	  _architecture (a), _registers (r), _statusRegister (sR), _instructions (ins),
	  _lastINOUTAddress (), _lastINOUTData (),
	  _programCounter (a.numberBytes ()), _memory (nullptr), _interrupts (),
	  _state (MCHEmul::CPU::_EXECUTINGINSTRUCTION),
	  _deepDebugFile (nullptr),
	  _error (_NOERROR), 
	  _currentInstruction (nullptr), _currentInterruption (nullptr), _cyclesPendingExecution (0),
	  _clockCycles (0), _lastCPUClockCycles (0),
	  _lastState (MCHEmul::CPU::_EXECUTINGINSTRUCTION),
	  _lastInstruction (nullptr),
	  _typeCycleStopped (0 /** nothing. */), _cyclesStopped (0), _cyclesAtStop (0), _counterCyclesStopped (0),
	  _interruptRequested (-1), _clyclesAtInterruption (0),
	  _rowInstructions (), // It will be fulfilled later!
	  _rowInterrupts () // Idem
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
}

// ---
void MCHEmul::CPU::setStop (bool s, unsigned int tC, unsigned int cC, int nC)
{ 
	// Stop requested...
	if (s)
	{
		if (deepDebugActive ())
			*_deepDebugFile 
				<< "\t\t\t\tStop CPU requested:" << std::to_string (nC) 
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
const MCHEmul::Instruction* MCHEmul::CPU::nextInstruction () const
{
	// Access the next instruction...
	// Using the row description of the instructions!
	MCHEmul::Instruction* inst = nullptr;
	unsigned int nInst = 
		MCHEmul::UInt (
			_memory -> values (
				programCounter ().asAddress (), architecture ().instructionLength ()), 
			architecture ().bigEndian ()).asUnsignedInt ();
	if (nInst >= _rowInstructions.size () ||
		(inst = _rowInstructions [nInst]) == nullptr)
		return (nullptr); // Not possible to execute the instruction...

	// Gets the data that the instruction occupies
	MCHEmul::UBytes dt = _memory -> values (programCounter ().asAddress (), inst -> memoryPositions ());

	inst -> setParameters (dt);

	return (inst);
}

// ---
bool MCHEmul::CPU::initialize ()
{
	if (! MCHEmul::MotherboardElement::initialize ())
		return (false);

	for (auto& i : _registers)
		i.initialize ();

	_lastINOUTAddress = MCHEmul::Address ();
	
	_lastINOUTData = { };

	_statusRegister.initialize ();

	_programCounter.initialize ();

	for (auto& i : _interrupts)
		i.second -> initialize ();

	_error = _NOERROR;

	_currentInterruption = nullptr;
	_currentInstruction = nullptr;

	_clockCycles = _lastCPUClockCycles = _cyclesPendingExecution = 0;

	_state = _lastState = MCHEmul::CPU::_EXECUTINGINSTRUCTION;

	// Related with the internal states...
	_lastInstruction = nullptr;
	_typeCycleStopped = 0; // Meaning none
	_cyclesStopped = 0;
	_cyclesAtStop = _counterCyclesStopped = 0;
	_interruptRequested = -1; // Meaning none...
	_clyclesAtInterruption = 0;

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
void MCHEmul::CPU::requestInterrupt (int id, unsigned int nC, MCHEmul::Chip* src, int cR)
{ 
	if (_interruptRequested == -1)
	{ 
		_interruptRequested = id; 
		
		_clyclesAtInterruption = nC; 

		if (deepDebugActive ())
			*_deepDebugFile 
				<< "\t\t\t\tInterrupt CPU requested:" << std::to_string (id) 
				<< " from:" << ((src == nullptr) ? "-" : std::to_string (src -> id ())) 
				<< " reason: " << std::to_string (cR) << "\n";
	} 
}

// ---
bool MCHEmul::CPU::executeNextCycle ()
{
	memoryRef () -> setCPUView (); // Always...

	bool result = false;

	auto execFunction = [&](bool f) -> bool
		{
			switch (_state)
			{
				case MCHEmul::CPU::_EXECUTINGINSTRUCTION:
					result = (f)
						? when_ExecutingInstruction_Full ()
						: when_ExecutingInstruction_PerCycle ();
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
		};

	if (ticksCounter () == nullptr)
	{
		execFunction (true);
	}
	else
	{
		for (unsigned int i = ticksCounter () -> elapsedTicks (); i > 0; i--)
			execFunction (false);
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

	result.add ("PC", std::move (programCounter ().asString ()));
	result.add ("SR", std::move (statusRegister ().asString ()));

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
bool MCHEmul::CPU::when_ExecutingInstruction_PerCycle ()
{
	assert (_state == MCHEmul::CPU::_EXECUTINGINSTRUCTION);

	// What to execute: Interrupt or Instruction?

	// If there is no any instruction already under execution...
	if (_currentInstruction == nullptr)
	{
		bool eInt = (_currentInterruption != nullptr);
		// ...and there is no any interruption under launching...
		if (!eInt)
		{
			// ...look for the possibility of launching an interruption...
			if (eInt = 
				(_interruptRequested != -1 && 
				 (_currentInterruption = _rowInterrupts [(size_t) _interruptRequested]) -> 
					canBeExecutedOver (this, _clyclesAtInterruption)))
				_cyclesPendingExecution = _currentInterruption -> cyclesToLaunch ();
			else
			{ 
				_interruptRequested = -1; // it can be requested later another time...

				_currentInterruption = nullptr;
			}
		}

		// but if even looking for an interruption it is not possible...
		if (!eInt)
		{
			// ...look for the next instruction to be executed...
			unsigned int nInst = 
				MCHEmul::UInt (
					_memory -> values (
						programCounter ().asAddress (), architecture ().instructionLength ()), 
					architecture ().bigEndian ()).asUnsignedInt ();
			if (nInst < _rowInstructions.size () && 
				(_currentInstruction = _rowInstructions [nInst]) != nullptr)
				_cyclesPendingExecution = 
					_currentInstruction -> clockCycles (); // There might be some additional cycles after execution...
		}
	}

	// At this point there must be either 
	// a interrupt or a instruction to be executed or under execution...
	// if not there is error in the CPU!
	if (_currentInterruption == nullptr && 
		_currentInstruction == nullptr)
	{ 
		_error = MCHEmul::_CPU_ERROR;

		return (false);
	}

	_lastCPUClockCycles = 1;

	_clockCycles++;

	// Now, what to execute?
	// The interrupt, if defined, has priority over the instruction!
	bool result = true;
	if (_currentInterruption != nullptr)
	{
		// ...but only when the cycles pending to launch are 0!...
		if (--_cyclesPendingExecution == 0)
		{
			if (deepDebugActive ())
				*_deepDebugFile << "\t\t\t\tInterrupt launched:" 
								<< std::to_string (_currentInterruption -> id ()) << "\n";

			if (!(result = _currentInterruption -> 
				executeOver (this, _clyclesAtInterruption)))
				return (result);

			_lastCPUClockCycles += _currentInterruption -> cycledAfterLaunch ();

			_interruptRequested = -1; // No more requested...

			_currentInterruption = nullptr; // ...already executed...
		}
		else
		{
			if (deepDebugActive ())
				*_deepDebugFile
					// Where
					<< "CPU\t"
					// When
					<< MCHEmul::fixLenStr (std::to_string (_clockCycles), 12, false)
					// What
					<< "Info Cycle\t\t"
					// Data
					<< "Interrupt in:" + std::to_string (_cyclesPendingExecution) + " cycles\n";
		}
	}
	// if not, then the instruction has to be executed...
	else
	{
		// ...but only when the cycles pending are 0!
		if (--_cyclesPendingExecution == 0)
		{ 
			std::string sdd = "";
			if (deepDebugActive ())
				sdd = MCHEmul::removeAll0 (_programCounter.asString ()) + "(Stack "
						+ std::to_string (memoryRef () -> stack () -> position ()) + ") ";

			if (!(result = _currentInstruction -> 
				execute (this, _memory, _memory -> stack (), &_programCounter)))
				return (result);

			_lastCPUClockCycles += _currentInstruction -> additionalClockCycles (); // Just in case...

			_clockCycles += _currentInstruction -> additionalClockCycles (); // Just in case...

			_lastInstruction = _currentInstruction;

			_currentInstruction = nullptr; // Another one is needed...

			if (deepDebugActive ())
			{
				std::string lSt = _lastInstruction -> asString (); 
				std::string lStA = 
					((_lastInstruction -> lastINOUTAddress ().value () != 0)
						? "$" + MCHEmul::removeAll0 // Was there address?
							(_lastInstruction -> lastINOUTAddress ().asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2))
						: "");
				std::string lStB = 
					((_lastInstruction -> lastINOUTData ().size () != 0)
						? "$" + MCHEmul::removeAll0 // Was there data used?
							(_lastInstruction -> lastINOUTData ().asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2))
						: "");
				std::string lStAB = lStA + ((lStA == "") ? "" : ((lStB == "") ? "" : ",")) + lStB;
				lStAB = ((lStAB != "") ? "(" : "") + lStAB + ((lStAB != "") ? ")" : "");
				*_deepDebugFile
						<< "\t\t\t\t"
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
					<< "CPU\t"
					// When
					<< MCHEmul::fixLenStr (std::to_string (_clockCycles), 12, false)
					// What
					<< "Info Cycle\t\t"
					// Data
					<< "Instruction in:" + std::to_string (_cyclesPendingExecution) + " cycles\n";
		}
	}

	return (result);
}

// ---
bool MCHEmul::CPU::when_ExecutingInstruction_Full ()
{
	assert (_state == MCHEmul::CPU::_EXECUTINGINSTRUCTION);

	if (deepDebugActive ())
		*_deepDebugFile
			// Where
			<< "CPU\t"
			// When
			<< MCHEmul::fixLenStr (std::to_string (_clockCycles), 12, false)
			// What
			<< "Info Cycle\t\t"
			// Data
			<< "Last:" + ((_lastInstruction == nullptr) ? "0" : std::to_string (_lastInstruction -> clockCycles ())) + " cycles\n";

	// The activation of the interruptions has priority over the execution of the code
	// But to execute, the last instruction must finish, 
	// otherwise the execution is not possible...
	if (_currentInstruction == nullptr && // No instruction under execution...
		_interruptRequested != -1 && // It has be required...
		(_currentInterruption = _rowInterrupts [(size_t) _interruptRequested]) != nullptr && // It has to exist...
		_currentInterruption -> canBeExecutedOver (this, _clyclesAtInterruption)) // ...and the execution can depend on internal state of the CPU...
	{
		if (deepDebugActive ())
			*_deepDebugFile << "\t\t\t\tInterrupt launched:" 
							<< std::to_string (_currentInterruption -> id ()) << "\n";

		_lastCPUClockCycles = _currentInterruption -> cyclesToLaunch ();
		_clockCycles += _lastCPUClockCycles;

		_interruptRequested = -1; // No longer valid...

		bool result = true;
		if (result = _currentInterruption -> executeOver (this, _clyclesAtInterruption))
			_lastCPUClockCycles += _currentInterruption -> cycledAfterLaunch ();

		_currentInterruption = nullptr;
	
		return (result);
	}
	else
	{
		_interruptRequested = -1;

		_currentInterruption = nullptr;
	}

	// Access the next instruction...
	// Using the row description of the instructions!
	unsigned int nInst = 
		MCHEmul::UInt (
			_memory -> values (
				programCounter ().asAddress (), architecture ().instructionLength ()), 
			architecture ().bigEndian ()).asUnsignedInt ();
	// If the instruction doesn't exist according with what is indicated in the memory of 
	// the computer, and an error is generated...
	if (nInst >= _rowInstructions.size () || 
		(_currentInstruction = _rowInstructions [nInst]) == nullptr)
	{ 
		_error = MCHEmul::_CPU_ERROR;

		return (false);
	}

	std::string sdd = "";
	if (deepDebugActive ())
		sdd = MCHEmul::removeAll0 (_programCounter.asString ()) + "(Stack "
				+ std::to_string (memoryRef () -> stack () -> position ()) + ") ";

	// Then, the instruction is executed and if everything went ok...
	bool result = true;
	if (result = _currentInstruction -> execute (this, _memory, _memory -> stack (), &_programCounter))
	{
		// Actualize the clock cycles used by the last instruction...
		_lastCPUClockCycles = (_currentInstruction -> clockCycles () + 
			_currentInstruction -> additionalClockCycles ());
		// The total clock is also actualized...
		_clockCycles += _lastCPUClockCycles;
		// ...and finally saves the instruction executed...
		_lastInstruction = _currentInstruction;

		if (deepDebugActive ())
		{
			std::string lSt = _lastInstruction -> asString (); 
			std::string lStA = 
				((_lastInstruction -> lastINOUTAddress ().value () != 0)
					? "$" + MCHEmul::removeAll0 // Was there address?
						(_lastInstruction -> lastINOUTAddress ().asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2))
					: "");
			std::string lStB = 
				((_lastInstruction -> lastINOUTData ().size () != 0)
					? "$" + MCHEmul::removeAll0 // Was there data used?
						(_lastInstruction -> lastINOUTData ().asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2))
					: "");
			std::string lStAB = lStA + ((lStA == "") ? "" : ((lStB == "") ? "" : ",")) + lStB;
			lStAB = ((lStAB != "") ? "(" : "") + lStAB + ((lStAB != "") ? ")" : "");
			*_deepDebugFile
					<< "\t\t\t\t"
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
	{ 
		_error = MCHEmul::_CPU_ERROR;

		result = false;
	}

	_currentInstruction = nullptr;

	return (result);
}

// ---
bool MCHEmul::CPU::when_Stopped ()
{
	assert (_state == MCHEmul::CPU::_STOPPED);

	if (deepDebugActive ())
		*_deepDebugFile 
			// Where
			<< "CPU\t"
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
	// No other value of this status is changed, because evrything has to continue 
	// at the situation it had when stopped, 
	// either running a instruction or launching a interruption...

	_clockCycles++;

	return (true);
}
