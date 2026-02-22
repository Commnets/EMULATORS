#include <CORE/MatrixPrinterEmulation.hpp>
#include <CORE/LogChannel.hpp>

const MCHEmul::MatrixPrinterEmulation::Paper 
	MCHEmul::MatrixPrinterEmulation::Paper::_9HALFx11INCHES = MCHEmul::MatrixPrinterEmulation::Paper 
			(MCHEmul::MatrixPrinterEmulation::Paper::Type::_BLUEBAND, 9.5, 11, 0.5);

// ---
MCHEmul::InfoStructure MCHEmul::PostscriptMatrixPrinterEmulation::Paper::getInfoStructure () const
{
	MCHEmul::InfoStructure result;

	result.add ("PAPERWIDTH",	_width);
	result.add ("PAPERHEIGHT",	_height);
	result.add ("PAPERBORDER",	_border);

	return (result);
}

// ---
MCHEmul::InfoStructure MCHEmul::MatrixPrinterEmulation::Configuration::getInfoStructure () const
{
	MCHEmul::InfoStructure result;

	result.add ("CHARWIDTH",	_wChar);
	result.add ("CHARHEIGHT",	_hChar);
	result.add ("CHARSPERLINE", _charsPerLine);

	return (result);
}

// ---
MCHEmul::MatrixPrinterEmulation::MatrixPrinterEmulation
		(const MCHEmul::MatrixPrinterEmulation::Configuration& cfg, 
			const MCHEmul::MatrixPrinterEmulation::Paper& p, const std::string& pFN)
	: MCHEmul::InfoClass ("MatrixPinterEmulation"),
	  _configuration (cfg),
	  _paper (p),
	  _printerFileName (pFN),
	  _printerFile (),
	  _posX (0), _posY (0),
	  _firstChar (true)
{
	// Nothing else to do...
}

// ----
bool MCHEmul::MatrixPrinterEmulation::initialize () 
{ 
	_printerFile.open (_printerFileName, std::ios::out | std::ios::trunc);
	if (!_printerFile)
		_LOG ("File " + _printerFileName + " for printer emulation cannot be open");

	// At the beginning of the page...
	_posX = _posY = 0;
	// The first char to be printed out...
	_firstChar = true;

	return (_printerFile.is_open ());
}

// ---
bool MCHEmul::MatrixPrinterEmulation::finalize () 
{ 
	if (!_printerFile.is_open ())
	{
		_printerFile.flush (); 
		_printerFile.close (); 
	}
	
	return (true);
}

// ---
short MCHEmul::MatrixPrinterEmulation::moveHeadFromX (short px)
{
	short result = 0;

	short nPosX = (short) _posX;
	short nPosY = (short) _posY;
	nPosX += px;

	if (nPosX >= (short) _configuration._charsPerLine)
		while (nPosX >= (short) _configuration._charsPerLine)
			{ nPosY++; result++; nPosX -= (short) _configuration._charsPerLine; }
	else
	if (nPosX < 0)
		while (nPosX < 0)
			{ nPosY--; result--; nPosX += (short) _configuration._charsPerLine; }

	if (nPosY < 0) nPosY = 0;

	_posX = (unsigned short) nPosX;
	_posY = (unsigned short) nPosY;

	return (result);
}

// --
void MCHEmul::MatrixPrinterEmulation::moveHeadFromY (short py)
{
	short nPosY = (short) _posY;
	if ((nPosY += py) < 0) nPosY = 0;
	_posY = (unsigned short) nPosY;
}

// ---
void MCHEmul::MatrixPrinterEmulation::printChar (unsigned char chr)
{
	if (_firstChar)
	{
		firstTimePrinting (chr);

		_firstChar = false;
	}

	beforePrintingChar (chr);
	
	short aL = 0;
	if (printCharImplementation (chr))
	{ 
		// Move the head one position...
		// If the head of printer changes the line, then a new line is printed out!
		// ...or better the number of new lines returned (one the most common situation)
		// ...but only when the number os positive!
		if ((aL = moveHeadFromX (1)) > 0)
		{
			for (unsigned short i = 0; i < aL; i++)
				printCharImplementation (_configuration._charNewLine);
		}
	}

	afterPrintingChar (chr);
}

// ---
MCHEmul::InfoStructure MCHEmul::MatrixPrinterEmulation::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (InfoClass::getInfoStructure ());

	result.add ("MatrixEmulationConfiguration",	std::move (_configuration.getInfoStructure ()));
	result.add ("MatrixEmulationPaper",			std::move (_paper.getInfoStructure ()));
	result.add ("PRINTERFILE",					_printerFileName);

	return (result);
}

// ---
bool MCHEmul::BasicMatrixPrinterEmulation::printCharImplementation (unsigned char chr)
{
	// Not possible by default...
	bool result = false;

	// Only if it can be printed out!
	if (std::isalnum (chr) ||
		chr == _configuration._charNewLine ||
		chr == _configuration._charTab)
	{
		printerFile () << 
			((chr == _configuration._charTab) 
				? MCHEmul::_SPACES.substr (0, _configuration._numSpacesTabChar)
				: std::string ((char) chr, 1));

		// Anytime a character is sent to the "printer" the channel is flushed!
		printerFile ().flush ();

		result = true;
	}

	return (result);
}

// ---
void MCHEmul::PostscriptMatrixPrinterEmulation::firstTimePrinting (unsigned char chr)
{
	// Copies the general postscript routines to simulate a printing...
	std::ifstream lFile ("PSBasic.ps");
	if (!lFile)
		return;

	_printerFile << lFile.rdbuf () << std::endl;

	lFile.close ();
}

// ---
bool MCHEmul::PostscriptMatrixPrinterEmulation::printCharImplementation (unsigned char chr)
{
	// Not possible by default....
	bool result = false;

	// Only if it can be printed out...
	if (std::isalnum (chr) ||
		chr == _configuration._charNewLine ||
		chr == _configuration._charTab)
	{
		if (chr == _configuration._charNewLine) 
			_printerFile << "newLine" << std::endl;
		else 
		if (chr == _configuration._charTab) 
			_printerFile << "32 " + 
				std::to_string (_configuration._numSpacesTabChar) + 
				" printReapeatChar" << std::endl;
		else
			_printerFile << std::to_string ((int) chr) + " printCharAtHead" << std::endl;

		// Anytime a character is sent to the "printer" the channel is flushed!
		_printerFile.flush ();

		result = true;
	}

	return (result);
}
