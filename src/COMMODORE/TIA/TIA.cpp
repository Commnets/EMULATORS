#include <COMMODORE/TIA/TIA.hpp>
#include <COMMODORE/TIA/TIARegisters.hpp>

// ---
COMMODORE::TIA::TIA ()
	: MCHEmul::Chip (_ID, 
		{ { "Name", "6523" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Year", "1984" } }),
	  _TIARegisters (nullptr)
{
	setClassName ("TIA");
}

// ---
bool COMMODORE::TIA::initialize ()
{
	assert (memoryRef () != nullptr);

	if (!MCHEmul::Chip::initialize ())
		return (false);

	// Gets the memory block dedicated to the TED
	if (!(_TIARegisters = 
		dynamic_cast <COMMODORE::TIARegisters*> (memoryRef () -> subset (COMMODORE::TIARegisters::_TIAREGS_SUBSET))))
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	_TIARegisters -> initialize ();

	return (true);
}

// ---
bool COMMODORE::TIA::simulate (MCHEmul::CPU* cpu)
{
	// TODO

	return (true);
}

// ---
MCHEmul::InfoStructure COMMODORE::TIA::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::Chip::getInfoStructure ());

	result.remove ("Memory"); // This info is not neccesary...
	result.add ("TIARegisters", std::move (_TIARegisters -> getInfoStructure ()));

	return (result);
}
