#include <COMMODORE/DatasettePort.hpp>

// ---
COMMODORE::DatasetteIOPort::DatasetteIOPort ()
	: MCHEmul::DatasetteIOPort (_ID, 
		{ { "Name", "Datasette Port" },
		  { "Type", "Input/Output" },
		  { "Manufacturer", "Commodore Business Machines CBM" } })
{
	// Nothing else to do...
}
