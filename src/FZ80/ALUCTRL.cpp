#include <FZ80/ALUCTRL.hpp>

// ---
_INST_IMPL (FZ80::DAA)
{
	assert (parameters ().size () == 1);

	MCHEmul::Register& r = registerA ();
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	// Into an int to make the operations right...
	int aVal = (int) r.values ()[0].value ();
	bool c = st.bitStatus (CZ80::_CARRYFLAG);
	bool ac = aVal > 0x99;
	int add = ((st.bitStatus (CZ80::_HALFCARRYFLAG) || ((aVal & 0x0f) > 9)) ? 0x06 : 0x00) /** The low nibble. */ +
			  ((c || ac) ? 0x60 : 0x00) /** The high nibble. */;
	aVal += st.bitStatus (CZ80::_HALFCARRYFLAG) ? -add : add;
	aVal &= 0xff;
	MCHEmul::UByte aValC ((unsigned char) aVal);

	// How the flags are affected...
	st.setBitStatus (CZ80::_CARRYFLAG, c || ac);
	// The negative flag doesn't change...
	st.setBitStatus (CZ80::_PARITYOVERFLOWFLAG, (aValC.numberBitsOn () % 2) == 0);  // If it is an even number then it is set...
	st.setBitStatus (CZ80::_BIT3FLAG, aValC.bit (3)); // Undocumented...
	st.setBitStatus (CZ80::_HALFCARRYFLAG, MCHEmul::UByte (aValC ^ r.values ()[0]).bit (4));
	st.setBitStatus (CZ80::_BIT5FLAG, aValC.bit (5)); // Undocumented...
	st.setBitStatus (CZ80::_ZEROFLAG, aValC == 0x00);
	st.setBitStatus (CZ80::_SIGNFLAG, (aValC & 0x80) != 0x00);

	// Set back the value into the accumulator
	r.set ({ aValC });

	return (true);
}

// ---
_INST_IMPL (FZ80::CPL)
{
	assert (parameters ().size () == 1);

	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	MCHEmul::Register& rA = registerA ();
	rA.set ({ ~registerA ().values ()[0] });

	// How the flags are affected...
	// Carry flag is not affected
	st.setBitStatus (FZ80::CZ80::_NEGATIVEFLAG, true);  // Always!
	// Parity flag is not affected...
	st.setBitStatus (FZ80::CZ80::_BIT3FLAG, rA.values ().bit (3)); // Undocumented...
	st.setBitStatus (FZ80::CZ80::_HALFCARRYFLAG, true); // Always!
	st.setBitStatus (FZ80::CZ80::_BIT5FLAG, rA.values ().bit (5)); // Undocumented...
	// Sign and Zero flags are not affected...

	return (true);
}

// ---
_INST_IMPL (FZ80::NEG)
{
	assert (parameters ().size () == 2);

	MCHEmul::Register& r = registerA ();
	MCHEmul::UByte rA = r.values ()[0];
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	// The operation...
	MCHEmul::UInt rst  = MCHEmul::UInt (0).substract (MCHEmul::UInt (r.values ()[0]), true);
	MCHEmul::UInt rstH = MCHEmul::UInt (0).substract (MCHEmul::UInt (r.values ()[0] & 0x0f), true); // Just to calculate the half borrow!
	r.set (rst.values ()); // and stored back...

	// How the flags are affected...
	st.setBitStatus (CZ80::_CARRYFLAG, rA != MCHEmul::UByte::_0); // As defined...
	st.setBitStatus (CZ80::_NEGATIVEFLAG, true); // Always in substractions!
	st.setBitStatus (CZ80::_PARITYOVERFLOWFLAG, rA == MCHEmul::UByte::_80);
	st.setBitStatus (CZ80::_BIT3FLAG, rst [0].bit (3)); // Undocumented...
	st.setBitStatus (CZ80::_HALFCARRYFLAG, !rstH.carry ()); // When borrow from bit 3 to 4...
	st.setBitStatus (CZ80::_BIT5FLAG, rst [0].bit (5)); // Undocumented...
	st.setBitStatus (CZ80::_ZEROFLAG, rst == MCHEmul::UInt::_0);
	st.setBitStatus (CZ80::_SIGNFLAG, rst.negative ());

	return (true);
}

// ---
_INST_IMPL (FZ80::CCF)
{
	assert (parameters ().size () == 1);

	MCHEmul::Register& rA = registerA ();
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	bool pC = st.bitStatus (FZ80::CZ80::_CARRYFLAG);

	// How the flags are affected...
	st.setBitStatus (FZ80::CZ80::_CARRYFLAG, !pC);
	st.setBitStatus (FZ80::CZ80::_NEGATIVEFLAG, false);  // Always!
	// Parity / Overflow flag is not affected...
	st.setBitStatus (FZ80::CZ80::_BIT3FLAG, rA.values ()[0].bit (3)); // Undocumented. From register A
	st.setBitStatus (FZ80::CZ80::_HALFCARRYFLAG, pC); // Undocumented. Like before the operation...
	st.setBitStatus (FZ80::CZ80::_BIT5FLAG, rA.values ()[0].bit (5)); // Undocumented. From register A
	// Zero and Sign flag are not affected...

	return (true);
}

// ---
_INST_IMPL (FZ80::SCF)
{
	assert (parameters ().size () == 1);

	MCHEmul::Register& rA = registerA ();
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	// How the flags are affected...
	st.setBitStatus (FZ80::CZ80::_CARRYFLAG, true);
	st.setBitStatus (FZ80::CZ80::_NEGATIVEFLAG, false);  // Always!
	// Parity / Overflow flag is not affected...
	st.setBitStatus (FZ80::CZ80::_BIT3FLAG, rA.values ()[0].bit (3)); // Undocumented. From register A
	st.setBitStatus (FZ80::CZ80::_HALFCARRYFLAG, false); // Always!
	st.setBitStatus (FZ80::CZ80::_BIT5FLAG, rA.values ()[0].bit (5)); // Undocumented. From register A
	// Zero and Sign flag are not affected...

	return (true);
}

// ---
_INST_IMPL (FZ80::NOP)
{
	assert (parameters ().size () == 1);

	// Does nothing...

	return (true);
}

// ---
_INST_IMPL (FZ80::HALT)
{
	assert (parameters ().size () == 1);

	FZ80::CZ80* cpuZ80 = dynamic_cast <FZ80::CZ80*> (cpu ());
	assert (cpuZ80 != nullptr); // It shouldn't...but just in case!
	if (!cpuZ80 -> stopped ()) // First time?
	{ 
		cpuZ80 -> setStop (true, MCHEmul::InstructionDefined::_CYCLEALL, 
			cpu () -> clockCycles ()); // Stops for ever...
		cpuZ80 -> setHaltActive (); // ...and marks this special situation in the CPU....
	}

	// Until unblocked by a INT execution... (@see CZ80::requestInterrupt)
	_FINISH = !cpuZ80 -> haltActive (); 

	return (true);
}

// ---
_INST_IMPL (FZ80::DI)
{
	assert (parameters ().size () == 1);

	static_cast <FZ80::CZ80*> (cpu ()) -> setIFF1 (false);
	static_cast <FZ80::CZ80*> (cpu ()) -> setIFF2 (false);

	return (true);
}

// ---
_INST_IMPL (FZ80::EI)
{
	assert (parameters ().size () == 1);

	static_cast <FZ80::CZ80*> (cpu ()) -> setIFF1 (true);
	static_cast <FZ80::CZ80*> (cpu ()) -> setIFF2 (true);

	return (true);
}

// ---
_INST_IMPL (FZ80::IM_0)
{
	assert (parameters ().size () == 2);

	static_cast <FZ80::CZ80*> (cpu ()) -> setINTMode (0);

	return (true);
}

// ---
_INST_IMPL (FZ80::IM_1)
{
	assert (parameters ().size () == 2);

	static_cast <FZ80::CZ80*> (cpu ()) -> setINTMode (1);

	return (true);
}

// ---
_INST_IMPL (FZ80::IM_2)
{
	assert (parameters ().size () == 2);

	static_cast <FZ80::CZ80*> (cpu ()) -> setINTMode (2);

	return (true);
}
