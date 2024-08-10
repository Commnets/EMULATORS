#include <ZXSpectrum/Commands.hpp>
#include <ZXSpectrum/ZXSpectrum.hpp>
#include <ZXSpectrum/Screen.hpp>
#include <ZXSpectrum/ULA.hpp>

const std::string ZXSPECTRUM::ULAStatusCommand::_NAME = "CULA";
const std::string ZXSPECTRUM::ULAShowEventsCommand::_NAME = "CULAEVENTS";

// ---
void ZXSPECTRUM::ULAStatusCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	// Only with a valid computer, but also a ZX81 one. 
	if (c == nullptr || 
		dynamic_cast <ZXSPECTRUM::SinclairZXSpectrum*> (c) == nullptr ||
		static_cast <ZXSPECTRUM::SinclairZXSpectrum*> (c) -> ula () == nullptr) // Just in case...
		return;

	rst.add ("ULA", std::move (static_cast <ZXSPECTRUM::SinclairZXSpectrum*> (c) -> ula () -> getInfoStructure ()));
}

// ---
void ZXSPECTRUM::ULAShowEventsCommand::executeImpl (MCHEmul::CommandExecuter* cE,
	MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	// Only with a valid computer, but also a ZX81 one. 
	if (c == nullptr || 
		dynamic_cast <ZXSPECTRUM::SinclairZXSpectrum*> (c) == nullptr ||
		static_cast <ZXSPECTRUM::SinclairZXSpectrum*> (c) -> ula () == nullptr) // Just in case...
		return;

	static_cast <ZXSPECTRUM::SinclairZXSpectrum*> (c) -> ula () -> setShowEvents ((parameter ("00") == "ON"));
}
