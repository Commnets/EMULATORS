#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>
#include <core/CPU.hpp>

// ---
bool F6500::SBC_General::executeWith (MCHEmul::UByte u)
{
	MCHEmul::Register& a = cpu () -> internalRegister (F6500::C6510::_ACCUMULATOR);
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	
	// The register and the value are 1 byte length in this architecture
	MCHEmul::UInt r = MCHEmul::UInt (a.values ()) + MCHEmul::UInt ({ u }).complement (); // Really a substraction
	if (st.bitStatus ("C")) r += MCHEmul::UInt::_1;

	// The result of the addition can be longer than 1 byte meaning carry generated
	bool c = r.size () > 1; 
	if (c) r = MCHEmul::UInt ({ r [1] }); /** Internally the number is stored in big-endian format always. */
	a.set (r.bytes ());

	// Time of the status register...
	st.setBitStatus ("N", r [0][7]);
	st.setBitStatus ("V", r [0][7]);
	st.setBitStatus ("Z", r [0] == MCHEmul::UByte::_0);
	st.setBitStatus ("C", c);

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
