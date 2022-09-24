#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>
#include <core/CPU.hpp>

// ---
bool F6500::STA_General::executeOn (const MCHEmul::Address& a)
{
	// Set the value...
	memory () -> set (a, cpu () -> internalRegister (F6500::C6510::_ACCUMULATOR).values ()); // 1 byte-length

	return (true);
}

// ---
_INST_IMPL (F6500::STA_Absolute)
{
	return (executeOn (address_absolute ()));
}

// ---
_INST_IMPL (F6500::STA_ZeroPage)
{
	return (executeOn (address_zeroPage ()));
}

// ---
_INST_IMPL (F6500::STA_ZeroPageIndirectX)
{
	return (executeOn (address_indirectZeroPageX ()));
}

// ---
_INST_IMPL (F6500::STA_ZeroPageIndirectY)
{
	return (executeOn (address_indirectZeroPageY ()));
}

// ---
_INST_IMPL (F6500::STA_ZeroPageX)
{
	return (executeOn (address_zeroPageX ()));
}

// ---
_INST_IMPL (F6500::STA_AbsoluteX)
{
	return (executeOn (address_absoluteX ()));
}

// ---
_INST_IMPL (F6500::STA_AbsoluteY)
{
	return (executeOn (address_absoluteY ()));
}
