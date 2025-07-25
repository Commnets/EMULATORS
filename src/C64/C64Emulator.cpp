#include <C64/C64Emulator.hpp>
#include <C64/C64.hpp>
#include <C64/Cartridge.hpp>

// ---
const unsigned char C64::C64Emulator::_PARAMNTSC = 'n';
const unsigned char C64::C64Emulator::_PARAMSID = 'w';
const unsigned char C64::C64Emulator::_PARAMBORDER = 'b';

// ---
void C64::C64Emulator::printOutParameters (std::ostream& o) const
{
	MCHEmul::Emulator::printOutParameters (o);

	o << "/n:\t\t" << "Emulation using NTSC parameters and screen size" << std::endl;
	o << "/b[COLOR]:\t" << "Draw a dark grid in the drawable screen. Color optional" << std::endl;
	o << "/w[SID]:\t" << "Select the emulation used for the SID chip" << std::endl;
	o << "\t\tSID for an emulation based on RESID library" << std::endl;
	o << "\t\tOWN or nothing for and emulation based on ICF library" << std::endl;
	o << "LANGUAGES allowed under command line /i:" << std::endl << 
		 "ENG:\tEnglish" << std::endl <<
		 "ESP:\tSpanish" << std::endl << 
		 "JAP:\tJapanesse" << std::endl << 
		 "SWE:\tSwedish" << std::endl <<
		 "DKA:\tDanish" << std::endl;
	o << "DEVICES allowed to be connected under command CONNECTPER:" << std::endl <<
		 std::to_string (C64::Cartridge::_ID) << ":\tCartridge" << std::endl <<
		 std::to_string (COMMODORE::Datasette1530::_ID) << ":\tCasette 1530/1" << std::endl <<
		 std::to_string (COMMODORE::Datasette1530Injection::_ID) << ":\tDirect injection into memory" << std::endl;
}

// ---
bool C64::C64Emulator::initialize ()
{
	if (!MCHEmul::Emulator::initialize ())
		return (false);

	// Draw border, Which color?
	dynamic_cast <C64::Screen*> (dynamic_cast <C64::Commodore64*> (computer ()) -> 
		device (C64::Screen::_ID)) -> setDrawGrid (drawBorder (), borderColor ());

	return (true);
}
