#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
_INST_IMPL (F6500::JAM)
{
	assert (parameters ().size () == 1);

	// JAM really stops the CPU
	// and the only way to continue is to reset it
	// So far, JAM has been implemented as to count down 1 cycle
	// an leave the PC at the same position it was...
	cpu () -> programCounter ().decrement (1);

	return (true);
}
