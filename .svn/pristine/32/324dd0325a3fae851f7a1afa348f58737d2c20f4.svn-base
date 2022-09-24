#include <CORE/UByte.hpp>
#include <sstream>

// ---
bool MCHEmul::UByte::shiftLeftC (bool c, size_t p)
{
	MCHEmul::UByte nV = MCHEmul::UByte::_0;

	bool nC = false; // Adjusted later
	for (size_t i = 0; i < p; i++)
	{
		for (size_t j = MCHEmul::UByte::sizeBits () - 1; j >= 1; j--)
		{
			if (j == MCHEmul::UByte::sizeBits () - 1) 
				nC = bit (j);

			nV.setBit (j, bit (j - 1));
		}

		nV.setBit (0, c);
	}

	*this = nV;

	return (nC);
}

// ---
bool MCHEmul::UByte::shiftRightC (bool c, size_t p)
{
	MCHEmul::UByte nV = MCHEmul::UByte::_0;

	bool nC = false; // Adjusted later
	for (size_t i = 0; i < p; i++)
	{
		for (size_t j = 0; j < (MCHEmul::UByte::sizeBits () - 1); j++)
		{
			if (j == 0) 
				nC = bit (j);

			nV.setBit (j, bit (j + 1));
		}

		nV.setBit (MCHEmul::UByte::sizeBits () - 1, c);
	}

	*this = nV;

	return (nC);
}

// ---
bool MCHEmul::UByte::rotateLeftC (bool c, size_t p)
{
	bool r = c;
	for (size_t i = 0; i < p; i++)
		r = shiftLeftC (r);

	return (r);
}

// ---
MCHEmul::UByte& MCHEmul::UByte::rotateLeft (size_t p)
{
	for (size_t i = 0; i < p; i++) 
		rotateLeftC (bit (sizeBits () - 1), 1);

	return (*this);
}

// ---
bool MCHEmul::UByte::rotateRightC (bool c, size_t p)
{
	bool r = c;
	for (size_t i = 0; i < p; i++)
		r = shiftRightC (r);

	return (r);
}

// ---
MCHEmul::UByte& MCHEmul::UByte::rotateRight (size_t p)
{
	for (size_t i = 0; i < p; i++)
		rotateRightC (bit (0), 1);

	return (*this);
}

// ---
std::string MCHEmul::UByte::asString (MCHEmul::UByte::OutputFormat oF, size_t l) const
{
	static const std::string _Spaces ("0", MCHEmul::UByte::sizeBits ());

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
		result = _Spaces.substr (0, l - result.length ()) + result;

	return (result);
}
