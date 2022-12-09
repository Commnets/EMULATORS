#include <CORE/CPU.hpp>
#include <CORE/FmterBuilder.hpp>
#include <CORE/Formatter.hpp>

// ---
MCHEmul::CPU::CPU (const MCHEmul::CPUArchitecture& a, const MCHEmul::Registers& r, 
	const MCHEmul::StatusRegister& sR, const MCHEmul::Instructions& ins)
	: InfoClass ("CPU"),
	  _architecture (a), _registers (r), _statusRegister (sR), _instructions (ins),
	  _programCounter (a.numberBytes ()), _memory (nullptr), _interrupts (),
	  _lastInstruction (nullptr),
	  _error (_NOERROR), _clockCycles (0),
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
bool MCHEmul::CPU::initialize ()
{
	for (auto& i : _registers)
		i.initialize ();

	_statusRegister.initialize ();

	_programCounter.initialize (); 

	_lastInstruction = nullptr;

	_clockCycles = 0;

	return (true);
}

// ---
void MCHEmul::CPU::addInterrupt (MCHEmul::CPUInterrupt* in)
{
	assert (in != nullptr);

	if (_interrupts.find (in -> id ()) != _interrupts.end ())
		return; // Only one with the the same id...

	_interrupts.insert (MCHEmul::CPUInterrups::value_type (in -> id (), in));
}

// ---
void MCHEmul::CPU::removeInterrrupt (int id)
{
	MCHEmul::CPUInterrups::const_iterator i;
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

	unsigned int nC = 0;
	for (const auto& i : _interrupts)
	{
		i.second -> executeOver (this, nC);

		_clockCycles += nC;
	}

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

	/**
		The access to the instruction should be done using the original map, \n
		but the row list is used instead just to speed up the access. 

		MCHEmul::Instructions::const_iterator i = 
			_instructions.find (
				MCHEmul::UInt (
					_memory -> values (
						programCounter ().asAddress (), architecture ().instructionLength ()), 
					architecture ().bigEndian ()).asUnsignedInt ());

		if (i == _instructions.end ())
			return (false);

		MCHEmul::Instruction* inst = (*i).second;
	  */

	// Gets the data that the instruction occupies
	MCHEmul::UBytes dt = _memory -> values (programCounter ().asAddress (), inst -> memoryPositions ());

	// Move the "Program Counter" to the next instruction...
	// This is done before executing the instruction because the intruction itself could
	// modify the value of the "Program Counter"
	_programCounter += (size_t) inst -> memoryPositions ();

	// Then executes the instruction
	bool result = inst -> execute (dt, this, _memory, _memory -> stack ());

	// And also, take into account what it costs in terms of cycles...
	_clockCycles += inst -> clockCycles () + inst -> additionalClockCycles ();

	_lastInstruction = inst;

	return (result);
}

// ---
MCHEmul::InfoStructure MCHEmul::CPU::getInfoStructure () const
{
	MCHEmul::InfoStructure result;

	result.add ("Architecture", _architecture.getInfoStructure ());

	MCHEmul::InfoStructure regs;
	for (const auto& i : internalRegisters ())
		regs.add (i.name (), i.asString ());
	result.add ("REGS", regs);

	result.add ("PC", programCounter ().asString ());
	result.add ("SR", statusRegister ().asString ());

	return (result);
}
