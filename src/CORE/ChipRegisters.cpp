#include <CORE/ChipRegisters.hpp>

// ---
std::vector <MCHEmul::UByte> MCHEmul::ChipRegisters::valueRegisters () const
{
	std::vector <MCHEmul::UByte> result; 
	for (size_t i = 0; i < numberRegisters (); result.push_back (readValue (i++))); 
	return (result);
}

// ---
MCHEmul::InfoStructure MCHEmul::ChipRegisters::getInfoStructure () const
{
	MCHEmul::InfoStructure result;

	result.add ("BYTES", valueRegisters ());

	return (result);
}
