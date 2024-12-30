#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
_INST_IMPL (F6500::PHA)
{
	assert (parameters ().size () == 1);

	stack () -> push (_lastExecutionData._INOUTData = 
		cpu () -> internalRegister (F6500::C6510::_ACCUMULATOR).values ()); // 1 byte long...

	return (!stack () -> overflow ());
}

// ---
_INST_IMPL (F6500::PHP)
{
	assert (parameters ().size () == 1);

	MCHEmul::UByte v = cpu () -> statusRegister ().values ()[0];
	// The bit BREAK is set just when keep,
	// but the value of the official status register doesn't change...
	v.setBit (F6500::C6500::_BREAKFLAG, true);
	// This bit should be useless in normal work because no instruction can change this flag
	// that is set when Status Register is initialized, 
	// but it is here to make 6500's testers work...
	v.setBit (5, true);
	stack () -> push (_lastExecutionData._INOUTData = MCHEmul::UBytes ({ v }));
	
	return (!stack () -> overflow ());
}

// ---
_INST_IMPL (F6500::PLA)
{
	assert (parameters ().size () == 1);

	MCHEmul::UBytes u = stack () -> pull (1 /** 1 byte long. */);
	cpu () -> internalRegister (F6500::C6510::_ACCUMULATOR).set 
		(_lastExecutionData._INOUTData = u);

	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	st.setBitStatus (F6500::C6500::_NEGATIVEFLAG, u [0][7]);
	st.setBitStatus (F6500::C6500::_ZEROFLAG, u [0] == MCHEmul::UByte::_0);

	return (!stack () -> overflow ());
}

// ---
_INST_IMPL (F6500::PLP)
{
	assert (parameters ().size () == 1);

	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	MCHEmul::UByte v = stack () -> pull (1 /** 1 bye long always. */)[0];
	// Respects the sitution of the BREAK flag...
	v.setBit (F6500::C6500::_BREAKFLAG, 
		st.bitStatus (F6500::C6500::_BREAKFLAG));
	// The bit 5 must always be set in this implementation...
	// In normal working this instruction is be useless,
	// but it is here to make 6500's testers work!
	v.setBit (5, true);
	st.set (_lastExecutionData._INOUTData = MCHEmul::UBytes ({ v }));

	return (!stack () -> overflow ());
}

// ---
_INST_IMPL (F6500::TSX)
{
	assert (parameters ().size () == 1);

	MCHEmul::UBytes u ({ (unsigned char) stack () -> position () });
	cpu () -> internalRegister (F6500::C6510::_XREGISTER).set (u);

	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	st.setBitStatus (F6500::C6500::_NEGATIVEFLAG, u [0][7]);
	st.setBitStatus (F6500::C6500::_ZEROFLAG, u [0] == MCHEmul::UByte::_0);

	return (true);
}

// ---
_INST_IMPL (F6500::TXS)
{
	assert (parameters ().size () == 1);

	stack () -> setPosition ((int) cpu () -> 
		internalRegister (F6500::C6510::_XREGISTER).values ()[0].value ()); // 1 byte long...

	return (true);
}
