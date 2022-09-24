#include <core/Address.hpp>

// ---
std::ostream& MCHEmul::operator << (std::ostream& o, const MCHEmul::Address& a)
{ 
	return (o << "$" << a.asString (UByte::OutputFormat::_HEXA)); 
}

// ---
MCHEmul::Address MCHEmul::Address::fromStr (const std::string& str)
{
	return (MCHEmul::Address (MCHEmul::UInt::fromStr (str)));
}
