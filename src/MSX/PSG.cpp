#include <MSX/PSG.hpp>

// ---
MSX::PSG::PSG (MCHEmul::SoundChip* sC)
	: _soundChip (sC)
{ 
	assert (_soundChip != nullptr); 
}

// ---
MCHEmul::InfoStructure MSX::PSG::getInfoStructure () const
{ 
	// Limit the number of bytes to print out...
	// Because in the case of a MSX it could be huge!
	size_t oL = MCHEmul::PhysicalStorageSubset::_GETINFOBYTESLIMIT;
	MCHEmul::PhysicalStorageSubset::_GETINFOBYTESLIMIT = 1024;
	MCHEmul::InfoStructure result = _soundChip -> getInfoStructure ();
	MCHEmul::PhysicalStorageSubset::_GETINFOBYTESLIMIT = oL;

	return (result); 
}
