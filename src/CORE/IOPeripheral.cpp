#include <CORE/IOPeripheral.hpp>
#include <CORE/FmterBuilder.hpp>
#include <CORE/Formatter.hpp>

// ---
MCHEmul::InfoStructure MCHEmul::IOPeripheral::getInfoStructure () const
{
	MCHEmul::InfoStructure result = MCHEmul::InfoClass::getInfoStructure ();

	result.add ("ID", _id);
	result.add ("ATTRS", _attributes);

	return (result);
}
