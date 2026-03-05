#include <COMMODORE/SerialPrinters.hpp>
#include <sstream>

// ---
COMMODORE::MPS801BasicMatrixPrinterEmulation::MPS801BasicMatrixPrinterEmulation (const std::string& pFN)
	: MCHEmul::BasicMatrixPrinterEmulation (80 /** Always the standard. */, pFN),
	  _businessMode (false),
	  _double (false) // There i no more configuration needed, so the default one is enought...
{
	// Nothing else...
}

// ---
bool COMMODORE::MPS801BasicMatrixPrinterEmulation::isControlChar (unsigned char chr)
{ 
	return (chr == (unsigned char) 0x0a /** line feed. */ ||
			chr == (unsigned char) 0x0e /** Double width. */ ||
			chr == (unsigned char) 0x0f /** Standard width. */ ||
			chr == (unsigned char) 0x11 /** Cursor down == business mode = lowercase. */ ||
			chr == (unsigned char) 0x91 /** Cursor up = normal mode = uppercase. */);
}

// ---
std::tuple <short, short, short> COMMODORE::MPS801BasicMatrixPrinterEmulation::manageControlChar (unsigned char chr)
{
	short aX = 0, aY = 0, aP = 0;

	switch (chr)
	{
		case 0x0a:
			{
				aY++;
			}

			break;

		case 0x0e:
			{
				_double = true;
			}

			break;

		case 0x0f:
			{
				_double = false;
			}

			break;

		case 0x11:
			{
				_businessMode = true;
			}

			break;

		case 0x91:
			{
				_businessMode = false;
			}

			break;
	}

	return (std::make_tuple (aX, aY, aP));
}

// ---
bool COMMODORE::MPS801BasicMatrixPrinterEmulation::isNormalChar (unsigned char chr)
{
	return (
		_businessMode 
			? ((chr >= (unsigned char) 0x20 && (unsigned char) chr <= 0x5a) ||
				(chr >= (unsigned char) 0xc1 && (unsigned char) chr <= 0xda))
			: (chr >= (unsigned char) 0x20 && (unsigned char) chr <= 0x5a));
}

// ---
size_t COMMODORE::MPS801BasicMatrixPrinterEmulation::printNormalChar (unsigned char chr)
{
	size_t result = 0;

	unsigned char nChr = chr;

	if (_businessMode)
		nChr += (chr >= (unsigned char) 0x41 && chr <= (unsigned char) 0x5a) 
			? 0x20 : -0x80; /** the other way around. */

	printerFile () << std::string (1, (char) nChr); result++;
	if (_double) { printerFile () << std::string (1, (char) nChr); result++; }
	printerFile ().flush ();

	return (result);
}

// ---
COMMODORE::MPS801PostscriptMatrixPrinterEmulation::MPS801PostscriptMatrixPrinterEmulation 
		(const MCHEmul::MatrixPrinterEmulation::Configuration& cfg,
		 const MCHEmul::MatrixPrinterEmulation::Paper& p, 
		 const std::string& pFN)
	: MCHEmul::PostscriptMatrixPrinterEmulation (cfg, p, pFN),
	  _double (false),
	  _businessMode (false),
	  _graphicMode (false),
	  _settingTab (false),
	  _charsSettingtabPending (0),
	  _nextTabSettingValue { 0, 0 },
	  _setSpecificDotAddress (false),
	  _reverse (false),
	  _posXInside (0), _posYInside (0)
{ 
	// Nothing else
}

// ---
void COMMODORE::MPS801PostscriptMatrixPrinterEmulation::firstTimePrinting (unsigned char chr)
{
	// Copies the first group of general postscript routines to simulate a printing...
	std::ifstream l1File ("PSMatrixPrinterI.ps");
	if (!l1File)
		_LOG ("File PSMatrixPrinterI.ps doesn't exist");
	std::stringstream sst; sst << l1File.rdbuf ();
	printerFile () << MCHEmul::replaceStrings (sst.str (),
		{ "[[PAGEWIDTHINCH]]",
		  "[[PAGEHEIGHTINCH]]",
		  "[[PAGECOLOR]]" },
		{ std::to_string (paper ()._width), 
		  std::to_string (paper ()._height),
		  paper ()._type == MCHEmul::MatrixPrinterEmulation::Paper::Type::_BLUEBAND 
			? "/Blue"
			: (paper ()._type == MCHEmul::MatrixPrinterEmulation::Paper::Type::_GREENBAND 		
				? "/Green"
				: "/Gray") });

	l1File.close ();

	// Print out the information about the chars...
	for (const auto& i : configuration ()._charSet)
	{
		printerFile () << i.first << " [";

		bool fC = true;
		for (const auto& j : i.second)
		{
			printerFile () 
				<< (!fC ? " " : "") 
				<< "16#" << j.asString (MCHEmul::UByte::OutputFormat::_HEXA,2);
			fC = false;
		}

		printerFile () << "] putGlyph" << std::endl;
	}

	// Copies the secong group of general postscript routines to simulate a printing...
	std::ifstream l2File ("PSMatrixPrinterII.ps");
	if (!l2File)
		_LOG ("File PSMatrixPrinterII.ps doesn't exist");
	printerFile () << l2File.rdbuf ();
	l2File.close ();

	// Print out information about the margins...
	// Just top start to print out at the right position in the page whatever its size was...
	setNewPage (_page);

	_posXInside = _posYInside = 0;
}

// ---
bool COMMODORE::MPS801PostscriptMatrixPrinterEmulation::isControlChar (unsigned char chr)
{
	// All control characters are under 0x80,
	// so to avoid confusion with a valid graphic byte (and when the printer is this mode)
	// the graphic bytes are always with the bit 7 to 1!
	return (chr == (unsigned char) 0x08 /** Activating graphics mode. */ ||
			chr == (unsigned char) 0x0a /** line feed. */ ||
			chr == (unsigned char) 0x10 /** Start setting tab. */ ||
			chr == (unsigned char) 0x0e /** Double width. */ ||
			chr == (unsigned char) 0x0f /** Standard width. */ ||
			chr == (unsigned char) 0x11 /** Cursor down == business mode = lowercase. */ ||
			chr == (unsigned char) 0x12 /** Reverse mode on. */ ||
			chr == (unsigned char) 0x1a /** Repeat graphic selected. */ ||
			chr == (unsigned char) 0x1b /** Specify Dot Address. */ ||
			chr == (unsigned char) 0x91 /** Cursor up = normal mode = uppercase. */ ||
			chr == (unsigned char) 0x92 /** Reverse mode off. */ );
}

// ---
std::tuple <short, short, short> 
COMMODORE::MPS801PostscriptMatrixPrinterEmulation::manageControlChar (unsigned char chr)
{
	short aX = 0, aY = 0, aP = 0;

	switch (chr)
	{
		// GRAPHIC MODE = ON
		case 0x08:
			{
				_graphicMode = true;
			}

			break;

		// LINE FEED
		case 0x0a:
			{
				aY++;
			}

			break;

		// DOUBLE WIDTH = ON
		case 0x0e:
			{
				_double = true;
			}

			break;

		// DOUBLE WIDTH = OFF
		case 0x0f:
			{
				_double = false;

				_graphicMode = false;
			}

			break;

		// START SETTING TAB
		// It is needed two valid chars more...
		case 0x10:
			{
				_settingTab = true;

				_charsSettingtabPending = 2;
			}

			break;

		// CURSOR DOWN MODE = BUSINESS MODE = LOWERCASE
		case 0x11:
			{
				_businessMode = true;
			}

			break;

		// REVERSE MODE = ON
		case 0x12:
			{
				_reverse = true;
			}

			break;

		// REPEAT GRAPHIC SELECTED
		case 0x1a:
			{
				// TODO
			}

			break;

		// SPECIFY DOT ADDRESS
		// It is needed two valid chars more...
		// And has the same format that setting the tab,
		// but instead of moving the head, 
		// it will move the position inside the char to print out the next byte of information!
		case 0x1b:
			{
				_setSpecificDotAddress = true;
			}

			break;

		// CURSOR UP MODE = NORMAL MODE = UPPERCASE
		case 0x91:
			{
				_businessMode = false;
			}

			break;

		// REVERSE MODE = OFF
		case 0x92:
			{
				_reverse = false;
			}

			break;

		// It shouldn't be here...
		default:
			{
				_LOG ("Unknown control char received: " + std::to_string ((unsigned int) chr));
			}

			break;
	}

	return (std::make_tuple (aX, aY, aP));
}

// ---
void COMMODORE::MPS801PostscriptMatrixPrinterEmulation::printNewLine ()
{
	// Starting a new line there is no "glide" inside the character...
	_posXInside = _posYInside = 0;
}

// ---
void COMMODORE::MPS801PostscriptMatrixPrinterEmulation::closePage (unsigned short p)
{
	printerFile ()
		<< std::endl << "showpage" << std::endl << std::endl;
}

// ---
void COMMODORE::MPS801PostscriptMatrixPrinterEmulation::setNewPage (unsigned short chr)
{
	// Beginning of the next one...
	printerFile ()
		<< "% ----Page:"
		<< MCHEmul::fixLenStr (std::to_string (_page), 2, true, MCHEmul::_CEROS) 
		<< "----------------------------------------------" << std::endl
		<< "setupPage" << std::endl
		<< "drawPaper" << std::endl << std::endl
		<< "initgraphics" << std::endl
		<< std::to_string ((unsigned int) (paper ()._border /** in inches. */ * 72.0f /** points per inch. */)) + " " +
		   "0 topLeftWithMargin /y0 exch def /x0 exch def" << std::endl << std::endl;		
}

// ---
bool COMMODORE::MPS801PostscriptMatrixPrinterEmulation::isNormalChar (unsigned char chr)
{
	// In the graphic mode, all the chars with bit 7 to 1 are valid...
	return (_setSpecificDotAddress 
		? true // When setting the specific address, any position is possible...
		: (_graphicMode 
			? (chr >= 0x80)
			: ((chr >= 0x20 && chr <= 0x7f) ||
			   (chr >= 0xa0 && chr <= 0xff))));
}

// ---
size_t COMMODORE::MPS801PostscriptMatrixPrinterEmulation::printNormalChar (unsigned char chr)
{
	// Not to print out anything by default... 
	// It will depend on whether a printable character was actually sent!
	// result has the number of bytesprinted out...
	size_t result = 0;
	
	// Routine to print aout a byte of information and advance...
	auto printByteColumnAndAdvance = [&](const MCHEmul::UByte& byte) -> void
		{
			printerFile ()
				<< std::to_string ((unsigned int) byte.value ()) + "\t" +
				   "x0 " +
				   std::to_string ((unsigned int) _posXInside) + " " +
				   std::to_string ((unsigned int) _posX + (unsigned short) result /** in the double. */) + " " +
				   std::to_string ((unsigned int) (configuration ()._wChar)) + 
				   " mul add dotStepX mul add\ty0 " + // dotStepX defined in PSMatrixPrinterII.ps
				   std::to_string ((unsigned int) _posY) + " " +
				   "dotStepY " + // dotStepY & lineGapY defined in PSMatrixPrinterII.ps
				   std::to_string ((unsigned int) (configuration ()._hChar)) + 
				   " mul lineGapY add mul sub drawByteBits" << std::endl;

			// The definition of the dotStep...
			if ((++_posXInside == (unsigned short) configuration ()._wChar)) // No more...
			{
				_posXInside = 0;

				result++;
			}
		};

	// Depending on the char received...
	if (_settingTab)
	{
		// When two chars more are defined, then the settin process has finished...
		_nextTabSettingValue [1 - (size_t) --_charsSettingtabPending] = 
				_setSpecificDotAddress 
					? chr // When setting the specific dot address, the number is "pure"
					: ((chr >= 0x30) ? chr - 0x30 : 0x00); // Otherwise the char is the number in ASCII!
		_settingTab = (_charsSettingtabPending != 0); // Still setting tab?
		if (!_settingTab)
		{
			unsigned short tV =
				_setSpecificDotAddress
					? (_nextTabSettingValue [0] * 256) + _nextTabSettingValue [1]  // the value is MSB/LSB...
					: (_nextTabSettingValue [0] * 10)  + _nextTabSettingValue [1]; // the value is a decimal value...
			if (_setSpecificDotAddress)
			{
				// ...but the position can never be bigger the 480,
				// otherwise it will be ignored!...
				// The position inside the char, and the position of the head...
				_setSpecificDotAddress = false;
				if (tV < 480) 
					{ _posXInside = tV % 6; tV /= 6; }
			}

			// Move the head to the right position if possible, 
			// otherwise it will be ignored...
			if (tV < 80)
				moveHeadFromX (-headXPosition () + tV); // Absolute...
		}
	}
	else
	if (_graphicMode)
	{
		printerFile () 
			<< "% Graphic value " << std::to_string ((unsigned int) chr) << std::endl;
		printByteColumnAndAdvance (chr);
	}
	else
	{
		bool existsChr = false;
		MCHEmul::MatrixPrinterEmulation::Configuration::CharSetDefinition::const_iterator i
			= configuration ()._charSet.find
				((unsigned int) chr + (_businessMode ? 0x100 /** The second set. */ : 0x00));
		const MCHEmul::MatrixPrinterEmulation::Configuration::CharDefinition& chrDef = // It is not needed a copy...
			(existsChr = (i != configuration ()._charSet.end ()))
				? (*i).second : (*configuration ()._charSet.begin ()).second; // The character 0 by default...
		printerFile () 
			<< "% Character " 
			<< (existsChr ? std::to_string ((unsigned int) chr) : "Unknown") << std::endl;
		for (const auto& j : (*i).second)
		{
			MCHEmul::UByte v = j; if (_reverse) v = ~v;
			printByteColumnAndAdvance (v);
			if (_double) printByteColumnAndAdvance (v); // twice...
		}
	}

	return (result);
}

// ---
COMMODORE::SerialPrinterPeripheralSimulation::SerialPrinterPeripheralSimulation 
		(int id, unsigned char dN, 
		 const COMMODORE::SerialIOPeripheralSimulation::Definition& dt,
		 const MCHEmul::Attributes& attrs, 
		 MCHEmul::MatrixPrinterEmulation* mPE)
	: COMMODORE::SerialIOPeripheralSimulation (id, dN, dt, attrs),
	  _emulation (mPE)
{ 
	// It cannot be nullptr...
	assert (_emulation != nullptr);

	setClassName ("SerialPrinterSimulation");
}

// ---
COMMODORE::SerialPrinterPeripheralSimulation::~SerialPrinterPeripheralSimulation ()
{ 
	finalize ();

	// No longer valid...
	delete (_emulation);
} 

// ---
bool COMMODORE::SerialPrinterPeripheralSimulation::executeCommand (int cId, const MCHEmul::Strings& prms)
{
	bool result = false;

	switch (cId)
	{
		case 1: // Flush the printer...
			{
				result = true;
			}
			break;

		case 2: // Cut the page...
			{
				_emulation -> closeCurrentPage ();

				result = true;
			}
			break;

		default:
			// It shouldn't be here...
			break;
	}

	return (result);
}

// ---
MCHEmul::InfoStructure COMMODORE::SerialPrinterPeripheralSimulation::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move 
		(COMMODORE::SerialIOPeripheralSimulation::getInfoStructure ());

	// Add info about the emulation...
	result.add ("MatrixPrinterEmulation", std::move (_emulation -> getInfoStructure ()));
	result.add ("COMMANDS", MCHEmul::concatenateStrings (commandDescriptions (),"\n"));

	return (result);
}

// ---
unsigned char COMMODORE::SerialPrinterPeripheralSimulation::sendByte (MCHEmul::CPU* cpu, const MCHEmul::UByte& b)
{
	_emulation -> printChar (b.value ());

	return (_definition._okResult); // Everything ok...
}
