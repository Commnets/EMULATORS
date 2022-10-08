#include <CORE/StatusRegister.hpp>

// ---
MCHEmul::UBytes MCHEmul::StatusRegister::valuesWithout (const MCHEmul::Strings& bN)
{
	MCHEmul::UBytes result = values ();
	for (const auto& i : _bitNames)
		if (std::find (bN.begin (), bN.end (), i.first) != bN.end ())
			result [i.second] = false;
	return (result);
}

// ---
std::string MCHEmul::StatusRegister::asString () const
{
	std::string result = _name + ":";
	for (const auto& i : _bitNames)
		result += i.first + "(" + (bitStatus (i.first) ? "1" : "0") + ")";

	return (result);
}
