#include <ZXSpectrum/Datasette.hpp>
#include <ZXSpectrum/FileIO.hpp>

// ---
ZXSPECTRUM::Datasette::Datasette (unsigned int rS)
	: MCHEmul::BasicDatasette (_ID, rS, false /** Controlled externally. */,
		{ { "Name", "Datasette ZXSpectrum" },
		  { "Manufacturer", "Almost anyone, No special connector needed" } })
{
	setClassName ("ZXSpectrumDatasette");
}

// ---
ZXSPECTRUM::DatasetteP::DatasetteP (unsigned int mS)
	: MCHEmul::BasicDatasetteP (_ID, mS, false /** Controlled externally. */,
		{ { "Name", "Datasette ZXSpectrum" },
		  { "Manufacturer", "Almost anyone, No special connector needed" } })
{
	setClassName ("ZXSpectrumDatasette");
}

// ---
ZXSPECTRUM::DatasetteInjection::DatasetteInjection (ZXSPECTRUM::Type t)
	: MCHEmul::DatasettePeripheral (_ID, 
		{ { "Name", "Datasette Injection ZXSpectrum" },
		  { "Manufacturer", "ICF to inject the code directly into the memory" } }),
	  _type (t),
	  _addressIn (), // TODO in function of the type of computer maybe...
	  _addressOut ()
{
	setClassName ("ZXSpectrumDatasetteI");
}

// ---
bool ZXSPECTRUM::DatasetteInjection::simulate (MCHEmul::CPU* cpu)
{
	if ((cpu -> programCounter ().internalRepresentation () == _addressIn.value ()) &&
		!_data._data.empty ()) // there must be data inside...
	{
		// The only type of data recognized is so far, OAndPFileData
		// This type of data is made up of only 1 program, so 
		// the data as a whole might be loaded!
		cpu -> memoryRef () -> set (_data._data);

		// To simulate the return from the routine...
		cpu -> memoryRef () -> stack () -> pull (2);

		cpu -> programCounter ().setAddress (_addressOut);
	}

	return (true);
}

// ---
bool ZXSPECTRUM::DatasetteInjection::connectData (MCHEmul::FileData* dt)
{
	// TODO: Pending to verify the type of IO content...

	_data = std::move (dt -> asMemoryBlocks ());
	
	return (true); 
}
