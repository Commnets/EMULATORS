#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>
#include <core/CPU.hpp>

_INST_IMPL (F6500::SEC)
{
	cpu () -> statusRegister ().setBitStatus ("C", true);

	return (true);
}

_INST_IMPL (F6500::SED)
{
	cpu () -> statusRegister ().setBitStatus ("D", true);

	return (true);
}

_INST_IMPL (F6500::SEI)
{
	cpu () -> statusRegister ().setBitStatus ("I", true);

	return (true);
}
