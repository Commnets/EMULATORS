#include <COMMODORE/ExpansionPeripherals.hpp>
#include <COMMODORE/FileReaders.hpp>

// ---
bool COMMODORE::ExpansionPeripheral::connectData (MCHEmul::FileData* dt)
{ 
	if (dynamic_cast <COMMODORE::CRTFileData*> (dt) == nullptr)
		return (false); // This type of data can not come from the cartridge...

	_data = dt -> asMemoryBlocks (); 
	
	return (true); 
}
