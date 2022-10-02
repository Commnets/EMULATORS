#include <CORE/Chip.hpp>

// ---
std::ostream& MCHEmul::operator << (std::ostream& o, const MCHEmul::Chip& c)
{
	o << c._id << std::endl;
	o << c._attributes;
	if (c.memoryRef () != nullptr)
		o << std::endl << (*c._memory);

	return (o);
}
