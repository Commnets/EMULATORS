#include <FZ80/Instruction.hpp>

// ---
bool FZ80::Instruction::execute (MCHEmul::CPU* c, MCHEmul::Memory* m,
	MCHEmul::Stack* stk, MCHEmul::ProgramCounter* pc)
{
	_lastExecutionData._INOUTAddress = MCHEmul::Address ();

	// Any fetch operation increment the register R (@see incrementRegisterR)
	incrementRegisterR (static_cast <FZ80::CZ80*> (c));

	bool result = MCHEmul::InstructionDefined::execute (c, m , stk, pc);
	static_cast <FZ80::CZ80*> (c) -> setInstructionExecuted (); // Just for the EI...

	// At T4 of the M1 cycle (first machine cycle) 
	// the IR is put value into the address bus.
	// Later (other cycles), the bus can be used for other purposes...
	// But if after those other uses, the _lastINOUTAddress is still nothing...
	// it would mean that the info to be sent to the address bus is the IR!
	if (_lastExecutionData._INOUTAddress.value () == 0)
		cpu () -> setLastINOUTAddress (addressIR ()); // This instruction already move the value into the _lastINOUTAddress...

	return (result);
}

// ---
FZ80::InstructionUndefined::InstructionUndefined (unsigned int c, const MCHEmul::Instructions& inst)
	: MCHEmul::InstructionUndefined (c, inst),
	  _rawInstructions ()
{
	auto numberBytes = [](unsigned int c) -> size_t
		{
			unsigned int cC = c;
			size_t nB = 0;
			while (cC != 0) { nB++; cC >>= 8; }
			return (nB);
		};

	_rawInstructions = MCHEmul::ListOfInstructions (0x100 /** 256 */, nullptr);
	size_t nBSC = numberBytes (c);
	for (const auto& i : inst)
	{
		assert (numberBytes (i.first) == (nBSC + 1));						// They must by different in just one byte long...
		assert (_rawInstructions [(size_t) (i.first & 0xff)] == nullptr);	// ...and no duplications allowed...
		assert ((c << 8) == (i.first & (~0xff)));							// ...and the up part of the code has to be the same...
		
		_rawInstructions 
			[size_t (i.first /** instead i.second -> code () because it might be duplicated instructions. */ & 0xff)] = i.second;
	}
}

// ---
bool FZ80::InstructionUndefined::execute (MCHEmul::CPU* c, MCHEmul::Memory* m,
	MCHEmul::Stack* stk, MCHEmul::ProgramCounter* pc)
{
	/** In the pre - fixed instructions the increment of the R register is double. */
	if (dynamic_cast <FZ80::Instruction*> (_lastInstruction) != nullptr)
		static_cast <FZ80::Instruction*> (_lastInstruction) -> incrementRegisterR (static_cast <FZ80::CZ80*> (c));

	bool result = MCHEmul::InstructionUndefined::execute (c, m, stk, pc);

	return (result);
}
