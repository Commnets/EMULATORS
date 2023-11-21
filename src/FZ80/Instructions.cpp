#include <FZ80/Instructions.hpp>

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
		assert ((c << 8) == (i.first & (~0xff)));						// ...and the up part of the code has to be the same...
		
		_rawInstructions 
			[size_t (i.first /** instead i.second -> code () because it might be duplicated instructions. */ & 0xff)] = i.second;
	}
}
