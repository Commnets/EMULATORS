#include <CORE/UBytes.hpp>
#include <algorithm>

// ---
const MCHEmul::UBytes MCHEmul::UBytes::_E = MCHEmul::UBytes ();

// ---
void MCHEmul::UBytes::setMinLength (size_t l, bool r)
{
	size_t s = size ();
	if (s < l)
	{
		for (size_t i = 0; i < (l - s); i++)
		{
			if (r) _values.push_back (MCHEmul::UByte::_0);
			else _values.insert (_values.begin (), MCHEmul::UByte::_0);
		}
	}
}

// ---
MCHEmul::UBytes MCHEmul::UBytes::complement () const
{
	MCHEmul::UBytes result = *this;
	for (auto &i : result._values) 
		i = i.complement ();
	
	return (result);
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
MCHEmul::UBytes MCHEmul::UBytes::bitAdding (const MCHEmul::UBytes& u, bool cin, bool& cout, bool& o) const
{
	// If the elements to add already have the same size, no silly copies are done...

	MCHEmul::UBytes result = *this;
	cout = cin /** To simplify the loop. */;
	if (size () != u.size ())
	{
		MCHEmul::UBytes u2 = u;
		size_t mL = std::max (result.size (), u2.size ());
		// The operation has to be done with UBytes of the same length...
		result.setMinLength (mL, false /** at the beginning the additional 0. */); u2.setMinLength (mL, false);

		for (int i = (int) (result.size () - 1); i >= 0; i--) 
			result [i] = result [i].bitAdding (u2 [i], cout, cout, o);
	}
	else
	{
		for (int i = (int) (result.size () - 1); i >= 0; i--) 
			result [i] = result [i].bitAdding (u [i], cout, cout, o);
	}

	return (result);
}

// ---
MCHEmul::UBytes MCHEmul::UBytes::reverse () const
{
	std::vector <MCHEmul::UByte> dt;
	for (auto i = _values.rbegin (); i != _values.rend (); i++) dt.push_back (*i);
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
std::string MCHEmul::UBytes::asString (MCHEmul::UByte::OutputFormat oF, char s, size_t l, size_t sb) const
{
	if (size () == 0)
		return ("");

	std::string result = "";
	for (size_t i = 0; i < size (); i += sb)
	{
		if (i != 0) result += '\n';
		for (size_t j = i; j < (i + sb) && j < size (); j++)
		{
			if (j != i) result += s;
			result += _values [j].asString (oF, l);
		}
	}

	return (result);
}
