#include <CORE/ProgramCounter.hpp>

// ---
std::ostream& MCHEmul::operator << (std::ostream& o, const MCHEmul::ProgramCounter& pc)
{
	return (o << "PC:$" << pc.asString (MCHEmul::UByte::OutputFormat::_HEXA));
}
