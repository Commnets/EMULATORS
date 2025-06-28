#include <MSX/MSXEmulator.hpp>
#include <MSX/MSX.hpp>
#include <MSX/Screen.hpp>

// ---
const unsigned char MSX::MSXEmulator::_PARAMNTSC = 'n';
const unsigned char MSX::MSXEmulator::_PARAMBORDER = 'b';
const unsigned char MSX::MSXEmulator::_PARAMCONFIGURATION = 'w';
const unsigned char MSX::MSXEmulator::_PARAMMACHINE = 'm';

// ---
MSX::MSXEmulator::~MSXEmulator ()
{ 
	// The model belongs to the emulator!
	delete (_model);
}

// ---
void MSX::MSXEmulator::printOutParameters (std::ostream& o) const
{
	MCHEmul::Emulator::printOutParameters (o);

	o << "/n:\t\t" << "Emulation using NTSC parameters and screen size" << std::endl;
	o << "/b[COLOR]:\t" << "Draw a dark grid in the drawable screen. Color optional" << std::endl;
	o << "/w[CONF]:\t" << "Starts in a different memory configuration. It is is optional" << std::endl;
	o << "/m[MACHINE]:\t" << "Define the type of ROM loaded" << std::endl;
	o << "LANGUAGES allowed under command line /i (not all languages are available for all models):" << std::endl << 
		 "When Machine is SVI728:" << std::endl <<
		 "ENG:\tEnglish" << std::endl <<
		 "ESP:\tSpanish" << std::endl <<
		 "When Machine is SONYHB10P:" << std::endl <<
		 "The language is standard and English" << std::endl <<
		 "When Machine is PHILIPSVG8010:" << std::endl <<
		 "ENG:\tEnglish" << std::endl <<
		 "FRA:\tFrench" << std::endl <<
		 "When Machine is SVI738:" << std::endl <<
		 "ENG:\tEnglish" << std::endl <<
		 "DEU:\tAleman" << std::endl <<
		 "SWE:\tSueco" << std::endl;
	o << "CONFIGURATIONS (depends on type of machine) allowed under command line /w:" << std::endl <<
		 "When Machine is SVI728" << std::endl <<
		 "0:\tBasic Machine" << std::endl <<
		 "When Machine is SONYHB10P" << std::endl <<
		 "0:\tBasic Machine" << std::endl;
	o << "MACHINE allowed under command line /m:" << std::endl << 
		 "SVI728:\tMSX1 Spectravideo 728" << std::endl <<
		 "SONYHB10P:\tMSX1 Sony HB10P" << std::endl <<
		 "PHILIPSVG8010:\tMSX1 Philips VG8010" << std::endl <<
		 "SVI738:\tMSX2 Spectravideo 738" << std::endl;
}

// ---
bool MSX::MSXEmulator::initialize ()
{
	if (!MCHEmul::Emulator::initialize ())
		return (false);

	// Draw border, Which color?
	dynamic_cast <MSX::Screen*> (dynamic_cast <MSX::MSXComputer*> (computer ()) -> 
		device (MSX::Screen::_ID)) -> setDrawGrid (drawBorder (), borderColor ());

	return (true);
}

// ---
MSX::MSXModel* MSX::MSXEmulator::createModel () const
{
	MSX::MSXModel* result = nullptr;
	if (cmdlineArguments ().existsArgument (_PARAMMACHINE))
	{
		std::string mT = 
			MCHEmul::upper (cmdlineArguments ().argumentAsString (_PARAMMACHINE));
		if (mT == "SVI728")	result = new MSX::SVI728 (NTSCSystem () 
			? MSX::MSXModel::VisualSystem::_NTSC : MSX::MSXModel::VisualSystem::_PAL);
		else if (mT == "SONYHB10P") result = new MSX::SonyHB10P (NTSCSystem ()
			? MSX::MSXModel::VisualSystem::_NTSC : MSX::MSXModel::VisualSystem::_PAL);
		else if (mT == "PHILIPSVG8010") result = new MSX::PhilipsVG8010 (NTSCSystem ()
			? MSX::MSXModel::VisualSystem::_NTSC : MSX::MSXModel::VisualSystem::_PAL);
		else if (mT == "SVI738") result = new MSX::SVI738 (NTSCSystem () 
			? MSX::MSXModel::VisualSystem::_NTSC : MSX::MSXModel::VisualSystem::_PAL);
		else
		{
			result = new MSX::MSXStdModel;

			_LOG ("Unknown machine type:" + mT); // But the standard one is assigned, 
												 // althought it is not correct!
		}
	}
	else
		result = new MSX::MSXStdModel; // Default model

	return (result);
}
