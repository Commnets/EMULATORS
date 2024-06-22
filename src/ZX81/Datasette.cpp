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
	  _addressIn ((t == ZX81::Type::_ZX80) 
		  ? MCHEmul::Address ({ 0x22, 0x02 }, false)
		  : MCHEmul::Address ({ 0x56, 0x03 }, false)),
	  _addressOut ((t == ZX81::Type::_ZX80) 
		  ? MCHEmul::Address ({ 0x03, 0x02 }, false)
		  : MCHEmul::Address ({ 0x07, 0x02 }, false))
{
	setClassName ("ZX81Datasette");
}

// ---
bool ZX81::DatasetteInjection::simulate (MCHEmul::CPU* cpu)
{
	if ((cpu -> programCounter ().internalRepresentation () == _addressIn.value ()) &&
		!_data._data.empty ()) // there must be data inside...
	{
		cpu -> memoryRef () -> set (_data._data);

		// To simulate the return from the routine...
		cpu -> memoryRef () -> stack () -> pull (2);

		cpu -> programCounter ().setAddress (_addressOut);
	}

	return (true);
}

// ---
bool ZX81::DatasetteInjection::connectData (MCHEmul::FileData* dt)
{
	if (dynamic_cast <ZX81::OAndPFileData*> (dt) == nullptr)
		return (false); // That type of info is not valid from the datasette...

	_data = std::move (dt -> asMemoryBlocks ());
	
	return (true); 
}
