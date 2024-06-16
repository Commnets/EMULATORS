#include <COMMODORE/1530Datasette.hpp>

// ---
COMMODORE::Datasette1530::Datasette1530 (unsigned int rS)
	: MCHEmul::BasicDatasette (_ID, rS, true /** Motor controlled internaly. */,
		{ { "Name", "Commodore 1530 (CN2)" },
		  { "Manufacturer", "Commodore Business Machines CBM" } })
{
	setClassName ("C2N1530");
}

// ---
COMMODORE::Datasette1530P::Datasette1530P (unsigned int mS)
	: MCHEmul::BasicDatasetteP (_ID, mS, true /** Motor controlled internally. */,
		{ { "Name", "Commodore 1530 Parallel (CN2)" },
		  { "Manufacturer", "Commodore Business Machines CBM" } })
{
	setClassName ("C2N1530");
}
