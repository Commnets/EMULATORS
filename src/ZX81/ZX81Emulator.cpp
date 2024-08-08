#include <ZX81/ZX81Emulator.hpp>
#include <ZX81/ZX81.hpp>
#include <ZX81/Screen.hpp>
#include <ZX81/Cartridge.hpp>
#include <ZX81/Datasette.hpp>

// ---
const unsigned char ZX81::ZX81Emulator::_PARAMNTSC = 'n';
const unsigned char ZX81::ZX81Emulator::_PARAMBORDER = 'b';
const unsigned char ZX81::ZX81Emulator::_PARAMCONFIGURATION = 'w';
const unsigned char ZX81::ZX81Emulator::_PARAMMACHINE = 'm';

// ---
void ZX81::ZX81Emulator::printOutParameters (std::ostream& o) const
{
	MCHEmul::Emulator::printOutParameters (o);

	o << "/n:\t\t" << "Emulation using NTSC parameters and screen size" << std::endl;
	o << "/b[COLOR]:\t" << "Draw a dark grid in the drawable screen. Color optional" << std::endl;
	o << "/w[CONFIGURATION]:\t" << "Starts in a different memory configuration. It is is optional" << std::endl;
	o << "/m[MACHINE]:\t" << "Define the type of ROM loaded" << std::endl;
	o << "LANGUAGES allowed under command line /i:" << std::endl << 
		 "ENG:\tEnglish" << std::endl;
	o << "CONFIGURATIONS allowed under command line /w:" << std::endl << 
		 "0:\tNot expanded" << std::endl <<
		 "1:\t16K expansion" << std::endl;
	o << "MACHINE allowed under command line /m:" << std::endl << 
		 "ZX80:\tZX80" << std::endl <<
		 "ZX811:\tZX81 old rom version" << std::endl <<
		 "ZX812:\tZX81 rare rom version" << std::endl <<
		 "ZX813:\tZX81 newest rom version" << std::endl;
	o << "DEVICES allowed to be connected under command CONNECTPER:" << std::endl <<
		 std::to_string (ZX81::Datasette::_ID) << ":\tSynchronous datasette" << std::endl <<
		 std::to_string (ZX81::DatasetteP::_ID) << ":\tASynchronous datasette" << std::endl <<
		 std::to_string (ZX81::DatasetteInjection::_ID) << ":\tDirect injection into memory" << std::endl <<
		 std::to_string (ZX81::Cartridge::_ID) << ":\tCartridge" << std::endl;
}

// ---
bool ZX81::ZX81Emulator::initialize ()
{
	if (!MCHEmul::Emulator::initialize ())
		return (false);

	// Draw border, Which color?
	dynamic_cast <ZX81::Screen*> (dynamic_cast <ZX81::SinclairZX81*> (computer ()) -> 
		device (ZX81::Screen::_ID)) -> setDrawGrid (drawBorder (), borderColor ());

	return (true);
}
