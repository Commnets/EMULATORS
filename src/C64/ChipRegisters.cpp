#include <C64/ChipRegisters.hpp>

// ---
std::vector <MCHEmul::UByte> C64::ChipRegisters::valueRegisters () const
{
	std::vector <MCHEmul::UByte> result; 
	for (size_t i = 0; i < numberRegisters (); result.push_back (readValue (i++))); 
	return (result);
}

// ---
MCHEmul::InfoStructure C64::ChipRegisters::getInfoStructure () const
{
	MCHEmul::InfoStructure result;

	result.add ("BYTES", valueRegisters ());

	return (result);
}
