#include <CORE/GraphicalChip.hpp>

// ---
bool MCHEmul::GraphicalChip::initialize ()
{
	if (!MCHEmul::Chip::initialize ())
		return (false);

	_screenMemory = createScreenMemory ();

	_graphicsReady = false;

	return (_screenMemory != nullptr);
}
