#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
bool F6500::RRA_General::executeOn (const MCHEmul::Address& a)
{
	MCHEmul::Register& ac = cpu () -> internalRegister (F6500::C6510::_ACCUMULATOR);
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	// Set the value in memory...
	MCHEmul::UByte v = memory () -> values (a, 1)[0]; // 1 byte long always
	bool c = v.rotateRightC (st.bitStatus (F6500::C6500::_CARRYFLAG), 1);
	memory () -> set (a, { v });
	// ...and in the accumulator...
	unsigned char ft = st.bitStatus (F6500::C6500::_DECIMALFLAG) 
		? MCHEmul::UInt::_PACKAGEDBCD : MCHEmul::UInt::_BINARY; // In BCD?
	MCHEmul::UInt r = MCHEmul::UInt (ac.values ()[0], ft).add (MCHEmul::UInt (v, ft), c);
	ac.set (r.bytes ());

	// Time of the status register...
	st.setBitStatus (F6500::C6500::_NEGATIVEFLAG, r.negative ());
	st.setBitStatus (F6500::C6500::_OVERFLOWFLAG, r.overflow ());
	st.setBitStatus (F6500::C6500::_ZEROFLAG, r == MCHEmul::UInt::_0);
	st.setBitStatus (F6500::C6500::_CARRYFLAG, r.carry ());

	return (true);
}

// ---
_INST_IMPL (F6500::RRA_Absolute)
{
	return (executeOn (address_absolute ()));
}

// ---
_INST_IMPL (F6500::RRA_ZeroPage)
{
	return (executeOn (address_zeroPage ()));
}

// ---
_INST_IMPL (F6500::RRA_ZeroPageIndirectX)
{
	return (executeOn (address_indirectZeroPageX ()));
}

// ---
_INST_IMPL (F6500::RRA_ZeroPageIndirectY)
{
	return (executeOn (address_indirectZeroPageY ()));
}

// ---
_INST_IMPL (F6500::RRA_ZeroPageX)
{
	return (executeOn (address_zeroPageX ()));
}

// ---
_INST_IMPL (F6500::RRA_AbsoluteX)
{
	return (executeOn (address_absoluteX ()));
}

// ---
_INST_IMPL (F6500::RRA_AbsoluteY)
{
	return (executeOn (address_absoluteY ()));
}
