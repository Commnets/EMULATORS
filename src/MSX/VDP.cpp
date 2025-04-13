#include <MSX/VDP.hpp>

// ---
MSX::VDP::VDP (MCHEmul::GraphicalChip* gC)
	: _graphicalChip (gC)
{ 
	assert (_graphicalChip != nullptr); 
}
