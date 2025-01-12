#include <ZXSpectrum/Commands.hpp>
#include <ZXSpectrum/ZXSpectrum.hpp>
#include <ZXSpectrum/Screen.hpp>
#include <ZXSpectrum/ULA.hpp>

const std::string ZXSPECTRUM::ULAStatusCommand::_NAME = "CULA";
const std::string ZXSPECTRUM::ULAShowEventsCommand::_NAME = "CULAEVENTS";
const std::string ZXSPECTRUM::ScreenMemoryDUMPCommand::_NAME = "CSCREENDUMP";
const std::string ZXSPECTRUM::ColorMemoryDUMPCommand::_NAME = "CCOLORDUMP";
const std::string ZXSPECTRUM::CharactersDrawCommand::_NAME = "CCHARSDRAW";

// ---
void ZXSPECTRUM::ULAStatusCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	ZXSPECTRUM::SinclairZXSpectrum* cS =
		dynamic_cast <ZXSPECTRUM::SinclairZXSpectrum*> (c);

	// Only with a valid computer, but also a ZXSpectrum one. 
	if (cS == nullptr || 
		cS -> ula () == nullptr) // Just in case...
		return;

	rst.add ("ULA", std::move (cS -> ula () -> getInfoStructure ()));
}

// ---
void ZXSPECTRUM::ULAShowEventsCommand::executeImpl (MCHEmul::CommandExecuter* cE,
	MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	ZXSPECTRUM::SinclairZXSpectrum* cS =
		dynamic_cast <ZXSPECTRUM::SinclairZXSpectrum*> (c);

	// Only with a valid computer, but also a ZXSpectrum one. 
	if (cS == nullptr || 
		cS -> ula () == nullptr) // Just in case...
		return;

	cS -> ula () -> setShowEvents ((parameter ("00") == "ON"));
}

// ---
void ZXSPECTRUM::ScreenMemoryDUMPCommand::executeImpl (MCHEmul::CommandExecuter* cE,
	MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	ZXSPECTRUM::SinclairZXSpectrum* cS =
		dynamic_cast <ZXSPECTRUM::SinclairZXSpectrum*> (c);

	if (cS == nullptr)
		return;

	rst.add ("BYTES", cS -> screenMemorySnapShot ());
}

// ---
void ZXSPECTRUM::ColorMemoryDUMPCommand::executeImpl (MCHEmul::CommandExecuter* cE,
	MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	ZXSPECTRUM::SinclairZXSpectrum* cS =
		dynamic_cast <ZXSPECTRUM::SinclairZXSpectrum*> (c);

	if (cS == nullptr)
		return;

	rst.add ("BYTES", cS -> colorMemorySnapShot ());
}

// ---
void ZXSPECTRUM::CharactersDrawCommand::executeImpl (MCHEmul::CommandExecuter* cE, 
	MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	ZXSPECTRUM::SinclairZXSpectrum* cS =
		dynamic_cast <ZXSPECTRUM::SinclairZXSpectrum*> (c);

	// Only with a valid computer, but also a ZXSpectrum one. 
	if (cS == nullptr)
		return;

	std::vector <size_t> chrs { };
	if (!parameters ().empty ())
	{
		for (const auto& i : parameters ())
		{ 
			size_t nchr = (size_t) MCHEmul::UInt::fromStr (i.second.c_str ()).asUnsignedInt ();
			if ((nchr >= 0 && nchr < 96) && 
				std::find (chrs.begin (), chrs.end (), nchr) == chrs.end ()) // Not added before...
				chrs.emplace_back (nchr);
		}
	}

	rst.add ("DRAW", MCHEmul::concatenateStrings
		(cS -> charsDrawSnapshot (c -> cpu (), chrs), "\n"));
}
