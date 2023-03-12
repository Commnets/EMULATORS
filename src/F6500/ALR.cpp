#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
bool F6500::ALR_General::executeWith (MCHEmul::UByte u)
{
	MCHEmul::Register& a = cpu () -> internalRegister (F6500::C6510::_ACCUMULATOR);
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	
	// Set the value...
	// The register is always 1 byte long...
	MCHEmul::UByte r = (a.values ()[0] & u);
	bool c = r.shiftRightC (false, 1);
	a.set ({ r });

	// Time of the status register...
	st.setBitStatus (F6500::C6500::_NEGATIVEFLAG, r [7]);
	st.setBitStatus (F6500::C6500::_ZEROFLAG, r == MCHEmul::UByte::_0);
	st.setBitStatus (F6500::C6500::_CARRYFLAG, c);

	return (true);
}

// ---
_INST_IMPL (F6500::ALR_Inmediate)
{
	return (executeWith (value_inmediate ()));
}
