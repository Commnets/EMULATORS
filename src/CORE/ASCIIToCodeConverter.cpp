#include <CORE/ASCIIToCodeConverter.hpp>

// ---
std::vector <MCHEmul::UByte> MCHEmul::ASCIIToCodeConverter::convert (const std::string& str) const
{
	std::vector <MCHEmul::UByte> result;
	for (const auto& i : str)
		result.emplace_back (convert ((unsigned char) i));
	return (result);
}

// ---
std::vector <MCHEmul::UByte> MCHEmul::ASCIIToCodeConverter::inverseConvert (const std::string& str) const
{
	std::vector <MCHEmul::UByte> result;
	for (const auto& i : str)
		result.emplace_back (inverseConvert ((unsigned char) i));
	return (result);
}

// ---
std::vector <MCHEmul::UByte> MCHEmul::ASCIIToCodeConverter::convert 
	(const std::vector <MCHEmul::UByte>& lst) const
{
	std::vector <MCHEmul::UByte> result;
	for (const auto& i : lst)
		result.emplace_back (convert (i.value ()));
	return (result);
}

// ---
std::vector <MCHEmul::UByte> MCHEmul::ASCIIToCodeConverter::inverseConvert 
	(const std::vector <MCHEmul::UByte>& lst) const
{
	std::vector <MCHEmul::UByte> result;
	for (const auto& i : lst)
		result.emplace_back (inverseConvert (i.value ()));
	return (result);
}
