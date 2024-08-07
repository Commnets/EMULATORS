#include <COMMODORE/C6529B/C6529B.hpp>
#include <COMMODORE/C6529B/C6529BRegisters.hpp>

// ---
COMMODORE::C6529B::C6529B (int id)
	: MCHEmul::Chip (id, 
		{ { "Name", "6529B" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Year", "1984" } }),
	  _C6529BRegisters (nullptr)
{
	setClassName ("6529B");
}

// ---
bool COMMODORE::C6529B::initialize ()
{
	assert (memoryRef () != nullptr);

	if (!MCHEmul::Chip::initialize ())
		return (false);

	// Gets the memory block dedicated to this register...
	if (!(_C6529BRegisters = 
		dynamic_cast <COMMODORE::C6529BRegisters*> 
			(memoryRef () -> subset (COMMODORE::C6529BRegisters::_C6529BREGS_SUBSET))))
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	_C6529BRegisters -> initialize ();

	return (true);
}

// ---
MCHEmul::InfoStructure COMMODORE::C6529B::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::Chip::getInfoStructure ());

	result.remove ("Memory"); // This info is not neccesary...
	result.add ("C6529Registers", std::move (_C6529BRegisters -> getInfoStructure ()));

	return (result);
}
