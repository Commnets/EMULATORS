#include <MSX/DatasettePort.hpp>

// ---
MSX::DatasetteIOPort::DatasetteIOPort ()
	: MCHEmul::DatasetteIOPort (_ID, 
		{ { "Name", "Datasette Port" },
		  { "Type", "Input/Output" },
		  { "Manufacturer", "MSX" } })
{
	// Nothing else to do...
}

// ---
void MSX::DatasetteIOPort::linkToChips (const MCHEmul::Chips& c)
{
	for (MCHEmul::Chips::const_iterator i = c.begin (); 
		i != c.end () && _sChip == nullptr; i++)
		if (_sChip == nullptr &&
			dynamic_cast <MCHEmul::SoundChip*> ((*i).second) != nullptr)
			_sChip = dynamic_cast <MCHEmul::SoundChip*> ((*i).second);

	// None can't be null after this method...
	assert (_sChip != nullptr);

	// Connections...
	_sChip -> observe (this); // Observing when bits are sent from the device...
}
