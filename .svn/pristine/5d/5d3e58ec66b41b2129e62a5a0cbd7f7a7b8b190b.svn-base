#include <core/CPU.hpp>
#include <iostream>

// ---
MCHEmul::CPU::~CPU ()
{
	for (auto i : _instructions)
		delete i.second;
}

// ---
bool MCHEmul::CPU::initialize ()
{
	for (auto i : _registers)
		i.initialize ();

	_programCounter.initialize (); 

	_clockCycles = 0;

	return (true);
}

// ---
bool MCHEmul::CPU::executeNextTransaction ()
{
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

#ifndef _NDEBUG
	std::cout << *inst << std::endl;
#endif

	return (result);
}

// ---
std::ostream& MCHEmul::operator << (std::ostream& o, const MCHEmul::CPU& c)
{
	o << "---" << std::endl;
	o << "CPU Info" << std::endl;
	o << c.architecture () << std::endl;
	for (auto i : c.internalRegisters ())
		o << i << std::endl;
	o << c.programCounter () << std::endl;
	o << c.statusRegister () << std::endl;
	o << c.clockCycles () << " clock cycles";

	return (o);
}
