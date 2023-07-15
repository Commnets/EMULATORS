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
	  _lastInstruction (nullptr),
	  _deepDebugFile (nullptr),
	  _error (_NOERROR), 
	  _clockCycles (0), _lastCPUClockCycles (0),
	  _stopped (false),
	  _rowInstructions () // It will be fulfilled later!
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

	_lastInstruction = nullptr;

	_clockCycles = _lastCPUClockCycles = 0;

	return (true);
}

// ---
void MCHEmul::CPU::addInterrupt (MCHEmul::CPUInterrupt* in)
{
	assert (in != nullptr);

	if (_interrupts.find (in -> id ()) != _interrupts.end ())
		return; // Only one with the the same id...

	_interrupts.insert (MCHEmul::CPUInterrupts::value_type (in -> id (), in));
}

// ---
void MCHEmul::CPU::removeInterrrupt (int id)
{
	MCHEmul::CPUInterrupts::const_iterator i;
	if ((i = _interrupts.find (id)) == _interrupts.end ())
		return;

	_interrupts.erase (i);
}

// ---
bool MCHEmul::CPU::executeNextInstruction ()
{
	memoryRef () -> setCPUView (); // Always...

	_lastCPUClockCycles = 0;

	if (_stopped)
		return (true);

	for (const auto& i : _interrupts)
	{
		unsigned int nCInt = 0;
		i.second -> executeOver (this, nCInt);

		_lastCPUClockCycles += nCInt;
	}

	// If the very deep debug is activated (dangerous)
	// Information about the Program Counter and the Stack position has to be first printed out...
	// ...see later!
	std::string sdd = "";
	if (deepDebugActive ())
		sdd = MCHEmul::removeAll0 (_programCounter.asString ()) + ":(SP " 
				   + std::to_string (memoryRef () -> stack () -> position ()) + ") ";

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
		return (false); // Not possible to execute the instruction...

	// Gets the data that the instruction occupies
	MCHEmul::UBytes dt = _memory -> values (programCounter ().asAddress (), inst -> memoryPositions ());

	// Move the "Program Counter" to the next instruction...
	// This is done before executing the instruction because the intruction itself could
	// modify the value of the "Program Counter"
	_programCounter += (size_t) inst -> memoryPositions ();

	// Then, executes the instruction
	bool result = inst -> execute (dt, this, _memory, _memory -> stack ());

	_lastCPUClockCycles += (inst -> clockCycles () + inst -> additionalClockCycles ());

	_clockCycles += _lastCPUClockCycles;

	_lastInstruction = inst;

	// If after the execution of the instruction, the deep debugging is still activated...
	// ...then information about the instruction executed and the stats of the CPU is printed out!
	if (deepDebugActive ())
	{
		std::string lSt = ""; 
		size_t lenI = (lSt = _lastInstruction -> asString ()).size ();
		*_deepDebugFile << sdd // ...The program counter and the stack position...
				   << lSt << MCHEmul::_SPACES.substr (0, 20 - lenI) << "\t" // The last instruction...
				   << _statusRegister.asString () << "\t"; // ...The status register...
		for (const auto& i : _registers) 
			*_deepDebugFile << " " << i.asString () << " "; // ...and the registers info.
		*_deepDebugFile << "\n";
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
