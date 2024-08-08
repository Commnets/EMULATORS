#include <ZX81/Commands.hpp>
#include <ZX81/ZX81.hpp>
#include <ZX81/Screen.hpp>

const std::string ZX81::ULAStatusCommand::_NAME = "CULA";
const std::string ZX81::ULAShowEventsCommand::_NAME = "CULAEVENTS";

// ---
void ZX81::ULAStatusCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	// Only with a valid computer, but also a ZX81 one. 
	if (c == nullptr || 
		dynamic_cast <ZX81::SinclairZX81*> (c) == nullptr ||
		static_cast <ZX81::SinclairZX81*> (c) -> ula () == nullptr) // Just in case...
		return;

	rst.add ("ULA", std::move (static_cast <ZX81::SinclairZX81*> (c) -> ula () -> getInfoStructure ()));
}

// ---
void ZX81::ULAShowEventsCommand::executeImpl (MCHEmul::CommandExecuter* cE,
	MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	// Only with a valid computer, but also a ZX81 one. 
	if (c == nullptr || 
		dynamic_cast <ZX81::SinclairZX81*> (c) == nullptr ||
		static_cast <ZX81::SinclairZX81*> (c) -> ula () == nullptr) // Just in case...
		return;

	static_cast <ZX81::SinclairZX81*> (c) -> ula () -> setShowEvents ((parameter ("00") == "ON"));
}
