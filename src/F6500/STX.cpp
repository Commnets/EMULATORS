#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
bool F6500::STX_General::executeOn (const MCHEmul::Address& a)
{
	// Set the value...
	memory () -> set (a, cpu () -> internalRegister (F6500::C6510::_XREGISTER).values ()); // 1 byte-length

	return (true);
}

// ---
_INST_IMPL (F6500::STX_Absolute)
{
	return (executeOn (address_absolute ()));
}

// ---
_INST_IMPL (F6500::STX_ZeroPage)
{
	return (executeOn (address_zeroPage ()));
}

// ---
_INST_IMPL (F6500::STX_ZeroPageY)
{
	return (executeOn (address_zeroPageY ()));
}
