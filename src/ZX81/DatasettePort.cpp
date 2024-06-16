#include <ZX81/DatasettePort.hpp>

// ---
ZX81::DatasetteIOPort::DatasetteIOPort ()
	: MCHEmul::DatasetteIOPort (_ID,
		{ { "Name", "Datasette Port" },
		  { "Type", "Input/Output" },
		  { "Manufacturer", "Sinclair Research/Timex Coporation" } })
{
	// Nothing else to do...
}
