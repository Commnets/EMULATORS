#include <core/StatusRegister.hpp>

// ---
std::string MCHEmul::StatusRegister::asString () const
{
	std::string result = "";
	for (auto i : _bitNames)
		result += i.first + "(" + (bitStatus (i.first) ? "1" : "0") + ")";

	return (result);
}
