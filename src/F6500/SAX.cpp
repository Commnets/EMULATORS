#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
bool F6500::SAX_General::executeOn (const MCHEmul::Address& a)
{
	// At the end the memory is stored with _ACCUMULATOR & _XREGISTER
	memory () -> set (a, cpu () -> internalRegister (F6500::C6510::_ACCUMULATOR).values ()[0] &
		cpu () -> internalRegister (F6500::C6510::_XREGISTER).values ()[0]); // Always 1 byte long...

	// ...with no impact in registers...

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
