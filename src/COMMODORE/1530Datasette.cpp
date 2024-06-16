#include <COMMODORE/1530Datasette.hpp>

// ---
COMMODORE::Datasette1530::Datasette1530 (unsigned int rS)
	: MCHEmul::BasicDatasette (_ID, rS,
		{ { "Name", "Commodore 1530 (CN2)" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Commands", "1:FORWARD, 2:REWIND, 4:STOP, 8:PLAY, 24:PLAY + RECORD, 32:EJECT(and clear data)" } })
{
	setClassName ("C2N1530");
}

// ---
COMMODORE::Datasette1530P::Datasette1530P (unsigned int mS)
	: MCHEmul::BasicDatasetteP (_ID, mS,
		{ { "Name", "Commodore 1530 Parallel (CN2)" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Commands", "1:FORWARD, 2:REWIND, 4:STOP, 8:PLAY, 24:PLAY + RECORD, 32:EJECT(and clear data)" } })
{
	setClassName ("C2N1530");
}
