#include <C64/Cartridge.hpp>
#include <C64/ExpansionPort.hpp>
#include <C64/Memory.hpp>
#include <C64/IOExpansionMemory.hpp>

// ---
C64::Cartridge::Cartridge ()
	: COMMODORE::ExpansionPeripheral (_ID,
		{ { "Name", "Commodore 64 Cartridge" },
		  { "Manufacturer", "Commodore Business Machines CBM" } }), // This parameters can be changed when connecting data...
	  _ultimax (false),
	  _dataDumped (false),
	  _memoryRef (nullptr), 
	  _memoryCPUView (nullptr), _memoryVICIIView (nullptr),
	  _storages (), 
	  _cpuSubsets (), _viciiSubsets (),
	  _activeBank (0), _bankActive (false)
{
	setClassName ("Cartridge");
}

// ---
void C64::Cartridge::configureMemoryStructure (bool romL, bool romH1, bool romH2CPU, bool romH2VIC)
{
	// Just in case...
	if (data ()._data.empty ())
		return;

	switch (type ())
	{
		// Normal type (incuding ULTIMAX)
		case C64::Cartridge::Type::_GENERIC:
			{
				assert (_cpuSubsets.size () == 4);

				_cpuSubsets [_EXPANSIONROMBASE_SUBSET + 0] -> setActive (romL);
				_cpuSubsets [_EXPANSIONROMBASE_SUBSET + 0] -> setActiveForReading (romL);
				_cpuSubsets [_EXPANSIONROMBASE_SUBSET + 1] -> setActive (romH1);
				_cpuSubsets [_EXPANSIONROMBASE_SUBSET + 1] -> setActiveForReading (romH1);
				_cpuSubsets [_EXPANSIONROMBASE_SUBSET + 2] -> setActive (romH2CPU);
				_cpuSubsets [_EXPANSIONROMBASE_SUBSET + 2] -> setActiveForReading (romH2CPU);
				_cpuSubsets [_EXPANSIONROMBASE_SUBSET + 3] -> setActive (romH2CPU);
				_cpuSubsets [_EXPANSIONROMBASE_SUBSET + 3] -> setActiveForReading (romH2CPU);

				if (_ultimax && // double check...
					!_viciiSubsets.empty ())
				{
					_viciiSubsets [_EXPANSIONROMBASEI_SUBSET + 0] -> setActive (romH2VIC);
					_viciiSubsets [_EXPANSIONROMBASEI_SUBSET + 0] -> setActiveForReading (romH2VIC);
					_viciiSubsets [_EXPANSIONROMBASEI_SUBSET + 1] -> setActive (romH2VIC);
					_viciiSubsets [_EXPANSIONROMBASEI_SUBSET + 1] -> setActiveForReading (romH2VIC);
					_viciiSubsets [_EXPANSIONROMBASEI_SUBSET + 2] -> setActive (romH2VIC);
					_viciiSubsets [_EXPANSIONROMBASEI_SUBSET + 2] -> setActiveForReading (romH2VIC);
					_viciiSubsets [_EXPANSIONROMBASEI_SUBSET + 3] -> setActive (romH2VIC);
					_viciiSubsets [_EXPANSIONROMBASEI_SUBSET + 3] -> setActiveForReading (romH2VIC);
				}
			}

			break;

		// The SIMON's basic type
		case C64::Cartridge::Type::_SIMONSBASIC:
			{
				// TODO

				assert (false); // Not supported so far...
			}

			break;

		// The OCEAN's type
		case C64::Cartridge::Type::_OCEANTYPE1:
			{

				assert (_data._data.size () == _cpuSubsets.size () &&
						_data._data.size () == 16 || 
						_data._data.size () == 32 ||
						_data._data.size () == 64 /** Terminator 2. */);

				// Initially only the first one will be active...
				// Remember that writting into the position $de00 will change the bank that is or not active...
				// To be really active the right ROML or ROMH1 has to be also active!...
				bool rH = _activeBank >= 16 && _cpuSubsets.size () != 64 /** Terminator 2. */;
				_cpuSubsets [size_t (_EXPANSIONROMBASE_SUBSET + (int) _activeBank)] -> 
					setActive (_bankActive = (rH ? romH1 : romL));
				_cpuSubsets [size_t (_EXPANSIONROMBASE_SUBSET + (int) _activeBank)] -> 
					setActiveForReading (_bankActive);
			}

			break;

		default:
			assert (false); // It shouldn't be here...
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
			: MCHEmul::upper ((*data ()._attributes.find ("_GAME")).second) == "YES"; // When active, _GAME = false...
	else if (nP == C64::ExpansionIOPort::_EXROM)
		result = data ()._data.empty () 
			? true // When there is no data in the cartridge...
			: MCHEmul::upper ((*data ()._attributes.find ("_EXROM")).second) == "YES"; // When active, _EXROM = false...
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

	_ultimax = false;
	
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
		// The normal cartridge, including the ULTIMAX format...
		case C64::Cartridge::Type::_GENERIC:
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

		// The SIMON's basic type
		// SIMON's basic is defined (in terms of _EXROM and _GAME signals) 
		// as a 8K GENERIC cartridge (_EXROM = false, _GAME = true), but _BASIC is disactive!
		case C64::Cartridge::Type::_SIMONSBASIC:
			{
				// TODO

				assert (false); // Not supported so far...
			}

			break;

		// The OCEAN's type
		// This type is a little bit complex.
		// It is like a 16k GENERIC Cartridge but different memory zones can be banked in and out
		// making possiblie to simulate sizes of up to 512 kByte!
		// To change the bank it is needed to write at the position $de00:
		// The lowest 6 bits would hold the number of the bank and the bit 8 must be always set to true.
		// Banks from 0 to 15 are banked between $8000 and $9fff.
		// Banks from 16 to 31 are banked between $a000 and $bfff.
		// Except Terminator 2 witch banks everything between $a000 and $bfff, but the length is 512k!
		// However it doesn't care here...
		case C64::Cartridge::Type::_OCEANTYPE1:
			{
				for (size_t i = 0; i < _data._data.size (); i++)
				{
					// Use the data info received, just in case...
					std::string bkS = _data._data [i].attribute ("BANK");
					int bk = std::atoi (bkS.c_str ());
					size_t nB = _data._data [i].bytes ().size ();

					// First of all, the physical storage is created...
					MCHEmul::PhysicalStorage* fS = nullptr;
					_storages.insert (MCHEmul::PhysicalStorages::value_type 
						(_EXPANSIONROMBASE + (int) bk, fS = new MCHEmul::PhysicalStorage 
							(_EXPANSIONROMBASE + (int) bk, MCHEmul::PhysicalStorage::Type::_ROM, nB)));

					// ...then the logical access...
					MCHEmul::PhysicalStorageSubset* fSS = nullptr;
					_cpuSubsets.insert (MCHEmul::PhysicalStorageSubsets::value_type
						(_EXPANSIONROMBASE_SUBSET + (int) bk, fSS = new MCHEmul::PhysicalStorageSubset
							(_EXPANSIONROMBASE_SUBSET + (int) bk, fS, 0x0000, _data._data [i].startAddress (), nB)));

					// ..load the data into it...
					fSS -> set (_data._data [i].startAddress (), _data._data [i].bytes (), true /** force. */);
					
					// ...and set a name to recognize it later...
					fSS -> setName ("Ocean1 Bank " + bkS);
				}

				_activeBank = 0;
				_bankActive = false;
			}

			break;

		default:
			assert (false); // It shouldn't be here...
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
void C64::Cartridge::expansionMemorySet (C64::IOExpansionMemory* eM, size_t p, const MCHEmul::UByte& v)
{
	// Makes only sense with certain types of cartridges...
	switch (type ())
	{
		case C64::Cartridge::Type::_OCEANTYPE1:
			{
				assert (eM != nullptr); // There must be a valid notifier...

				if (_cpuSubsets.size () == 0)
					break; // Not still initialized!

				if (eM -> id () == C64::IOExpansionMemoryI::_IO1_SUBSET &&
					p == 0) // Only in the position 0x00!
				{
					// Desactive the one currently active...
					_cpuSubsets [size_t (_EXPANSIONROMBASE_SUBSET + (int) _activeBank)] -> setActive (false);
					_cpuSubsets [size_t (_EXPANSIONROMBASE_SUBSET + (int) _activeBank)] -> setActiveForReading (false);
					// ...and active the new one requested...
					_activeBank = size_t (v.value () & 0x3f); // Never bigger than 64...
					_cpuSubsets [size_t (_EXPANSIONROMBASE_SUBSET + (int) _activeBank)] -> setActive (_bankActive);
					_cpuSubsets [size_t (_EXPANSIONROMBASE_SUBSET + (int) _activeBank)] -> setActiveForReading (_bankActive);
				}
			}

			break;

		default:
			break; // Does nothing...
	}
}

// ---
void C64::Cartridge::expansionMemoryRead (C64::IOExpansionMemory* eM, size_t p, const MCHEmul::UByte& v)
{
	// TODO
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

	_activeBank = 0;
	_bankActive = false;

	// The memory has no longer a cartridge...
	_memoryRef -> setExtensionAt (nullptr);

	_memoryCPUView = nullptr;
	_memoryVICIIView = nullptr;
	_memoryRef = nullptr;
}
