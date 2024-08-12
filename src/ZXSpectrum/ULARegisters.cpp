#include <ZXSpectrum/ULARegisters.hpp>

// ---
ZXSPECTRUM::ULARegisters::ULARegisters ()
	: MCHEmul::InfoClass ("ULARegisters"),
	  _MICSignalChanged (false),
	  _EARSignalChanged (false),
	  _keyboardStatus (8, MCHEmul::UByte::_0)
{ 
	initializeInternalValues ();
}

// ---
void ZXSPECTRUM::ULARegisters::initialize ()
{
	initializeInternalValues ();
}

// ---
MCHEmul::InfoStructure ZXSPECTRUM::ULARegisters::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	result.add ("BORDER",		_borderColor);
	result.add ("MICST",		_MICActivated);
	result.add ("MIC",			_MICSignal);
	result.add ("EARST",		_EARActivated);
	result.add ("EAR",			_EARSignal);

	return (result);
}

// ---
void ZXSPECTRUM::ULARegisters::initializeInternalValues ()
{
	_borderColor = 0;

	_MICActivated = false;
	_MICSignal = false;
	_MICSignalChanged = false;

	_EARActivated = false;
	_EARSignal = false;
	_EARSignalChanged = false;

	for (size_t i = 0; i < 8; 
		_keyboardStatus [i++] = MCHEmul::UByte::_0);
}
