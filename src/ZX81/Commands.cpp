#include <ZX81/Commands.hpp>
#include <ZX81/ZX81.hpp>
#include <ZX81/Screen.hpp>

const std::string ZX81::ULAStatusCommand::_NAME = "CULA";
const std::string ZX81::ULAShowEventsCommand::_NAME = "CULAEVENTS";
const std::string ZX81::DisplayFileDUMPCommand::_NAME = "CDFDUMP";
const std::string ZX81::CharactersDrawCommand::_NAME = "CCHARSDRAW";

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

// ---
void ZX81::DisplayFileDUMPCommand::executeImpl (MCHEmul::CommandExecuter* cE,
	MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr || 
		dynamic_cast <ZX81::SinclairZX81*> (c) == nullptr ||
		static_cast <ZX81::SinclairZX81*> (c) -> ula () == nullptr)
		return;

	rst.add ("BYTES", 
		static_cast <ZX81::SinclairZX81*> (c) -> displayFileSnapShot ());
}

// ---
void ZX81::CharactersDrawCommand::executeImpl (MCHEmul::CommandExecuter* cE, 
	MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr || 
		dynamic_cast <ZX81::SinclairZX81*> (c) == nullptr ||
		static_cast <ZX81::SinclairZX81*> (c) -> ula () == nullptr)
		return;

	std::vector <size_t> chrs { };
	if (!parameters ().empty ())
	{
		for (const auto& i : parameters ())
		{ 
			size_t nchr = (size_t) MCHEmul::UInt::fromStr (i.second.c_str ()).asUnsignedInt ();
			if ((nchr >= 0 && nchr < 64) && 
				std::find (chrs.begin (), chrs.end (), nchr) == chrs.end ()) // Not added before...
				chrs.emplace_back (nchr);
		}
	}

	rst.add ("DRAW", MCHEmul::concatenateStrings 
		(static_cast <ZX81::SinclairZX81*> (c) -> charsDrawSnapshot (c -> cpu (), chrs), "\n"));
}
