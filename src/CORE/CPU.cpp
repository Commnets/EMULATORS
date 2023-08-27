#include <CORE/CPU.hpp>
#include <CORE/FmterBuilder.hpp>
#include <CORE/Formatter.hpp>
#include <CORE/Stack.hpp>
#include <iostream>

// ---
MCHEmul::CPU::CPU (const MCHEmul::CPUArchitecture& a, const MCHEmul::Registers& r, 
	const MCHEmul::StatusRegister& sR, const MCHEmul::Instructions& ins)
	: InfoClass ("CPU"),
	  _architecture (a), _registers (r), _statusRegister (sR), _instructions (ins),
	  _programCounter (a.numberBytes ()), _memory (nullptr), _interrupts (),
	  _state (MCHEmul::CPU::_EXECUTINGINSTRUCTION),
	  _deepDebugFile (nullptr),
	  _error (_NOERROR), 
	  _clockCycles (0), _lastCPUClockCycles (0),
	  _lastState (MCHEmul::CPU::_EXECUTINGINSTRUCTION),
	  _lastInstruction (nullptr),
	  _cyclesStopped (0), _cyclesAtStop (0), _counterCyclesStopped (0),
	  _interruptRequested (-1), _clyclesAtInterruption (0),
	  _rowInstructions (), // It will be fulfilled later!
	  _rowInterrupts () // Idem
{ 
	assert (_registers.size () > 0 && _instructions.size () > 0); 

	// Put the instrucctions in an array to speed up things later...
	_rowInstructions = std::vector <MCHEmul::Instruction*> 
		((*_instructions.rbegin ()).second -> code () + 1 /** the last code plus 1. */, nullptr);
	for (const auto& i : _instructions)
		_rowInstructions [i.second -> code ()] = i.second;
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
				<< "\t\t\t\tStop CPU requested:" << std::to_string (nC) << " cycles\n";

		// If the CPU was already stopped...
		// the counter starts back, but neither the state changes
		// nor the point where the stop started...
		if (_state == CPU::_STOPPED)
		{ 
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
	for (auto& i : _registers)
		i.initialize ();

	_statusRegister.initialize ();

	_programCounter.initialize ();

	for (auto& i : _interrupts)
		i.second -> initialize ();

	_error = _NOERROR;

	_clockCycles = _lastCPUClockCycles = 0;

	_state = _lastState = MCHEmul::CPU::_EXECUTINGINSTRUCTION;

	// Related with the internal states...
	_lastInstruction = nullptr;
	_cyclesStopped = _cyclesAtStop = _counterCyclesStopped = 0;
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
void MCHEmul::CPU::requestInterrupt (int id, unsigned int nC, MCHEmul::Chip* src)
{ 
	if (_interruptRequested == -1)
	{ 
		_interruptRequested = id; 
		
		_clyclesAtInterruption = nC; 

		if (deepDebugActive ())
			*_deepDebugFile 
				<< "\t\t\t\tInterrupt CPU requested:" << std::to_string (id) << "\n";
	} 
}

// ---
bool MCHEmul::CPU::executeNextCycle ()
{
	memoryRef () -> setCPUView (); // Always...

	bool result = false;

	switch (_state)
	{
		case MCHEmul::CPU::_EXECUTINGINSTRUCTION:
			result = when_ExecutingInstruction ();
			break;
		
		case MCHEmul::CPU::_STOPPED:
			result = when_Stopped ();
			break;

		default:
			assert (0); // It shouldn't be here. It would mean that there are states with no treatment...
			break;
	}

	return (result);
}

// ---
MCHEmul::InfoStructure MCHEmul::CPU::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

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
	_rowInterrupts = std::vector <MCHEmul::CPUInterrupt*> 
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
			<< std::to_string (_clockCycles) << "\t"
			// What
			<< "Info Cycle\t\t"
			// Data
			<< "Last:" + ((_lastInstruction == nullptr) ? "0" : std::to_string (_lastInstruction -> clockCycles ())) + " cycles\n";

	// The activation of the interruptions has priority over the execution of the code
	// But to execute, the last instruction must finish, 
	// otherwise the execution is not possible...
	MCHEmul::CPUInterrupt* intr = nullptr;
	if (_interruptRequested != -1 && // It has be required...
		(intr = _rowInterrupts [(size_t) _interruptRequested]) != nullptr && // It has to exist...
		intr -> canBeExecutedOver (this, _clyclesAtInterruption)) // ...and the execution can depend on internal state of the CPU...
	{
		if (deepDebugActive ())
			*_deepDebugFile << "\t\t\t\tInterrupt launched:" << std::to_string (intr -> id ()) << "\n";

		_lastCPUClockCycles = intr -> cyclesToLaunch ();
		_clockCycles += _lastCPUClockCycles;

		_interruptRequested = -1; // No longer valid...

		return (intr -> executeOver (this, _clyclesAtInterruption));
	}
	else
		_interruptRequested = -1;

	// Access the next instruction...
	// Using the row description of the instructions!
	MCHEmul::Instruction* inst = nullptr;
	unsigned int nInst = 
		MCHEmul::UInt (
			_memory -> values (
				programCounter ().asAddress (), architecture ().instructionLength ()), 
			architecture ().bigEndian ()).asUnsignedInt ();
	// If the instruction doesn't exist according with what is indicated in the memory of 
	// the computer, and an error is generated...
	if (nInst >= _rowInstructions.size () || 
		(inst = _rowInstructions [nInst]) == nullptr)
	{ 
		_error = MCHEmul::_CPU_ERROR;

		return (false);
	}

	std::string sdd = "";
	if (deepDebugActive ())
		sdd = MCHEmul::removeAll0 (_programCounter.asString ()) + "(Stack "
				+ std::to_string (memoryRef () -> stack () -> position ()) + ") ";

	// Gets the data that the instruction occupies
	// before updating the Program Counter...
	MCHEmul::UBytes dt = _memory -> values (programCounter ().asAddress (), inst -> memoryPositions ());
	// Then, the Program Counter is moved to the next instruction...
	// This is done in this way because the intruction itself could
	// modify the value of the Program Counter (Juimps, returns,...)
	_programCounter += (size_t) inst -> memoryPositions ();

	// Then, the instruction is executed and if everything went ok...
	bool result = true;
	if (result = inst -> execute (dt, this, _memory, _memory -> stack ()))
	{
		// Actualize the clock cycles used by the last instruction...
		_lastCPUClockCycles = (inst -> clockCycles () + inst -> additionalClockCycles ());
		// The total clock is also actualized...
		_clockCycles += _lastCPUClockCycles;
		// ...and finally saves the instruction executed...
		_lastInstruction = inst;

		if (deepDebugActive ())
		{
			std::string lSt = ""; 
			size_t lenI = (lSt = _lastInstruction -> asString ()).size ();
			*_deepDebugFile 
					<< "\t\t\t\t"
					<< sdd // ...The program counter and the stack position...
					<< lSt << MCHEmul::_SPACES.substr (0, 15 - lenI) << "\t" // The last instruction...
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
			<< std::to_string (_clockCycles) << "\t"
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
