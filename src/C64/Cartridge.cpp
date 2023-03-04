#include <C64/Cartridge.hpp>
#include <C64/ExpansionPort.hpp>
#include <C64/Memory.hpp>

// ---
C64::Cartridge::Cartridge ()
	: COMMODORE::ExpansionPeripheral (_ID,
		{ { "Name", "Commodore 64 Cartridge" },
		  { "Manufacturer", "Commodore Business Machibes CBM" } }), // This parameters can be changed when connecting data...
	  _dataDumped (false)
{
	setClassName ("Cartridge");
}

// ---
bool C64::Cartridge::PIN_UP (unsigned char nP) const
{
	bool result = true;
	
	if (nP == C64::ExpansionIOPort::_GAME)
		result = MCHEmul::upper ((*_data._attributes.find ("_GAME")).second) == "YES"; // When it is true, then goes low...
	else if (nP == C64::ExpansionIOPort::_EXROM)
		result = MCHEmul::upper ((*_data._attributes.find ("_EXROM")).second) == "YES"; // When it is true, then goes low...
	else 
		result = COMMODORE::ExpansionPeripheral::PIN_UP (nP);

	return (result);
}

// ---
bool C64::Cartridge::connectData (MCHEmul::FileData* dt)
{ 
	if (dynamic_cast <COMMODORE::CRTFileData*> (dt) == nullptr)
		return (false); // This type of data can not come from the cartridge...

	setData (dt -> asMemoryBlocks ()); 

	setDataJustLoaded ();

	_dataDumped = false;
	
	return (true); 
}

// ---
bool C64::Cartridge::simulate (MCHEmul::CPU* cpu)
{
	if (!data ()._data.empty () && dataDumped ())
		setPinStatusChanged (); // The pin status has to be notified...

	return (true);
}

// ---
void C64::Cartridge::dumpDataInto (MCHEmul::PhysicalStorageSubset* eL,
	MCHEmul::PhysicalStorageSubset* eH1, MCHEmul::PhysicalStorageSubset* eH2)
{
	_dataDumped = true;
}
