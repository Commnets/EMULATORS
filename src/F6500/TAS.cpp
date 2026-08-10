#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
_INST_IMPL (F6500::TAS_AbsoluteY)
{
	assert (parameters ().size () == 3);

	// The original address is needed because a page crossing can corrupt
	// the high byte of the effective address instead of adding a cycle.
	MCHEmul::Address bA (
		{ parameters ()[1], parameters ()[2] },
		false /** little endian. */);
	MCHEmul::UByte v =
		registerA ().values ()[0] & registerX ().values ()[0];

	stack () -> setPosition ((int) v.value ());

	return (executeOn (bA, registerY ().values ()[0], v));
}
