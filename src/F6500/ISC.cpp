#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
bool F6500::ISC_General::executeOn (const MCHEmul::Address& a)
{
	MCHEmul::Register& ac = cpu () -> internalRegister (F6500::C6510::_ACCUMULATOR);
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	// Read the value, makes the operation and sets it back...
	MCHEmul::UInt v = MCHEmul::UInt ((memory () -> value (a))) /** 1 bytelong. */ + MCHEmul::UInt::_1; // INC...
	// A carry could be generated, but it will be ignored...
	memory () -> set (a, v.bytes ());

	// ...and then the substract...
	unsigned char ft = st.bitStatus (F6500::C6500::_DECIMALFLAG) 
		? MCHEmul::UInt::_PACKAGEDBCD : MCHEmul::UInt::_BINARY; // In BCD?
	MCHEmul::UInt r = MCHEmul::UInt (ac.values ()[0], ft).substract 
		(MCHEmul::UInt (v.bytes (), ft), st.bitStatus (F6500::C6500::_CARRYFLAG)); // ...SBC
	ac.set (r.bytes ());

	// Time of the status register...
	st.setBitStatus (F6500::C6500::_NEGATIVEFLAG, r.negative ());
	st.setBitStatus (F6500::C6500::_OVERFLOWFLAG, r.overflow ());
	st.setBitStatus (F6500::C6500::_ZEROFLAG, r == MCHEmul::UInt::_0);
	st.setBitStatus (F6500::C6500::_CARRYFLAG, r.carry ());

	return (true);
}

// ---
_INST_IMPL (F6500::ISC_Absolute)
{
	return (executeOn (address_absolute ()));
}

// ---
_INST_IMPL (F6500::ISC_ZeroPage)
{
	return (executeOn (address_zeroPage ()));
}

// ---
_INST_IMPL (F6500::ISC_ZeroPageIndirectX)
{
	return (executeOn (address_indirectZeroPageX ()));
}

// ---
_INST_IMPL (F6500::ISC_ZeroPageIndirectY)
{
	return (executeOn (address_indirectZeroPageY ()));
}

// ---
_INST_IMPL (F6500::ISC_ZeroPageX)
{
	return (executeOn (address_zeroPageX ()));
}

// ---
_INST_IMPL (F6500::ISC_AbsoluteX)
{
	return (executeOn (address_absoluteX ()));
}

// ---
_INST_IMPL (F6500::ISC_AbsoluteY)
{
	return (executeOn (address_absoluteY ()));
}
