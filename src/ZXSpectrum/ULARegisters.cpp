#include <ZXSpectrum/ULARegisters.hpp>

// ---
ZXSPECTRUM::ULARegisters::ULARegisters ()
	: MCHEmul::InfoClass ("ULARegisters"),
	  _NTSC (false),
	  _casetteSignalChanged (false),
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

	result.add ("NTSC",			_NTSC);
	result.add ("CASETTE",		_casetteSignal);

	return (result);
}

// ---
void ZXSPECTRUM::ULARegisters::initializeInternalValues ()
{
	_NTSC = false;

	// TODO...

	for (size_t i = 0; i < 8; 
		_keyboardStatus [i++] = MCHEmul::UByte::_0);
}
