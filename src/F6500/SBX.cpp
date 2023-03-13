#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
bool F6500::SBX_General::executeWith (MCHEmul::UByte u)
{
	MCHEmul::Register& x = cpu () -> internalRegister (F6500::C6510::_XREGISTER);
	MCHEmul::UInt v = MCHEmul::UInt (cpu () -> 
		internalRegister (F6500::C6510::_ACCUMULATOR).values ()[0] & x.values ()[0]) - MCHEmul::UInt ({ u });
	x.set (v.values ()); // But always 1 byte long...

	// Time of the status register...
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	st.setBitStatus (F6500::C6500::_NEGATIVEFLAG, v.negative ());
	st.setBitStatus (F6500::C6500::_ZEROFLAG, v == MCHEmul::UInt::_0);
	st.setBitStatus (F6500::C6500::_CARRYFLAG, v.carry ());

	return (true);
}

// ---
_INST_IMPL (F6500::SBX_Inmediate)
{
	return (executeWith (value_inmediate ()));
}
