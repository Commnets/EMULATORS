#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>
#include <core/CPU.hpp>
#include <core/Stack.hpp>

// ---
_INST_IMPL (F6500::PHA)
{
	assert (parameters ().size () == 1);

	stack () -> push (cpu () -> internalRegister (F6500::C6510::_ACCUMULATOR).values ()); // 1 byte long...

	return (true);
}

// ---
_INST_IMPL (F6500::PHP)
{
	assert (parameters ().size () == 1);

	stack () -> push (cpu () -> statusRegister ().values ()); // 1 byte long...

	return (true);
}

// ---
_INST_IMPL (F6500::PLA)
{
	assert (parameters ().size () == 1);

	cpu () -> internalRegister (F6500::C6510::_ACCUMULATOR).set (stack () -> pull (1));

	return (true);
}

// ---
_INST_IMPL (F6500::PLP)
{
	assert (parameters ().size () == 1);

	cpu () -> statusRegister ().set (stack () -> pull (1));

	return (true);
}

// ---
_INST_IMPL (F6500::TSX)
{
	assert (parameters ().size () == 1);

	cpu () -> internalRegister (F6500::C6510::_XREGISTER).set (stack () -> pull (1));

	return (true);
}

// ---
_INST_IMPL (F6500::TXS)
{
	assert (parameters ().size () == 1);

	stack () -> push (cpu () -> internalRegister (F6500::C6510::_XREGISTER).values ()); // 1 byte long...

	return (true);
}
