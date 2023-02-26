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
	// Notice that the method from class InfoClass is invoked instead the one from the direct parent class.
	// It is because the info of the memory as it is is not neccesary, but the register value...
	MCHEmul::InfoStructure result = MCHEmul::InfoClass::getInfoStructure ();

	result.add ("BYTES", valueRegisters ());

	return (result);
}
