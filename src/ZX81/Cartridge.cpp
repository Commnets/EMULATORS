#include <ZX81/Cartridge.hpp>
#include <ZX81/EdgeConnector.hpp>

// ---
ZX81::Cartridge::Cartridge ()
	: ZX81::EdgeConnectorPeripheral (_ID,
		{ { "Name", "Sinclair ZX80 Cartridge" },
		  { "Manufacturer", "Various" } }), // This parameters can be changed when connecting data...
	  _dataDumped (false),
	  _memoryRef (nullptr), _memoryView (nullptr)
{
	setClassName ("Cartridge");
}

// ---
bool ZX81::Cartridge::connectData (MCHEmul::FileData* dt)
{ 
	// TODO: 
	// Check that the format is valid...

	setData (std::move (dt -> asMemoryBlocks ())); 

	setDataJustLoaded ();

	_dataDumped = false;
	
	return (true); 
}

// ---
bool ZX81::Cartridge::finalize ()
{ 
	setData ({ }); // No data...
	
	return (true);
}

// ---
bool ZX81::Cartridge::simulate (MCHEmul::CPU* cpu)
{
	if (!data ()._data.empty () && dataDumped ())
		setPinStatusChanged (); // The pin status has to be notified...

	return (true);
}

// ---
void ZX81::Cartridge::dumpDataInto (ZX81::Memory* m, MCHEmul::MemoryView* mV)
{
	_memoryRef = m;
	_memoryView = mV;

	if (data ()._data.empty ())
		return; // Makes no sense to continue...

	// The structure of the memoy will depend on the type of cartridge...
	switch (type ())
	{
		// TODO
		// Possible to configure the memory accordinly...

		default:
			break;
	}

	_dataDumped = true;
}
