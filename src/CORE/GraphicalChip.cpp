#include <CORE/GraphicalChip.hpp>

// ---
bool MCHEmul::GraphicalChip::initialize ()
{
	_screenMemory = createScreenMemory ();

	_graphicsReady = false;

	return (_screenMemory != nullptr);
}
