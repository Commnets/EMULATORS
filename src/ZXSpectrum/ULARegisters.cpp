#include <ZXSpectrum/ULARegisters.hpp>

// ---
ZXSPECTRUM::ULARegisters::ULARegisters ()
	: MCHEmul::InfoClass ("ULARegisters"),
	  _borderColor (0),
	  _MICOutputSignal (false),
	  _MICOutputSignalChanged (false), // It is an OBool...
	  _EAROutputSignal (false),
	  _EAROutputSignalChanged (false), // It is an OBool...
	  _EARInputSignal (false),
	  _EARInputSignalChanged (false), // It is an OBool...
	  _buzzerSignal (false),
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
	result.add ("MICOUT",		_MICOutputSignal);
	result.add ("EAROUT",		_EAROutputSignal);
	result.add ("EARIN",		_EARInputSignal);
	result.add ("EARREAD",		EARReadSignal ());
	result.add ("BUZZER",		_buzzerSignal);

	return (result);
}

// ---
void ZXSPECTRUM::ULARegisters::initializeInternalValues ()
{
	_borderColor = 0;

	_MICOutputSignal = false;
	_MICOutputSignalChanged = false;

	_EAROutputSignal = false;
	_EAROutputSignalChanged = false;

	_EARInputSignal = false;
	_EARInputSignalChanged = false;

	_buzzerSignal = false;

	for (size_t i = 0; i < 8; 
		_keyboardStatus [i++] = MCHEmul::UByte::_0);
	for (size_t i = 0; i < 5; 
		_joystickStatus [i++] = false);
}
