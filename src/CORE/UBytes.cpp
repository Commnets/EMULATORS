#include <CORE/UBytes.hpp>

// ---
const MCHEmul::UBytes MCHEmul::UBytes::_E = MCHEmul::UBytes ();

// ---
void MCHEmul::UBytes::setMinLength (size_t l, bool r)
{
	if (size () < l)
	{
		for (size_t i = 0; i < (l - size ()); i++)
		{
			if (r) _values.push_back (MCHEmul::UByte::_0);
			else _values.insert (_values.begin (), MCHEmul::UByte::_0);
		}
	}
}

// ---
bool MCHEmul::UBytes::shiftLeftC (bool c, size_t p)
{
	bool nC = c;
	for (size_t i = 0; i < p; i++)
	{
		bool mc = c;
		for (int j = (int) (size () - 1); j >= 0; j--)
			mc = _values [(size_t) j].shiftLeftC (mc, 1);
		nC = mc;
	}

	return (nC);
}

// ---
bool MCHEmul::UBytes::shiftRightC (bool c, size_t p)
{
	bool nC = c;
	for (size_t i = 0; i < p; i++)
	{
		bool mc = c;
		for (size_t j = 0; j < size (); j++)
			mc = _values [(size_t) j].shiftRightC (mc, 1);
		nC = mc;
	}

	return (nC);
}

// ---
bool MCHEmul::UBytes::rotateLeftC (bool c, size_t p)
{
	bool nC = c;
	for (size_t i = 0; i < p; i++)
	{
		bool mc = c;
		for (int j = (int) (size () - 1); j >= 0; j--)
			mc = _values [(size_t) j].rotateLeftC (mc, 1);
		nC = mc;
	}

	return (nC);
}

// ---
MCHEmul::UBytes& MCHEmul::UBytes::rotateLeft (size_t p)
{
	for (size_t i = 0; i < p; i++)
		rotateLeftC (bit (sizeBits () - 1), 1);

	return (*this);
}

// ---
bool MCHEmul::UBytes::rotateRightC (bool c, size_t p)
{
	bool nC = c;
	for (size_t i = 0; i < p; i++)
	{
		bool mc = c;
		for (size_t j = 0; j < size (); j++)
			mc = _values [(size_t) j].rotateRightC (mc, 1);
		nC = mc;
	}

	return (nC);
}

// ---
MCHEmul::UBytes& MCHEmul::UBytes::rotateRight (size_t p)
{
	for (size_t i = 0; i < p; i++)
		rotateRightC (bit (0), 1);

	return (*this);
}

// ---
MCHEmul::UBytes MCHEmul::UBytes::reverse () const
{
	std::vector <MCHEmul::UByte> dt;
	for (auto i : _values)
		dt.insert (dt.begin (), i);

	return (MCHEmul::UBytes (dt));
}

// ---
bool MCHEmul::UBytes::operator == (const MCHEmul::UBytes& u) const
{
	bool result = (size () == u.size ());
	for (size_t i = 0; i < _values.size () && result; i++)
		result = (_values [i] == u._values [i]);
	return (result);
}

// ---
std::string MCHEmul::UBytes::asString (MCHEmul::UByte::OutputFormat oF, char s, size_t l) const
{
	std::string result = "";

	size_t c = 0;
	for (auto i : _values)
		result += ((c++ != 0) ? std::string (1, s) : "") + i.asString (oF, l);
	return (result);
}
