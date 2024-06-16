#include <ZX81/Commands.hpp>
#include <ZX81/ZX81.hpp>

const std::string ZX81::ULAStatusCommand::_NAME = "CULA";
const std::string ZX81::ULAShowEventsCommand::_NAME = "CULAEVENTS";
const std::string ZX81::LoadPAnd81FileCommand::_NAME = "LOADPAND81";

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
void ZX81::LoadPAnd81FileCommand::executeImpl (MCHEmul::CommandExecuter* cE,
	MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	// Only with a valid computer, but also a ZX81 one. 
	if (c == nullptr || 
		dynamic_cast <ZX81::SinclairZX81*> (c) == nullptr) // Just in case...
		return;

	ZX81::SinclairZX81* zx = static_cast <ZX81::SinclairZX81*> (c);
	
	bool e = false;
	std::string fN;
	SINCLAIR::Pand81FileTypeIO f;
	if (f.canRead (fN = parameter ("00"))) // The name of the file...
	{
		SINCLAIR::Pand81FileData* d = 
			dynamic_cast <SINCLAIR::Pand81FileData*> (f.readFile (fN, c -> cpu () -> architecture ().bigEndian ()));
		if (!(e = (d == nullptr)))
		{
			c -> memory () -> set (d -> asMemoryBlocks ()._data);

			c -> cpu () -> programCounter ().setAddress (MCHEmul::Address ({ 0xe5, 0x03 }, false));
		}
	}
	else
		e = true;

	if (e)
		rst.add ("ERROR", std::string ("File can not be read"));
}

