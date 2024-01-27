#include "ParamTemplate.hpp"

// ---
bool VIC20::ParameterTemplate::validValues (const MCHEmul::Strings& v) const
{
	bool result = true;

	if (v.size () < numberMinValues () ||
		v.size () > numberMaxValues ())
		result = false;
	else
		for (const auto& i : v)
			result &= validValue (i);

	return (result); 
}

// ---
std::ostream& VIC20::i_ParameterTemplate::printOutHelp (std::ostream& o) const
{
	return 
		(o << "-i [FILE1] [FILE2] ..." << "\t\t" << "The input files." << std::endl
		   << "\t\t\t\t" << "There can be a list of files after this parameter." << std::endl
		   << "\t\t\t\t" << "This parameter is mandatory.");
}

// ---
std::ostream& VIC20::o_ParameterTemplate::printOutHelp (std::ostream& o) const
{
	return 
		(o << "-o [FILE1]" << "\t\t\t" << "The output file." << std::endl
		   << "\t\t\t\t" << "This parameter is mandatory.");
}

// ---
std::ostream& VIC20::n_ParameterTemplate::printOutHelp (std::ostream& o) const
{
	return 
		(o << "-n" << "\t\t\t\t" << "Name of the Cartridge." << std::endl
		   << "\t\t\t\t" << "The parameter is not mandatory." << std::endl
		   << "\t\t\t\t" << "If it is not provided, a default name will be given.");
}
