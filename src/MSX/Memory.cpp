#include <MSX/Memory.hpp>

// ---
MSX::Memory::Memory (MSX::MSXModel* m, unsigned int cfg, 
		const std::string& lang)
	: MCHEmul::Memory (0, m -> memoryContent (), { }),
	  _model (m),
	  _configuration (cfg),
	  _ROM (nullptr), _BASICRAM (nullptr), _EMPTYBASICRAM (nullptr),
	  _bank (),
	  _STACK_SUBSET (0) // Initially the version not expanded...
{
	// In the content...
	if (error () != MCHEmul::_NOERROR)
		return;

	bool ok = true;
	assert (physicalStorage (_ROM_SET) != nullptr);
	ok &= _model -> loadROMOverForLanguage (physicalStorage (_ROM_SET), lang);
	subset (_ROMBIOS_SUBSET) -> fixDefaultValues (); // Fix the values in the ROM...
	if (!ok)
		_error = MCHEmul::_INIT_ERROR;

	// The basic elements...
	_ROM = subset (_ROMBIOS_SUBSET);
	_BASICRAM = subset (_RAM16KSLOT0SUBSLOT0_SUBSET);
	_EMPTYBASICRAM = subset (_ERAM16KSLOT0SUBSLOT0_SUBSET);

	// None of them can be nullptr...
	assert (_ROM != nullptr && 
			_BASICRAM != nullptr && 
			_EMPTYBASICRAM != nullptr);

	// Create the internal variable banks to facilitate the access later...
	createBanks ();

	// Sets the configuration of the memory...
	setConfiguration (_configuration);
}

// ---
bool MSX::Memory::initialize ()
{
	bool result = MCHEmul::Memory::initialize ();
	if (!result)
		return (false);

	// TODO

	// The active view has to be initially the CPU view...
	setCPUView ();

	return (true);
}

// ---
void MSX::Memory::createBanks ()
{
	// Organize the elements created by bank
	// See the note in the header file of the Memory class...
	// To organize them, the id of the subset is used...
	// @see Memory class header...
	for (const auto& i : view (_CPU_VIEW) -> subsets ())
	{
		unsigned char slot, sslot, bank;
		getSlotSubSlotAndBank (i.first, slot, sslot, bank);

		// Very block is 16k wide, but the element to insert can be in many banks, depending on the size...
		// For instance the ROM of the BIOS is 32k, so it will be in 2 banks...
		for (size_t j = 0; j < (i.second -> size () / 0x4000); j++)
			_bank [(size_t) slot][(size_t) sslot][(size_t) bank + j].emplace_back (i.second);
	}
}
