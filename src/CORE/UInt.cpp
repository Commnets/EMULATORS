#include <CORE/UInt.hpp>
#include <algorithm>
#include <sstream>

// ---
const MCHEmul::UInt MCHEmul::UInt::_0 = MCHEmul::UInt ({ MCHEmul::UByte::_0});
const MCHEmul::UInt MCHEmul::UInt::_1 = MCHEmul::UInt ({ MCHEmul::UByte::_1});

// ---
MCHEmul::UInt MCHEmul::UInt::add (const MCHEmul::UInt& u, bool iC) const
{
	MCHEmul::UInt nU1 = *this;
	MCHEmul::UInt nU2 = u;
	size_t mL = std::max (nU1.size (), nU2.size ());
	// The operation has to be done with uints of the same length...
	nU1.setMinLength (mL); nU2.setMinLength (mL);

	// Numbers are always stored in Big-endian format...
	std::vector <MCHEmul::UByte> dt;
	bool c = iC;
	for (int i = (int) (nU1.size () - 1); i >= 0; i--) 
	{
		unsigned int s = 
			(unsigned int) (nU1._values [(size_t) i].value ()) + 
			(unsigned int) (nU2._values [(size_t) i].value ()) + 
			(unsigned int) ((c) ? MCHEmul::UByte::_1 : MCHEmul::UByte::_0); 
		c = (s > (unsigned int) MCHEmul::UByte::_F);
		dt.insert (dt.begin (), MCHEmul::UByte ((unsigned char) s)); // Cut
	}

	MCHEmul::UInt result (dt);
	result._carry = c;
	result._overflow = (nU1.negative () ^ result.negative ()) & (nU2.negative () ^ result.negative ());

	return (result);
}

// ---
MCHEmul::UInt MCHEmul::UInt::complement () const
{
	std::vector <MCHEmul::UByte> dt;
	for (size_t i = 0; i < size (); i++) 
		dt.push_back (_values [(size_t) i].complement ());

	MCHEmul::UInt result (dt);

	return (result);
}

// ---
MCHEmul::UInt MCHEmul::UInt::substract (const MCHEmul::UInt& u) const
{ 
	MCHEmul::UInt nU1 = *this;
	MCHEmul::UInt nU2 = u;
	size_t mL = std::max (nU1.size (), nU2.size ());
	// The operation has to be done with uints of the same length...
	nU1.setMinLength (mL); nU2.setMinLength (mL);

	return (nU1.add (nU2.complement_2 ()));
}

// ---
MCHEmul::UInt MCHEmul::UInt::multiply (const MCHEmul::UInt& u) const
{
	MCHEmul::UInt nU1 = *this;
	MCHEmul::UInt nU2 = u;
	size_t mL = std::max (nU1.size (), nU2.size ());
	// The operation has to be done with uints of the same length...
	nU1.setMinLength (mL); nU2.setMinLength (mL);

	MCHEmul::UInt result = MCHEmul::UInt::fromInt (0);
	unsigned int n = (unsigned int) std::abs (nU2.asInt ());
	for (unsigned int i = 0; i < n; i++)
		result += nU1;

	return (result);
}

// ---
bool MCHEmul::UInt::operator > (const MCHEmul::UInt& u) const
{
	if (positive () && u.negative ())
		return (true);
	if (negative () && u.positive ())
		return (false);

	// Both with the same sign...
	// When there is two negative numbers represented in complement_2, the comparation is the same... 
	bool result = false;
	bool cont = true;
	for (size_t i = 0; i < size () && cont; i++)
		cont = !(result = (_values [i].value () > u._values [i].value ())) && 
			(_values [i].value () == u._values [i].value ());
	return (result);
}

// ---
bool MCHEmul::UInt::operator < (const MCHEmul::UInt& u) const
{
	if (negative () && u.positive ())
		return (true);
	if (positive () && u.negative ())
		return (false);

	// Both with the same sign...
	// When there is two negative numbers represented in complement_2, the comparation is the same... 
	bool result = false;
	bool cont = true;
	for (size_t i = 0; i < size () && cont; i++)
		cont = !(result = (_values [i].value () < u._values [i].value ())) && 
			(_values [i].value () == u._values [i].value ());
	return (result);
}

// ---
unsigned int MCHEmul::UInt::asUnsignedInt () const
{
	unsigned int result = 0;

	int c = 0;
	for (int i = (int) (size () - 1); i >= 0; i--, c++)
		result += _values [(size_t) i].value () << (c * MCHEmul::UByte::sizeBits ());

	return (result);
}

// ---
MCHEmul::UInt MCHEmul::UInt::fromUnsignedInt (unsigned int n)
{
	std::vector <MCHEmul::UByte> dt;

	size_t nB = 1;
	while ((n / (MCHEmul::UByte::_1 << (nB * MCHEmul::UByte::sizeBits ()))) != 0) nB++; 
	
	unsigned int r = n;
	for (size_t i = nB - 1; i > 0; i--)
	{
			unsigned int dv = MCHEmul::UByte::_1 << (i * MCHEmul::UByte::sizeBits ());
			dt.push_back (r / dv);
			r = r % dv;
	}

	dt.push_back (r);

	return (MCHEmul::UInt (MCHEmul::UBytes (dt)));
}

// ---
MCHEmul::UInt MCHEmul::UInt::fromInt (int n)
{
	std::vector <MCHEmul::UByte> dt;

	unsigned int r = (n > 0) ? n : -n;

	size_t nB = 1;
	while ((r / (MCHEmul::UByte::_1 << (nB * MCHEmul::UByte::sizeBits ()))) != 0) nB++;
	if (r > (unsigned int) (0x01 << (nB * MCHEmul::UByte::sizeBits () - 1))) nB++; // One bit more for the the sign...

	for (size_t i = nB - 1; i > 0; i--)
	{
			unsigned int dv = MCHEmul::UByte::_1 << (i * MCHEmul::UByte::sizeBits ());
			dt.push_back (r / dv);
			r = r % dv;
	}

	dt.push_back (r);

	return ((n < 0) ? MCHEmul::UInt (dt).complement_2 () : MCHEmul::UInt (dt));
}

// ---
MCHEmul::UInt MCHEmul::UInt::fromStr (const std::string& s)
{
	MCHEmul::UInt result ({ 0x00 });

	if (!MCHEmul::validBytes (s))
		return (result);

	switch (s [0])
	{
		case '$':
		{
			unsigned int i;
			std::istringstream ss (s.substr (1));
			ss >> std::hex >> i;
			result = MCHEmul::UInt::fromUnsignedInt (i); // Big - endian
		}
		
		break;

		case '0':
		{
			unsigned int i;
			std::istringstream ss (s.substr (1));
			ss >> std::oct >> i;
			result = MCHEmul::UInt::fromUnsignedInt (i); // Big - endian
		}
		
		break;

		default:
		{
			unsigned int i;
			std::istringstream ss (s);
			ss >> std::dec >> i;
			result = MCHEmul::UInt::fromUnsignedInt (i); // Big - endian
		}
			
		break;
	}

	return (result);
}
