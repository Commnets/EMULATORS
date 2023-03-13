#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
_INST_IMPL (F6500::JAM)
{
	assert (parameters ().size () == 1);

	// Just start...
	cpu () -> restart ();

	return (true);
}
