#include <SINCLAIR/Printer.hpp>

// ---
SINCLAIR::BasicThermalPrinterEmulation::BasicThermalPrinterEmulation 
	(SINCLAIR::ZXCodeToASCII* c, const std::string& pFN)
	: MCHEmul::BasicMatrixPrinterEmulation (32 /** As wide as the screen. */, pFN),
	  _ZXCodeConversor (c)
{
	// Can not be nullptr...
	assert (_ZXCodeConversor != nullptr);

	configuration ()._description = "Basic Thermal ZX81 Printer Simulation";
}

// ---
SINCLAIR::PostscriptThermalPrinterEmulation::PostscriptThermalPrinterEmulation 
		(const MCHEmul::MatrixPrinterEmulation::Configuration& cfg, const std::string& pFN)
	: MCHEmul::PostscriptMatrixPrinterEmulation 
		(cfg, { }, pFN),
	  _posXInside (0)
{
	configuration ()._description = "Postscript Thermal ZX81 Printer Simulation";
}

// ---
void SINCLAIR::PostscriptThermalPrinterEmulation::firstTimePrinting (unsigned char chr)
{
	// TODO
}

// ---
void SINCLAIR::PostscriptThermalPrinterEmulation::closePage (unsigned short p)
{
	// TODO
}

// ---
void SINCLAIR::PostscriptThermalPrinterEmulation::setNewPage (unsigned short p)
{
	// TODO
}

// ---
bool SINCLAIR::PostscriptThermalPrinterEmulation::printNewLine ()
{
	// TODO

	return (true);
}

// ---
unsigned short SINCLAIR::PostscriptThermalPrinterEmulation::printNormalChar (unsigned char chr)
{
	// TODO

	return (1);
}
