#include <VIC20/VIC20Emulator.hpp>
#include <VIC20/VIC20.hpp>
#include <VIC20/Screen.hpp>

// ---
const unsigned char VIC20::VIC20Emulator::_PARAMNTSC = 'n';
const unsigned char VIC20::VIC20Emulator::_PARAMBORDER = 'b';

// ---
void VIC20::VIC20Emulator::printOutParameters (std::ostream& o) const
{
	MCHEmul::Emulator::printOutParameters (o);

	o << "/n:\t\t" << "Emulation using NTSC parameters and screen size" << std::endl;
	o << "/b[COLOR]:\t" << "Draw a dark border in the drawable screen. Color optional" << std::endl;
	o << "LANGUAGES allowed under command line /i:" << std::endl << 
		 "ENG:\tEnglish" << std::endl <<
		 "ESP:\tSpanish" << std::endl << 
		 "JAP:\tJapanesse" << std::endl << 
		 "SWE:\tSwedish" << std::endl <<
		 "DKA:\tDanish" << std::endl;
	o << "DEVICES allowed to be connected under command CONNECTPER:" << std::endl <<
		 std::to_string (COMMODORE::Datasette1530::_ID) << ":\tCasette 1530 / 1" << std::endl;
}

// ---
bool VIC20::VIC20Emulator::initialize ()
{
	if (!MCHEmul::Emulator::initialize ())
		return (false);

	// Draw border, Which color?
	dynamic_cast <VIC20::Screen*> (dynamic_cast <VIC20::CommodoreVIC20*> (computer ()) -> 
		device (VIC20::Screen::_ID)) -> setDrawBorder (drawBorder (), borderColor ());

	return (true);
}
