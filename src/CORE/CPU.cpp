#include <CORE/CPU.hpp>

// ---
MCHEmul::CPU::~CPU ()
{
	for (auto i : _instructions)
		delete (i.second);

	for (auto i : _interrupts)
		delete (i.second);
}

// ---
bool MCHEmul::CPU::initialize ()
{
	for (auto i : _registers)
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
	for (auto i : _interrupts)
	{
		i.second -> executeOver (this, nC);
		_clockCycles += nC;
	}

	MCHEmul::Instructions::const_iterator i = 
		_instructions.find (
			MCHEmul::UInt (
				_memory -> values (
					programCounter ().asAddress (), architecture ().instructionLength ()), 
				architecture ().bigEndian ()).asUnsignedInt ());

	if (i == _instructions.end ())
		return (false);

	bool result = false;

	MCHEmul::Instruction* inst = (*i).second;

	// Gets the data that the instruction occupies
	MCHEmul::UBytes dt = _memory -> values (programCounter ().asAddress (), inst -> memoryPositions ());
	// Move the "Program Counter" to the next instruction...
	// This is done before executing the instruction because the intruction itself could
	// modify the value of the "Program Counter"
	_programCounter += (size_t) inst -> memoryPositions ();
	// Then executes the instruction
	result = inst -> execute (dt, this, _memory, _memory -> stack ());

	// And also, take into account what it costs in terms of cycles...
	_clockCycles += inst -> clockCycles () + inst -> additionalClockCycles ();

	_lastInstruction = inst;

	return (result);
}

// ---
std::ostream& MCHEmul::operator << (std::ostream& o, const MCHEmul::CPU& c)
{
	o << c.architecture () << std::endl;
	for (auto i : c.internalRegisters ())
		o << i << std::endl;
	o << c.programCounter () << std::endl;
	o << c.statusRegister ();

	return (o);
}
