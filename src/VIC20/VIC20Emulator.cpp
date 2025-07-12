#include <VIC20/VIC20Emulator.hpp>
#include <VIC20/VIC20.hpp>
#include <VIC20/Screen.hpp>
#include <VIC20/Cartridge.hpp>

// ---
const unsigned char VIC20::VIC20Emulator::_PARAMNTSC = 'n';
const unsigned char VIC20::VIC20Emulator::_PARAMBORDER = 'b';
const unsigned char VIC20::VIC20Emulator::_PARAMCONFIGURATION = 'w';

// ---
void VIC20::VIC20Emulator::printOutParameters (std::ostream& o) const
{
	MCHEmul::Emulator::printOutParameters (o);

	o << "/n:\t\t" << "Emulation using NTSC parameters and screen size" << std::endl;
	o << "/b[COLOR]:\t" << "Draw a dark grid in the drawable screen. Color optional" << std::endl;
	o << "/w[CONF]:\t" << "Starts in a different memory configuration. It is is optional" << std::endl;
	o << "LANGUAGES allowed under command line /i:" << std::endl << 
		 "ENG:\tEnglish" << std::endl <<
		 "JAP:\tJapanesse" << std::endl << 
		 "SWE:\tSwedish" << std::endl <<
		 "DKA:\tDanish" << std::endl;
	o << "CONFIGURATIONS allowed under command line /w:" << std::endl << 
		 "0:\tNot expanded" << std::endl <<
		 "1:\t+3K expansion" << std::endl << 
		 "2:\t+8K expansion (+8k)" << std::endl <<
		 "3:\t+16K expansion (+8k +8k)" << std::endl <<
		 "4:\t+24K expansion (+8k +8k +8k)" << std::endl <<
		 "5:\t+32K expansion (+8k +8k +8k +8k)" << std::endl <<
		 "6:\t+11K expansion (+3K +8k)" << std::endl <<
		 "7:\t+19K expansion (+3K +8k +8k)" << std::endl <<
		 "8:\t+27K expansion (+3K +8k +8k +8k)" << std::endl <<
		 "9:\t+35K expansion (+3K +8k +8k +8k +8k)" << std::endl;
	o << "DEVICES allowed to be connected under command CONNECTPER:" << std::endl <<
		 std::to_string (VIC20::Cartridge::_ID) << ":\tCartridge" << std::endl <<
		 std::to_string (COMMODORE::Datasette1530::_ID) << ":\tCasette 1530/1" << std::endl <<
		 std::to_string (COMMODORE::Datasette1530Injection::_ID) << ":\tDirect injection into memory" << std::endl;
}

// ---
bool VIC20::VIC20Emulator::initialize ()
{
	if (!MCHEmul::Emulator::initialize ())
		return (false);

	// Draw border, Which color?
	dynamic_cast <VIC20::Screen*> (dynamic_cast <VIC20::CommodoreVIC20*> (computer ()) -> 
		device (VIC20::Screen::_ID)) -> setDrawGrid (drawBorder (), borderColor ());

	return (true);
}
