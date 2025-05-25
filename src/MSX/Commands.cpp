#include <MSX/Commands.hpp>
#include <MSX/MSX.hpp>
#include <MSX/Screen.hpp>

// ---
const std::string MSX::VDPStatusCommand::_NAME = "CVDP";
const std::string MSX::PatternNameTableDUMPCommand::_NAME = "CPNAME";
const std::string MSX::PatternGenerationTableDUMPCommand::_NAME = "CPGEN";
const std::string MSX::ColorNameTableDUMPCommand::_NAME = "CCNAME";
const std::string MSX::SpritesDrawCommand::_NAME = "CSPRITESDRAW";
const std::string MSX::VDPMemoryStatusCommand::_NAME = "CVDPMEMORY";
const std::string MSX::SetVDPMemoryValueCommand::_NAME = "CVDPSETMEMORY";
const std::string MSX::VDPShowEventsCommand::_NAME = "CVDPEVENTS";

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

// ---

// ---
void MSX::VDPMemoryStatusCommand::executeImpl 
	(MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr || 
		dynamic_cast <MSX::MSXComputer*> (c) == nullptr)
		return;

	MSX::MSXComputer* mC = static_cast <MSX::MSXComputer*> (c);

	MCHEmul::Address a1 = MCHEmul::Address::fromStr (parameter ("00"));
	if (a1 > c -> cpu () -> architecture ().longestAddressPossible ()) 
		return;

	if (_parameters.size () == 2)
	{
		MCHEmul::Address a2 = MCHEmul::Address::fromStr (parameter ("01"));
		if (a2 > c -> cpu () -> architecture ().longestAddressPossible ())
			return;

		MCHEmul::Address iA = ((a1 <= a2) ? a1 : a2);
		rst.add ("BYTES", MCHEmul::UBytes (mC -> vdp () -> videoData (iA, (((a2 >= a1) ? a2 : a1) - iA) + 1)).
			asString (MCHEmul::UByte::OutputFormat::_HEXA, ',', 2));
	}
	else
		rst.add ("BYTES", mC -> vdp () -> videoData (a1).
			asString (MCHEmul::UByte::OutputFormat::_HEXA, 2));
}

// ---
void MSX::SetVDPMemoryValueCommand::executeImpl
	(MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr || 
		dynamic_cast <MSX::MSXComputer*> (c) == nullptr)
		return;

	MSX::MSXComputer* mC = static_cast <MSX::MSXComputer*> (c);

	MCHEmul::Address a1 = MCHEmul::Address::fromStr (parameter ("00"));
	if (a1 > c -> cpu () -> architecture ().longestAddressPossible ())
	{ 
		rst.add ("ERROR", std::string ("Address out of limits"));

		return;
	}
	
	MCHEmul::Address a2 = a1;
	MCHEmul::UByte v;
	if (_parameters.size () == 3)
	{
		a2 = MCHEmul::Address::fromStr (parameter ("01"));
		if (a2 > c -> cpu () -> architecture ().longestAddressPossible ())
		{
			rst.add ("ERROR", std::string ("Address out of limits"));

			return;
		}

		v = MCHEmul::UInt::fromStr (parameter ("02")).bytes ()[0];
	}
	else
		v = MCHEmul::UInt::fromStr (parameter ("01")).bytes ()[0];

	MCHEmul::Address iA = (a1 <= a2) ? a1 : a2;
	MCHEmul::Address fA = (a2 >= a1) ? a2 : a1;
	for (size_t i = 0; i <= (size_t) (fA - iA); i += v.size ())
		mC -> vdp () -> setVideoData (iA + i, v);
	rst.add ("ERROR", std::string ("No errors"));
}

// ---

// ---
void MSX::VDPShowEventsCommand::executeImpl (MCHEmul::CommandExecuter* cE,
	MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr || 
		dynamic_cast <MSX::MSXComputer*> (c) == nullptr)
		return;

	MSX::MSXComputer* mC = static_cast <MSX::MSXComputer*> (c);

	mC -> vdp () -> setShowEvents ((parameter ("00") == "ON"));
}
