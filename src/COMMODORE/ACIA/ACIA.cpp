#include <COMMODORE/ACIA/ACIA.hpp>
#include <COMMODORE/ACIA/ACIARegisters.hpp>

// ---
COMMODORE::ACIA::ACIA ()
	: MCHEmul::Chip (_ID, 
		{ { "Name", "6551" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Year", "1984" } }),
	  _ACIARegisters (nullptr)
{
	setClassName ("ACIA");
}

// ---
bool COMMODORE::ACIA::initialize ()
{
	assert (memoryRef () != nullptr);

	if (!MCHEmul::Chip::initialize ())
		return (false);

	// Gets the memory block dedicated to the TED
	if (!(_ACIARegisters = 
		dynamic_cast <COMMODORE::ACIARegisters*> (memoryRef () -> subset (COMMODORE::ACIARegisters::_ACIAREGS_SUBSET))))
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	_ACIARegisters -> initialize ();

	return (true);
}

// ---
bool COMMODORE::ACIA::simulate (MCHEmul::CPU* cpu)
{
	// TODO

	return (true);
}

// ---
MCHEmul::InfoStructure COMMODORE::ACIA::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::Chip::getInfoStructure ());

	result.remove ("Memory"); // This info is not neccesary...
	result.add ("ACIARegisters", std::move (_ACIARegisters -> getInfoStructure ()));

	return (result);
}
