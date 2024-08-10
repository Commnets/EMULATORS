#include <ZXSpectrum/Cartridge.hpp>
#include <ZXSpectrum/EdgeConnector.hpp>

// ---
ZXSPECTRUM::Cartridge::Cartridge ()
	: ZXSPECTRUM::EdgeConnectorPeripheral (_ID,
		{ { "Name", "Sinclair ZXSpectrum Cartridge" },
		  { "Manufacturer", "Various" } }), // This parameters can be changed when connecting data...
	  _dataDumped (false),
	  _memoryRef (nullptr), _memoryView (nullptr)
{
	setClassName ("Cartridge");
}

// ---
bool ZXSPECTRUM::Cartridge::connectData (MCHEmul::FileData* dt)
{ 
	// TODO: 
	// Check that the format is valid...

	setData (std::move (dt -> asMemoryBlocks ())); 

	setDataJustLoaded ();

	_dataDumped = false;
	
	return (true); 
}

// ---
bool ZXSPECTRUM::Cartridge::finalize ()
{ 
	setData ({ }); // No data...
	
	return (true);
}

// ---
bool ZXSPECTRUM::Cartridge::simulate (MCHEmul::CPU* cpu)
{
	if (!data ()._data.empty () && dataDumped ())
		setPinStatusChanged (); // The pin status has to be notified...

	return (true);
}

// ---
void ZXSPECTRUM::Cartridge::dumpDataInto (ZXSPECTRUM::Memory* m, MCHEmul::MemoryView* mV)
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
