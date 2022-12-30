#include <CORE/ASCIIConverter.hpp>

// ---
std::vector <MCHEmul::UByte> MCHEmul::ASCIIConverter::convert (const std::string& str) const
{
	std::vector <MCHEmul::UByte> result;
	
	for (const auto& i : str)
		result.push_back (MCHEmul::UByte (i));

	return (result);
}
