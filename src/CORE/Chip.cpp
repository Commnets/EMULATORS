#include <CORE/Chip.hpp>
#include <CORE/FmterBuilder.hpp>
#include <CORE/Formatter.hpp>

// ---
MCHEmul::InfoStructure MCHEmul::Chip::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::MotherboardElement::getInfoStructure ());

	result.add ("ID",		_id);
	result.add ("ATTRS",	_attributes);
	result.add ("Memory",	std::move (_memory -> getInfoStructure ()));

	return (result);
}
