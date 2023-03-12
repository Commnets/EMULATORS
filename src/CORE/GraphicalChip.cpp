#include <CORE/GraphicalChip.hpp>

// ---
bool MCHEmul::GraphicalChip::initialize ()
{
	if (_screenMemory != nullptr)
		delete (_screenMemory); // When reinit e.g....

	_screenMemory = createScreenMemory ();

	return (_screenMemory != nullptr);
}
