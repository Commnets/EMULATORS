#include <MSX/Memory.hpp>

// ---
MSX::Memory::Memory (MSX::MSXModel* m, unsigned int cfg, 
		const std::string& lang)
	: MCHEmul::Memory (0, m -> memoryContent (), { }),
	  _model (m),
	  _configuration (cfg),
	  _STACK_SUBSET (0) // Initially the version not expanded...
{
	// In the content...
	if (error () != MCHEmul::_NOERROR)
		return;

	bool ok = true;
	assert (physicalStorage (_ROMBIOS_SET) != nullptr);
	ok &= _model -> loadROMOverForLanguage (physicalStorage (_ROMBIOS_SET), lang);
	subset (_ROMBIOS_SUBSET) -> fixDefaultValues (); // Fix the values in the ROM...
	if (!ok)
		_error = MCHEmul::_INIT_ERROR;

	// Sets the configuration of the memory...
	setConfiguration (_configuration);
}

// ---
void MSX::Memory::setConfiguration (unsigned int cfg)
{
	// TODO
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
