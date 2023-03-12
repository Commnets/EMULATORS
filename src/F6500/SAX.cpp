#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
bool F6500::SAX_General::executeOn (const MCHEmul::Address& a)
{
	memory () -> set (a, cpu () -> internalRegister (F6500::C6510::_XREGISTER).values ());
	memory () -> set (a, cpu () -> internalRegister (F6500::C6510::_ACCUMULATOR).values ()[0] /** 1 byte long. */ & a.values () [0]);

	// No impact in registers...

	return (true);
}

// ---
_INST_IMPL (F6500::SAX_Absolute)
{
	return (executeOn (address_absolute ()));
}

// ---
_INST_IMPL (F6500::SAX_ZeroPage)
{
	return (executeOn (address_zeroPage ()));
}

// ---
_INST_IMPL (F6500::SAX_ZeroPageY)
{
	return (executeOn (address_zeroPageY ()));
}

// ---
_INST_IMPL (F6500::SAX_ZeroPageIndirectX)
{
	return (executeOn (address_indirectZeroPageX ()));
}
