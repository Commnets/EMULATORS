#include <core/Register.hpp>

// ---
std::ostream& MCHEmul::operator << (std::ostream& o, const Register& r)
{ 
	return (o << r._id << ":" << r.asString (UByte::OutputFormat::_HEXA, ' ')); 
}
