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

	result.add ("MATRIXEMULATIONDES",	(_description == "") ? "Not defined" : _description);
	result.add ("CHARWIDTH",			_wChar);
	result.add ("CHARHEIGHT",			_hChar);
	result.add ("CHARSPERLINE",			_charsPerLine);
	result.add ("CHARSPERPAGE",			_charsPerPage);

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
	  _page (0), // Starting from the beginning...
	  _posX (0), _posY (0),
	  _firstChar (true)
{
	assert (_configuration._charsPerLine > 0 && 
			_configuration._charsPerPage > 0);

	// Trying to verify whether the configuration matches or not with the paper...
	float charWidthInInches = _paper._width / _configuration._charsPerLine; // The char width in inches...
	// Every inch is made up of 72 points, so this is the size of the char in inches...
	if (charWidthInInches < (_configuration._wChar / 72.0f))
		_LOG ("The configuration of the printer is not compatible with the paper. The char width in inches is " + 
			std::to_string (charWidthInInches) + " but the char width in the configuration is " + 
			std::to_string (_configuration._wChar));

	// SOmething similar for the size of pthe page...
	float charHeightInInches = _paper._height / _configuration._charsPerPage;
	if (charHeightInInches < (_configuration._hChar / 72.0f))
		_LOG ("The configuration of the printer is not compatible with the paper. The char height in inches is " + 
			std::to_string (charHeightInInches) + " but the char height in the configuration is " + 
			std::to_string (_configuration._hChar));
}

// ---
MCHEmul::MatrixPrinterEmulation::~MatrixPrinterEmulation ()
{ 
	// Just in case it wasn't done before...
	finalize ();
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
	// If the file was open, it must be closed...
	if (_printerFile.is_open ())
	{
		// ..and if something was printer out before...
		// ...it must be closed!
		if (!_firstChar)
			closeCurrentPage ();

		_printerFile.flush ();
		_printerFile.close ();
	}
	
	// Always ok
	return (true);
}

// ---
std::tuple <bool, bool> MCHEmul::MatrixPrinterEmulation::moveHeadFromX (short px)
{
	bool moveLine = false;
	bool movePage = false;
	
	short nPosX = (short) _posX;
	short nPosY = (short) _posY;
	short nPage = (short) _page;
	
	nPosX += px;
	if (moveLine = (nPosX >= (short) _configuration._charsPerLine))
		while (nPosX >= (short) _configuration._charsPerLine)
			{ nPosY++; nPosX -= (short) _configuration._charsPerLine; }
	else
	if (nPosX < 0)
		while (nPosX < 0)
			{ nPosY--; nPosX += (short) _configuration._charsPerLine; }

	if (nPosY < 0) nPosY = 0; // never less than the beginning of the page...
	if (movePage = (nPosY >= (short) _configuration._charsPerPage))
		while (nPosY >= (short) _configuration._charsPerPage)
			{ nPage++; nPosY -= (short) _configuration._charsPerPage; }

	_posX = (unsigned short) nPosX;
	_posY = (unsigned short) nPosY;
	_page = (unsigned short) nPage;

	return (std::make_tuple (moveLine, movePage));
}

// --
std::tuple <bool, bool> MCHEmul::MatrixPrinterEmulation::moveHeadFromY (short py)
{
	bool moveLine = false;
	bool movePage = false;

	short nPosY = (short) _posY;
	short nPage = (short) _page;

	nPosY += py;
	moveLine = (nPosY != _posY);
	if (nPosY < 0) nPosY = 0;
	if (movePage = (nPosY > (short) _configuration._charsPerPage)) nPage++;

	_posY = (unsigned short) nPosY;
	_page = (unsigned short) nPage;

	return (std::make_tuple (moveLine, movePage));
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
	short aX = 0, aY = 0, aP = 0;
	std::tie (aX, aY, aP) = printCharImplementation (chr);
	_posX += aX, _posY += aY; _page += aP;
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
std::tuple <short, short, short> 
MCHEmul::MatrixPrinterEmulation::printCharImplementation (unsigned char chr)
{
	// Not possible by default...
	short aX = 0, aY = 0, aP = 0;

	// When a new line happens...
	// The possibility to be at the end of the page is taken into account...
	auto newLine = [&]() -> void
		{
			aX -= (short) headXPosition (); // Always at the beginning of the line...
			if ((headYPosition () + 1) == configuration ()._charsPerPage)
			{ 
				aY -= (short) headYPosition (); 
				
				closePage (_page + aP); 
				
				setNewPage (_page + (++aP)); 
			}
			else 
			{ 
				printNewLine (); 
				
				aY++; 
			}
		};

	// When just a new char ins printed out...
	// The possibility of a new line has to be taken into account...
	auto justChar = [&](unsigned char chr) -> void
		{
			size_t numCharsPrinted = 0;
			if ((numCharsPrinted = printNormalChar (chr)) != 0) // it should be usually one, but it cannoT!!
				aX += (short) numCharsPrinted;
			if ((headXPosition () + (unsigned short) numCharsPrinted) >= configuration ()._charsPerLine)
			{ 
				printNewLine (); 
				
				aY++; 
				
				aX -= configuration ()._charsPerLine;
			}
		};

	// Only if it can be printed out!
	if (isControlChar (chr))
	{ 
		short aXP = 0, aYP = 0, aPP = 0;
		std::tie (aXP, aYP, aPP) = manageControlChar (chr);
		aX += aXP; aY += aYP; aP += aPP;
	}
	else 
	if (chr == _configuration._charNewLine) 
		newLine ();
	else 
	if (chr == _configuration._charTab)
	{
		for (size_t i = 0; i < (size_t) _configuration._numSpacesTabChar; i++)
			justChar (_configuration._charSpace); // It has to be a printable character...
	}
	else
	if (isNormalChar (chr))
		justChar (chr);

	printerFile ().flush ();

	return (std::make_tuple (aX, aY, aP));
}

// ---
void MCHEmul::PostscriptMatrixPrinterEmulation::firstTimePrinting (unsigned char chr)
{
	// Copies the general postscript routines to simulate a printing...
	std::ifstream lFile ("PSBasic.ps");
	if (!lFile)
		_LOG ("File PSBasic.ps doesn't exist");
	printerFile () << lFile.rdbuf () << std::endl;
	lFile.close ();
}
