#include <ZXSpectrum/ZXSpectrumEmulator.hpp>
#include <ZXSpectrum/ZXSpectrum.hpp>
#include <ZXSpectrum/Screen.hpp>
#include <ZXSpectrum/Cartridge.hpp>
#include <ZXSpectrum/Datasette.hpp>

// ---
const unsigned char ZXSPECTRUM::ZXSpectrumEmulator::_PARAMNTSC = 'n';
const unsigned char ZXSPECTRUM::ZXSpectrumEmulator::_PARAMBORDER = 'b';
const unsigned char ZXSPECTRUM::ZXSpectrumEmulator::_PARAMCONFIGURATION = 'w';
const unsigned char ZXSPECTRUM::ZXSpectrumEmulator::_PARAMMACHINE = 'm';

// ---
void ZXSPECTRUM::ZXSpectrumEmulator::printOutParameters (std::ostream& o) const
{
	MCHEmul::Emulator::printOutParameters (o);

	o << "/n:\t\t" << "Emulation using NTSC parameters and screen size" << std::endl;
	o << "/b[COLOR]:\t" << "Draw a dark grid in the drawable screen. Color optional" << std::endl;
	o << "/w[CONF]:\t" << "Starts in a different memory configuration. It is is optional" << std::endl;
	o << "/m[MACHINE]:\t" << "Define the type of ROM loaded" << std::endl;
	o << "LANGUAGES allowed under command line /i (not all languages are available for all models):" << std::endl << 
		 "ENG:\tEnglish" << std::endl <<
		 "ESP:\tSpanish" << std::endl <<
		 "NOR:\tNordics" << std::endl <<
		 "ARA:\tArabic" << std::endl;
	o << "CONFIGURATIONS (depends on type of machine) allowed under command line /w:" << std::endl << 
		 "When ZXSTD:" << std::endl <<
		 "0:\tNot expanded. 16k" << std::endl <<
		 "1:\t48K expansion" << std::endl;
	o << "MACHINE allowed under command line /m:" << std::endl << 
		 "ZXSTD:\t16k/48k ZX Spectrum" << std::endl;
	o << "DEVICES allowed to be connected under command CONNECTPER:" << std::endl <<
		 std::to_string (MCHEmul::Typewriter::_ID) << ":\tTypewriter" << std::endl <<
		 std::to_string (SINCLAIR::Datasette::_ID) << ":\tDatasette" << std::endl <<
		 std::to_string (ZXSPECTRUM::DatasetteInjection::_ID) << ":\tDirect injection into memory" << std::endl <<
		 std::to_string (ZXSPECTRUM::Cartridge::_ID) << ":\tCartridge" << std::endl;
}

// ---
bool ZXSPECTRUM::ZXSpectrumEmulator::initialize ()
{
	if (!MCHEmul::Emulator::initialize ())
		return (false);

	// Draw border, Which color?
	dynamic_cast <ZXSPECTRUM::Screen*> (dynamic_cast <ZXSPECTRUM::SinclairZXSpectrum*> (computer ()) -> 
		device (ZXSPECTRUM::Screen::_ID)) -> setDrawGrid (drawBorder (), borderColor ());

	return (true);
}
