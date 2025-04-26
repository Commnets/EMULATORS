#include <MSX/VDP.hpp>

// ---
MSX::VDP::VDP (MCHEmul::GraphicalChip* gC)
	: _graphicalChip (gC)
{ 
	assert (_graphicalChip != nullptr); 
}

// ---
MCHEmul::InfoStructure MSX::VDP::getInfoStructure () const
{ 
	// Limit the number of bytes to print out...
	// Because in the case of a MSX it could be huge!
	size_t oL = MCHEmul::PhysicalStorageSubset::_GETINFOBYTESLIMIT;
	MCHEmul::PhysicalStorageSubset::_GETINFOBYTESLIMIT = 1024;
	MCHEmul::InfoStructure result = _graphicalChip -> getInfoStructure ();
	MCHEmul::PhysicalStorageSubset::_GETINFOBYTESLIMIT = oL;

	return (result); 
}
