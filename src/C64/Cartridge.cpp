#include <C64/Cartridge.hpp>
#include <C64/ExpansionPort.hpp>
#include <C64/Memory.hpp>

// ---
C64::Cartridge::Cartridge ()
	: COMMODORE::ExpansionPeripheral (_ID,
		{ { "Name", "Commodore 64 Cartridge" },
		  { "Manufacturer", "Commodore Business Machines CBM" } }), // This parameters can be changed when connecting data...
	  _ultimax (true),
	  _dataDumped (false),
	  _memoryRef (nullptr), 
	  _memoryCPUView (nullptr), _memoryVICIIView (nullptr),
	  _storages (), 
	  _cpuSubsets (), _viciiSubsets ()
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
			{
				assert (_cpuSubsets.size () == 4);
				_cpuSubsets [_EXPANSIONROMBASE_SUBSET + 0] -> setActive (true);
				_cpuSubsets [_EXPANSIONROMBASE_SUBSET + 0] -> setActiveForReading (ROML);
				_cpuSubsets [_EXPANSIONROMBASE_SUBSET + 1] -> setActive (true);
				_cpuSubsets [_EXPANSIONROMBASE_SUBSET + 1] -> setActiveForReading (ROMH1);
				_cpuSubsets [_EXPANSIONROMBASE_SUBSET + 2] -> setActive (true);
				_cpuSubsets [_EXPANSIONROMBASE_SUBSET + 2] -> setActiveForReading (ROMH2);
				_cpuSubsets [_EXPANSIONROMBASE_SUBSET + 3] -> setActive (true);
				_cpuSubsets [_EXPANSIONROMBASE_SUBSET + 3] -> setActiveForReading (ROMH2);

				assert (_ultimax == ROMH2); // Just in case
				if (_ultimax)
				{
					_viciiSubsets [_EXPANSIONROMBASEI_SUBSET + 0] -> setActive (true);
					_viciiSubsets [_EXPANSIONROMBASEI_SUBSET + 0] -> setActiveForReading (true);
					_viciiSubsets [_EXPANSIONROMBASEI_SUBSET + 1] -> setActive (true);
					_viciiSubsets [_EXPANSIONROMBASEI_SUBSET + 1] -> setActiveForReading (true);
					_viciiSubsets [_EXPANSIONROMBASEI_SUBSET + 2] -> setActive (true);
					_viciiSubsets [_EXPANSIONROMBASEI_SUBSET + 2] -> setActiveForReading (true);
					_viciiSubsets [_EXPANSIONROMBASEI_SUBSET + 3] -> setActive (true);
					_viciiSubsets [_EXPANSIONROMBASEI_SUBSET + 3] -> setActiveForReading (true);
				}
			}

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

	setData (std::move (dt -> asMemoryBlocks ())); 
	_ultimax = 
		(*_data._attributes.find ("_EXROM")).second == "YES" &&
		(*_data._attributes.find ("_GAME")).second == "NO"; // ultimax data?

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
void C64::Cartridge::dumpDataInto (C64::Memory* m, 
	MCHEmul::MemoryView* cV, MCHEmul::MemoryView* vV)
{
	cleanUpAdditionalSubsets ();

	_memoryRef = m;
	_memoryCPUView = cV;
	_memoryVICIIView = vV;

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
	
				MCHEmul::PhysicalStorageSubset *ss0, *ss1, *ss2, *ss3;
				_cpuSubsets = MCHEmul::PhysicalStorageSubsets
					({ { _EXPANSIONROMBASE_SUBSET + 0, ss0 = new MCHEmul::PhysicalStorageSubset	
							(_EXPANSIONROMBASE_SUBSET + 0, _storages [_EXPANSIONROMBASE + 0], 0x0000, 
								MCHEmul::Address ({ 0x00, 0x80 }, false), 0x2000) },
					   { _EXPANSIONROMBASE_SUBSET + 1, ss1 = new MCHEmul::PhysicalStorageSubset
							(_EXPANSIONROMBASE_SUBSET + 1, _storages [_EXPANSIONROMBASE + 1], 0x0000, 
								MCHEmul::Address ({ 0x00, 0xa0 }, false), 0x2000) },
					   { _EXPANSIONROMBASE_SUBSET + 2, ss2 = new MCHEmul::PhysicalStorageSubset
							(_EXPANSIONROMBASE_SUBSET + 2, _storages [_EXPANSIONROMBASE + 2], 0x0000, 
								MCHEmul::Address ({ 0x00, 0xe0 }, false), 0x1000) },
					   { _EXPANSIONROMBASE_SUBSET + 3, ss3 = new MCHEmul::PhysicalStorageSubset
							(_EXPANSIONROMBASE_SUBSET + 3, _storages [_EXPANSIONROMBASE + 2], 0x1000, 
								MCHEmul::Address ({ 0x00, 0xf0 }, false), 0x1000) }
					 });

				// To set them with a name
				ss0 -> setName ("ROM LO");
				ss1 -> setName ("ROM HI1");
				ss2 -> setName ("ROM HI2A");
				ss3 -> setName ("ROM HI2B");

				// Is it an ULTIMAX cartridge?
				if (_ultimax)
				{
					MCHEmul::PhysicalStorageSubset *sv0, *sv1, *sv2, *sv3;
					_viciiSubsets = MCHEmul::PhysicalStorageSubsets
						({ { _EXPANSIONROMBASEI_SUBSET + 0, sv0 = new MCHEmul::MirrorPhysicalStorageSubset
								(_EXPANSIONROMBASEI_SUBSET + 0, ss3, MCHEmul::Address ({ 0x00, 0x30 }, false)) },
						   { _EXPANSIONROMBASEI_SUBSET + 1, sv1 = new MCHEmul::MirrorPhysicalStorageSubset
								(_EXPANSIONROMBASEI_SUBSET + 1, ss3, MCHEmul::Address ({ 0x00, 0x70 }, false)) },
						   { _EXPANSIONROMBASEI_SUBSET + 2, sv2 = new MCHEmul::MirrorPhysicalStorageSubset
								(_EXPANSIONROMBASEI_SUBSET + 2, ss3, MCHEmul::Address ({ 0x00, 0xb0 }, false)) },
						   { _EXPANSIONROMBASEI_SUBSET + 3, sv3 = new MCHEmul::MirrorPhysicalStorageSubset
								(_EXPANSIONROMBASEI_SUBSET + 3, ss3, MCHEmul::Address ({ 0x00, 0xf0 }, false)) }
						});

					// To set them with a name
					sv0 -> setName ("Bank 0 HI1 Image Ultimax");
					sv1 -> setName ("Bank 1 HI1 Image Ultimax");
					sv2 -> setName ("Bank 2 HI1 Image Ultimax");
					sv3 -> setName ("Bank 3 HI1 Image Ultimax");
				}

				// Dump the data into...
				for (const auto& i : data ()._data)
				{
					size_t ct = 0;
					MCHEmul::Address a = i.startAddress ();
					for (const auto& j : i.bytes ())
					{
						MCHEmul::Address dA = a + ct++;
						for (const auto& k : _cpuSubsets)
							k.second -> set (dA, j, true /** force. */);
					}
				}
			}

			break;
	}

	// Add the subsets to the view...
	_memoryRef -> addAdditionalSubsets (0, _cpuSubsets, _memoryCPUView);
	_memoryRef -> addAdditionalSubsets (1, _viciiSubsets, _memoryVICIIView);

	// The subsets are initially disactivated...
	// They will be activated as needed, when PLA determines the configuration...
	for (const auto& i : _cpuSubsets)
		i.second -> setActive (false);
	for (const auto& i : _viciiSubsets)
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
	_memoryRef -> removeAdditionalSubsets (0, _memoryCPUView);
	_memoryRef -> removeAdditionalSubsets (1, _memoryVICIIView);

	// Delete the subsets and the storages...
	for (const auto& i : _cpuSubsets)
		delete (i.second);
	_cpuSubsets = { };
	for (const auto& i : _viciiSubsets)
		delete (i.second);
	_viciiSubsets = { };
	for (const auto& i : _storages)
		delete (i.second);
	_storages = { };

	// The memory has no longer a cartridge...
	_memoryRef -> setExtensionAt (nullptr);

	_memoryCPUView = nullptr;
	_memoryVICIIView = nullptr;
	_memoryRef = nullptr;
}
