#include <SINCLAIR/ThermalPrinter.hpp>
#include <sstream>

// ---
SINCLAIR::BasicThermalPrinterEmulation::BasicThermalPrinterEmulation 
	(const std::function <unsigned char (unsigned char)>& cvt, const std::string& pFN)
	: MCHEmul::BasicMatrixPrinterEmulation (32 /** As wide as the screen. */, pFN),
	  _ZXCodeConversorFunction (std::move (cvt))
{
	configuration ()._description = "Basic Thermal ZX81 Printer Simulation";
}

// ---
SINCLAIR::PostscriptThermalPrinterEmulation::PostscriptThermalPrinterEmulation 
		(const MCHEmul::MatrixPrinterEmulation::Configuration& cfg, const std::string& pFN)
	: MCHEmul::PostscriptMatrixPrinterEmulation 
		(cfg, MCHEmul::MatrixPrinterEmulation::Paper 
			(MCHEmul::MatrixPrinterEmulation::Paper::Type::_WHITE, 3.94f, 20.0f, false), pFN)
{
	configuration ()._description = "Postscript Thermal ZX81 Printer Simulation";
}

// ---
void SINCLAIR::PostscriptThermalPrinterEmulation::firstTimePrinting (unsigned char chr)
{
	// Copies the first group of general postscript routines to simulate a printing...
	std::ifstream l1File ("PSThermalPrinterI.ps");
	if (!l1File)
		_LOG ("File PSThermalPrinterI.ps doesn't exist");
	std::stringstream sst; sst << l1File.rdbuf ();
	printerFile () << MCHEmul::replaceStrings (sst.str (),
		{ "[[PAGEWIDTHINCH]]",
		  "[[PAGEHEIGHTINCH]]" },
		{ std::to_string (paper ()._width), 
		  std::to_string (paper ()._height) });

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
	std::ifstream l2File ("PSThermalPrinterII.ps");
	if (!l2File)
		_LOG ("File PSThermalPrinterII.ps doesn't exist");
	printerFile () << l2File.rdbuf ();
	l2File.close ();

	// Print out information about the margins...
	// Just top start to print out at the right position in the page whatever its size was...
	setNewPage (_page);
}

// ---
void SINCLAIR::PostscriptThermalPrinterEmulation::closePage (unsigned short p)
{
	printerFile ()
		<< std::endl << "grestore" << std::endl // The graphics must be ready for the next...
		<< "showpage" << std::endl << std::endl;
}

// ---
void SINCLAIR::PostscriptThermalPrinterEmulation::setNewPage (unsigned short p)
{
	// Beginning of the next one...
	printerFile ()
		<< "% ----Page:"
		<< MCHEmul::fixLenStr (std::to_string (p), 2, true, MCHEmul::_CEROS) 
		<< "----------------------------------------------" << std::endl
		<< "setupPage" << std::endl
		<< "drawPaper" << std::endl << std::endl
		<< "initgraphics" << std::endl
		<< std::to_string ((unsigned int) (paper ()._border /** in inches. */ * 72.0f /** points per inch. */)) + " " +
		   "0 topLeftWithMargin /y0 exch def /x0 exch def" << std::endl
		<< "gsave" << std::endl
		// As it is said in the documentation:
		// The char advance (x axis) in this printer per character is 0.1 inches = 2,54mm
		// In a resolution of 72 dots per inch, 0,1 inches = 7,2 dots.
		// Horizontally a character takes 7 (6+1) * 2 dots = 14, then the scale to apply in the x axis is = 7,2/14 = 0,5142857
		// In the printer MPS801 there is 6 lines per inch, so each line is 1/6 inches = 0,1666667 inches.
		// Every line is made up of 16 dots = 8 (7+1) * 2
		// The scale to apply is 0,75 because 0,1666667 inches = 12 dots, and 12/16 = 0,75
		<< "0.5142857 0.75 scale" << std::endl
		<< "/x0 x0 1.9444444 mul def" << std::endl
		<< "/y0 y0 1.3333333 mul def" << std::endl << std::endl;
}

// ---
unsigned short SINCLAIR::PostscriptThermalPrinterEmulation::printNormalChar (unsigned char chr)
{
	// Looks for the character...
	MCHEmul::MatrixPrinterEmulation::Configuration::CharSetDefinition::const_iterator i
		= configuration ()._charSet.find ((unsigned char) chr);
	bool existsChr = (i != configuration ()._charSet.end ());

	printerFile () 
		<< "% Character " << (((chr & 0x80) != 0x00) ? "reverse" : "normal") << " mode: "
		<< (existsChr ? std::to_string ((unsigned int) (chr & 0x7f)) : "Unknown") << std::endl;
	printerFile ()
		<< std::to_string ((unsigned int) (existsChr ? chr : 0x00)) << "\tx0 " +
		   std::to_string ((unsigned int) headXPosition ()) + " " +
		   std::to_string ((unsigned int) (configuration ()._wChar)) +
		   " mul dotStepX mul add\ty0 " + // dotStepX defined in ThermalPrinterI.ps
		   std::to_string ((unsigned int) headYPosition ()) + " " +
		   "dotStepY " + // dotStepY & lineGapY defined in ThermalPrinterI.ps
		   std::to_string ((unsigned int) (configuration ()._hChar)) + 
		   " mul lineGapY add mul sub drawCharByCode" << std::endl;

	// Always something is printed out...
	return (1);
}
