#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
_INST_IMPL (F6500::SHX_AbsoluteY)
{
	assert (parameters ().size () == 3);

	MCHEmul::Address bA (
		{ parameters ()[1], parameters ()[2] },
		false /** little endian. */);

	return (executeOn (
		bA,
		registerY ().values ()[0],
		registerX ().values ()[0]));
}
