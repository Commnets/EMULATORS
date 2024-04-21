#include <CORE/ByteCodeLine.hpp>

// ---
std::string MCHEmul::ByteCodeLine::asString 
	(MCHEmul::UByte::OutputFormat oF, char s, size_t l) const
{
	// Breakpoint associated?
	std::string result = ((_actionOn != MCHEmul::Computer::_ACTIONNOTHING) ? "@" : " ") + // Breakpoint?
		_address.asString (oF, s, l) + ":";

	// Then the bytes (in the way they are written into the memory)
	bool f = true;
	for (const auto& i : _bytes)
	{
		result += ((!f) ? " " : "") + i.asString (oF, l);

		f = false;
	}

	// Label?
	result += " " + ((_label == "") ? "" : _label + ":"); // Space for a potential label...

	// Finally, the instruction separated!...
	result += MCHEmul::_SPACES.substr (0, (20 - result.length ()));
	if (_instruction != nullptr)
	{ 
		MCHEmul::ByteCodeLine cL = *this; // A copy to put the parameters into...
		cL._instruction -> setParameters (_bytes);
		result += cL._instruction -> asString ();
	}

	return (result);
}
