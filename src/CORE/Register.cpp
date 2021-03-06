#include <CORE/Register.hpp>

// ---
std::ostream& MCHEmul::operator << (std::ostream& o, const Register& r)
{ 
	return (o << r._name << "(" << r._id << "):$" << r.asString (UByte::OutputFormat::_HEXA, ' ', 2)); 
}
