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

	stack () -> push (_lastExecutionData._INOUTData = 
		cpu () -> statusRegister ().values ()); // 1 byte long...
	
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
	st.set (_lastExecutionData._INOUTData = stack () -> pull (1 /** 1 byte long. */));
	// The but 5 must always be set in this implementation
	st.setBitStatus (5, true);

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
