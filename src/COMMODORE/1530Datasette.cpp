#include <COMMODORE/1530Datasette.hpp>

// ---
COMMODORE::Datasette1530::Datasette1530 ()
	: COMMODORE::DatasettePeripheral (_ID, 
		{ { "Name", "Commodore 1530 (CN2)" },
		  { "Manufacturer", "Commodore Business Machines CBM" } })
{
	// Nothing else to do...
}

// ---
bool COMMODORE::Datasette1530::executeCommand (int id, const MCHEmul::Strings& prms)
{
	// TODO

	return (false);
}

// ---
bool COMMODORE::Datasette1530::simulate (MCHEmul::CPU* cpu)
{
	// TODO

	return (true);
}
