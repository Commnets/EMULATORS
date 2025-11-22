#include <C264/Commands.hpp>
#include <C264/C264.hpp>

const std::string C264::ScreenMemoryDUMPCommand::_NAME = "CSCREENDUMP";
const std::string C264::AttributeMemoryDUMPCommand::_NAME = "CATTRIBUTEDUMP";
const std::string C264::BitmapMemoryDUMPCommand::_NAME = "CBITMAPDUMP";
const std::string C264::CharactersDrawCommand::_NAME = "CCHARSDRAW";
const std::string C264::GridOnCommand::_NAME = "CGRIDON";
const std::string C264::GridOffCommand::_NAME = "CGRIDOFF";

// ---
void C264::ScreenMemoryDUMPCommand::executeImpl (MCHEmul::CommandExecuter* cE,
	MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr || 
		dynamic_cast <C264::Commodore264*> (c) == nullptr ||
		static_cast <C264::Commodore264*> (c) -> ted () == nullptr)
		return;

	rst.add ("BYTES", 
		static_cast <C264::Commodore264*> (c) -> ted () -> screenMemorySnapShot (c -> cpu ()));
}

// ---
void C264::AttributeMemoryDUMPCommand::executeImpl (MCHEmul::CommandExecuter* cE,
	MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr || 
		dynamic_cast <C264::Commodore264*> (c) == nullptr ||
		static_cast <C264::Commodore264*> (c) -> ted () == nullptr)
		return;

	rst.add ("BYTES", 
		static_cast <C264::Commodore264*> (c) -> ted () -> attributeMemorySnapShot (c -> cpu ()));
}

// ---
void C264::BitmapMemoryDUMPCommand::executeImpl (MCHEmul::CommandExecuter* cE,
	MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr || 
		dynamic_cast <C264::Commodore264*> (c) == nullptr ||
		static_cast <C264::Commodore264*> (c) -> ted () == nullptr)
		return;

	rst.add ("BYTES", 
		static_cast <C264::Commodore264*> (c) -> ted () -> bitmapMemorySnapShot (c -> cpu ()));
}

// ---
void C264::CharactersDrawCommand::executeImpl (MCHEmul::CommandExecuter* cE, 
	MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr || 
		dynamic_cast <C264::Commodore264*> (c) == nullptr ||
		static_cast <C264::Commodore264*> (c) -> ted () == nullptr)
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
		(static_cast <C264::Commodore264*> (c) -> ted () -> charsDrawSnapshot (c -> cpu (), chrs), "\n"));
}

// ---
void C264::GridOnCommand::executeImpl (MCHEmul::CommandExecuter* cE,
	MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return;

	c -> screen () -> setDrawGrid (true, std::atoi (parameter ("00").c_str ()));

	if (!existParameter ("01") ||
		(existParameter ("01") &&
		 (dynamic_cast <C264::Commodore264*> (c) == nullptr ||
		  static_cast <C264::Commodore264*> (c) -> ted () == nullptr)))
		return;

	// Draw the position of the raster interrupt...
	static_cast <COMMODORE::TED*> (static_cast <C264::Commodore264*> (c) -> ted ()) -> 
		setDrawRasterInterruptPositions (true);
}

// ---
void C264::GridOffCommand::executeImpl (MCHEmul::CommandExecuter* cE,
	MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return;

	c -> screen () -> setDrawGrid (false, 0 /** It doesn't matter. */);

	if (dynamic_cast <C264::Commodore264*> (c) == nullptr ||
		static_cast <C264::Commodore264*> (c) -> ted () == nullptr)
		return;

	static_cast <COMMODORE::TED*> (static_cast <C264::Commodore264*> (c) -> ted ()) -> 
		setDrawRasterInterruptPositions (false);
}
