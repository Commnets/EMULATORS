#include <MSX/DatasettePort.hpp>

// ---
MSX::DatasetteIOPort::DatasetteIOPort ()
	: MCHEmul::DatasetteIOPort (_ID,
		{ { "Name", "Datasette Port" },
		  { "Type", "Input/Output" },
		  { "Manufacturer", "MSX" } })
{
	// Physical PSG/PPI cassette lines are not needed by the BIOS trap peripheral.
}
