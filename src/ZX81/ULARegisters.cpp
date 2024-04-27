#include <ZX81/ULARegisters.hpp>

// ---
ZX81::ULARegisters::ULARegisters ()
	: MCHEmul::InfoClass ("ULARegisters"),
	  _keyboardStatus (8, MCHEmul::UByte::_0),
	  _casetteSignalChanged (false)
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

	_syncOutputWhite = true;

	_LINECNTRL = 0;
	_LINECNTRLBlocked = true;

	for (size_t i = 0; i < 8; 
		_keyboardStatus [i++] = MCHEmul::UByte::_0);

	_currentRasterLine = 0;
}
