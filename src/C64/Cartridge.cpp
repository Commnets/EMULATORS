#include <C64/Cartridge.hpp>
#include <C64/ExpansionPort.hpp>
#include <C64/Memory.hpp>

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

// ---
void C64::Cartridge::configureMemoryAndLoadData (C64::Memory* m)
{
	bool lR = true, hR = true, cR = true;
	for (const auto& i : _data._data)
	{
		if (std::abs ((C64::Memory::_BASICROMEND_ADDRESS - i.startAddress ())) <=
			std::abs (((int) i.size () + (C64::Memory::_BASICROMEND_ADDRESS - C64::Memory::_BASICROMINIT_ADDRESS + 1)))) 
			lR = false;
		if (std::abs ((C64::Memory::_CHARROMEND_ADDRESS - i.startAddress ())) <=
			std::abs (((int) i.size () + (C64::Memory::_CHARROMEND_ADDRESS - C64::Memory::_CHARROMINIT_ADDRESS + 1)))) 
			hR = false;
		if (std::abs ((C64::Memory::_KERNELROMEND_ADDRESS - i.startAddress ())) <=
			std::abs (((int) i.size () + (C64::Memory::_KERNELROMEND_ADDRESS - C64::Memory::_KERNELROMINIT_ADDRESS + 1)))) 
			cR = false;
	}

	// Configure the memory...
	MCHEmul::UByte dr = m -> value (C64::Memory::_POS1_ADDRESS);
	dr.setBit (0, lR); dr.setBit (1, hR); dr.setBit (2, cR);
	m -> set (C64::Memory::_POS1_ADDRESS, dr); // To keep the data in the registers...
	m -> configureMemoryAccess (lR, hR, cR); // But the configuration is now needed...

	// ...and keep the data
	for (const auto& i : _data._data)
		m -> set (i.startAddress (), i.bytes ());
}
