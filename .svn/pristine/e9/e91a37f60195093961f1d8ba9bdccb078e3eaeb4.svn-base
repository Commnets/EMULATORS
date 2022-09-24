#include <CORE/Chip.hpp>

// ---
std::ostream& MCHEmul::operator << (std::ostream& o, const MCHEmul::Chip& c)
{
	o << "---" << std::endl;
	o << "Chip Info" << std::endl;
	o << c._id << std::endl;
	o << c._attributes;
	if (c.memoryRef () != nullptr)
		o << std::endl << (*c._memory);

	return (o);
}
