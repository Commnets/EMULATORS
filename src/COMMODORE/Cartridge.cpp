#include <COMMODORE/Cartridge.hpp>

// ---
COMMODORE::Cartridge::Cartridge ()
	: COMMODORE::ExpansionPeripheral (_ID,
		{ { "Name", "Commodore Cartridge" },
		  { "Manufacturer", "Commodore Business Machibes CBM" } }), // This parameters can be changed when connecting data...
	  _fileData (nullptr)
{
	setClassName ("CARTRIDGE");
}

// ---
bool COMMODORE::Cartridge::connectData (MCHEmul::FileData* dt)
{
	// TODO

	return (false);
}
