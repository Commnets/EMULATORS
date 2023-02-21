#include <COMMODORE/Cartridge.hpp>
#include <COMMODORE/FileReaders.hpp>

// ---
COMMODORE::Cartridge::Cartridge ()
	: COMMODORE::ExpansionPeripheral (_ID,
		{ { "Name", "Commodore Cartridge" },
		  { "Manufacturer", "Commodore Business Machibes CBM" } }), // This parameters can be changed when connecting data...
	  _cartridgeData ()
{
	setClassName ("Cartridge");
}

// ---
bool COMMODORE::Cartridge::connectData (MCHEmul::FileData* dt)
{
	COMMODORE::CRTFileData* cDT = nullptr;
	if (dt == nullptr ||
		(cDT = dynamic_cast <COMMODORE::CRTFileData*> (dt)) == nullptr)
	{
		_cartridgeData = { };

		return (true);
	}

	_cartridgeData._type	= cDT -> _cartridgeType;
	_cartridgeData._version = cDT -> _cartridgeVersion;
	_cartridgeData._name	= cDT -> _name;
	for (const auto& i : cDT -> _chipsData)
		_cartridgeData._memoryBlocks.emplace_back 
			(MCHEmul::DataMemoryBlock (i._startingLoadAddress, i._content.bytes ()));

	return (true);
}
