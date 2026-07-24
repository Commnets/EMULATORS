#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
_INST_IMPL (F6500::SHY_AbsoluteX)
{
	assert (parameters ().size () == 3);

	MCHEmul::Address bA (
		{ parameters ()[1], parameters ()[2] },
		false /** little endian. */);

	return (executeOn (
		bA,
		registerX ().values ()[0],
		registerY ().values ()[0]));
}
