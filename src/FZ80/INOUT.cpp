#include <FZ80/INOUT.hpp>

// ---
void FZ80::IN_General::completePortRead
	(const MCHEmul::UByte& v, const FZ80::Z80Port::Access::ReadContext& rC)
{
	// The destination was selected before the port read.
	// In particular, reading into B or C must not change the port address.
	if (rC._destination != nullptr)
		rC._destination -> set ({ v });

	// IN A,(n) preserves flags; register and flags-only IN (C) update them.
	if (rC._affectFlags)
		affectFlags (v);
}

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
void FZ80::INBlock_General::completePortRead
	(const MCHEmul::UByte& v, const FZ80::Z80Port::Access::ReadContext& rC)
{
	// HL may already have advanced during preparation.
	// Store the input byte at the original, captured destination.
	memory () -> set (MCHEmul::Address (2, rC._memoryAddress), v);

	// Preserve the existing block-input flag calculation using the saved
	// decremented B and direction-adjusted C, not the live CPU registers.
	unsigned short sum =
		(unsigned short) rC._cAdjusted + (unsigned short) v.value ();
	bool carry = sum > 0x00ff;
	unsigned char parity =
		(unsigned char) ((sum & 0x0007) ^ rC._bAfter);

	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	st.setBitStatus (FZ80::CZ80::_CARRYFLAG, carry);
	st.setBitStatus (FZ80::CZ80::_NEGATIVEFLAG, v.bit (7));
	st.setBitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG,
		(MCHEmul::UByte (parity).numberBitsOn () % 2) == 0);
	st.setBitStatus (FZ80::CZ80::_BIT3FLAG,
		MCHEmul::UByte (rC._bAfter).bit (3));
	st.setBitStatus (FZ80::CZ80::_HALFCARRYFLAG, carry);
	st.setBitStatus (FZ80::CZ80::_BIT5FLAG,
		MCHEmul::UByte (rC._bAfter).bit (5));
	st.setBitStatus (FZ80::CZ80::_ZEROFLAG, rC._bAfter == 0);
	st.setBitStatus (FZ80::CZ80::_SIGNFLAG,
		MCHEmul::UByte (rC._bAfter).bit (7));
}

// ---
bool FZ80::INBlock_General::executeWith (int a)
{
	assert (a == 1 || a == -1);

	MCHEmul::Register& rB = registerB ();
	unsigned char rBA = rB.values ()[0].value ();
	unsigned char rCA = registerC ().values ()[0].value ();
	MCHEmul::Address hlA = addressHL ();

	// INI/IND present the original BC before decrementing B.
	_lastExecutionData._INOUTAddress = addressBC ();
	unsigned short ab =
		(unsigned short) _lastExecutionData._INOUTAddress.value ();
	prepareIOAccess (ab, rCA, _IOSTARTCYCLE, false);
	setIOAccessClockCycle (_IOACCESSCYCLE);

	// Save everything completion needs before advancing the CPU registers.
	// The byte and its dependent flags/memory write are produced only on release.
	Z80Port::Access access;
	access._type = Z80Port::Access::Type::_READ;
	access._clockCycle = IOAccessClockCycle ();
	access._address = ab;
	access._instruction = this;
	access._readContext._memoryAddress = (unsigned short) hlA.value ();
	access._readContext._bAfter = (unsigned char) (rBA - 1);
	access._readContext._cAdjusted = (unsigned char) (rCA + a);
	if (!static_cast <CZ80*> (cpu ()) -> schedulePortAccess (access))
		return (false);

	static_cast <FZ80::CZ80*> (cpu ()) -> setRWInternalRegister
		((unsigned char) ((ab + 1) >> 8));

	// Keep the final memory-bus address available to machine-level INT sampling,
	// even though completePortRead will perform the actual memory write later.
	_lastExecutionData._INOUTAddress = hlA;
	hlA = (a > 0) ? (hlA + 1) : (hlA - 1);
	rBA = access._readContext._bAfter;

	// Repetition and its additional cycles depend on B, not on the input byte.
	_b0 = rBA == 0;
	rB.set ({ rBA });
	registerH ().set ({ hlA.bytes ()[0] }); registerL ().set ({ hlA.bytes ()[1] });

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
		addAdditionalClockCycles (5); // ...and if not, it costs 5 additional cycles always!

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
		addAdditionalClockCycles (5); // ...and if not it costs 5 additional cycles always!

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
	unsigned char rBA			= rB.values ()[0].value ();
	MCHEmul::Register& rC		= registerC ();
	unsigned char rCA			= rC.values ()[0].value ();
	MCHEmul::Register& rH		= registerH ();
	MCHEmul::Register& rL		= registerL ();
	unsigned short rLA			= rL.values ()[0].value ();

	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	// OUTI/OUTD read memory before starting the I/O cycle.
	MCHEmul::Address hlA = addressHL (); // Target...
	MCHEmul::UByte vR = memory () -> value (hlA);
	// The number of elements to move is decremented into 1, 
	// and _b becomes true if data data is 0
	_b0 = (--rBA) == 0; 
	// The value of the component BC is pushed into the address bus...
	_lastExecutionData._INOUTAddress = MCHEmul::Address ({ rBA, rCA }, true);
	unsigned short ab =
		(unsigned short) _lastExecutionData._INOUTAddress.value ();
	// The internal register RW used later in BIT instructions...
	static_cast <FZ80::CZ80*> (cpu ()) -> setRWInternalRegister 
		((unsigned char) ((ab + 1) >> 8));
	prepareIOAccess (ab, rCA, _IOSTARTCYCLE);
	setIOAccessClockCycle (_IOACCESSCYCLE);

	// OUTI/OUTD use decremented B in the port address. Capture the memory byte
	// now; releasing the output must not reread memory after HL has advanced.
	Z80Port::Access access;
	access._type = Z80Port::Access::Type::_WRITE;
	access._clockCycle = IOAccessClockCycle ();
	access._address = ab;
	access._value = vR;
	if (!static_cast <CZ80*> (cpu ()) -> schedulePortAccess (access))
		return (false);

	// Moves to the next position 
	// or the previous (depending on the value of a...
	hlA = (a > 0) ? (hlA + 1) : (hlA - 1);
	rLA = (a > 0) ? (rLA + 1) : (rLA - 1);

	// How the flags are affected...
	// http://www.z80.info/zip/z80-documented.pdf (section 4.3)
	bool ec = ((rLA & 0x00ff) + (unsigned short) vR.value ()) > 0x00ff;
	unsigned char pc = (unsigned char) ((((rLA & 0x00ff) + 
		(unsigned short) vR.value ()) & 0x0007) ^ (unsigned short) rBA);
	st.setBitStatus (FZ80::CZ80::_CARRYFLAG, ec);
	st.setBitStatus (FZ80::CZ80::_NEGATIVEFLAG, vR.bit (7));
	st.setBitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG, (MCHEmul::UByte (pc).numberBitsOn () % 2) == 0x00);
	st.setBitStatus (FZ80::CZ80::_BIT3FLAG, MCHEmul::UByte (rBA).bit (3));
	st.setBitStatus (FZ80::CZ80::_HALFCARRYFLAG, ec);
	st.setBitStatus (FZ80::CZ80::_BIT5FLAG, MCHEmul::UByte (rBA).bit (5));
	st.setBitStatus (FZ80::CZ80::_ZEROFLAG, rBA == 0);
	st.setBitStatus (FZ80::CZ80::_SIGNFLAG, MCHEmul::UByte (rBA).bit (7));
	
	// Restore the new content into the registers...
	rB.set ({ rBA });
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
		addAdditionalClockCycles (5); // ...and if not, it costs 5 additional cycles always!

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
		addAdditionalClockCycles (5); // ...and if not it costs 5 additional cycles always!

	return (result);
}
