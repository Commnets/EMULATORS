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
	  _deepDebugActivated (false), _debugFile (),
	  _error (_NOERROR), 
	  _clockCycles (0), _lastClockCycles (0),
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

	_debugFile.close (); // Just in case...
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

	_clockCycles = 0;

	_lastClockCycles = 0;

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

	if (_stopped)
		return (true);

	// Number of cycles calling interruptions...
	unsigned int nCInt = 0;
	// ..and number of cycles executing the last instruction...
	unsigned int nCInst = 0;

	for (const auto& i : _interrupts)
	{
		i.second -> executeOver (this, nCInt);

		_clockCycles += nCInt;
	}

	// If the very deep debug is activated (dangerous)
	// Information about the Program Counter and the Stack position is first printed out...
	// ...see later!
	if (_deepDebugActivated)
		_debugFile << MCHEmul::removeAll0 (_programCounter.asString ()) << ":(SP " 
				   << memoryRef () -> stack () -> position () << ") ";

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

	_clockCycles += (nCInst = inst -> clockCycles () + inst -> additionalClockCycles ());

	_lastClockCycles = nCInt + nCInst;

	_lastInstruction = inst;

	// If after the execution of the instruction, the deep debugging is activated,
	// then additional information about the instruction executed and the stats of the CPU is printed out!
	if (_deepDebugActivated)
	{
		std::string lSt = ""; size_t lenI = (lSt = _lastInstruction -> asString ()).size ();;
		_debugFile << lSt << MCHEmul::_SPACES.substr (0, 20 - lenI) << "\t";
		_debugFile << _statusRegister.asString () << "\t";
		for (const auto& i : _registers) _debugFile << " " << i.asString () << " ";
		_debugFile << std::endl;
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
bool MCHEmul::CPU::activateDeepDebug (const std::string fn, bool a)
{
	// If deepDebug were already activated
	if (_deepDebugActivated)
		return (false); // ...no activation will be possible...

	_debugFile.open (fn, a 
		? (std::ios::out | std::ios::app) /** Add at the end */
		: (std::ios::out | std::ios::trunc) /** Remove everything */ );
	if (!_debugFile)
		return (false);

	_deepDebugActivated = true;

	return (true);
}

// ---
bool MCHEmul::CPU::desactivateDeepDebug ()
{
	// If not deepDebugging were activated
	if (!_deepDebugActivated)
		return (false); // ...no desactivation will be possible...

	_debugFile.close ();

	_deepDebugActivated = false;

	return (true);
}