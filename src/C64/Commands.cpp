#include <C64/Commands.hpp>
#include <C64/C64.hpp>
#include <sstream>

const std::string C64::CIA1StatusCommand::_NAME = "CCIA1";
const std::string C64::CIA2StatusCommand::_NAME = "CCIA2";
const std::string C64::PLAStatusCommand::_NAME = "CPLA";
const std::string C64::ScreenMemoryDUMPCommand::_NAME = "CSCREENDUMP";
const std::string C64::ColorMemoryDUMPCommand::_NAME = "CCOLORDUMP";
const std::string C64::BitmapMemoryDUMPCommand::_NAME = "CBITMAPDUMP";
const std::string C64::SpritesMemoryDUMPCommand::_NAME = "CSPRITESDUMP";
const std::string C64::SpritesDrawCommand::_NAME = "CSPRITESDRAW";
const std::string C64::CharactersDrawCommand::_NAME = "CCHARSDRAW";
const std::string C64::GridOnCommand::_NAME = "CGRIDON";
const std::string C64::GridOffCommand::_NAME = "CGRIDOFF";

// ---
void C64::CIA1StatusCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	// Only with a valid computer, but also a C64 one. 
	if (c == nullptr || 
		dynamic_cast <C64::Commodore64*> (c) == nullptr ||
		static_cast <C64::Commodore64*> (c) -> cia1 () == nullptr)
		return;

	rst.add ("CIA1", std::move (static_cast <C64::Commodore64*> (c) -> cia1 () -> getInfoStructure ()));
}

// ---
void C64::CIA2StatusCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	// Only with a valid computer, but also a C64 one. 
	if (c == nullptr || 
		dynamic_cast <C64::Commodore64*> (c) == nullptr ||
		static_cast <C64::Commodore64*> (c) -> cia2 () == nullptr)
		return;

	// Only with a valid computer, but also a C64 one. 
	rst.add ("CIA2", std::move (static_cast <C64::Commodore64*> (c) -> cia2 () -> getInfoStructure ()));
}

// ---
void C64::PLAStatusCommand::executeImpl (MCHEmul::CommandExecuter* cE,
	MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr || 
		dynamic_cast <C64::Commodore64*> (c) == nullptr ||
		static_cast <C64::Commodore64*> (c) -> pla () == nullptr)
		return;

	// Only with a valid computer, but also a C64 one. 
	rst.add ("PLA", std::move (static_cast <C64::Commodore64*> (c) -> pla () -> getInfoStructure ()));

}

// ---
void C64::ScreenMemoryDUMPCommand::executeImpl (MCHEmul::CommandExecuter* cE,
	MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr || 
		dynamic_cast <C64::Commodore64*> (c) == nullptr ||
		static_cast <C64::Commodore64*> (c) -> vicII () == nullptr)
		return;

	rst.add ("BYTES", 
		static_cast <C64::Commodore64*> (c) -> vicII () -> screenMemorySnapShot (c -> cpu ()));
}

// ---
void C64::ColorMemoryDUMPCommand::executeImpl (MCHEmul::CommandExecuter* cE,
	MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr || 
		dynamic_cast <C64::Commodore64*> (c) == nullptr ||
		static_cast <C64::Commodore64*> (c) -> vicII () == nullptr)
		return;

	rst.add ("BYTES", 
		static_cast <C64::Commodore64*> (c) -> vicII () -> colorMemorySnapShot (c -> cpu ()));
}

// ---
void C64::BitmapMemoryDUMPCommand::executeImpl (MCHEmul::CommandExecuter* cE,
	MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr || 
		dynamic_cast <C64::Commodore64*> (c) == nullptr ||
		static_cast <C64::Commodore64*> (c) -> vicII () == nullptr)
		return;

	rst.add ("BYTES", 
		static_cast <C64::Commodore64*> (c) -> vicII () -> bitmapMemorySnapShot (c -> cpu ()));
}

// ---
void C64::SpritesMemoryDUMPCommand::executeImpl (MCHEmul::CommandExecuter* cE,
	MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr || 
		dynamic_cast <C64::Commodore64*> (c) == nullptr ||
		static_cast <C64::Commodore64*> (c) -> vicII () == nullptr)
		return;

	rst.add ("BYTES", 
		static_cast <C64::Commodore64*> (c) -> vicII () -> spritesMemorySnapShot (c -> cpu ()));
}

// ---
void C64::SpritesDrawCommand::executeImpl (MCHEmul::CommandExecuter* cE,
	MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr || 
		dynamic_cast <C64::Commodore64*> (c) == nullptr ||
		static_cast <C64::Commodore64*> (c) -> vicII () == nullptr)
		return;

	std::vector <size_t> sprs { };
	if (!parameters ().empty ())
	{
		for (const auto& i : parameters ())
		{ 
			size_t nspr = (size_t) MCHEmul::UInt::fromStr (i.second.c_str ()).asUnsignedInt ();
			if ((nspr > 0 && nspr <= 8) && 
				std::find (sprs.begin (), sprs.end (), nspr) == sprs.end ()) // Not added before...
				sprs.emplace_back (nspr);
		}
	}

	rst.add ("DRAW", MCHEmul::concatenateStrings 
		(static_cast <C64::Commodore64*> (c) -> vicII () -> spritesDrawSnapshot (c -> cpu (), sprs), "\n"));
}

// ---
void C64::CharactersDrawCommand::executeImpl (MCHEmul::CommandExecuter* cE, 
	MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr || 
		dynamic_cast <C64::Commodore64*> (c) == nullptr ||
		static_cast <C64::Commodore64*> (c) -> vicII () == nullptr)
		return;

	std::vector <size_t> chrs { };
	if (!parameters ().empty ())
	{
		for (const auto& i : parameters ())
		{ 
			size_t nchr = (size_t) MCHEmul::UInt::fromStr (i.second.c_str ()).asUnsignedInt ();
			if ((nchr >= 0 && nchr < 256) && 
				std::find (chrs.begin (), chrs.end (), nchr) == chrs.end ()) // Not added before...
				chrs.emplace_back (nchr);
		}
	}

	rst.add ("DRAW", MCHEmul::concatenateStrings 
		(static_cast <C64::Commodore64*> (c) -> vicII () -> charsDrawSnapshot (c -> cpu (), chrs), "\n"));
}

// ---
void C64::GridOnCommand::executeImpl (MCHEmul::CommandExecuter* cE,
	MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr || 
		dynamic_cast <C64::Commodore64*> (c) == nullptr ||
		static_cast <C64::Commodore64*> (c) -> vicII () == nullptr)
		return;

	static_cast <C64::Screen*> (c -> device (C64::Screen::_ID)) -> 
		setDrawBorder (true, std::atoi (parameter ("00").c_str ()));
	static_cast <COMMODORE::VICII*> (static_cast <C64::Commodore64*> (c) -> vicII ()) -> 
		setDrawRasterInterruptPositions (true);
}

// ---
void C64::GridOffCommand::executeImpl (MCHEmul::CommandExecuter* cE,
	MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr || 
		dynamic_cast <C64::Commodore64*> (c) == nullptr ||
		static_cast <C64::Commodore64*> (c) -> vicII () == nullptr)
		return;

	dynamic_cast <C64::Screen*> (c -> device (C64::Screen::_ID)) -> 
		setDrawBorder (false, 0 /** It is not usefull. */);
	static_cast <COMMODORE::VICII*> (static_cast <C64::Commodore64*> (c) -> vicII ()) -> 
		setDrawRasterInterruptPositions (false);
}
