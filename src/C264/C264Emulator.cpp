#include <C264/C264Emulator.hpp>
#include <C264/C264.hpp>
#include <C264/Screen.hpp>
#include <C264/Cartridge.hpp>

// ---
const unsigned char C264::C264Emulator::_PARAMNTSC = 'n';
const unsigned char C264::C264Emulator::_PARAMBORDER = 'b';
const unsigned char C264::C264Emulator::_PARAMMACHINE = 'm';
const unsigned char C264::C264Emulator::_PARAMCONFIGURATION = 'w';

// ---
void C264::C264Emulator::printOutParameters (std::ostream& o) const
{
	MCHEmul::Emulator::printOutParameters (o);

	o << "/n:\t\t" << "Emulation using NTSC parameters and screen size" << std::endl;
	o << "/b[COLOR]:\t" << "Draw a dark border in the drawable screen. Color optional" << std::endl;
	o << "/m[MACHINE]:\t" << "To select the type of machine of the series to be instantiated. C16 by default" << std::endl;
	o << "/w[CONFIGURATION]:\t" << "Starts in a different memory configuration. It is is optional. 0 by default" << std::endl;
	o << "LANGUAGES allowed under command line /i:" << std::endl << 
		 "ENG:\tEnglish" << std::endl <<
		 "FRA:\tFrench" << std::endl << 
		 "SWE:\tSwedish" << std::endl <<
		 "HUN:\tHungarian" << std::endl;
	o << "CONFIGURATIONS allowed under command line /w:" << std::endl <<
		 "Only valid when the /m = 0 or default what means that a C16/166 is started" << std::endl <<
		 "0: 16k RAM free" << std::endl <<
		 "1: 32k RAM free" << std::endl <<
		 "2: 48k RAM free" << std::endl <<
		 "3: 64k RAM free" << std::endl;
	o << "MACHINES allowed to be emulated under command line /m:" << std::endl <<
		 "C16:\tCommodore 16/116 not expanded" << std::endl <<
		 "CP4:\tCommodore Plus/4 not expanded" << std::endl;
	o << "DEVICES allowed to be connected under command CONNECTPER:" << std::endl <<
		 std::to_string (COMMODORE::Datasette1530::_ID) << ":\tCasette 1530 / 1" << std::endl <<
		 std::to_string (C264::Cartridge::_ID) << ":\tCartridge" << std::endl;
}

// ---
bool C264::C264Emulator::initialize ()
{
	if (!MCHEmul::Emulator::initialize ())
		return (false);

	// Draw border, Which color?
	dynamic_cast <C264::Screen*> (dynamic_cast <C264::Commodore264*> (computer ()) -> 
		device (C264::Screen::_ID)) -> setDrawBorder (drawBorder (), borderColor ());

	return (true);
}

// ---
MCHEmul::Computer* C264::C264Emulator::createComputer () const
{ 
	MCHEmul::Computer* result = nullptr;

	// This emulation is able to replicate three types of computers!
	switch (emulattedComputer ())
	{
		case 1:
			result = new C264::CommodorePlus4 (computerLanguage (), 
				NTSCSystem () ? C264::Commodore264::VisualSystem::_NTSC : C264::Commodore264::VisualSystem::_PAL);
			break;

		case 0:
		default:
			result = new C264::Commodore16_116 (configurationMode (), computerLanguage (),
				NTSCSystem () ? C264::Commodore264::VisualSystem::_NTSC : C264::Commodore264::VisualSystem::_PAL);
			break;
	}

	return (result);
}
