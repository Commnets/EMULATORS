#include <C264/C264Emulator.hpp>
#include <C264/C264.hpp>
#include <C264/Screen.hpp>
#include <C264/Cartridge.hpp>

// ---
const unsigned char C264::C264Emulator::_PARAMNTSC = 'n';
const unsigned char C264::C264Emulator::_PARAMBORDER = 'b';
const unsigned char C264::C264Emulator::_PARAMCONFIGURATION = 'w';
const unsigned char C264::C264Emulator::_PARAMMACHINE = 'm';

// ---
void C264::C264Emulator::printOutParameters (std::ostream& o) const
{
	MCHEmul::Emulator::printOutParameters (o);

	o << "/n:\t\t" << "Emulation using NTSC parameters and screen size" << std::endl;
	o << "/b[COLOR]:\t" << "Draw a dark grid in the drawable screen. Color optional" << std::endl;
	o << "/m[MACHINE]:\t" << "To select the type of machine of the series to be instantiated. C16 by default" << std::endl;
	o << "LANGUAGES allowed under command line /i:" << std::endl << 
		 "ENG:\tEnglish" << std::endl <<
		 "FRA:\tFrench" << std::endl << 
		 "SWE:\tSwedish" << std::endl <<
		 "HUN:\tHungarian" << std::endl;
	o << "MACHINES allowed to be emulated under command line /m:" << std::endl <<
		 "C16:\tCommodore 16/116 not expanded" << std::endl <<
		 "C116:\tCommodore 16/116 not expanded" << std::endl <<
		 "CP4:\tCommodore Plus/4 not expanded" << std::endl;
	o << "CONFIGURATION MODES allowed under command line /w:" << std::endl <<
		 "C16:\t0 = 16k, 1 = 32k" << std::endl <<
		 "C116:\t0 = 16k, 1 = 32k" << std::endl <<
		 "CP4:\t0 = Internal Programs" << std::endl;
	o << "DEVICES allowed to be connected under command CONNECTPER:" << std::endl <<
		 std::to_string (COMMODORE::Datasette1530::_ID) << ":\tCasette 1530/1" << std::endl <<
		 std::to_string (C264::Cartridge::_ID) << ":\tCartridge" << std::endl;
}

// ---
bool C264::C264Emulator::initialize ()
{
	if (!MCHEmul::Emulator::initialize ())
		return (false);

	// Draw border, Which color?
	dynamic_cast <C264::Screen*> (dynamic_cast <C264::Commodore264*> (computer ()) -> 
		device (C264::Screen::_ID)) -> setDrawGrid (drawBorder (), borderColor ());

	return (true);
}

// ---
MCHEmul::Computer* C264::C264Emulator::createComputer () const
{ 
	MCHEmul::Computer* result = nullptr;

	// This emulation is able to replicate three types of computers!
	switch (emulattedComputer ())
	{
		case C264::Type::_CPLUS4:
			result = new C264::CommodorePlus4 (
				NTSCSystem () 
					? C264::Commodore264::VisualSystem::_NTSC 
					: C264::Commodore264::VisualSystem::_PAL,
				configurationMode (),
				computerLanguage ());
			break;

		case C264::Type::_C16:
		case C264::Type::_C116:
		default:
			result = new C264::Commodore16_116 (
				NTSCSystem () 
					? C264::Commodore264::VisualSystem::_NTSC 
					: C264::Commodore264::VisualSystem::_PAL,
				configurationMode (),
				computerLanguage ());
			break;
	}

	return (result);
}
