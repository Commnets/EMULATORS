#include <C64/C64Emulator.hpp>
#include <C64/C64.hpp>

// ---
const unsigned char C64::C64Emulator::_PARAMNTSC = 'n';
const unsigned char C64::C64Emulator::_PARAMBORDER = 'b';

// ---
void C64::C64Emulator::printOutParameters (std::ostream& o) const
{
	MCHEmul::Emulator::printOutParameters (o);

	o << "/n:\t\t" << "Emulation using NTSC parameters and screen size." << std::endl;
	o << "/b:\t\t" << "Draw a dark border in the drawable screen." << std::endl;
	o << "LANGUAGES allowed under command line /i:" << std::endl << 
		 "ENG:\tEnglish" << std::endl <<
		 "ESP:\tSpanish" << std::endl << 
		 "JAP:\tJapanesse" << std::endl << 
		 "SWE:\tSwedish" << std::endl <<
		 "DKA:\tDanish" << std::endl;
	o << "DEVICES allowed to be connected under command CONNECTPER" << std::endl <<
		 "100:\tCartridge" << std::endl <<
		 "200:\tCasette 1530/1" << std::endl;
}

// ---
bool C64::C64Emulator::initialize ()
{
	if (!MCHEmul::Emulator::initialize ())
		return (false);

	if (drawBorder ())
		dynamic_cast <C64::Commodore64*> (computer ()) -> vicII () -> setDrawBorder (true);

	// TODO
	// To link the peripherals...

	return (true);
}
