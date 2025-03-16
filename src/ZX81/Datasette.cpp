#include <ZX81/Datasette.hpp>
#include <ZX81/FileIO.hpp>

// ---
ZX81::Datasette::Datasette (unsigned int rS)
	: MCHEmul::BasicDatasette (_ID, rS, false /** Controlled externally. */,
		{ { "Name", "Datasette ZX81" },
		  { "Manufacturer", "Almost anyone, No special connector needed" } })
{
	setClassName ("ZX81Datasette");
}

// ---
ZX81::DatasetteP::DatasetteP (unsigned int mS)
	: MCHEmul::BasicDatasetteP (_ID, mS, false /** Controlled externally. */,
		{ { "Name", "Datasette ZX81" },
		  { "Manufacturer", "Almost anyone, No special connector needed" } })
{
	setClassName ("ZX81Datasette");
}

// ---
ZX81::DatasetteInjection::DatasetteInjection (ZX81::Type t)
	: MCHEmul::DatasettePeripheral (_ID, 
		{ { "Name", "Datasette Injection ZX81" },
		  { "Manufacturer", "ICF to inject the code directly into the memory" } }),
	  _type (t),
	  _loadTrap { 
		0, 
		"LOAD", 
		(t == ZX81::Type::_ZX80) 
		  ? MCHEmul::Address ({ 0x22, 0x02 }, false)
		  : MCHEmul::Address ({ 0x56, 0x03 }, false),
		(t == ZX81::Type::_ZX80) 
		  ? MCHEmul::Address ({ 0x03, 0x02 }, false)
		  : MCHEmul::Address ({ 0x07, 0x02 }, false),
		{  }
	  }
{
	setClassName ("ZX81DatasetteInjection");
}

// ---
bool ZX81::DatasetteInjection::connectData (MCHEmul::FileData* dt)
{
	if (dynamic_cast <ZX81::OAndPFileData*> (dt) == nullptr)
		return (false); // That type of info is not valid from the datasette...

	_data = std::move (dt -> asMemoryBlocks ());
	
	return (true); 
}

// ---
bool ZX81::DatasetteInjection::simulate (MCHEmul::CPU* cpu)
{
	if ((cpu -> programCounter ().internalRepresentation () == _loadTrap._addressIn.value ()) &&
		!_data._data.empty ()) // there must be data inside...
	{
		_IFDEBUG debugSimulation (cpu);

		// Empty any pending set action...
		// Because a trap is breaking the normal flow of the code...
		// so no set commands pendings can be left!
		MCHEmul::Memory::configuration ().executeMemorySetCommandsBuffered ();

		// The only type of data recognized is so far, OAndPFileData
		// This type of data is made up of only 1 program, so 
		// the data as a whole might be loaded!
		cpu -> memoryRef () -> put (_data._data);

		// To simulate the return from the routine...
		cpu -> memoryRef () -> stack () -> pull (2);

		cpu -> programCounter ().setAddress (_loadTrap._addressOut);
	}

	return (true);
}

// ---
MCHEmul::InfoStructure ZX81::DatasetteInjection::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::DatasettePeripheral::getInfoStructure ());

	result.add ("TYPE", std::to_string ((int) _type));
	result.add ("TRAP", _loadTrap.asString ());

	return (result);
}

// ---
void ZX81::DatasetteInjection::debugSimulation (MCHEmul::CPU* cpu)
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeCompleteLine ("ZX81DN", cpu -> clockCycles (), "Datasette trap executed");
}
