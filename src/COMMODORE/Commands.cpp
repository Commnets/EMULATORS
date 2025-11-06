#include <COMMODORE/Commands.hpp>
#include <COMMODORE/Computer.hpp>
#include <sstream>

const std::string COMMODORE::VICIStatusCommand::_NAME = "CVICI";
const std::string COMMODORE::VICIIStatusCommand::_NAME = "CVICII";
const std::string COMMODORE::VICIIShowEventsCommand::_NAME = "CVICIIEVENTS";
const std::string COMMODORE::TEDStatusCommand::_NAME = "CTED";
const std::string COMMODORE::TEDShowEventsCommand::_NAME = "CTEDEVENTS";
const std::string COMMODORE::VIAStatusCommand::_NAME = "CVIA";
const std::string COMMODORE::CIAStatusCommand::_NAME = "CCIA";
const std::string COMMODORE::SIDStatusCommand::_NAME = "CSID";
const std::string COMMODORE::SIDWrapperCommand::_NAME = "CSIDW";

// ---
void COMMODORE::VICIStatusCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	// Only with a valid computer, but also a COMMODORE one. 
	if (c == nullptr || 
		dynamic_cast <COMMODORE::Computer*> (c) == nullptr ||
		static_cast <COMMODORE::Computer*> (c) -> vicI () == nullptr)
		return;

	rst.add ("VICI", std::move (static_cast <COMMODORE::Computer*> (c) -> vicI () -> getInfoStructure ()));
}

// ---
void COMMODORE::VICIIStatusCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	// Only with a valid computer, but also a COMMODORE one. 
	if (c == nullptr || 
		dynamic_cast <COMMODORE::Computer*> (c) == nullptr ||
		static_cast <COMMODORE::Computer*> (c) -> vicII () == nullptr)
		return;

	rst.add ("VICII", std::move (static_cast <COMMODORE::Computer*> (c) -> vicII () -> getInfoStructure ()));
}

// ---
void COMMODORE::VICIIShowEventsCommand::executeImpl (MCHEmul::CommandExecuter* cE,
	MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	// Only with a valid computer, but also a ZX81 one. 
	if (c == nullptr || 
		dynamic_cast <COMMODORE::Computer*> (c) == nullptr ||
		static_cast <COMMODORE::Computer*> (c) -> vicII () == nullptr) // Just in case...
		return;

	static_cast <COMMODORE::Computer*> (c) -> vicII () -> setDrawOtherEvents ((parameter ("00") == "ON"));
}

// ---
void COMMODORE::TEDStatusCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	// Only with a valid computer, but also a COMMODORE one. 
	if (c == nullptr || 
		dynamic_cast <COMMODORE::Computer*> (c) == nullptr ||
		static_cast <COMMODORE::Computer*> (c) -> ted () == nullptr)
		return;

	rst.add ("TED", std::move (static_cast <COMMODORE::Computer*> (c) -> ted () -> getInfoStructure ()));
}

// ---
void COMMODORE::TEDShowEventsCommand::executeImpl (MCHEmul::CommandExecuter* cE,
	MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	// Only with a valid computer, but also a ZX81 one. 
	if (c == nullptr || 
		dynamic_cast <COMMODORE::Computer*> (c) == nullptr ||
		static_cast <COMMODORE::Computer*> (c) -> ted () == nullptr) // Just in case...
		return;

	static_cast <COMMODORE::Computer*> (c) -> ted () -> setDrawOtherEvents ((parameter ("00") == "ON"));
}

// ---
void COMMODORE::VIAStatusCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	// Only with a valid computer, but also a COMMODORE one. 
	if (c == nullptr || 
		dynamic_cast <COMMODORE::Computer*> (c) == nullptr ||
		static_cast <COMMODORE::Computer*> (c) -> via () == nullptr)
		return;

	rst.add ("VIA", std::move (static_cast <COMMODORE::Computer*> (c) -> via () -> getInfoStructure ()));
}

// ---
void COMMODORE::CIAStatusCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	// Only with a valid computer, but also a COMMODORE one. 
	if (c == nullptr || 
		dynamic_cast <COMMODORE::Computer*> (c) == nullptr ||
		static_cast <COMMODORE::Computer*> (c) -> cia () == nullptr)
		return;

	rst.add ("CIA", std::move (static_cast <COMMODORE::Computer*> (c) -> cia () -> getInfoStructure ()));
}

// ---
void COMMODORE::SIDStatusCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	// Only with a valid computer, but also a COMMODORE one. 
	if (c == nullptr || 
		dynamic_cast <COMMODORE::Computer*> (c) == nullptr ||
		static_cast <COMMODORE::Computer*> (c) -> sid () == nullptr)
		return;

	rst.add ("SID", std::move (static_cast <COMMODORE::Computer*> (c) -> sid () -> getInfoStructure ()));
}

// ---
void COMMODORE::SIDWrapperCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	// Only with a valid computer, but also a C64 one. 
	if (c == nullptr || 
		dynamic_cast <COMMODORE::Computer*> (c) == nullptr ||
		static_cast <COMMODORE::Computer*> (c) -> sid () == nullptr)
		return;

	MCHEmul::SoundLibWrapper* wrapper = nullptr;
	switch (std::atoi (parameter ("00").c_str ()))
	{
		case 0:
			{
				wrapper = new COMMODORE::SoundRESIDWrapper
					(c -> clock ().cyclesPerSecond (), RESID::SAMPLE_FAST,
						static_cast <COMMODORE::Computer*> (c) -> sid () -> samplingFrecuency ());
			}

			break;

		case 1:
			{
				wrapper = new COMMODORE::SoundSIDSimpleWrapper 
					(c -> clock ().cyclesPerSecond (),
					 static_cast <COMMODORE::Computer*> (c) -> sid () -> samplingFrecuency ());
			}

			break;

		default:
			break;
	}

	if (wrapper == nullptr)
		rst.add ("ERRORS", std::string ("Wrapper not defined"));
	else
		static_cast <COMMODORE::Computer*> (c) -> sid () -> setSoundWrapper (wrapper);
}
