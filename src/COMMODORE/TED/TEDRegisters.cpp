#include <COMMODORE/TED/TEDRegisters.hpp>

// ---
COMMODORE::TEDRegisters::TEDRegisters (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: MCHEmul::ChipRegisters (_TEDREGS_SUBSET, ps, pp, a, s),
	  _lastValueRead (MCHEmul::PhysicalStorage::_DEFAULTVALUE),
	  _backgroundColor (4, 0x00)
	  // At this point the rest internal variables will have random values...
	  // The vector are initialized just to given them a default size!
{
	setClassName ("TEDRegisters");

	initializeInternalValues ();
}

// ---
void COMMODORE::TEDRegisters::initialize ()
{
	MCHEmul::PhysicalStorageSubset::initialize ();

	initializeInternalValues ();
}

// ---
MCHEmul::InfoStructure COMMODORE::TEDRegisters::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::ChipRegisters::getInfoStructure ());

	result.add ("MODE",				(unsigned int) _graphicModeActive);
	result.add ("40C",				_textDisplay40ColumnsActive);
	result.add ("25R",				_textDisplay25RowsActive);
	result.add ("SCROLLX",			_horizontalScrollPosition);
	result.add ("SCROLLY",			_verticalScrollPosition);
	result.add ("FORECOLOR",		_foregroundColor);
	result.add ("BKCOLOR1",			_backgroundColor [0]);
	result.add ("BKCOLOR2",			_backgroundColor [1]);
	result.add ("BKCOLOR3",			_backgroundColor [2]);
	result.add ("BKCOLOR4",			_backgroundColor [3]);
	result.add ("IRQ",				_rasterIRQActive);
	result.add ("IRQLINE",			_IRQRasterLineAt);
	result.add ("CHARADDRESS",		charDataMemory ());
	result.add ("SCREENADDRESS",	screenMemory ());
	result.add ("BITMAPADDRESS",	bitmapMemory ());
	result.add ("LIGHTPENX",		_currentLightPenHorizontalPosition);
	result.add ("LIGHTPENY",		_currentLightPenVerticalPosition);

	return (result);
}

// ---
void COMMODORE::TEDRegisters::setValue (size_t p, const MCHEmul::UByte& v)
{
	size_t pp = p % 0x40;

	// The 64 first bytes will keep the right value...
	MCHEmul::PhysicalStorageSubset::setValue (p, v);

	switch (pp)
	{
		// TODO

		default:
			break;
	}
}       

// ---
const MCHEmul::UByte& COMMODORE::TEDRegisters::readValue (size_t p) const
{
	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	size_t pp = p % 0x40;

	switch (pp)
	{
		// TODO

		default:
			{
				result = MCHEmul::UByte::_FF;
			}

			break;
	}

	return (_lastValueRead = result);
}

// ---
void COMMODORE::TEDRegisters::initializeInternalValues ()
{
	// TODO

	// Managed direclty by the TED Chip...
	_currentRasterLine = 0x0000;
	_currentLightPenHorizontalPosition = 0x0000;
	_currentLightPenVerticalPosition = 0x0000;
	_lightPenActive = false;
	// Reasons for IRQ
	_rasterIRQHappened = false;
	_lightPenIRQHappened = false;
}

// ---
void COMMODORE::TEDRegisters::setGraphicModeActive ()
{
	_graphicModeActive = COMMODORE::TEDRegisters::GraphicMode::_INVALIDBITMAPMODE2; // All on...

	if (!graphicExtendedColorTextModeActive () && !graphicBitModeActive () && !graphicMulticolorTextModeActive ())
		_graphicModeActive = COMMODORE::TEDRegisters::GraphicMode::_CHARMODE;
	else if (!graphicExtendedColorTextModeActive () && !graphicBitModeActive () && graphicMulticolorTextModeActive ())
		_graphicModeActive = COMMODORE::TEDRegisters::GraphicMode::_MULTICOLORCHARMODE;
	else if (!graphicExtendedColorTextModeActive () && graphicBitModeActive () && !graphicMulticolorTextModeActive ())
		_graphicModeActive = COMMODORE::TEDRegisters::GraphicMode::_BITMAPMODE;
	else if (!graphicExtendedColorTextModeActive () && graphicBitModeActive () && graphicMulticolorTextModeActive ())
		_graphicModeActive = COMMODORE::TEDRegisters::GraphicMode::_MULTICOLORBITMAPMODE;
	else if (graphicExtendedColorTextModeActive () && !graphicBitModeActive () && !graphicMulticolorTextModeActive ())
		_graphicModeActive = COMMODORE::TEDRegisters::GraphicMode::_EXTENDEDBACKGROUNDMODE;
	else if (graphicExtendedColorTextModeActive () && !graphicBitModeActive () && graphicMulticolorTextModeActive ())
		_graphicModeActive = COMMODORE::TEDRegisters::GraphicMode::_INVALIDTEXMODE;
	else if (graphicExtendedColorTextModeActive () && graphicBitModeActive () && !graphicMulticolorTextModeActive ())
		_graphicModeActive = COMMODORE::TEDRegisters::GraphicMode::_INVALIDBITMAPMODE1;

	_textMode = (_graphicModeActive == COMMODORE::TEDRegisters::GraphicMode::_CHARMODE ||
				 _graphicModeActive == COMMODORE::TEDRegisters::GraphicMode::_MULTICOLORCHARMODE ||
				 _graphicModeActive == COMMODORE::TEDRegisters::GraphicMode::_EXTENDEDBACKGROUNDMODE ||
				 _graphicModeActive == COMMODORE::TEDRegisters::GraphicMode::_INVALIDTEXMODE);
}
