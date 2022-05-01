#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>
#include <core/CPU.hpp>

// ---
_INST_IMPL (F6500::TAX)
{
	assert (parameters ().size () == 1);

	cpu () -> internalRegister (F6500::C6510::_XREGISTER).set (cpu () -> internalRegister (F6500::C6510::_ACCUMULATOR));

	return (true);
}

// ---
_INST_IMPL (F6500::TAY)
{
	assert (parameters ().size () == 1);

	cpu () -> internalRegister (F6500::C6510::_YREGISTER).set (cpu () -> internalRegister (F6500::C6510::_ACCUMULATOR));

	return (true);
}

// ---
_INST_IMPL (F6500::TXA)
{
	assert (parameters ().size () == 1);

	cpu () -> internalRegister (F6500::C6510::_ACCUMULATOR).set (cpu () -> internalRegister (F6500::C6510::_XREGISTER));

	return (true);
}

// ---
_INST_IMPL (F6500::TYA)
{
	assert (parameters ().size () == 1);

	cpu () -> internalRegister (F6500::C6510::_ACCUMULATOR).set (cpu () -> internalRegister (F6500::C6510::_YREGISTER));

	return (true);
}
