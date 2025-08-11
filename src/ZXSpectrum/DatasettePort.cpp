#include <ZXSpectrum/DatasettePort.hpp>
#include <ZXSpectrum/ULA.hpp>

// ---
ZXSPECTRUM::DatasetteIOPort::DatasetteIOPort ()
	: MCHEmul::DatasetteIOPort (_ID,
		{ { "Name", "Datasette Port" },
		  { "Type", "Input/Output" },
		  { "Manufacturer", "Sinclair Research/Timex Coporation" } }),
	  _ULA (nullptr) // asigned later, when linking to chips...
{
	// Nothing else to do...
}

// ---
void ZXSPECTRUM::DatasetteIOPort::linkToChips (const MCHEmul::Chips& c)
{
	for (MCHEmul::Chips::const_iterator i = c.begin (); 
			i != c.end () && _ULA == nullptr; i++)
		_ULA = dynamic_cast <ZXSPECTRUM::ULA*> ((*i).second);

	// It cannot be nullptr...
	assert (_ULA != nullptr);
}
