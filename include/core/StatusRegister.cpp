#include <core/StatusRegister.hpp>

// ---
MCHEmul::UBytes MCHEmul::StatusRegister::valuesWithout (const std::vector <std::string>& bN)
{
	MCHEmul::UBytes result = values ();
	for (auto i : _bitNames)
		if (std::find (bN.begin (), bN.end (), i.first) != bN.end ())
			result [i.second] = false;
	return (result);
}

// ---
std::string MCHEmul::StatusRegister::asString () const
{
	std::string result = "";
	for (auto i : _bitNames)
		result += i.first + "(" + (bitStatus (i.first) ? "1" : "0") + ")";

	return (result);
}
