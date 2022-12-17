#include <CORE/UInt.hpp>
#include <algorithm>
#include <sstream>

// ---
MCHEmul::UInt::FormatManagers* MCHEmul::UInt::FormatManagers::_instance = nullptr;
const MCHEmul::UInt MCHEmul::UInt::_0 = MCHEmul::UInt ({ MCHEmul::UByte::_0 }, false, false);
const MCHEmul::UInt MCHEmul::UInt::_1 = MCHEmul::UInt ({ MCHEmul::UByte::_1 }, false, false);
MCHEmul::UInt::FormatManagers MCHEmul::UInt::_formaters 
	( { { MCHEmul::UInt::_BINARY, new MCHEmul::UInt::BinaryFormatManager },
		{ MCHEmul::UInt::_PACKAGEDBCD, new MCHEmul::UInt::PackagedBCDFormatManager } } );

// ---
MCHEmul::UInt MCHEmul::UInt::BinaryFormatManager::add 
	(const MCHEmul::UInt& u1, const MCHEmul::UInt& u2, bool cIn) const
{ 
	bool c = false, o = false; 
	MCHEmul::UInt result (u1._values.bitAdding (u2._values, cIn, c, o)); 
	result._carry = c; result._overflow = o; 
	return (result);
}

// ---
MCHEmul::UInt MCHEmul::UInt::BinaryFormatManager::substract 
	(const MCHEmul::UInt& u1, const MCHEmul::UInt& u2, bool cIn) const
{ 
	bool c = false, o = false; 
	MCHEmul::UInt result (u1._values.bitAdding (u2.complement ()._values, cIn, c, o)); 
	result._carry = c; result._overflow = o; 
	return (result);
}

// ---
unsigned int MCHEmul::UInt::BinaryFormatManager::asUnsignedInt (const MCHEmul::UInt& u) const
{
	unsigned int result = 0;

	// Just to speed up the most common situations...
	// Let's say when the number of bytes are 0, 1, or 2...
	switch (u.size ())
	{
		case 0:
			break;

		case 1:
			result = (unsigned int) u [0].value ();
			break;

		case 2:
			result = (unsigned int) ((u [0].value () << MCHEmul::UByte::sizeBits ()) + u [1].value ());
			break;

		default:
			{
			// This is like the previous situation but with a generic number of bytes...
			// The algorithm is done thinking in reducing the number of multiplications and change them into additions,
			// and also for not to use signed numbers!
			size_t c = (u.size () - 1) * MCHEmul::UByte::sizeBits (); // The only multiplication...
				for (size_t i = 0; i < u.size (); i++, c -= MCHEmul::UByte::sizeBits ())
					result += (unsigned int) (u._values [i].value () << c);
			}
	}

	return (result);
}

// ---
MCHEmul::UInt MCHEmul::UInt::BinaryFormatManager::fromUnsignedInt (unsigned int n)
{
	static constexpr unsigned int _1BYTELIMIT = MCHEmul::UByte::_1 << MCHEmul::UByte::sizeBits ();
	static constexpr unsigned int _2BYTELIMIT = _1BYTELIMIT << MCHEmul::UByte::sizeBits ();

	std::vector <MCHEmul::UByte> dt;

	if (n < _1BYTELIMIT)
		dt.push_back (MCHEmul::UByte ((unsigned char) n));
	else if (n >= _1BYTELIMIT && n < _2BYTELIMIT)
	{
		dt.push_back (MCHEmul::UByte ((unsigned char) (n >> MCHEmul::UByte::sizeBits ())));
		dt.push_back (MCHEmul::UByte ((unsigned char) (n & MCHEmul::UByte::_FF)));
	}
	else
	{
		// This is like the previous situation but with a generic number of bytes...
		// The algorithm is done thinking in reducing the number of multiplications and change them into additions,
		// and also for not to use signed numbers!
		size_t nB = 1; unsigned int r = n; while ((r = (r >> MCHEmul::UByte::sizeBits ())) != 0) nB++;
		size_t sft = (nB - 1) * MCHEmul::UByte::sizeBits (); // The only multiplication...
		for (size_t i = nB - 1; i > 0; i--, sft -= MCHEmul::UByte::sizeBits ())
			dt.push_back (MCHEmul::UByte ((unsigned char) (n >> sft)));
		dt.push_back (MCHEmul::UByte ((unsigned char) (n & MCHEmul::UByte::_FF))); // The last ubyte...
	}

	return (MCHEmul::UInt (MCHEmul::UBytes (dt), true, MCHEmul::UInt::_BINARY));
}

// ---
MCHEmul::UInt MCHEmul::UInt::BinaryFormatManager::fromInt (int n)
{
	static constexpr unsigned int _1BYTELIMIT = MCHEmul::UByte::_1 << (MCHEmul::UByte::sizeBits () - 1 /** Because the sign. */);
	static constexpr unsigned int _2BYTELIMIT = _1BYTELIMIT << MCHEmul::UByte::sizeBits ();

	std::vector <MCHEmul::UByte> dt;
	unsigned int r = (n >= 0) ? n : -n;

	if (r < _1BYTELIMIT)
		dt.push_back (MCHEmul::UByte ((unsigned char) r));
	else if (r >= _1BYTELIMIT && r < _2BYTELIMIT)
	{
		dt.push_back (MCHEmul::UByte ((unsigned char) (r >> MCHEmul::UByte::sizeBits ())));
		dt.push_back (MCHEmul::UByte ((unsigned char) (r & MCHEmul::UByte::_FF)));
	}
	else
	{
		// This is like the previous situation but with a generic number of bytes...
		// The algorithm is done thinking in reducing the number of multiplications and change them into additions,
		// and also for not to use signed numbers!
		size_t nB = 2; unsigned r1 = r;  while ((r1 = (r1 >> MCHEmul::UByte::sizeBits ())) != 0) nB++;
		size_t sft = (nB - 1) * MCHEmul::UByte::sizeBits (); // The only multiplication...
		for (size_t i = nB - 1; i > 0; i--, sft -= MCHEmul::UByte::sizeBits ())
			dt.push_back (MCHEmul::UByte ((unsigned char) (r >> sft)));
		dt.push_back (MCHEmul::UByte ((unsigned char) (r & MCHEmul::UByte::_FF))); // The last ubyte...
	}

	return ((n < 0) 
		? MCHEmul::UInt (dt, true, MCHEmul::UInt::_BINARY).complement_2 () 
		: MCHEmul::UInt (dt, true, MCHEmul::UInt::_BINARY));
}

// ---
MCHEmul::UInt MCHEmul::UInt::PackagedBCDFormatManager::add 
	(const MCHEmul::UInt& u1, const MCHEmul::UInt& u2, bool cIn) const
{
	// At this point both elements has the same size...

	unsigned short r = 0;
	bool c = cIn;
	MCHEmul::UInt result = u1;
	for (int i = (int) (u2.bytes ().size () - 1); i >= 0; i--) 
	{
		r  = (unsigned short) (result [i].value () & MCHEmul::UByte::_0F) + 
			 (unsigned short) (u2 [i].value () & MCHEmul::UByte::_0F) + (c ? MCHEmul::UByte::_1 : MCHEmul::UByte::_0);
		if (r > MCHEmul::UByte::_09) r += MCHEmul::UByte::_06;
		r += (unsigned short) (result [i].value () & MCHEmul::UByte::_F0) + (unsigned short) (u2 [i].value () & MCHEmul::UByte::_F0);
		if ((r & 0x01f0) > MCHEmul::UByte::_90) r += MCHEmul::UByte::_60;

		c = r > MCHEmul::UByte::_FF;
		
		result [i] = (unsigned char) r;
	}

	result._carry = c;
	result._overflow = !((u1 [0].value () ^ u2 [0].value ()) & MCHEmul::UByte::_80) &&
		((u1 [0].value () ^ result [0].value ()) & MCHEmul::UByte::_80);

	return (result);
}

// ---
MCHEmul::UInt MCHEmul::UInt::PackagedBCDFormatManager::substract
	(const MCHEmul::UInt& u1, const MCHEmul::UInt& u2, bool cIn) const
{
	// At this point both elements has the same size...

	unsigned short r = 0;
	bool c = cIn;
	MCHEmul::UInt result = u1;
	for (int i = (int) (u2.bytes ().size () - 1); i >= 0; i--) 
	{
		r  = (unsigned short) (result [i].value () & MCHEmul::UByte::_0F) - 
			 (unsigned short) (u2 [i].value () & MCHEmul::UByte::_0F) - (c ? MCHEmul::UByte::_0 : MCHEmul::UByte::_1);
		r = ((r & MCHEmul::UByte::_10) != MCHEmul::UByte::_0)
				? (unsigned short) ((r - MCHEmul::UByte::_06) & MCHEmul::UByte::_0F) |
				  (unsigned short) ((result [i].value () & MCHEmul::UByte::_F0) - (u2 [i].value () & MCHEmul::UByte::_F0) - MCHEmul::UByte::_10)
				: (unsigned short) (r & MCHEmul::UByte::_0F) |
				  (unsigned short) ((result [i].value () & MCHEmul::UByte::_F0) - (u2 [i].value () & MCHEmul::UByte::_F0));
		if ((r & 0x0100) != MCHEmul::UByte::_0)
			r -= MCHEmul::UByte::_60;

		c = r < 0x0100;
		
		result [i] = (unsigned char) r;
	}

	result._carry = c;
	result._overflow = ((u1 [0].value () ^ u2 [0].value ()) & MCHEmul::UByte::_80) &&
		((u1 [0].value () ^ result [0].value ()) & MCHEmul::UByte::_80);

	return (result);
}

// ---
unsigned int MCHEmul::UInt::PackagedBCDFormatManager::asUnsignedInt (const UInt& u) const
{
	unsigned int result = 0;

	int c = 1;
	for (int i = (int) (u.size () - 1); i >= 0; i -= 2, c *= 100)
		result += ((((u._values [(size_t) i].value () & 0xf0) >> 4) * 10) + 
			(u._values [(size_t) i].value () & (unsigned int) MCHEmul::UByte::_0F)) * c;

	return (result);
}

// ---
MCHEmul::UInt MCHEmul::UInt::PackagedBCDFormatManager::fromUnsignedInt (unsigned int n)
{
	std::vector <MCHEmul::UByte> dt;

	std::string nS = std::to_string (n);
	for (int i = ((int) nS.length () - 1); i >= 0; i -= 2) // It can be negative..
	{
		if (i == 0) // odd number of digits...
			dt.insert (dt.begin (), nS [(size_t) i] - '0');
		else 
			dt.insert (dt.begin (), ((nS [(size_t) i - 1] - '0') << 4 /** MSNibble. */) | (nS [(size_t) i] - '0' /** LSNibble. */));
	}

	return (MCHEmul::UInt (MCHEmul::UBytes (dt), true, MCHEmul::UInt::_PACKAGEDBCD));
}

// ---
MCHEmul::UInt MCHEmul::UInt::PackagedBCDFormatManager::fromInt (int n)
{
	std::vector <MCHEmul::UByte> dt;

	// Negative?
	unsigned int r = (n > 0) ? n : -n;

	std::string nS = std::to_string (r);
	for (int i = ((int) nS.length () - 1); i >= 0; i -= 2) // It might be negative..
	{
		if (i == 0) // odd number of digits...
			dt.insert (dt.begin (), nS [(size_t) i] - '0');
		else 
			dt.insert (dt.begin (), ((nS [(size_t) i - 1] - '0') << 4 /** MSNibble. */) | (nS [(size_t) i] - '0' /** LSNibble. */));
	}

	// In case the number (abs) is bigger than the maximum allowed at the saize already calculated...
	if (r > (unsigned int) std::pow (100, dt.size ())) dt.insert (dt.begin (), 0); // One bit more for the sign...

	return ((n < 0) 
		? MCHEmul::UInt (dt, true, MCHEmul::UInt::_PACKAGEDBCD).complement_2 () 
		: MCHEmul::UInt (dt, true, MCHEmul::UInt::_PACKAGEDBCD));
}

// ---
MCHEmul::UInt MCHEmul::UInt::add (const MCHEmul::UInt& u, bool iC) const
{
	if (_format != u._format)
		return (MCHEmul::UInt::_0); // Impossible to add different "structures"

	MCHEmul::UInt result;
	if (size () != u.size ())
	{
		size_t l = std::max (size (), u.size ());
		MCHEmul::UInt u1 = *this; MCHEmul::UInt u2 = u;
		u1.setMinLength (l); u2.setMinLength (l);

		result = MCHEmul::UInt::_formaters._formatManagers [_format] -> add (u1, u2, iC);
	}
	else
		result = MCHEmul::UInt::_formaters._formatManagers [_format] -> add (*this, u, iC);

	return (result);
}

// ---
MCHEmul::UInt MCHEmul::UInt::substract (const MCHEmul::UInt& u, bool iC) const
{
	if (_format != u._format)
		return (MCHEmul::UInt::_0); // Impossible to add different "structures"

	MCHEmul::UInt result;
	if (size () != u.size ())
	{
		size_t l = std::max (size (), u.size ());
		MCHEmul::UInt u1 = *this; MCHEmul::UInt u2 = u;
		u1.setMinLength (l); u2.setMinLength (l);

		result = MCHEmul::UInt::_formaters._formatManagers [_format] -> substract (u1, u2, iC);
	}
	else
		result = MCHEmul::UInt::_formaters._formatManagers [_format] -> substract (*this, u, iC);

	return (result);
}

// ---
MCHEmul::UInt MCHEmul::UInt::multiply (const MCHEmul::UInt& u) const
{
	MCHEmul::UInt result = *this;
	MCHEmul::UInt u2 = u;

	// Is the final outcome going to be negative?
	bool neg = (result.negative () && u.positive ()) || 
		(result.positive () && u.negative ());

	// All positive just for calculus...
	if (result.negative ()) result = result.complement_2 ();
	if (u2.negative ()) u2 = u2.complement_2 ();

	// Do the calculus...
	for (unsigned int i = (unsigned int) u2.asInt () /** always positive here. */; 
			i > 0; i--, result += u2);

	// The sign is assigned at the end!
	if (neg) result = result.complement_2 ();

	return (result);
}

// ---
MCHEmul::UInt MCHEmul::UInt::divide (const MCHEmul::UInt& u) const
{
	MCHEmul::UInt result = *this;
	MCHEmul::UInt u2 = u;

	// If the second is 0, the division is not possible and -0 is returned...
	if (u2 == MCHEmul::UInt::_0)
		return (MCHEmul::UInt::_0);

	// Is the final outcome going to be negative?
	bool neg = (result.negative () && u.positive ()) || 
		(result.positive () && u.negative ());

	// All positive just for calculus...
	if (result.negative ()) result = result.complement_2 ();
	if (u2.negative ()) u2 = u2.complement_2 ();

	// Do the calculus...
	for (unsigned int i = (unsigned int) u2.asInt () /** always positive here. */; 
			i > 0 && result != MCHEmul::UInt::_0; i--, result -= (result > u2) ? u2 : result);

	// The sign is assigned at the end!
	if (neg) result = result.complement_2 ();

	return (result);
}

// ---
MCHEmul::UInt MCHEmul::UInt::fromStr (const std::string& s, unsigned char f)
{
	MCHEmul::UInt result;

	if (s == "" || s == "-")
		return (result);

	bool n = false;
	std::string str = s;
	if (str [0] == '-')
	{
		n = true;
		str = str.substr (1);
	}

	if (!MCHEmul::validBytes (str))
		return (result);

	switch (str [0])
	{
		case 'z':
		case 'Z':
			{
				if (str.length () == 1)
					break;
				
				str = str.substr (1);

				auto toBin8 = [](const std::string& dt) -> unsigned int
				{
					unsigned int r = 0;
					for (size_t ct = 0; ct < dt.length (); ct++)
						r += (dt [dt.length () - 1 - ct] == '1') ? (1 << ct) : 0;
					return (r);
				};

				std::vector <MCHEmul::UByte> by;
				while (str != "")
				{ 
					if (str.length () <= 8)
						by.insert (by.begin (), fromUnsignedInt (toBin8 (str), f).bytes ()[0]);
					else
						by.insert (by.begin (), 
							fromUnsignedInt (toBin8 (str.substr (str.length () - 8, 8))).bytes ()[0] /** Sure there only one byte. */);
					str = (str.length () <= 8) ? "" : str.substr (0, str.length () - 8);
				}

					result = MCHEmul::UInt (by);
			}

			break;

		case '$':
			{
				if (str.length () == 1)
					break;

				str = str.substr (1);

				auto toHexa2 = [](const std::string& dt) -> unsigned int
				{
					unsigned int r = 0;
					std::istringstream ss (dt);
					ss >> std::hex >> r;
					return (r);
				};

				std::vector <MCHEmul::UByte> by;
				while (str != "")
				{ 
					if (str.length () <= 2)
						by.insert (by.begin (), fromUnsignedInt (toHexa2 (str), f).bytes ()[0]);
					else
						by.insert (by.begin (), 
							fromUnsignedInt (toHexa2 (str.substr (str.length () - 2, 2))).bytes ()[0] /** Sure there is only one byte. */);
					str = (str.length () <= 2) ? "" : str.substr (0, str.length () - 2);
				}

				result = MCHEmul::UInt (by);
			}
		
			break;

		case '0':
			{
				unsigned int i = 0;
				if (str.length () > 1)
				{
					std::istringstream ss (str.substr (1));
					ss >> std::oct >> i;
				}
			
				result = MCHEmul::UInt::fromUnsignedInt (i, f); // Big - endian
			}
		
			break;

		default:
			{
				unsigned int i;
				std::istringstream ss (str);
				ss >> std::dec >> i;
				result = MCHEmul::UInt::fromUnsignedInt (i, f); // Big - endian
			}
			
			break;
	}

	if (n)
		result = -result;

	return (result);
}
