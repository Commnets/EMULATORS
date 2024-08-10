#include <ZXSpectrum/DatasettePort.hpp>

// ---
ZXSPECTRUM::DatasetteIOPort::DatasetteIOPort ()
	: MCHEmul::DatasetteIOPort (_ID,
		{ { "Name", "Datasette Port" },
		  { "Type", "Input/Output" },
		  { "Manufacturer", "Sinclair Research/Timex Coporation" } })
{
	// Nothing else to do...
}
