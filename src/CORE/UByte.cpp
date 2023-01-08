#include <CORE/UByte.hpp>
#include <sstream>

// ---
std::string MCHEmul::UByte::asString (MCHEmul::UByte::OutputFormat oF, size_t l) const
{
	std::string result = "";

	std::stringstream ss;
	switch (oF)
	{
		case MCHEmul::UByte::OutputFormat::_DECIMAL:
			result = std::to_string (_value);
			break;

		case MCHEmul::UByte::OutputFormat::_BINARY:
			for (size_t i = 0; i < MCHEmul::UByte::sizeBits (); result = (bit (i++) ? "1" : "0") + result);
			break;

		case MCHEmul::UByte::OutputFormat::_OCTAL:
			ss.clear ();
			ss << std::oct << (int) _value;
			result = ss.str ();
			break;

		case MCHEmul::UByte::OutputFormat::_HEXA:
		default:
			ss.clear ();
			ss << std::hex << (int) _value;
			result = ss.str ();
			break;
	}

	if (result.length () < l)
		result = MCHEmul::_CEROS.substr (0, l - result.length ()) + result;

	return (result);
}
