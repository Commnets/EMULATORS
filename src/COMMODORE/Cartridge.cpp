#include <COMMODORE/Cartridge.hpp>
#include <COMMODORE/FileReaders.hpp>

// ---
COMMODORE::Cartridge::Cartridge ()
	: COMMODORE::ExpansionPeripheral (_ID,
		{ { "Name", "Commodore Cartridge" },
		  { "Manufacturer", "Commodore Business Machibes CBM" } }) // This parameters can be changed when connecting data...
{
	setClassName ("Cartridge");
}
