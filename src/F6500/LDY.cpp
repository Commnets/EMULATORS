#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>
#include <core/CPU.hpp>

// ---
bool F6500::LDY_General::executeWith (MCHEmul::UByte u)
{
	// Set the value...
	cpu () -> internalRegister (F6500::C6510::_YREGISTER).set ({ u });

	// Time of the status register...
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	st.setBitStatus ("N", u [7]);
	st.setBitStatus ("Z", u == MCHEmul::UByte::_0);

	return (true);
}

// ---
_INST_IMPL (F6500::LDY_Inmediate)
{
	return (executeWith (value_inmediate ()));
}

// ---
_INST_IMPL (F6500::LDY_Absolute)
{
	return (executeWith (value_absolute ()));
}

// ---
_INST_IMPL (F6500::LDY_ZeroPage)
{
	return (executeWith (value_zeroPage ()));
}

// ---
_INST_IMPL (F6500::LDY_ZeroPageX)
{
	return (executeWith (value_zeroPageX ()));
}

// ---
_INST_IMPL (F6500::LDY_AbsoluteX)
{
	return (executeWith (value_absoluteX ()));
}
