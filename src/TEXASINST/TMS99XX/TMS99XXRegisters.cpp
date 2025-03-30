#include <TEXASINST/TMS99XX/TMS99XXRegisters.hpp>

// ---
TEXASINSTRUMENTS::TMS99XXRegisters::TMS99XXRegisters ()
	: MCHEmul::InfoClass ("TMS9918Registers")
{ 
	initializeInternalValues ();
}

// ---
void TEXASINSTRUMENTS::TMS99XXRegisters::initialize ()
{
	initializeInternalValues ();
}

// ---
MCHEmul::InfoStructure TEXASINSTRUMENTS::TMS99XXRegisters::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	// TODO

	return (result);
}

// ---
void TEXASINSTRUMENTS::TMS99XXRegisters::initializeInternalValues ()
{
	// TODO
}
