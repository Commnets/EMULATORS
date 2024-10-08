#include <VIC20/Cartridge.hpp>
#include <VIC20/ExpansionPort.hpp>

// ---
VIC20::Cartridge::Cartridge ()
	: COMMODORE::ExpansionPeripheral (_ID,
		{ { "Name", "Commodore VIC20 Cartridge" },
		  { "Manufacturer", "Commodore Business Machines CBM" } }), // This parameters can be changed when connecting data...
	  _dataDumped (false),
	  _memoryRef (nullptr), _memoryView (nullptr)
{
	setClassName ("Cartridge");
}

// ---
bool VIC20::Cartridge::connectData (MCHEmul::FileData* dt)
{ 
	if (dynamic_cast <COMMODORE::CRTFileData*> (dt) == nullptr)
		return (false); // This type of data can not come from the cartridge...

	setData (std::move (dt -> asMemoryBlocks ())); 

	setDataJustLoaded ();

	_dataDumped = false;
	
	return (true); 
}

// ---
bool VIC20::Cartridge::finalize ()
{ 
	setData ({ }); // No data...
	
	return (true);
}

// ---
bool VIC20::Cartridge::simulate (MCHEmul::CPU* cpu)
{
	if (!data ()._data.empty () && dataDumped ())
		setPinStatusChanged (); // The pin status has to be notified...

	return (true);
}

// ---
void VIC20::Cartridge::dumpDataInto (VIC20::Memory* m, MCHEmul::MemoryView* mV)
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
				m -> setConfiguration ((unsigned char) 
					std::atoi ((*data ()._attributes.find ("TYPE")).second.c_str ()));

				// Dump the data into...
				int trush = 0; // Not useful...
				for (const auto& i : data ()._data)
				{
					size_t ct = 0;
					MCHEmul::Address a = i.startAddress ();
					for (const auto& j : i.bytes ())
					{
						MCHEmul::Address dA = a + ct++;
						m -> put (dA, j, true /** force. */);
					}
				}
			}

			break;
	}

	_dataDumped = true;
}
