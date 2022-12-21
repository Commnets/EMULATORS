#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
_INST_IMPL (F6500::CLC)
{
	assert (parameters ().size () == 1);

	cpu () -> statusRegister ().setBitStatus (F6500::C6500::_CARRYFLAG, false);

	return (true);
}

// ---
_INST_IMPL (F6500::CLD)
{
	assert (parameters ().size () == 1);

	cpu () -> statusRegister ().setBitStatus (F6500::C6500::_DECIMALFLAG, false);

	return (true);
}

// ---
_INST_IMPL (F6500::CLI)
{
	assert (parameters ().size () == 1);

	cpu () -> statusRegister ().setBitStatus (F6500::C6500::_IRQFLAG, false);

	return (true);
}

// ---
_INST_IMPL (F6500::CLV)
{
	assert (parameters ().size () == 1);

	cpu () -> statusRegister ().setBitStatus (F6500::C6500::_OVERFLOWFLAG, false);

	return (true);
}

// ---
_INST_IMPL (F6500::SEC)
{
	assert (parameters ().size () == 1);

	cpu () -> statusRegister ().setBitStatus (F6500::C6500::_CARRYFLAG, true);

	return (true);
}

// ---
_INST_IMPL (F6500::SED)
{
	assert (parameters ().size () == 1);

	cpu () -> statusRegister ().setBitStatus (F6500::C6500::_DECIMALFLAG, true);

	return (true);
}

// ---
_INST_IMPL (F6500::SEI)
{
	assert (parameters ().size () == 1);

	cpu () -> statusRegister ().setBitStatus (F6500::C6500::_IRQFLAG, true);

	return (true);
}
