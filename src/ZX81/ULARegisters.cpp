#include <ZX81/ULARegisters.hpp>

// ---
ZX81::ULARegisters::ULARegisters (ZX81::Type t)
	: MCHEmul::InfoClass ("ULARegisters"),
	  _type (t),
	  _casetteSignalChanged (false),
	  _INTack (false), _INTackClock (0),
	  _keyboardStatus (8, MCHEmul::UByte::_0),
	  _joystickStatus (5, MCHEmul::UByte::_0) // Just to define the initial size...
{ 
	initializeInternalValues ();
}

// ---
void ZX81::ULARegisters::initialize ()
{
	initializeInternalValues ();
}

// ---
MCHEmul::InfoStructure ZX81::ULARegisters::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	result.add ("NTSC",			_NTSC);
	result.add ("NMIGEN",		_NMIGenerator);
	result.add ("SYNCWHITE",	_syncOutputWhite);
	result.add ("LINECNTRL",	_LINECNTRL);
	result.add ("CASETTE",		_casetteSignal);

	return (result);
}

// ---
void ZX81::ULARegisters::initializeInternalValues ()
{
	_NMIGenerator = false;

	_NTSC = false;

	_inVSync = true;
	_syncOutputWhite = true;

	_INTack = false;
	_INTackClock = 0;

	// To start to count from 0 at the very first INC
	_LINECNTRL = (_type == Type::_ZX80) ? 0 : 7; 
	_LINECNTRLBlocked = true;

	_reverseVideo = false;

	_SHIFTR = 0;
	_originalSHIFTR = 0;
	_shiftedBit = 8; // To load a value first time memory is read...

	_casetteSignal = false;
	_casetteSignalChanged = false;

	for (size_t i = 0; i < 8; 
		_keyboardStatus [i++] = MCHEmul::UByte::_0);
	for (size_t i = 0; i < 5; 
		_joystickStatus [i++] = false);
}
