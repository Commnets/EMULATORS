#include <C64/Cartridge.hpp>

// ---
C64::Cartridge::Cartridge ()
	: COMMODORE::ExpansionPeripheral (_ID,
		{ { "Name", "Commodore 64 Cartridge" },
		  { "Manufacturer", "Commodore Business Machibes CBM" } }) // This parameters can be changed when connecting data...
{
	setClassName ("Cartridge");
}

// ---
bool C64::Cartridge::_GAME () const
{
	if (_data._data.empty ())
		return (true);
	return (MCHEmul::upper ((*_data._attributes.find ("_GAME")).second) == "YES");
}

// ---
bool C64::Cartridge::_EXROM () const
{
	if (_data._data.empty ())
		return (true);
	return (MCHEmul::upper ((*_data._attributes.find ("_EXROM")).second) == "YES");
}

// ---
bool C64::Cartridge::connectData (MCHEmul::FileData* dt)
{ 
	if (dynamic_cast <COMMODORE::CRTFileData*> (dt) == nullptr)
		return (false); // This type of data can not come from the cartridge...

	_data = dt -> asMemoryBlocks (); 
	
	return (true); 
}
