#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
bool F6500::LAX_General::executeOn (const MCHEmul::Address& a)
{
	MCHEmul::UByte v = memory () -> value (a);
	cpu () -> internalRegister (F6500::C6510::_ACCUMULATOR).set ({ v }); // 1 byte long always...
	cpu () -> internalRegister (F6500::C6510::_XREGISTER).set ({ v }); // 1 byte long always...

	// Time of the status register...
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	st.setBitStatus (F6500::C6500::_NEGATIVEFLAG, v.bit (7));
	st.setBitStatus (F6500::C6500::_ZEROFLAG, v == MCHEmul::UByte::_0);

	return (true);
}

// ---
_INST_IMPL (F6500::LAX_Absolute)
{
	return (executeOn (address_absolute ()));
}

// ---
_INST_IMPL (F6500::LAX_ZeroPage)
{
	return (executeOn (address_zeroPage ()));
}

// ---
_INST_IMPL (F6500::LAX_ZeroPageIndirectX)
{
	return (executeOn (address_indirectZeroPageX ()));
}

// ---
_INST_IMPL (F6500::LAX_ZeroPageIndirectY)
{
	return (executeOn (address_indirectZeroPageY ()));
}

// ---
_INST_IMPL (F6500::LAX_ZeroPageY)
{
	return (executeOn (address_zeroPageY ()));
}

// ---
_INST_IMPL (F6500::LAX_AbsoluteY)
{
	return (executeOn (address_absoluteY ()));
}
