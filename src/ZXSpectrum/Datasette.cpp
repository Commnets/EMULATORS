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
	  _loadTrap { 
		0, 
		"LOAD", 
		MCHEmul::Address ({ 0x56, 0x05 }, false),
		MCHEmul::Address ({ 0xe2, 0x05 }, false),
		{  }
	  }
{
	setClassName ("ZXSpectrumDatasetteI");
}

// ---
bool ZXSPECTRUM::DatasetteInjection::simulate (MCHEmul::CPU* cpu)
{
	if ((cpu -> programCounter ().internalRepresentation () == _loadTrap._addressIn.value ()) &&
		!_data._data.empty ()) // there must be data inside...
	{
		_IFDEBUG debugSimulation (cpu);

		// TODO

		cpu -> programCounter ().setAddress (_loadTrap._addressOut);
	}

	return (true);
}

// ---
bool ZXSPECTRUM::DatasetteInjection::connectData (MCHEmul::FileData* dt)
{
	if (dynamic_cast <ZXSPECTRUM::TAPFileData*> (dt) == nullptr)
		return (false); // That type of info is not valid from the datasette...

	_data = std::move (dt -> asMemoryBlocks ());

	// Starting from the beginning...
	_blockRead = 0;
	
	return (true); 
}

// ---
void ZXSPECTRUM::DatasetteInjection::debugSimulation (MCHEmul::CPU* cpu)
{
	assert (_deepDebugFile != nullptr);

	// TODO

	_deepDebugFile -> writeCompleteLine 
		("ZXSPECTRUMDN", 
		 cpu -> clockCycles (), 
		 "Datasette requested:",
		 { "" });
}
