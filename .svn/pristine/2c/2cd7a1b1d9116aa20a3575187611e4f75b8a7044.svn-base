#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
bool F6500::SBC_General::executeWith (MCHEmul::UByte u)
{
	MCHEmul::Register& a = cpu () -> internalRegister (F6500::C6510::_ACCUMULATOR);
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	// Read the value, makes the operation and set it back!
	unsigned char ft = st.bitStatus ("D") ? MCHEmul::UInt::_PACKAGEDBCD : MCHEmul::UInt::_BINARY; // In BCD?
	MCHEmul::UInt r = MCHEmul::UInt (a.values () /** 1 byte long. */, false, ft).
		substract (MCHEmul::UInt ({ u }), st.bitStatus ("C"));
	a.set (r.bytes ()); // The carry is taken into account in the substraction

	// Time of the status register...
	st.setBitStatus ("N", r.negative ());
	st.setBitStatus ("V", r.overflow ());
	st.setBitStatus ("Z", r == MCHEmul::UInt::_0);
	st.setBitStatus ("C", r.carry ());

	return (true);
}

// ---
_INST_IMPL (F6500::SBC_Inmediate)
{
	return (executeWith (value_inmediate ()));
}

// ---
_INST_IMPL (F6500::SBC_Absolute)
{
	return (executeWith (value_absolute ()));
}

// ---
_INST_IMPL (F6500::SBC_ZeroPage)
{
	return (executeWith (value_zeroPage ()));
}

// ---
_INST_IMPL (F6500::SBC_ZeroPageIndirectX)
{
	return (executeWith (value_indirectZeroPageX ()));
}

// ---
_INST_IMPL (F6500::SBC_ZeroPageIndirectY)
{
	return (executeWith (value_indirectZeroPageY ()));
}

// ---
_INST_IMPL (F6500::SBC_ZeroPageX)
{
	return (executeWith (value_zeroPageX ()));
}

// ---
_INST_IMPL (F6500::SBC_AbsoluteX)
{
	return (executeWith (value_absoluteX ()));
}

// ---
_INST_IMPL (F6500::SBC_AbsoluteY)
{
	return (executeWith (value_absoluteY ()));
}
