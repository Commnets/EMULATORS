#include <core/Chip.hpp>

// ---
std::ostream& MCHEmul::operator << (std::ostream& o, const MCHEmul::Chip& c)
{
	o << "---" << std::endl;
	o << "Chip Info" << std::endl;
	o << c._id << std::endl;
	o << c._attributes << std::endl;
	o << (*c._memory);

	return (o);
}
