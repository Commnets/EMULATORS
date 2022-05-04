#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>
#include <core/CPU.hpp>

// ---
bool F6500::CPY_General::executeWith (MCHEmul::UByte u)
{
	MCHEmul::UInt r = 
		MCHEmul::UInt (cpu () -> internalRegister (F6500::C6510::_YREGISTER).values () /** 1 byte long */) - 
		MCHEmul::UInt ({ u }); 

	// Time of the status register...
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	st.setBitStatus ("N", r [0][7]);
	st.setBitStatus ("Z", r [0] == MCHEmul::UByte::_0);
	st.setBitStatus ("C", !r [0][7]); // When the result is positive (a > u)

	return (true);
}

// ---
_INST_IMPL (F6500::CPY_Inmediate)
{
	return (executeWith (value_inmediate ()));
}

// ---
_INST_IMPL (F6500::CPY_Absolute)
{
	return (executeWith (value_absolute ()));
}

// ---
_INST_IMPL (F6500::CPY_ZeroPage)
{
	return (executeWith (value_zeroPage ()));
}
