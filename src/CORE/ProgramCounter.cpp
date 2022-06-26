#include <CORE/ProgramCounter.hpp>

// ---
std::ostream& MCHEmul::operator << (std::ostream& o, const MCHEmul::ProgramCounter& pc)
{
	return (o << pc._name << ":$" << pc.asString (MCHEmul::UByte::OutputFormat::_HEXA));
}
