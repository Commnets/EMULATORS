#include <MSX/Commands.hpp>
#include <MSX/MSX.hpp>
#include <MSX/Screen.hpp>

// ---
const std::string MSX::VDPStatusCommand::_NAME = "CVDP";
const std::string MSX::PatternNameTableDUMPCommand::_NAME = "CPNAME";
const std::string MSX::PatternGenerationTableDUMPCommand::_NAME = "CPGEN";
const std::string MSX::ColorNameTableDUMPCommand::_NAME = "CCNAME";
const std::string MSX::SpritesDrawCommand::_NAME = "CSPRITESDRAW";

// ---
void MSX::VDPStatusCommand::executeImpl 
	(MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	// Only with a valid computer, but also a TEXASINSTRUMENTS one. 
	if (c == nullptr || 
		dynamic_cast <MSX::MSXComputer*> (c) == nullptr ||
		static_cast <MSX::MSXComputer*> (c) -> vdp () == nullptr)
		return;

	MSX::VDP* vdp = static_cast <MSX::MSXComputer*> (c) -> vdp ();

	// The structure is generated for the right chip!
	rst.add (vdp -> graphicalChip () -> className (), 
		std::move (static_cast <MSX::MSXComputer*> (c) -> vdp () -> getInfoStructure ()));
}

// ---
void MSX::PatternNameTableDUMPCommand::executeImpl (MCHEmul::CommandExecuter* cE,
	MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr || 
		dynamic_cast <MSX::MSXComputer*> (c) == nullptr)
		return;

	rst.add ("BYTES", 
		static_cast <MSX::MSXComputer*> (c) -> patternNameTableSnapShot ());
}

// ---
void MSX::PatternGenerationTableDUMPCommand::executeImpl (MCHEmul::CommandExecuter* cE,
	MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr || 
		dynamic_cast <MSX::MSXComputer*> (c) == nullptr)
		return;

	rst.add ("BYTES", 
		static_cast <MSX::MSXComputer*> (c) -> patternGenerationTableSnapShot ());
}

// ---
void MSX::ColorNameTableDUMPCommand::executeImpl (MCHEmul::CommandExecuter* cE,
	MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr || 
		dynamic_cast <MSX::MSXComputer*> (c) == nullptr)
		return;

	rst.add ("BYTES", 
		static_cast <MSX::MSXComputer*> (c) -> colorNameTableSnapShot ());
}

// ---
void MSX::SpritesDrawCommand::executeImpl (MCHEmul::CommandExecuter* cE,
	MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr || 
		dynamic_cast <MSX::MSXComputer*> (c) == nullptr)
		return;

	std::vector <size_t> sprs { };
	if (!parameters ().empty ())
	{
		for (const auto& i : parameters ())
		{ 
			size_t nspr = (size_t) MCHEmul::UInt::fromStr (i.second.c_str ()).asUnsignedInt ();
			if ((nspr > 0 && nspr <= 32) && 
				std::find (sprs.begin (), sprs.end (), nspr) == sprs.end ()) // Not added before...
				sprs.emplace_back (nspr);
		}
	}

	rst.add ("DRAW", MCHEmul::concatenateStrings 
		(static_cast <MSX::MSXComputer*> (c) -> spritesDrawSnapShot (sprs), "\n"));
}
