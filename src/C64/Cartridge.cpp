#include <C64/Cartridge.hpp>
#include <C64/ExpansionPort.hpp>
#include <C64/Memory.hpp>

// ---
C64::Cartridge::Cartridge ()
	: COMMODORE::ExpansionPeripheral (_ID,
		{ { "Name", "Commodore 64 Cartridge" },
		  { "Manufacturer", "Commodore Business Machines CBM" } }), // This parameters can be changed when connecting data...
	  _dataDumped (false),
	  _memoryRef (nullptr), _memoryView (nullptr),
	  _storages (), _subsets ()
{
	setClassName ("Cartridge");
}

// ---
void C64::Cartridge::configureMemoryStructure (bool ROML, bool ROMH1, bool ROMH2)
{
	// Just in case...
	if (data ()._data.empty ())
		return;

	switch (type ())
	{
		default:
			assert (_subsets.size () == 3);
			_subsets [_EXPANSIONROMBASE + 0] -> setActive (true);
			_subsets [_EXPANSIONROMBASE + 0] -> setActiveForReading (ROML);
			_subsets [_EXPANSIONROMBASE + 1] -> setActive (true);
			_subsets [_EXPANSIONROMBASE + 1] -> setActiveForReading (ROMH1);
			_subsets [_EXPANSIONROMBASE + 2] -> setActive (true);
			_subsets [_EXPANSIONROMBASE + 2] -> setActiveForReading (ROMH2);
			break;
	}
}

// ---
bool C64::Cartridge::PIN_UP (unsigned char nP) const
{
	bool result = true;
	
	if (nP == C64::ExpansionIOPort::_GAME)
		result = data ()._data.empty () 
			? true // When there is no data in the cartridge...
			: MCHEmul::upper ((*data ()._attributes.find ("_GAME")).second) == "YES"; // When it is true, then goes low...
	else if (nP == C64::ExpansionIOPort::_EXROM)
		result = data ()._data.empty () 
			? true // When there is no data in the cartridge...
			: MCHEmul::upper ((*data ()._attributes.find ("_EXROM")).second) == "YES"; // When it is true, then goes low...
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
bool C64::Cartridge::finalize ()
{ 
	cleanUpAdditionalSubsets (); // No memory...
	
	setData ({ }); // No data...
	
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
void C64::Cartridge::dumpDataInto (C64::Memory* m, MCHEmul::MemoryView* mV)
{
	cleanUpAdditionalSubsets ();

	_memoryRef = m;
	_memoryView = mV;

	if (data ()._data.empty ())
		return; // Makes no sense to continue...

	// The structure of the memoy will depend on the type of cartridge...
	switch (type ())
	{
		default:
			{
				_storages = MCHEmul::PhysicalStorages 
					({ { _EXPANSIONROMBASE + 0, new MCHEmul::PhysicalStorage 
							(_EXPANSIONROMBASE + 0, MCHEmul::PhysicalStorage::Type::_ROM, 0x2000) },
					   { _EXPANSIONROMBASE + 1, new MCHEmul::PhysicalStorage 
							(_EXPANSIONROMBASE + 1, MCHEmul::PhysicalStorage::Type::_ROM, 0x2000) },
					   { _EXPANSIONROMBASE + 2, new MCHEmul::PhysicalStorage 
							(_EXPANSIONROMBASE + 2, MCHEmul::PhysicalStorage::Type::_ROM, 0x2000) }
					 });
	
				_subsets = MCHEmul::PhysicalStorageSubsets
					({ { _EXPANSIONROMBASE + 0, new MCHEmul::PhysicalStorageSubset	
							(_EXPANSIONROMBASE + 0, _storages [_EXPANSIONROMBASE + 0], 0x0000, 
								MCHEmul::Address ({ 0x00, 0x80 }, false), 0x2000) },
					   { _EXPANSIONROMBASE + 1, new MCHEmul::PhysicalStorageSubset
							(_EXPANSIONROMBASE + 1, _storages [_EXPANSIONROMBASE + 1], 0x0000, 
								MCHEmul::Address ({ 0x00, 0xa0 }, false), 0x2000) },
					   { _EXPANSIONROMBASE + 2, new MCHEmul::PhysicalStorageSubset
							(_EXPANSIONROMBASE + 2, _storages [_EXPANSIONROMBASE + 2], 0x0000, 
								MCHEmul::Address ({ 0x00, 0xe0 }, false), 0x2000) }
					 });

				// Dump the data into...
				int trush = 0; // Not useful...
				for (const auto& i : data ()._data)
				{
					size_t ct = 0;
					MCHEmul::Address a = i.startAddress ();
					for (const auto& j : i.bytes ())
					{
						MCHEmul::Address dA = a + ct++;
						for (const auto& k : _subsets)
							k.second -> set (dA, j, true /** force. */);
					}
				}
			}

			break;
	}

	// Add the subsets to the view...
	_memoryRef -> addAdditionalSubsets (0 /** always 0. */, _subsets, _memoryView);

	// The subsets are initially disactivated...
	// They will be activated as needed...
	for (const auto& i : _subsets)
		i.second -> setActive (false);

	// The memory is informed that there is now an extension...
	_memoryRef -> setExtensionAt (this);

	_dataDumped = true;
}

// ---
void C64::Cartridge::cleanUpAdditionalSubsets ()
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

	// The memory has no longer a cartridge...
	_memoryRef -> setExtensionAt (nullptr);

	_memoryView = nullptr;
	_memoryRef = nullptr;
}
