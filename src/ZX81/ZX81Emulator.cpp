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

	o << "/n:\t\t" << "Emulation using NTSC parameters and screen size" << std::endl
	  << "\t\tDepending on the computer type emulatted the combination might not be possible" << std::endl;
	o << "/b[COLOR]:\t" << "Draw a dark grid in the drawable screen. Color optional" << std::endl;
	o << "/w[CONF]:\t" << "Starts in a different memory configuration" << std::endl
	  << "\t\tIt is is optional and, depending on the computer emulated the combination might not be possible" << std::endl;
	o << "/m[MACHINE]:\t" << "Define the type of ROM loaded" << std::endl;
	o << "LANGUAGES allowed under command line /i:" << std::endl << 
		 "ENG:\tEnglish" << std::endl;
	o << "CONFIGURATIONS allowed under command line /w:" << std::endl <<
		 "ZX80:" << std::endl <<
		 "The parameter will be ingnored" << std::endl <<
		 "ZX81:" << std::endl <<
		 "0:\tNot expanded" << std::endl <<
		 "1:\t16K expansion" << std::endl;
	o << "MACHINE allowed under command line /m:" << std::endl << 
		 "ZX80:\tZX80" << std::endl <<
		 "ZX811:\tZX81 old rom version" << std::endl <<
		 "ZX812:\tZX81 rare rom version" << std::endl <<
		 "ZX813:\tZX81 newest rom version" << std::endl;
	o << "DEVICES allowed to be connected under command CONNECTPER:" << std::endl <<
		 std::to_string (MCHEmul::Typewriter::_ID) << ":\tTypewriter" << std::endl <<
		 std::to_string (SINCLAIR::Datasette::_ID) << ":\tDatasette" << std::endl <<
		 std::to_string (ZX81::DatasetteInjection::_ID) << ":\tDirect injection into memory" << std::endl <<
		 std::to_string (ZX81::Cartridge::_ID) << ":\tCartridge" << std::endl;
	o << "Check LOG file if the emulation doesn't start as expected" << std::endl;
}

// ---
ZX81::Type ZX81::ZX81Emulator::emulatedComputer () const
{ 
	// The default type...
	ZX81::Type result = Type::_ZX80;

	if (cmdlineArguments ().existsArgument (_PARAMMACHINE))
	{
		std::string mT = 
			MCHEmul::upper (cmdlineArguments ().argumentAsString (_PARAMMACHINE));

		if (mT == "ZX80") result = ZX81::Type::_ZX80;
		else if (mT == "ZX811") result = ZX81::Type::_ZX811;
		else if (mT == "ZX812") result = ZX81::Type::_ZX812;
		else if (mT == "ZX813") result = ZX81::Type::_ZX813;
		else
			_LOG ("Type of computer " + mT + " not vailable. ZX80 taken as default");
	}

	return (result);
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

// ----
MCHEmul::Computer* ZX81::ZX81Emulator::createComputer () const
{ 
	MCHEmul::Computer* result = nullptr;

	// By default it will be a ZX80
	// @see emulattedCompuer method!
	ZX81::Type t = emulatedComputer ();

	// Depending on the type of computer 
	// the possibilities allowed will be ones or others!
	switch (t)
	{
		case ZX81::Type::_ZX80:
			{
				result = new ZX81::SinclairZX81 (
					ZX81::Memory::Configuration::_NOEXPANDED /** No expansion admitted. */,
					ZX81::SinclairZX81::VisualSystem::_PAL /** PAL is always mandatory in this emulation. */,
					ZX81::Type::_ZX80);

				if (NTSCSystem ())
					_LOG ("ZX80 emulation is ready only for PAL systems");
				if (configurationMode () != (unsigned int) ZX81::Memory::Configuration::_NOEXPANDED)
					_LOG ("ZX80 emulation is ready only under _NOTEXPANDED configuration");
			}

			break;

		case ZX81::Type::_ZX811:
		case ZX81::Type::_ZX812:
		case ZX81::Type::_ZX813:
			{
				unsigned int cM = configurationMode ();
				if (cM != 0 && cM != 1)
				{
					cM = 0;

					_LOG ("ZX81 configuration mode " + std::to_string (cM) + 
						" not allowed. _NOTEXPANDED taken as default");
				}
				
				result = new ZX81::SinclairZX81 (
					(ZX81::Memory::Configuration) cM,
					ZX81::SinclairZX81::VisualSystem::_PAL /** PAL is always mandatory in this emulation. */,
					t);

				if (NTSCSystem ())
					_LOG ("ZX81 (any type) emulation is ready only for PAL systems");
			}

			break;

		// It shouldn't happen (emulattedComputer function controls this), but just in case...
		default:
			{
				_LOG ("ZX81 type of computer " + 
					std::to_string ((int) t) + " not supported");

				assert (false);
			}

			break;
	}

	return (result);
}
