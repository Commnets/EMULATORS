#include <ZX81/Datasette.hpp>

// ---
ZX81::Datasette::Datasette (unsigned int rS)
	: MCHEmul::BasicDatasette (_ID, rS,
		{ { "Name", "Datasette ZX81" },
		  { "Manufacturer", "Almost anyone, No special connector needed" },
		  { "Commands", "1:FORWARD, 2:REWIND, 4:STOP, 8:PLAY, 24:PLAY + RECORD, 32:EJECT(and clear data)" } })
{
	setClassName ("ZX81Datasette");
}

// ---
ZX81::DatasetteP::DatasetteP (unsigned int mS)
	: MCHEmul::BasicDatasetteP (_ID, mS,
		{ { "Name", "Datasette ZX81" },
		  { "Manufacturer", "Almost anyone, No special connector needed" },
		  { "Commands", "1:FORWARD, 2:REWIND, 4:STOP, 8:PLAY, 24:PLAY + RECORD, 32:EJECT(and clear data)" } })
{
	setClassName ("ZX81Datasette");
}
