#include <FZ80/INOUT.hpp>

// ---
_INST_IMPL (FZ80::IN_A)
{
	assert (parameters ().size () == 2);

	return (executeAWith (parameters ()[1].value ()));
}

// ---
_INST_IMPL (FZ80::IN_AFromC)
{
	assert (parameters ().size () == 2);

	return (executeWithFromC (registerA ()));
}

// ---
_INST_IMPL (FZ80::IN_BFromC)
{
	assert (parameters ().size () == 2);

	return (executeWithFromC (registerB ()));
}

// ---
_INST_IMPL (FZ80::IN_CFromC)
{
	assert (parameters ().size () == 2);

	return (executeWithFromC (registerC ()));
}

// ---
_INST_IMPL (FZ80::IN_DFromC)
{
	assert (parameters ().size () == 2);

	return (executeWithFromC (registerD ()));
}

// ---
_INST_IMPL (FZ80::IN_EFromC)
{
	assert (parameters ().size () == 2);

	return (executeWithFromC (registerE ()));
}

// ---
_INST_IMPL (FZ80::IN_HFromC)
{
	assert (parameters ().size () == 2);

	return (executeWithFromC (registerH ()));
}

// ---
_INST_IMPL (FZ80::IN_LFromC)
{
	assert (parameters ().size () == 2);

	return (executeWithFromC (registerL ()));
}

// ---
_INST_IMPL (FZ80::IN_FFromC)
{
	assert (parameters ().size () == 2);

	return (execute0FromC ());
}

// ---
bool FZ80::INBlock_General::executeWith (int a)
{
	assert (a == 1 || a == -1);

	// The registers involved...
	MCHEmul::Register& rB		= registerB ();
	MCHEmul::Register& rC		= registerC ();
	MCHEmul::Register& rH		= registerH ();
	MCHEmul::Register& rL		= registerL ();

	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	MCHEmul::UInt bI	 (MCHEmul::UBytes ({ rB.values ()[0] })); // Number of data to move... 
	MCHEmul::Address hlA (MCHEmul::UBytes ({ rH.values ()[0], rL.values ()[0] })); // Target...

	MCHEmul::UByte vR = MCHEmul::UByte::_0;
	// ...and then the value read from the port is pushed into the memory...
	memory () -> set (addressHL (), vR = static_cast <CZ80*> (cpu ()) -> portValue (rC.values ()[0].value ()));
	// The value of the component BC is pushed into the address bus...
	_lastINOUTAddress = addressBC ();

	// Moves to the next position 
	// or the previous (depending on the value of a...
	hlA += a; 
	// The number of elements to move is decremented into 1, 
	// and _b becomes true if data data is 0
	_b0 = ((bI -= 1).asUnsignedInt () == (unsigned int) 0); 

	// How the flags are affected...
	// http://www.z80.info/zip/z80-documented.pdf (section 4.3)
	bool ec = ((unsigned int) hlA.value () + (((unsigned int) rC.values ()[0].value () + a) & 0xff)) > 0xff;
	bool pc = ((unsigned int) hlA.value () + (((unsigned int) rC.values ()[0].value () + a) & 0xff) & 0x07) ^ rB.values ()[0].value ();
	st.setBitStatus (FZ80::CZ80::_CARRYFLAG, ec);
	st.setBitStatus (FZ80::CZ80::_NEGATIVEFLAG, vR.bit (7));
	st.setBitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG, pc);
	st.setBitStatus (FZ80::CZ80::_BIT3FLAG, bI [0].bit (5));
	st.setBitStatus (FZ80::CZ80::_HALFCARRYFLAG, ec);
	st.setBitStatus (FZ80::CZ80::_BIT5FLAG, bI [0].bit (5));
	st.setBitStatus (FZ80::CZ80::_ZEROFLAG, bI [0] == MCHEmul::UByte::_0);
	st.setBitStatus (FZ80::CZ80::_SIGNFLAG, bI [0].bit (7));
	
	// Restore the new content into the registers...
	rB.set ({ bI.bytes ()[0] });
	rH.set ({ hlA.bytes ()[0] }); rL.set ( { hlA.bytes ()[1] });

	return (true);
}

// ---
_INST_IMPL (FZ80::INI)
{
	assert (parameters ().size () == 2);

	return (executeWith (1)); // move up...
}

// ---
_INST_IMPL (FZ80::INIR)
{
	assert (parameters ().size () == 2);

	bool result = executeWith (1); // move up...
	// The instruction only finish when reaches the limit...
	if (result && (_FINISH = _b0) == false) 
		_additionalCycles = 5; // ...and if not, it costs 5 additional cycles always!

	return (result);
}

// ---
_INST_IMPL (FZ80::IND)
{
	assert (parameters ().size () == 2);

	return (executeWith (-1)); // move down...
}

// ---
_INST_IMPL (FZ80::INDR)
{
	assert (parameters ().size () == 2);

	bool result = executeWith (-1); // move down...
	// The instruction only finish when reaches the limit...
	if (result && (_FINISH = _b0) == false) 
		_additionalCycles = 5; // ...and if not it costs 5 additional cycles always!

	return (result);
}

// ---
_INST_IMPL (FZ80::OUT_A)
{
	assert (parameters ().size () == 2);

	return (executeAWith (parameters ()[1].value ()));
}

// ---
_INST_IMPL (FZ80::OUT_AToC)
{
	assert (parameters ().size () == 2);

	return (executeWithToC (registerA ()));
}

// ---
_INST_IMPL (FZ80::OUT_BToC)
{
	assert (parameters ().size () == 2);

	return (executeWithToC (registerB ()));
}

// ---
_INST_IMPL (FZ80::OUT_CToC)
{
	assert (parameters ().size () == 2);

	return (executeWithToC (registerC ()));
}

// ---
_INST_IMPL (FZ80::OUT_DToC)
{
	assert (parameters ().size () == 2);

	return (executeWithToC (registerD ()));
}

// ---
_INST_IMPL (FZ80::OUT_EToC)
{
	assert (parameters ().size () == 2);

	return (executeWithToC (registerE ()));
}

// ---
_INST_IMPL (FZ80::OUT_HToC)
{
	assert (parameters ().size () == 2);

	return (executeWithToC (registerH ()));
}

// ---
_INST_IMPL (FZ80::OUT_LToC)
{
	assert (parameters ().size () == 2);

	return (executeWithToC (registerL ()));
}

// ---
_INST_IMPL (FZ80::OUT_0ToC)
{
	assert (parameters ().size () == 2);

	return (execute0WithToC ());
}

// ---
bool FZ80::OUTBlock_General::executeWith (int a)
{
	assert (a == 1 || a == -1);

	// The registers involved...
	MCHEmul::Register& rB		= registerB ();
	MCHEmul::Register& rC		= registerC ();
	MCHEmul::Register& rH		= registerH ();
	MCHEmul::Register& rL		= registerL ();

	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	MCHEmul::UInt bI	 (MCHEmul::UBytes ({ rB.values ()[0] })); // Number of data to move... 
	MCHEmul::Address hlA (MCHEmul::UBytes ({ rH.values ()[0], rL.values ()[0] })); // Target...

	MCHEmul::UByte vR = MCHEmul::UByte::_0;
	// ...and then the value read from the port is pushed into the memory...
	static_cast <CZ80*> (cpu ()) -> setPortValue (rC.values ()[0].value (), vR = memory () -> value (addressHL ()));
	// The value of the component BC is pushed into the address bus...
	_lastINOUTAddress = addressBC ();

	// Moves to the next position 
	// or the previous (depending on the value of a...
	hlA += a; 
	// The number of elements to move is decremented into 1, 
	// and _b becomes true if data data is 0
	_b0 = ((bI -= 1).asUnsignedInt () == (unsigned int) 0); 

	// How the flags are affected...
	// http://www.z80.info/zip/z80-documented.pdf (section 4.3)
	bool ec = ((unsigned int) hlA.value () + (unsigned int) rL.values ()[0].value ()) > 0xff;
	bool pc = (((unsigned int) hlA.value () + (unsigned int) rL.values ()[0].value ()) & 0x07) ^ rB.values ()[0].value ();
	st.setBitStatus (FZ80::CZ80::_CARRYFLAG, ec);
	st.setBitStatus (FZ80::CZ80::_NEGATIVEFLAG, vR.bit (7));
	st.setBitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG, pc);
	st.setBitStatus (FZ80::CZ80::_BIT3FLAG, bI [0].bit (5));
	st.setBitStatus (FZ80::CZ80::_HALFCARRYFLAG, ec);
	st.setBitStatus (FZ80::CZ80::_BIT5FLAG, bI [0].bit (5));
	st.setBitStatus (FZ80::CZ80::_ZEROFLAG, bI [0] == MCHEmul::UByte::_0);
	st.setBitStatus (FZ80::CZ80::_SIGNFLAG, bI [0].bit (7));
	
	// Restore the new content into the registers...
	rB.set ({ bI.bytes ()[0] });
	rH.set ({ hlA.bytes ()[0] }); rL.set ( { hlA.bytes ()[1] });

	return (true);
}

// ---
_INST_IMPL (FZ80::OUTI)
{
	assert (parameters ().size () == 2);

	return (executeWith (1)); // move up...
}

// ---
_INST_IMPL (FZ80::OTIR)
{
	assert (parameters ().size () == 2);

	bool result = executeWith (1); // move up...
	// The instruction only finish when reaches the limit...
	if (result && (_FINISH = _b0) == false) 
		_additionalCycles = 5; // ...and if not, it costs 5 additional cycles always!

	return (result);
}

// ---
_INST_IMPL (FZ80::OUTD)
{
	assert (parameters ().size () == 2);

	return (executeWith (-1)); // move down...
}

// ---
_INST_IMPL (FZ80::OTDR)
{
	assert (parameters ().size () == 2);

	bool result = executeWith (-1); // move down...
	// The instruction only finish when reaches the limit...
	if (result && (_FINISH = _b0) == false) 
		_additionalCycles = 5; // ...and if not it costs 5 additional cycles always!

	return (result);
}
