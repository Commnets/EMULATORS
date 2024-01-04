#include <VIC20/Cartridge.hpp>
#include <VIC20/ExpansionPort.hpp>

// ---
VIC20::Cartridge::Cartridge ()
	: COMMODORE::ExpansionPeripheral (_ID,
		{ { "Name", "Commodore VIC20 Cartridge" },
		  { "Manufacturer", "Commodore Business Machines CBM" } }), // This parameters can be changed when connecting data...
	  _dataDumped (false),
	  _memoryRef (nullptr), _memoryView (nullptr),
	  _storages (), _subsets ()
{
	setClassName ("Cartridge");
}

// ---
bool VIC20::Cartridge::PIN_UP (unsigned char nP) const
{
	bool result = true;
	
	// TODO
	
	result = COMMODORE::ExpansionPeripheral::PIN_UP (nP);

	return (result);
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
	cleanUpAdditionalSubsets (); // No memory...
	
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
	cleanUpAdditionalSubsets ();

	_memoryRef = m;
	_memoryView = mV;

	if (data ()._data.empty ())
		return; // Makes no sense to continue...

	// The structure of the memoy will depend on the type of cartridge...
	switch (type ())
	{
		// TODO
		
		default:
			break;
	}

	// Add the subsets to the view...
	_memoryRef -> addAdditionalSubsets (0 /** always 0. */, _subsets, _memoryView);

	// The subsets are initially disactivated...
	// They will be activated as needed...
	for (const auto& i : _subsets)
		i.second -> setActive (false);

	_dataDumped = true;
}

// ---
void VIC20::Cartridge::cleanUpAdditionalSubsets ()
{
	if (_memoryRef == nullptr)
		return; // Nothing else to do...
				// There is no data inside...

	// The memory takes no longer into account the subset...
	_memoryRef -> removeAdditionalSubsets (0 /** The same 0. */, _memoryView);

	// Delete the subsets and the storages...
	for (const auto& i : _subsets)
		delete (i.second);
	_subsets = { };
	for (const auto& i : _storages)
		delete (i.second);
	_storages = { };

	_memoryView = nullptr;
	_memoryRef = nullptr;
}
