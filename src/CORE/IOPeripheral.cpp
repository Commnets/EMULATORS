#include <CORE/IOPeripheral.hpp>
#include <CORE/FmterBuilder.hpp>
#include <CORE/Formatter.hpp>

// ---
MCHEmul::InfoStructure MCHEmul::IOPeripheral::Info::getInfoStructure () const
{
	MCHEmul::InfoStructure result;

	result.add ("ID",		_id);
	result.add ("ATTRS",	_attributes);

	return (result);
}

// ---
MCHEmul::InfoStructure MCHEmul::IOPeripheral::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	result.add ("ID",		_id);
	result.add ("ATTRS",	_attributes);
	result.add ("COMMANDS", MCHEmul::concatenateStrings (commandDescriptions (), "\n"));
	// One below the next...

	return (result);
}
