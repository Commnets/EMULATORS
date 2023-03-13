#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
bool F6500::DCP_General::executeOn (const MCHEmul::Address& a)
{
	// The memoy location is reduced by 1...
	MCHEmul::UInt v = MCHEmul::UInt ((memory () -> value (a))) /** 1 byte long always. */ - MCHEmul::UInt::_1; // DEC...
	// A carry could be generated, but it will be ignored...
	memory () -> set (a, v.bytes ()); // ...and stored back
	// To compare is like to substract...
	MCHEmul::UInt r = 
		MCHEmul::UInt (cpu () -> internalRegister (F6500::C6510::_ACCUMULATOR).values ()[0]) - v;  //...CMP

	// Time of the status register...
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	st.setBitStatus (F6500::C6500::_NEGATIVEFLAG, r.negative ());
	st.setBitStatus (F6500::C6500::_ZEROFLAG, r == MCHEmul::UInt::_0);
	st.setBitStatus (F6500::C6500::_CARRYFLAG, r.carry ()); // When the result is positive (a >= u)

	return (true);
}

// ---
_INST_IMPL (F6500::DCP_Absolute)
{
	return (executeOn (address_absolute ()));
}

// ---
_INST_IMPL (F6500::DCP_ZeroPage)
{
	return (executeOn (address_zeroPage ()));
}

// ---
_INST_IMPL (F6500::DCP_ZeroPageIndirectX)
{
	return (executeOn (address_indirectZeroPageX ()));
}

// ---
_INST_IMPL (F6500::DCP_ZeroPageIndirectY)
{
	return (executeOn (address_indirectZeroPageY ()));
}

// ---
_INST_IMPL (F6500::DCP_ZeroPageX)
{
	return (executeOn (address_zeroPageX ()));
}

// ---
_INST_IMPL (F6500::DCP_AbsoluteX)
{
	return (executeOn (address_absoluteX ()));
}

// ---
_INST_IMPL (F6500::DCP_AbsoluteY)
{
	return (executeOn (address_absoluteY ()));
}
