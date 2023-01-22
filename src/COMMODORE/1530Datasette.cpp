#include <COMMODORE/1530Datasette.hpp>

// ---
COMMODORE::Datasette1530::Datasette1530 ()
	: COMMODORE::DatasettePeripheral (_ID, 
		{ { "Name", "Commodore 1530 (CN2)" },
		  { "Code", "1530" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Year", "1978" } })
{ 
	// TODO
}

// ---
bool COMMODORE::Datasette1530::pinD4 () const
{
	// TODO

	return (false);
}

// ---
void COMMODORE::Datasette1530::pintE5 (bool d)
{
	// TODO
}

//---
bool COMMODORE::Datasette1530::pinF6 () const
{
	// TODO

	return (false);
}

// ---
bool COMMODORE::Datasette1530::initialize ()
{
	// TODO

	return (true);
}

// ---
bool COMMODORE::Datasette1530::simulate ()
{
	// TODO

	return (true);
}
