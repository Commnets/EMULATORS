#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
bool F6500::RLA_General::executeOn (const MCHEmul::Address& a)
{
	MCHEmul::Register& ac = cpu () -> internalRegister (F6500::C6510::_ACCUMULATOR);
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	// The memory is affected...
	MCHEmul::UByte v = memory () -> value (a); // 1 byte long always...
	bool c = v.rotateLeftC (st.bitStatus (F6500::C6500::_CARRYFLAG), 1); // ROL...
	memory () -> set (a, { v });

	// ...and also the accumulator...
	v = ac.values ()[0] & v; // ...AND
	ac.set ({ v });

	// Time of the status register...
	st.setBitStatus (F6500::C6500::_NEGATIVEFLAG, v [7]);
	st.setBitStatus (F6500::C6500::_ZEROFLAG, v == MCHEmul::UByte::_0);
	st.setBitStatus (F6500::C6500::_CARRYFLAG, c);

	return (true);
}

// ---
_INST_IMPL (F6500::RLA_Absolute)
{
	return (executeOn (address_absolute ()));
}

// ---
_INST_IMPL (F6500::RLA_ZeroPage)
{
	return (executeOn (address_zeroPage ()));
}

// ---
_INST_IMPL (F6500::RLA_ZeroPageIndirectX)
{
	return (executeOn (address_indirectZeroPageX ()));
}

// ---
_INST_IMPL (F6500::RLA_ZeroPageIndirectY)
{
	return (executeOn (address_indirectZeroPageY ()));
}

// ---
_INST_IMPL (F6500::RLA_ZeroPageX)
{
	return (executeOn (address_zeroPageX ()));
}

// ---
_INST_IMPL (F6500::RLA_AbsoluteX)
{
	return (executeOn (address_absoluteX ()));
}

// ---
_INST_IMPL (F6500::RLA_AbsoluteY)
{
	return (executeOn (address_absoluteY ()));
}
