#include <FZ80/Instruction.hpp>

// ---
bool FZ80::Instruction::execute (MCHEmul::CPU* c, MCHEmul::Memory* m,
	MCHEmul::Stack* stk, MCHEmul::ProgramCounter* pc)
{
	bool result = MCHEmul::InstructionDefined::execute (c, m , stk, pc);

	incrementRegisterR ();

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
	bool result = MCHEmul::InstructionUndefined::execute (c, m, stk, pc);

	/** In the pre - fixed instructions the increment of the R register is double. */
	if (dynamic_cast <FZ80::Instruction*> (_lastInstruction) != nullptr)
		static_cast <FZ80::Instruction*> (_lastInstruction) -> incrementRegisterR ();

	return (result);
}
