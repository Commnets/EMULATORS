#include <CORE/GraphicalChip.hpp>

// ---
bool MCHEmul::GraphicalChip::initialize ()
{
	_screenMemory = createScreenMemory ();

	return (_screenMemory != nullptr);
}
