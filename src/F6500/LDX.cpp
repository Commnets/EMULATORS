#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
bool F6500::LDX_General::executeWith (MCHEmul::UByte u)
{
	// Set the value...
	cpu () -> internalRegister (F6500::C6510::_XREGISTER).set ({ u });

	// Time of the status register...
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	st.setBitStatus (F6500::C6500::_NEGATIVEFLAG, u [7]);
	st.setBitStatus (F6500::C6500::_ZEROFLAG, u == MCHEmul::UByte::_0);

	return (true);
}

// ---
_INST_IMPL (F6500::LDX_Inmediate)
{
	return (executeWith (value_inmediate ()));
}

// ---
_INST_IMPL (F6500::LDX_Absolute)
{
	return (executeWith (value_absolute ()));
}

// ---
_INST_IMPL (F6500::LDX_ZeroPage)
{
	return (executeWith (value_zeroPage ()));
}

// ---
_INST_IMPL (F6500::LDX_AbsoluteY)
{
	return (executeWith (value_absoluteY ()));
}

// ---
_INST_IMPL (F6500::LDX_ZeroPageY)
{
	return (executeWith (value_zeroPageY ()));
}
