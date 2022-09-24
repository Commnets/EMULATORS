#include <core/Address.hpp>

// ---
MCHEmul::Address MCHEmul::Address::next (size_t n) const
{ 
	MCHEmul::UInt result = _value + MCHEmul::UInt::fromUnsignedInt ((unsigned int) n);

	if (result > _limit)
	{
		result -= _limit; 
		result = result.LSUInt (_value.size ());
	}
	
	return (MCHEmul::Address (result, _limit));
}

// ---
MCHEmul::Address MCHEmul::Address::previous (size_t n) const
{ 
	MCHEmul::UInt result = _value - MCHEmul::UInt::fromUnsignedInt ((unsigned int) n);

	if (result > _limit) /** But it is ngative... */
	{
		result += _limit + MCHEmul::UInt::_1; /** It is negative, so the carry has to be backed in. */
		result = result.LSUInt (_value.size ());
	}

	return (MCHEmul::Address (result, _limit));
}

// ---
std::ostream& MCHEmul::operator << (std::ostream& o, const MCHEmul::Address& a)
{ 
	return (o << "$" << a.asString (UByte::OutputFormat::_HEXA)); 
}
