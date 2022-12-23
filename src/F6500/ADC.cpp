#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
bool F6500::ADC_General::executeWith (MCHEmul::UByte u)
{
	MCHEmul::Register& a = cpu () -> internalRegister (F6500::C6510::_ACCUMULATOR);
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	
	// Calculate the addition...
	unsigned char ft = st.bitStatus (F6500::C6500::_DECIMALFLAG) 
		? MCHEmul::UInt::_PACKAGEDBCD : MCHEmul::UInt::_BINARY; // In BCD?
	MCHEmul::UInt r = MCHEmul::UInt (a.values ()[0], ft).
		add (MCHEmul::UInt (u, ft), st.bitStatus (F6500::C6500::_CARRYFLAG));
	a.set (r.bytes ()); // The carry register is taken into account in the addition...

	// Time of the status register...
	st.setBitStatus (F6500::C6500::_NEGATIVEFLAG, r.negative ());
	st.setBitStatus (F6500::C6500::_OVERFLOWFLAG, r.overflow ());
	st.setBitStatus (F6500::C6500::_ZEROFLAG, r == MCHEmul::UInt::_0);
	st.setBitStatus (F6500::C6500::_CARRYFLAG, r.carry ());

	return (true);
}

// ---
_INST_IMPL (F6500::ADC_Inmediate)
{
	return (executeWith (value_inmediate ()));
}

// ---
_INST_IMPL (F6500::ADC_Absolute)
{
	return (executeWith (value_absolute ()));
}

// ---
_INST_IMPL (F6500::ADC_ZeroPage)
{
	return (executeWith (value_zeroPage ()));
}

// ---
_INST_IMPL (F6500::ADC_ZeroPageIndirectX)
{
	return (executeWith (value_indirectZeroPageX ()));
}

// ---
_INST_IMPL (F6500::ADC_ZeroPageIndirectY)
{
	return (executeWith (value_indirectZeroPageY ()));
}

// ---
_INST_IMPL (F6500::ADC_ZeroPageX)
{
	return (executeWith (value_zeroPageX ()));
}

// ---
_INST_IMPL (F6500::ADC_AbsoluteX)
{
	return (executeWith (value_absoluteX ()));
}

// ---
_INST_IMPL (F6500::ADC_AbsoluteY)
{
	return (executeWith (value_absoluteY ()));
}
