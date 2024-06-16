#include <ZX81/Datasette.hpp>

// ---
ZX81::Datasette::Datasette (unsigned int rS)
	: MCHEmul::BasicDatasette (_ID, rS, false /** Controlled externally. */,
		{ { "Name", "Datasette ZX81" },
		  { "Manufacturer", "Almost anyone, No special connector needed" } })
{
	setClassName ("ZX81Datasette");
}

// ---
ZX81::DatasetteP::DatasetteP (unsigned int mS)
	: MCHEmul::BasicDatasetteP (_ID, mS, false /** Controlled externally. */,
		{ { "Name", "Datasette ZX81" },
		  { "Manufacturer", "Almost anyone, No special connector needed" } })
{
	setClassName ("ZX81Datasette");
}
