#include <ZX81/DatasettePort.hpp>
#include <ZX81/ULA.hpp>

// ---
ZX81::DatasetteIOPort::DatasetteIOPort ()
	: MCHEmul::DatasetteIOPort (_ID,
		{ { "Name", "Datasette Port" },
		  { "Type", "Input/Output" },
		  { "Manufacturer", "Sinclair Research/Timex Coporation" } })
{
	// Nothing else to do...
}

// ---
void ZX81::DatasetteIOPort::linkToChips (const MCHEmul::Chips& c)
{
	for (MCHEmul::Chips::const_iterator i = c.begin (); 
			i != c.end () && _ULA == nullptr; i++)
		_ULA = dynamic_cast <ZX81::ULA*> ((*i).second);

	// It cannot be nullptr...
	assert (_ULA != nullptr);

	// The ULA observes this datasette port...
	_ULA -> observe (this);
}

