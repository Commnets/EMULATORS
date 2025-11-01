#include <COMMODORE/C6529B/C6529BRegisters.hpp>

// ---
COMMODORE::C6529BRegisters::C6529BRegisters 
		(int id, MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: MCHEmul::ChipRegisters (id, ps, pp, a, s),
	  _portValue (MCHEmul::UByte::_FF) // pulled up by default
{ 
	setClassName ("6529Registers");
}

// ---
void COMMODORE::C6529BRegisters::initialize ()
{
	MCHEmul::ChipRegisters::initialize ();

	_portValue = MCHEmul::UByte::_FF; // pulled up by default
}

// ---
MCHEmul::InfoStructure COMMODORE::C6529BRegisters::getInfoStructure () const
{
	MCHEmul::InfoStructure result = 
		std::move (MCHEmul::ChipRegisters::getInfoStructure ());

	result.add ("PORT", _portValue);

	return (result);
}
