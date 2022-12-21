#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
bool F6500::CPY_General::executeWith (MCHEmul::UByte u)
{
	// To compare is like to substract...
	MCHEmul::UInt r = 
		MCHEmul::UInt (cpu () -> internalRegister (F6500::C6510::_YREGISTER).values () /** 1 byte long */) - 
		MCHEmul::UInt ({ u });  // Never longer that 1 byte, but the result could be negative...

	// Time of the status register...
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	st.setBitStatus (F6500::C6500::_NEGATIVEFLAG, r.negative ());
	st.setBitStatus (F6500::C6500::_ZEROFLAG, r [0] == MCHEmul::UByte::_0);
	st.setBitStatus (F6500::C6500::_CARRYFLAG, r.carry ()); // When the result is positive (a >= u)

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
