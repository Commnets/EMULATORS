#include <SINCLAIR/Printer.hpp>

const MCHEmul::MatrixPrinterEmulation::Configuration 
	SINCLAIR::PostscriptThermalPrinterEmulation::_CONFIGURATION = 
	MCHEmul::MatrixPrinterEmulation::Configuration (
		(unsigned char) 6, (unsigned char) 7,
		MCHEmul::MatrixPrinterEmulation::Configuration::CharSetDefinition 
			({
			}),
		// max width in chars, this printer can print up!
		(unsigned short) 80, 
		// usual height in chars (when the paper is 11 inches height), 
		// although it has to be adjusted to the the height in inches of the paper...
		(unsigned short) 66, 
		(unsigned char) 0x0d, (unsigned char) 0x20, (unsigned char) 0x09,
		(unsigned short) 4);

// ---
unsigned short SINCLAIR::BasicThermalPrinterEmulation::printNormalChar (unsigned char chr)
{
	// By default what to print out is an space...
	// unless the char received has an quivalent in "printable" ascii.
	unsigned char nChr = ' ';

    // En ZX81, los caracteres en vídeo inverso están en el bloque 128..191
    // y corresponden a los códigos 0..63 con el bit 7 a 1.
    // Los tratamos igual que su versión normal.
	if (chr >= 0x80 && chr <= 0xbf)
		chr &= 0x7F;

	switch (chr) {
		case 0x00: nChr = ' ';
		case 0x0b: nChr = '"';
		case 0x0d: nChr = '$';
		case 0x0e: nChr = ':';
		case 0x0f: nChr = '?';
		case 0x10: nChr = '(';
		case 0x11: nChr = ')';
		case 0x12: nChr = '>';
		case 0x13: nChr = '<';
		case 0x14: nChr = '=';
		case 0x15: nChr = '+';
		case 0x16: nChr = '-';
		case 0x17: nChr = '*';
		case 0x18: nChr = '/';
		case 0x19: nChr = ';';
		case 0x1a: nChr = ',';
		case 0x1b: nChr = '.';
		default: break;
	}

	// Digits 0..9
	if (chr >= 0x1c && chr <= 0x25) nChr = '0' + (chr - 0x1c);
    // Letras A..Z
    if (chr >= 0x26 && chr <= 0x3f) nChr = 'A' + (chr - 0x26);

	printerFile () << std::string (1, (char) nChr);

	return (1);
}

// ---
SINCLAIR::PostscriptThermalPrinterEmulation::PostscriptThermalPrinterEmulation (const std::string& pFN)
	: MCHEmul::PostscriptMatrixPrinterEmulation 
		(_CONFIGURATION, { }, pFN),
	  _posXInside (0)
{
	// TODO
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
