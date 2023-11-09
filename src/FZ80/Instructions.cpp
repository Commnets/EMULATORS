#include <FZ80/Instructions.hpp>

// ---
FZ80::Instruction::Instruction 
		(unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, const std::string& t)
	: MCHEmul::Instruction (c, mp, cc, t, false)
{
	// Nothing else...
}
