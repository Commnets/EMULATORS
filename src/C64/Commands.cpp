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
const std::string C64::ManagePaddlesCommand::_NAME = "CPADDLE";

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
	if (c == nullptr)
		return;

	c -> screen () -> setDrawGrid (true, std::atoi (parameter ("00").c_str ()));

	if (!existParameter ("01") ||
		(existParameter ("01") &&
		 (dynamic_cast <C64::Commodore64*> (c) == nullptr ||
		  static_cast <C64::Commodore64*> (c) -> vicII () == nullptr)))
		return;

	// Draw the position of the raster interrupt...
	static_cast <COMMODORE::VICII*> (static_cast <C64::Commodore64*> (c) -> vicII ()) -> 
		setDrawRasterInterruptPositions (true);
	// Draw the position of the sprites...
	static_cast <COMMODORE::VICII*> (static_cast <C64::Commodore64*> (c) -> vicII ()) ->
		setDrawSpritesBorder (true);
}

// ---
void C64::GridOffCommand::executeImpl (MCHEmul::CommandExecuter* cE,
	MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return;

	c -> screen () -> setDrawGrid (false, 0 /** It doesn't matter. */);

	if (dynamic_cast <C64::Commodore64*> (c) == nullptr ||
		static_cast <C64::Commodore64*> (c) -> vicII () == nullptr)
		return;

	static_cast <COMMODORE::VICII*> (static_cast <C64::Commodore64*> (c) -> vicII ()) -> 
		setDrawRasterInterruptPositions (false);
	static_cast <COMMODORE::VICII*> (static_cast <C64::Commodore64*> (c) -> vicII ()) ->
		setDrawSpritesBorder (false);
}

// ---
void C64::ManagePaddlesCommand::executeImpl (MCHEmul::CommandExecuter* cE,
	MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return;

	if (dynamic_cast <C64::Commodore64*> (c) == nullptr ||
		static_cast <C64::Commodore64*> (c) -> cia1 () == nullptr)
		return;

	C64::CIA1* cia1 = static_cast <C64::Commodore64*> (c) -> cia1 ();
	bool act = parameter ("00") == "ON"; // Active?
	std::vector <size_t> portsId; // In which ports?
	if (parameters ().size () > 1)
	{
		size_t i = 1;
		std::string pIdS = MCHEmul::fixLenStr (std::to_string (i), 2, true, MCHEmul::_CEROS);
		while (existParameter (pIdS))
		{
			size_t pId = (size_t) std::atoi (parameter (pIdS).c_str ());
			if (pId == 0 || pId == 1) // The ids can only be for port 0 or 1...
				portsId.emplace_back (pId);
		}
	}

	// Action for all id or just for a coupe (no more than 2)
	if (portsId.empty ())
	{
		if (act) cia1 -> connectAllPaddles ();
		else cia1 -> disconnectAllPaddles ();
	}
	else
	{
		for (auto i : portsId)
		{
			if (act) cia1 -> connectPaddleAtPort (i);
			else cia1 -> disconnectPaddleAtPort (i);
		}
	}
}
