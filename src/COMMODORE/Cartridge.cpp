#include <COMMODORE/Cartridge.hpp>

// ---
COMMODORE::Cartridge::Cartridge ()
	: COMMODORE::ExpansionPeripheral (_ID,
		{ { "Name", "Commodore Cartridge" },
		  { "Manufacturer", "Commodore Business Machibes CBM" } }), // This parameters can be changed when connecting data...
	  _cartridgeData (nullptr),
	  _restartData (true)
{
	setClassName ("Cartridge");
}

// ---
bool COMMODORE::Cartridge::connectData (MCHEmul::FileData* dt)
{
	// The old data has to be forgotten...
	delete (_cartridgeData);
	
	_cartridgeData = dynamic_cast <COMMODORE::CRTFileData*> (dt);

	// Returns true when a new data has been connected and false in other case...
	return ((_restartData = (_cartridgeData != nullptr)));
}

// ---
bool COMMODORE::Cartridge::simulate (MCHEmul::CPU* cpu)
{ 
	if (!_restartData)
		return (true); // Nothing to do, but everything ok...

	// TODO

	return (true);
} 
