#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>
#include <core/CPU.hpp>

// ---
_INST_IMPL (F6500::CLC)
{
	assert (parameters ().size () == 1);

	cpu () -> statusRegister ().setBitStatus ("C", false);

	return (true);
}

// ---
_INST_IMPL (F6500::CLD)
{
	assert (parameters ().size () == 1);

	cpu () -> statusRegister ().setBitStatus ("D", false);

	return (true);
}

// ---
_INST_IMPL (F6500::CLI)
{
	assert (parameters ().size () == 1);

	cpu () -> statusRegister ().setBitStatus ("I", false);

	return (true);
}

// ---
_INST_IMPL (F6500::CLV)
{
	assert (parameters ().size () == 1);

	cpu () -> statusRegister ().setBitStatus ("V", false);

	return (true);
}

// ---
_INST_IMPL (F6500::SEC)
{
	assert (parameters ().size () == 1);

	cpu () -> statusRegister ().setBitStatus ("C", true);

	return (true);
}

// ---
_INST_IMPL (F6500::SED)
{
	assert (parameters ().size () == 1);

	cpu () -> statusRegister ().setBitStatus ("D", true);

	return (true);
}

// ---
_INST_IMPL (F6500::SEI)
{
	assert (parameters ().size () == 1);

	cpu () -> statusRegister ().setBitStatus ("I", true);

	return (true);
}
