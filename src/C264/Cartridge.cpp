#include <C264/Cartridge.hpp>
#include <C264/ExpansionPort.hpp>

// ---
C264::Cartridge::Cartridge ()
	: COMMODORE::ExpansionPeripheral (_ID,
		{ { "Name", "Commodore C264 Cartridge" },
		  { "Manufacturer", "Commodore Business Machines CBM" } }), // This parameters can be changed when connecting data...
	  _dataDumped (false),
	  _memoryRef (nullptr), _memoryView (nullptr)
{
	setClassName ("Cartridge");
}

// ---
bool C264::Cartridge::connectData (MCHEmul::FileData* dt)
{ 
	if (dynamic_cast <COMMODORE::CRTFileData*> (dt) == nullptr)
		return (false); // This type of data can not come from the cartridge...

	setData (std::move (dt -> asMemoryBlocks ())); 

	setDataJustLoaded ();

	_dataDumped = false;
	
	return (true); 
}

// ---
bool C264::Cartridge::finalize ()
{ 
	setData ({ }); // No data...
	
	return (true);
}

// ---
bool C264::Cartridge::simulate (MCHEmul::CPU* cpu)
{
	if (!data ()._data.empty () && dataDumped ())
		setPinStatusChanged (); // The pin status has to be notified...

	return (true);
}

// ---
void C264::Cartridge::dumpDataInto (C264::Memory* m, MCHEmul::MemoryView* mV)
{
	_memoryRef = m;
	_memoryView = mV;

	if (data ()._data.empty ())
		return; // Makes no sense to continue...

	// The structure of the memoy will depend on the type of cartridge...
	switch (type ())
	{
		// The default support all formats!
		default:
			{
				// TODO
			}

			break;
	}

	_dataDumped = true;
}
