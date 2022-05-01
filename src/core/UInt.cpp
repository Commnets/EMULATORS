#include <core/UInt.hpp>
#include <algorithm>

// ---
const MCHEmul::UInt MCHEmul::UInt::_0 = MCHEmul::UInt (MCHEmul::UBytes ({ MCHEmul::UByte::_0}));
const MCHEmul::UInt MCHEmul::UInt::_1 = MCHEmul::UInt (MCHEmul::UBytes ({ MCHEmul::UByte::_1}));

// ---
MCHEmul::UInt MCHEmul::UInt::add (const MCHEmul::UInt& u) const
{
	MCHEmul::UInt nU1 = *this;
	MCHEmul::UInt nU2 = u;
	size_t mL = std::max (nU1.size (), nU2.size ());
	nU1.setMinLength (mL); nU2.setMinLength (mL);

	std::vector <MCHEmul::UByte> dt;
	bool c = false;
	for (int i = (int) (nU1.size () - 1); i >= 0; i--) 
	{
		unsigned int s = 
			(unsigned int) (nU1._bytes [(size_t) i].value ()) + 
			(unsigned int) (nU2._bytes [(size_t) i].value ()) + 
			(unsigned int) ((c) ? 1 : 0); 
		c = (s > (unsigned int) MCHEmul::UByte::_F);
		dt.insert (dt.begin (), MCHEmul::UByte ((unsigned char) s)); // Cut
	}

	if (c)
		dt.insert (dt.begin (), MCHEmul::UByte::_1);

	return (MCHEmul::UInt (MCHEmul::UBytes (dt)));
}

// ---
MCHEmul::UInt MCHEmul::UInt::complement () const
{
	std::vector <MCHEmul::UByte> dt;
	for (size_t i = 0; i < size (); i++) 
		dt.push_back (_bytes [(size_t) i].complement ());

	return (MCHEmul::UInt (MCHEmul::UBytes (dt)));
}

// ---
MCHEmul::UInt MCHEmul::UInt::substract (const MCHEmul::UInt& u) const
{ 
	MCHEmul::UInt nU = u; 

	nU.setMinLength (size ()); /** for the addition with the complement to be done properly. */

	MCHEmul::UInt result = add (nU.complement ());

	if (result.size () > size ()) /** The result is positive meaning a carry has been generated. */
		result = result.LSUInt (size ()).add (MCHEmul::UInt::_1); 

	return (result); 
}

// ---
bool MCHEmul::UInt::operator > (const MCHEmul::UInt& u) const
{
	if (size () < u.size ())
		return (false);

	if (size () > u.size ())
		return (true);

	bool result = false;
	for (size_t i = 0; i < size () && !result; i++)
		result = (_bytes [i].value () > u._bytes [i].value ());

	return (result);
}

// ---
bool MCHEmul::UInt::operator < (const MCHEmul::UInt& u) const
{
	if (size () > u.size ())
		return (false);

	if (size () < u.size ())
		return (true);

	bool result = false;
	for (size_t i = 0; i < size () && !result; i++)
		result = (_bytes [i].value () < u._bytes [i].value ());

	return (result);
}

// ---
unsigned int MCHEmul::UInt::asUnsignedInt () const
{
	unsigned int result = 0;

	int c = 0;
	for (int i = (int) (size () - 1); i >= 0; i--, c++)
		result += _bytes [(size_t) i].value () << (c * MCHEmul::UByte::sizeBits ());

	return (result);
}

// ---
int MCHEmul::UInt::asInt () const
{
	int result = 0;

	// The sign if in the first bit of the first byte (kept always in Big-endian format)
	// Then the sign is removed from the bytes to convert int int (it will be used later)
	bool n = _bytes [0][MCHEmul::UByte::sizeBits () - 1];
	MCHEmul::UBytes bC = _bytes;
	bC << 1 >> 1; // bC.shiftLeft (1).shiftRight (1); To eliminate the last bit

	int c = 0;
	for (int i = (int) (size () - 1); i >= 0; i--, c++)
		result += (int) bC [(size_t) i].value () << (c * MCHEmul::UByte::sizeBits ());

	return (n ? -result : result);
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

	// Calculate the number of bytes necessary to store the value...
	size_t nB = 1;
	while ((std::abs (n) / (MCHEmul::UByte::_1 << (nB * MCHEmul::UByte::sizeBits ()))) != 0) nB++;
	if (std::abs (n) > (0x01 << (nB * MCHEmul::UByte::sizeBits () - 1))) nB++;

	// Then convert the number (the positive versión)
	// The sign will be considered later...
	unsigned int r = (unsigned int) n;
	for (size_t i = nB - 1; i > 0; i--)
	{
			unsigned int dv = MCHEmul::UByte::_1 << (i * MCHEmul::UByte::sizeBits ());
			dt.push_back (r / dv);
			r = r % dv;
	}

	dt.push_back (r);
	dt [0].setBit (MCHEmul::UByte::sizeBits () - 1, n < 0);

	return (MCHEmul::UInt (MCHEmul::UBytes (dt)));
}
