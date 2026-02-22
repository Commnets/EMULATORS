#include <COMMODORE/1540SeriesDisk.hpp>

// ---
COMMODORE::Disk1540SeriesSimulation::Disk1540SeriesSimulation 
		(int id, unsigned char dN, const COMMODORE::SerialIOPeripheralSimulation::Definition& def)
	: COMMODORE::SerialIOPeripheralSimulation (id, dN,
		def,
		{ { "Name", "1540/1541 Disk Injection" },
		  { "Manufacturer", "ICF 1540/1541 Software Simulation" } }),
	  _actualTrack (0),
	  _actualSector (0)
{ 
	setClassName ("Disk1540Simulation");
}

// ---
bool COMMODORE::Disk1540SeriesSimulation::connectData (MCHEmul::FileData* dt)
{
	if (dynamic_cast <COMMODORE::D64FileData*> (dt) == nullptr)
		return (false); // These formats are the only ones accepted only...

	_data = dt -> asMemoryBlocks ();

	_actualTrack = 0;
	_actualSector = 0;

	return (true);
}

// ---
MCHEmul::InfoStructure COMMODORE::Disk1540SeriesSimulation::getInfoStructure () const
{
	MCHEmul::InfoStructure result = 
		std::move (COMMODORE::SerialIOPeripheralSimulation::getInfoStructure ());

	result.add ("DATANAME", (_data._name == "") ? "-" : _data._name);
	result.add ("DATASIZE", _data._data.size ());

	MCHEmul::InfoStructure dS;
	for (size_t i = 0; i < (size_t) _data._data.size (); i++)
	{
		MCHEmul::InfoStructure dSA;

		dSA.add ("ID",		i); // The id...
		dSA.add ("SIZE",	_data._data [i].size ());
		if (_data._data [i].bytes ().size () > 256)
		{
			dSA.add ("FBYTES", _data._data [i].bytes ()); // All bytes...
			dSA.add ("BYTES", std::vector <MCHEmul::UByte> 
				(_data._data [i].bytes ().begin (), _data._data [i].bytes ().begin () + 256));
		}
		else // This way because the type of method used varies...
			dSA.add ("BYTES", _data._data [i].bytes ());

		dS.add (MCHEmul::fixLenStr (std::to_string (i), 4, true, MCHEmul::_CEROS), 
			std::move (dSA));
	}

	result.add ("DATA", dS);

	return (result);
}

// ---
unsigned char COMMODORE::Disk1540SeriesSimulation::sendByte (MCHEmul::CPU* cpu, const MCHEmul::UByte& b)
{
	// TODO

	return (0); // No errors...
}

// ---
unsigned char COMMODORE::Disk1540SeriesSimulation::receiveByte (MCHEmul::CPU* cpu, MCHEmul::UByte& b)
{
	// TODO

	b = 0; // Nothing...

	return (0); // No errors
}

// ---
size_t COMMODORE::Disk1540SeriesSimulation::actualBlock () const
{
	// TODO

	return (0);
}
