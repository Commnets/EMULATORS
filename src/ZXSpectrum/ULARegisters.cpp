#include <ZXSpectrum/ULARegisters.hpp>

// ---
ZXSPECTRUM::ULARegisters::ULARegisters ()
	: MCHEmul::InfoClass ("ULARegisters"),
	  _MICSignalChanged (false), // It is an OBool...
	  _EARSignalChanged (false), // It is an OBool...
	  _keyboardStatus (8, MCHEmul::UByte::_0), // To define the initial size...
	  _joystickStatus (5, MCHEmul::UByte::_0) // Just to define the initial size...
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
	for (size_t i = 0; i < 5; 
		_joystickStatus [i++] = false);
}
