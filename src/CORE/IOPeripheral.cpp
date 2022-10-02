#include <CORE/IOPeripheral.hpp>

// ---
std::ostream& MCHEmul::operator << (std::ostream& o, const MCHEmul::IOPeripheral& d)
{
	o << d._id << std::endl;
	o << d._attributes;

	return (o);
}
