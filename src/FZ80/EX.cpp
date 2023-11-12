#include <FZ80/Instructions.hpp>

// ---
bool FZ80::EX_General::executeWith (MCHEmul::RefRegisters& r, MCHEmul::RefRegisters& rp)
{
	// Keep a copy of first register content..
	MCHEmul::UBytes o ({ r [0] -> values ()[0], r [1] -> values ()[0] });
	// ...Move the second register content into the first
	r  [0] -> set (MCHEmul::UBytes ({ rp [0]  -> values ()[0] }));
	r  [1] -> set (MCHEmul::UBytes ({ rp [1]  -> values ()[0] }));
	// ...Move the copy kept into the second register...
	rp [0] -> set (MCHEmul::UBytes ({ o  [0] }));
	rp [1] -> set (MCHEmul::UBytes ({ o  [1] }));

	return (true);
}

// ---
_INST_IMPL (FZ80::EX_AF)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerAF (), registerAFP ()));
}

// ---
_INST_IMPL (FZ80::EX_DEWithHL)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerDE (), registerHL ()));
}

// ---
_INST_IMPL (FZ80::EX_X)
{
	assert (parameters ().size () == 1);

	return (
		executeWith (registerBC (), registerBCP ()) ||
		executeWith (registerDE (), registerDEP ()) ||
		executeWith (registerHL (), registerHLP ()));
}

// ---
_INST_IMPL (FZ80::EX_SPWithHL)
{
	assert (parameters ().size () == 1);

	MCHEmul::UBytes o = 
		MCHEmul::UInt::fromUnsignedInt (memory () -> stack () -> position ()).values ();
	o.setMinLength (2, false /** on the left. */);
	memory () -> stack () -> setPosition ((unsigned int) ((registerH ().values ()[0].value () << 8) + registerL ()[0].value ()));
	registerH ().set (MCHEmul::UBytes ({ o [0] }));
	registerL ().set (MCHEmul::UBytes ({ o [1] }));

	return (true);
}

// ---
_INST_IMPL (FZ80::EX_SPWithIX)
{
	assert (parameters ().size () == 2);

	MCHEmul::UBytes o = 
		MCHEmul::UInt::fromUnsignedInt (memory () -> stack () -> position ()).values ();
	o.setMinLength (2, false /** on the left. */);
	memory () -> stack () -> setPosition ((unsigned int) ((registerIXH ().values ()[0].value () << 8) + registerIXH ()[0].value ()));
	registerIXH ().set (MCHEmul::UBytes ({ o [0] }));
	registerIXL ().set (MCHEmul::UBytes ({ o [1] }));

	return (true);
}

// ---
_INST_IMPL (FZ80::EX_SPWithIY)
{
	assert (parameters ().size () == 2);

	MCHEmul::UBytes o = 
		MCHEmul::UInt::fromUnsignedInt (memory () -> stack () -> position ()).values ();
	o.setMinLength (2, false /** on the left. */);
	memory () -> stack () -> setPosition ((unsigned int) ((registerIYH ().values ()[0].value () << 8) + registerIYH ()[0].value ()));
	registerIYH ().set (MCHEmul::UBytes ({ o [0] }));
	registerIYL ().set (MCHEmul::UBytes ({ o [1] }));

	return (true);
}

// ---
bool FZ80::InstBlock_General::executeMoveWith (int a)
{
	// The registers involved...
	MCHEmul::Register& rB		= registerB ();
	MCHEmul::Register& rC		= registerC ();
	MCHEmul::Register& rD		= registerD ();
	MCHEmul::Register& rE		= registerE ();
	MCHEmul::Register& rH		= registerH ();
	MCHEmul::Register& rL		= registerL ();
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	// The data of those registers, and the way they have to be managed...
	// Numbers and Registers from registers are big - endian...
	MCHEmul::UInt bcI	 (MCHEmul::UBytes ({ rB.values ()[0], rC.values ()[0] })); // Number of data to move... 
	MCHEmul::Address deA (MCHEmul::UBytes ({ rD.values ()[0], rE.values ()[0] })); // Destination...
	MCHEmul::Address hlA (MCHEmul::UBytes ({ rH.values ()[0], rL.values ()[0] })); // Target...

	// Move the content from (hlA) into (deA)
	memory () -> set (deA, memory () -> values (hlA, 1)); // 1 byte...
	// The positions target and destination are moved according to "a"
	deA += a; hlA += a; 
	// The number of elements to move is decremented into 1, and _bc= becomes true if data data is 0
	_bc0 = ((bcI -= 1).asUnsignedInt () == (unsigned int) 0); 

	st.setBitStatus (FZ80::CZ80::_NEGATIVEFLAG, false);
	st.setBitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG, bcI != 0);
	st.setBitStatus (FZ80::CZ80::_HALFCARRYFLAG, false);
	// The rest of the flags are not affected...
	
	// Restore the new content into the registers...
	rB.set ({ bcI.bytes ()[0] }); rC.set ( { bcI.bytes ()[1] });
	rD.set ({ deA.bytes ()[0] }); rE.set ( { deA.bytes ()[1] });
	rH.set ({ hlA.bytes ()[0] }); rL.set ( { hlA.bytes ()[1] });

	return (true);
}

// ---
bool FZ80::InstBlock_General::executeCompareWith (int a)
{
	// The registers involved...
	MCHEmul::Register& rB		= registerB ();
	MCHEmul::Register& rC		= registerC ();
	MCHEmul::Register& rH		= registerH ();
	MCHEmul::Register& rL		= registerL ();
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	// The data of those registers, and the way they have to be managed...
	// Numbers and Registers from registers are big - endian...
	MCHEmul::UInt bcI	 (MCHEmul::UBytes ({ rB.values ()[0], rC.values ()[0] })); // Number of data to move... 
	MCHEmul::Address hlA (MCHEmul::UBytes ({ rH.values ()[0], rL.values ()[0] })); // Target...

	// To compare the two registers a mathematical opertion is done...
	MCHEmul::UInt r = MCHEmul::UInt (registerA ().values () [0]).
		substract (MCHEmul::UInt (memory () -> value (hlA)), false);

	// The next position is moved according to "a"
	hlA += a; 
	// The number of elements to move is decremented into 1, 
	// and _bc= becomes true if data data is 0
	_bc0 = ((bcI -= 1).asUnsignedInt () == (unsigned int) 0); 

	st.setBitStatus (FZ80::CZ80::_NEGATIVEFLAG, true);
	st.setBitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG, bcI != 0);
	st.setBitStatus (FZ80::CZ80::_HALFCARRYFLAG, false); // TODO...
	st.setBitStatus (FZ80::CZ80::_ZEROFLAG, r == MCHEmul::UInt::_0);
	st.setBitStatus (FZ80::CZ80::_SIGNFLAG, r.negative ());
	// The rest of the flags are not affected...
	
	// Restore the new content into the registers...
	rB.set ({ bcI.bytes ()[0] }); rC.set ( { bcI.bytes ()[1] });
	rH.set ({ hlA.bytes ()[0] }); rL.set ( { hlA.bytes ()[1] });

	return (true);
}

// ---
_INST_IMPL (FZ80::LDI)
{
	assert (parameters ().size () == 2);

	return (executeMoveWith (1)); // move up...
}

// ---
_INST_IMPL (FZ80::LDIR)
{
	assert (parameters ().size () == 2);

	bool result = executeMoveWith (1); // move up...
	// The instruction only finish when reaches the limit...
	if (result && (_FINISH = _bc0) == false) 
		_additionalCycles = 5; // ...and if not, it costs 5 additional cycles always!

	return (result);
}

// ---
_INST_IMPL (FZ80::LDD)
{
	assert (parameters ().size () == 2);

	return (executeMoveWith (-1)); // move down...
}

// ---
_INST_IMPL (FZ80::LDDR)
{
	assert (parameters ().size () == 2);

	bool result = executeMoveWith (-1); // move down...
	// The instruction only finish when reaches the limit...
	if (result && (_FINISH = _bc0) == false) 
		_additionalCycles = 5; // ...and if not it costs 5 additional cycles always!

	return (result);
}

// ---
_INST_IMPL (FZ80::CPI)
{
	assert (parameters ().size () == 2);

	return (executeCompareWith (1)); // move up...
}

// ---
_INST_IMPL (FZ80::CPIR)
{
	assert (parameters ().size () == 2);

	bool result = executeCompareWith (1); // move up...
	// The instruction only finish when reaches the limit or A has been found!
	if (result && (_FINISH = (_bc0 || cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_ZEROFLAG)) == false)) 
		_additionalCycles = 5; // ...and if not, it costs 5 additional cycles always!

	return (result);
}

// ---
_INST_IMPL (FZ80::CPD)
{
	assert (parameters ().size () == 2);

	return (executeCompareWith (-1)); // move down...
}

// ---
_INST_IMPL (FZ80::CPDR)
{
	assert (parameters ().size () == 2);

	bool result = executeCompareWith (-1); // move up...
	// The instruction only finish when reaches the limit or A has been found!
	if (result && (_FINISH = (_bc0 || cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_ZEROFLAG)) == false)) 
		_additionalCycles = 5; // ...and if not, it costs 5 additional cycles always!

	return (result);
}
