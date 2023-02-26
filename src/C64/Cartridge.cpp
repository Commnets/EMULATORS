#include <C64/Cartridge.hpp>
#include <C64/ExpansionPort.hpp>

// ---
C64::Cartridge::Cartridge ()
	: COMMODORE::ExpansionPeripheral (_ID,
		{ { "Name", "Commodore 64 Cartridge" },
		  { "Manufacturer", "Commodore Business Machibes CBM" } }) // This parameters can be changed when connecting data...
{
	setClassName ("Cartridge");
}

// ---
bool C64::Cartridge::PIN_UP (unsigned char nP) const
{
	bool result = false;
	
	if (nP == C64::ExpansionIOPort::_GAME)
		result = MCHEmul::upper ((*_data._attributes.find ("_GAME")).second) == "YES";
	else if (nP == C64::ExpansionIOPort::_EXROM)
		result = MCHEmul::upper ((*_data._attributes.find ("_EXROM")).second) == "YES";

	return (result);
}

// ---
bool C64::Cartridge::connectData (MCHEmul::FileData* dt)
{ 
	if (dynamic_cast <COMMODORE::CRTFileData*> (dt) == nullptr)
		return (false); // This type of data can not come from the cartridge...

	_data = dt -> asMemoryBlocks (); 

	_dataJustLoaded = true;
	
	return (true); 
}
