#include <FZ80/LD.hpp>

// ---
_INST_IMPL (FZ80::LD_A)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA (), parameters ()[1].value ()));
}

// ---
_INST_IMPL (FZ80::U1LD_A)
{
	assert (parameters ().size () == 3);

	return (executeWith (registerA (), parameters ()[2].value ()));
}

// ---
_INST_IMPL (FZ80::U2LD_A)
{
	assert (parameters ().size () == 3);

	return (executeWith (registerA (), parameters ()[2].value ()));
}

// ---
_INST_IMPL (FZ80::LD_AFromA)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), valueRegisterA ()));
}

// ---
_INST_IMPL (FZ80::U1LD_AFromA)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA (), valueRegisterA ()));
}

// ---
_INST_IMPL (FZ80::U2LD_AFromA)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA (), valueRegisterA ()));
}

// ---
_INST_IMPL (FZ80::LD_AFromB)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), valueRegisterB ()));
}

// ---
_INST_IMPL (FZ80::U1LD_AFromB)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA (), valueRegisterB ()));
}

// ---
_INST_IMPL (FZ80::U2LD_AFromB)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA (), valueRegisterB ()));
}

// ---
_INST_IMPL (FZ80::LD_AFromC)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), valueRegisterC ()));
}

// ---
_INST_IMPL (FZ80::U1LD_AFromC)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA (), valueRegisterC ()));
}

// ---
_INST_IMPL (FZ80::U2LD_AFromC)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA (), valueRegisterC ()));
}

// ---
_INST_IMPL (FZ80::LD_AFromD)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), valueRegisterD ()));
}

// ---
_INST_IMPL (FZ80::U1LD_AFromD)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA (), valueRegisterD ()));
}

// ---
_INST_IMPL (FZ80::U2LD_AFromD)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA (), valueRegisterD ()));
}

// ---
_INST_IMPL (FZ80::LD_AFromE)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), valueRegisterE ()));
}

// ---
_INST_IMPL (FZ80::U1LD_AFromE)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA (), valueRegisterE ()));
}

// ---
_INST_IMPL (FZ80::U2LD_AFromE)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA (), valueRegisterE ()));
}

// ---
_INST_IMPL (FZ80::LD_AFromH)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), valueRegisterH ()));
}

// ---
_INST_IMPL (FZ80::LD_AFromL)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), valueRegisterL ()));
}

// ---
_INST_IMPL (FZ80::LD_AFromI)
{
	assert (parameters ().size () == 2);

	MCHEmul::UByte v;
	bool result = executeWith (registerA (), v = valueRegisterI ());
	if (result)
	{
		MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

		// How the flags are affected...
		// The carry flag is not affected!
		st.setBitStatus (FZ80::CZ80::_NEGATIVEFLAG, false);
		st.setBitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG, static_cast <FZ80::CZ80*> (cpu ()) -> IFF2 ());
		st.setBitStatus (FZ80::CZ80::_BIT3FLAG, v.bit (3)); // Undocumented...
		st.setBitStatus (FZ80::CZ80::_HALFCARRYFLAG, false);
		st.setBitStatus (FZ80::CZ80::_BIT5FLAG, v.bit (5)); // Undocumented...
		st.setBitStatus (FZ80::CZ80::_ZEROFLAG, v == MCHEmul::UByte::_0);
		st.setBitStatus (FZ80::CZ80::_SIGNFLAG, v.bit (7));
	}

	return (result);
}

// ---
_INST_IMPL (FZ80::LD_AFromR)
{
	assert (parameters ().size () == 2);

	MCHEmul::UByte v;
	bool result = executeWith (registerA (), v = valueRegisterR ());
	if (result)
	{
		MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

		// How the flags are affected...
		// The carry flag is not affected!
		st.setBitStatus (FZ80::CZ80::_NEGATIVEFLAG, false);
		st.setBitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG, static_cast <FZ80::CZ80*> (cpu ()) -> IFF2 ());
		st.setBitStatus (FZ80::CZ80::_BIT3FLAG, v.bit (3));
		st.setBitStatus (FZ80::CZ80::_HALFCARRYFLAG, false);
		st.setBitStatus (FZ80::CZ80::_BIT5FLAG, v.bit (5));
		st.setBitStatus (FZ80::CZ80::_ZEROFLAG, v == MCHEmul::UByte::_0);
		st.setBitStatus (FZ80::CZ80::_SIGNFLAG, v.bit (7));
	}

	return (result);
}

// ---
_INST_IMPL (FZ80::LD_AFromAddressHL)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), valueAddressHL ()));
}

// ---
_INST_IMPL (FZ80::LD_AFromAddressBC)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), valueAddressBC ()));
}

// ---
_INST_IMPL (FZ80::LD_AFromAddressDE)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), valueAddressDE ()));
}

// ---
_INST_IMPL (FZ80::LD_AFromAddressIndexIX)
{
	assert (parameters ().size () == 3);

	char n = (char) parameters ()[2].value ();
	static_cast <FZ80::CZ80*> (_lastExecutionData._cpu) -> 
		setRWInternalRegister ((n >= 0) 
			? (MCHEmul::Address (valueRegisterIX (), true) + (size_t) n)[0]
			: (MCHEmul::Address (valueRegisterIX (), true) - (size_t) (-n))[0]);

	return (executeWith (registerA (), valueAddressIX (n)));
}

// ---
_INST_IMPL (FZ80::LD_AFromAddressIndexIY)
{
	assert (parameters ().size () == 3);

	char n = (char) parameters ()[2].value ();
	static_cast <FZ80::CZ80*> (_lastExecutionData._cpu) -> 
		setRWInternalRegister ((n >= 0) 
			? (MCHEmul::Address (valueRegisterIY (), true) + (size_t) n)[0]
			: (MCHEmul::Address (valueRegisterIY (), true) - (size_t) (-n))[0]);

	return (executeWith (registerA (), valueAddressIY (n)));
}

// ---
_INST_IMPL (FZ80::LD_AFromAddress)
{
	assert (parameters ().size () == 3);

	/** Addresses in memory are kept little - endian. */
	const MCHEmul::UByte& dt = memory () -> value
		(_lastExecutionData._INOUTAddress = MCHEmul::Address 
			({ parameters ()[1].value (), parameters ()[2].value () }, false /** little - endian */));
	_lastExecutionData._INOUTData = MCHEmul::UBytes ({ dt });
	return (executeWith (registerA (), dt));
}

// ---
_INST_IMPL (FZ80::LD_AFromIXH)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA (), valueRegisterIXH ()));
}

// ---
_INST_IMPL (FZ80::LD_AFromIXL)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA (), valueRegisterIXL ()));
}

// ---
_INST_IMPL (FZ80::LD_AFromIYH)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA (), valueRegisterIYH ()));
}

// ---
_INST_IMPL (FZ80::LD_AFromIYL)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA (), valueRegisterIYL ()));
}

// ---
_INST_IMPL (FZ80::LD_B)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB (), parameters ()[1].value ()));
}

// ---
_INST_IMPL (FZ80::U1LD_B)
{
	assert (parameters ().size () == 3);

	return (executeWith (registerB (), parameters ()[2].value ()));
}

// ---
_INST_IMPL (FZ80::U2LD_B)
{
	assert (parameters ().size () == 3);

	return (executeWith (registerB (), parameters ()[2].value ()));
}

// ---
_INST_IMPL (FZ80::LD_BFromA)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerB (), valueRegisterA ()));
}

// ---
_INST_IMPL (FZ80::U1LD_BFromA)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB (), valueRegisterA ()));
}

// ---
_INST_IMPL (FZ80::U2LD_BFromA)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB (), valueRegisterA ()));
}

// ---
_INST_IMPL (FZ80::LD_BFromB)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerB (), valueRegisterB ()));
}

// ---
_INST_IMPL (FZ80::U1LD_BFromB)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB (), valueRegisterB ()));
}

// ---
_INST_IMPL (FZ80::U2LD_BFromB)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB (), valueRegisterB ()));
}

// ---
_INST_IMPL (FZ80::LD_BFromC)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerB (), valueRegisterC ()));
}

// ---
_INST_IMPL (FZ80::U1LD_BFromC)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB (), valueRegisterC ()));
}

// ---
_INST_IMPL (FZ80::U2LD_BFromC)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB (), valueRegisterC ()));
}

// ---
_INST_IMPL (FZ80::LD_BFromD)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerB (), valueRegisterD ()));
}

// ---
_INST_IMPL (FZ80::U1LD_BFromD)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB (), valueRegisterD ()));
}

// ---
_INST_IMPL (FZ80::U2LD_BFromD)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB (), valueRegisterD ()));
}

// ---
_INST_IMPL (FZ80::LD_BFromE)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerB (), valueRegisterE ()));
}

// ---
_INST_IMPL (FZ80::U1LD_BFromE)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB (), valueRegisterE ()));
}

// ---
_INST_IMPL (FZ80::U2LD_BFromE)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB (), valueRegisterE ()));
}

// ---
_INST_IMPL (FZ80::LD_BFromH)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerB (), valueRegisterH ()));
}

// ---
_INST_IMPL (FZ80::LD_BFromL)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerB (), valueRegisterL ()));
}

// ---
_INST_IMPL (FZ80::LD_BFromAddressHL)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerB (), valueAddressHL ()));
}

// ---
_INST_IMPL (FZ80::LD_BFromAddressIndexIX)
{
	assert (parameters ().size () == 3);

	char n = (char) parameters ()[2].value ();
	static_cast <FZ80::CZ80*> (_lastExecutionData._cpu) -> 
		setRWInternalRegister ((n >= 0) 
			? (MCHEmul::Address (valueRegisterIX (), true) + (size_t) n)[0]
			: (MCHEmul::Address (valueRegisterIX (), true) - (size_t) (-n))[0]);

	return (executeWith (registerB (), valueAddressIX (n)));
}

// ---
_INST_IMPL (FZ80::LD_BFromAddressIndexIY)
{
	assert (parameters ().size () == 3);

	char n = (char) parameters ()[2].value ();
	static_cast <FZ80::CZ80*> (_lastExecutionData._cpu) -> 
		setRWInternalRegister ((n >= 0) 
			? (MCHEmul::Address (valueRegisterIY (), true) + (size_t) n)[0]
			: (MCHEmul::Address (valueRegisterIY (), true) - (size_t) (-n))[0]);

	return (executeWith (registerB (), valueAddressIY (n)));
}

// ---
_INST_IMPL (FZ80::LD_BFromIXH)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB (), valueRegisterIXH ()));
}

// ---
_INST_IMPL (FZ80::LD_BFromIXL)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB (), valueRegisterIXL ()));
}

// ---
_INST_IMPL (FZ80::LD_BFromIYH)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB (), valueRegisterIYH ()));
}

// ---
_INST_IMPL (FZ80::LD_BFromIYL)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB (), valueRegisterIYL ()));
}

// ---
_INST_IMPL (FZ80::LD_C)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC (), parameters ()[1].value ()));
}

// ---
_INST_IMPL (FZ80::U1LD_C)
{
	assert (parameters ().size () == 3);

	return (executeWith (registerC (), parameters ()[2].value ()));
}

// ---
_INST_IMPL (FZ80::U2LD_C)
{
	assert (parameters ().size () == 3);

	return (executeWith (registerC (), parameters ()[2].value ()));
}

// ---
_INST_IMPL (FZ80::LD_CFromA)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerC (), valueRegisterA ()));
}

// ---
_INST_IMPL (FZ80::U1LD_CFromA)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC (), valueRegisterA ()));
}

// ---
_INST_IMPL (FZ80::U2LD_CFromA)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC (), valueRegisterA ()));
}

// ---
_INST_IMPL (FZ80::LD_CFromB)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerC (), valueRegisterB ()));
}

// ---
_INST_IMPL (FZ80::U1LD_CFromB)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC (), valueRegisterB ()));
}

// ---
_INST_IMPL (FZ80::U2LD_CFromB)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC (), valueRegisterB ()));
}

// ---
_INST_IMPL (FZ80::LD_CFromC)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerC (), valueRegisterC ()));
}

// ---
_INST_IMPL (FZ80::U1LD_CFromC)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC (), valueRegisterC ()));
}

// ---
_INST_IMPL (FZ80::U2LD_CFromC)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC (), valueRegisterC ()));
}

// ---
_INST_IMPL (FZ80::LD_CFromD)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerC (), valueRegisterD ()));
}

// ---
_INST_IMPL (FZ80::U1LD_CFromD)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC (), valueRegisterD ()));
}

// ---
_INST_IMPL (FZ80::U2LD_CFromD)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC (), valueRegisterD ()));
}

// ---
_INST_IMPL (FZ80::LD_CFromE)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerC (), valueRegisterE ()));
}

// ---
_INST_IMPL (FZ80::U1LD_CFromE)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC (), valueRegisterE ()));
}

// ---
_INST_IMPL (FZ80::U2LD_CFromE)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC (), valueRegisterE ()));
}

// ---
_INST_IMPL (FZ80::LD_CFromH)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerC (), valueRegisterH ()));
}

// ---
_INST_IMPL (FZ80::LD_CFromL)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerC (), valueRegisterL ()));
}

// ---
_INST_IMPL (FZ80::LD_CFromAddressHL)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerC (), valueAddressHL ()));
}

// ---
_INST_IMPL (FZ80::LD_CFromAddressIndexIX)
{
	assert (parameters ().size () == 3);

	char n = (char) parameters ()[2].value ();
	static_cast <FZ80::CZ80*> (_lastExecutionData._cpu) -> 
		setRWInternalRegister ((n >= 0) 
			? (MCHEmul::Address (valueRegisterIX (), true) + (size_t) n)[0]
			: (MCHEmul::Address (valueRegisterIX (), true) - (size_t) (-n))[0]);

	return (executeWith (registerC (), valueAddressIX (n)));
}

// ---
_INST_IMPL (FZ80::LD_CFromAddressIndexIY)
{
	assert (parameters ().size () == 3);

	char n = (char) parameters ()[2].value ();
	static_cast <FZ80::CZ80*> (_lastExecutionData._cpu) -> 
		setRWInternalRegister ((n >= 0) 
			? (MCHEmul::Address (valueRegisterIY (), true) + (size_t) n)[0]
			: (MCHEmul::Address (valueRegisterIY (), true) - (size_t) (-n))[0]);

	return (executeWith (registerC (), valueAddressIY (n)));
}

// ---
_INST_IMPL (FZ80::LD_CFromIXH)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC (), valueRegisterIXH ()));
}

// ---
_INST_IMPL (FZ80::LD_CFromIXL)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC (), valueRegisterIXL ()));
}

// ---
_INST_IMPL (FZ80::LD_CFromIYH)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC (), valueRegisterIYH ()));
}

// ---
_INST_IMPL (FZ80::LD_CFromIYL)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC (), valueRegisterIYL ()));
}

// ---
_INST_IMPL (FZ80::LD_D)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD (), parameters ()[1].value ()));
}

// ---
_INST_IMPL (FZ80::U1LD_D)
{
	assert (parameters ().size () == 3);

	return (executeWith (registerD (), parameters ()[2].value ()));
}

// ---
_INST_IMPL (FZ80::U2LD_D)
{
	assert (parameters ().size () == 3);

	return (executeWith (registerD (), parameters ()[2].value ()));
}

// ---
_INST_IMPL (FZ80::LD_DFromA)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerD (), valueRegisterA ()));
}

// ---
_INST_IMPL (FZ80::U1LD_DFromA)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD (), valueRegisterA ()));
}

// ---
_INST_IMPL (FZ80::U2LD_DFromA)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD (), valueRegisterA ()));
}

// ---
_INST_IMPL (FZ80::LD_DFromB)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerD (), valueRegisterB ()));
}

// ---
_INST_IMPL (FZ80::U1LD_DFromB)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD (), valueRegisterB ()));
}

// ---
_INST_IMPL (FZ80::U2LD_DFromB)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD (), valueRegisterB ()));
}

// ---
_INST_IMPL (FZ80::LD_DFromC)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerD (), valueRegisterC ()));
}

// ---
_INST_IMPL (FZ80::U1LD_DFromC)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD (), valueRegisterC ()));
}

// ---
_INST_IMPL (FZ80::U2LD_DFromC)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD (), valueRegisterC ()));
}

// ---
_INST_IMPL (FZ80::LD_DFromD)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerD (), valueRegisterD ()));
}

// ---
_INST_IMPL (FZ80::U1LD_DFromD)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD (), valueRegisterD ()));
}

// ---
_INST_IMPL (FZ80::U2LD_DFromD)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD (), valueRegisterD ()));
}

// ---
_INST_IMPL (FZ80::LD_DFromE)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerD (), valueRegisterE ()));
}

// ---
_INST_IMPL (FZ80::U1LD_DFromE)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD (), valueRegisterE ()));
}

// ---
_INST_IMPL (FZ80::U2LD_DFromE)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD (), valueRegisterE ()));
}

// ---
_INST_IMPL (FZ80::LD_DFromH)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerD (), valueRegisterH ()));
}

// ---
_INST_IMPL (FZ80::LD_DFromL)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerD (), valueRegisterL ()));
}

// ---
_INST_IMPL (FZ80::LD_DFromAddressHL)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerD (), valueAddressHL ()));
}

// ---
_INST_IMPL (FZ80::LD_DFromAddressIndexIX)
{
	assert (parameters ().size () == 3);

	char n = (char) parameters ()[2].value ();
	static_cast <FZ80::CZ80*> (_lastExecutionData._cpu) -> 
		setRWInternalRegister ((n >= 0) 
			? (MCHEmul::Address (valueRegisterIX (), true) + (size_t) n)[0]
			: (MCHEmul::Address (valueRegisterIX (), true) - (size_t) (-n))[0]);

	return (executeWith (registerD (), valueAddressIX (n)));
}

// ---
_INST_IMPL (FZ80::LD_DFromAddressIndexIY)
{
	assert (parameters ().size () == 3);

	char n = (char) parameters ()[2].value ();
	static_cast <FZ80::CZ80*> (_lastExecutionData._cpu) -> 
		setRWInternalRegister ((n >= 0) 
			? (MCHEmul::Address (valueRegisterIY (), true) + (size_t) n)[0]
			: (MCHEmul::Address (valueRegisterIY (), true) - (size_t) (-n))[0]);

	return (executeWith (registerD (), valueAddressIY (n)));
}

// ---
_INST_IMPL (FZ80::LD_DFromIXH)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD (), valueRegisterIXH ()));
}

// ---
_INST_IMPL (FZ80::LD_DFromIXL)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD (), valueRegisterIXL ()));
}

// ---
_INST_IMPL (FZ80::LD_DFromIYH)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD (), valueRegisterIYH ()));
}

// ---
_INST_IMPL (FZ80::LD_DFromIYL)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD (), valueRegisterIYL ()));
}

// ---
_INST_IMPL (FZ80::LD_E)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE (), parameters ()[1].value ()));
}

// ---
_INST_IMPL (FZ80::U1LD_E)
{
	assert (parameters ().size () == 3);

	return (executeWith (registerE (), parameters ()[2].value ()));
}

// ---
_INST_IMPL (FZ80::U2LD_E)
{
	assert (parameters ().size () == 3);

	return (executeWith (registerE (), parameters ()[2].value ()));
}

// ---
_INST_IMPL (FZ80::LD_EFromA)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerE (), valueRegisterA ()));
}

// ---
_INST_IMPL (FZ80::U1LD_EFromA)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE (), valueRegisterA ()));
}

// ---
_INST_IMPL (FZ80::U2LD_EFromA)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE (), valueRegisterA ()));
}

// ---
_INST_IMPL (FZ80::LD_EFromB)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerE (), valueRegisterB ()));
}

// ---
_INST_IMPL (FZ80::U1LD_EFromB)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE (), valueRegisterB ()));
}

// ---
_INST_IMPL (FZ80::U2LD_EFromB)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE (), valueRegisterB ()));
}

// ---
_INST_IMPL (FZ80::LD_EFromC)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerE (), valueRegisterC ()));
}

// ---
_INST_IMPL (FZ80::U1LD_EFromC)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE (), valueRegisterC ()));
}

// ---
_INST_IMPL (FZ80::U2LD_EFromC)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE (), valueRegisterC ()));
}

// ---
_INST_IMPL (FZ80::LD_EFromD)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerE (), valueRegisterD ()));
}

// ---
_INST_IMPL (FZ80::U1LD_EFromD)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE (), valueRegisterD ()));
}

// ---
_INST_IMPL (FZ80::U2LD_EFromD)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE (), valueRegisterD ()));
}

// ---
_INST_IMPL (FZ80::LD_EFromE)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerE (), valueRegisterE ()));
}

// ---
_INST_IMPL (FZ80::U1LD_EFromE)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE (), valueRegisterE ()));
}

// ---
_INST_IMPL (FZ80::U2LD_EFromE)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE (), valueRegisterE ()));
}

// ---
_INST_IMPL (FZ80::LD_EFromH)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerE (), valueRegisterH ()));
}

// ---
_INST_IMPL (FZ80::LD_EFromL)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerE (), valueRegisterL ()));
}

// ---
_INST_IMPL (FZ80::LD_EFromAddressHL)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerE (), valueAddressHL ()));
}

// ---
_INST_IMPL (FZ80::LD_EFromAddressIndexIX)
{
	assert (parameters ().size () == 3);

	char n = (char) parameters ()[2].value ();
	static_cast <FZ80::CZ80*> (_lastExecutionData._cpu) -> 
		setRWInternalRegister ((n >= 0) 
			? (MCHEmul::Address (valueRegisterIX (), true) + (size_t) n)[0]
			: (MCHEmul::Address (valueRegisterIX (), true) - (size_t) (-n))[0]);

	return (executeWith (registerE (), valueAddressIX (n)));
}

// ---
_INST_IMPL (FZ80::LD_EFromAddressIndexIY)
{
	assert (parameters ().size () == 3);

	char n = (char) parameters ()[2].value ();
	static_cast <FZ80::CZ80*> (_lastExecutionData._cpu) -> 
		setRWInternalRegister ((n >= 0) 
			? (MCHEmul::Address (valueRegisterIY (), true) + (size_t) n)[0]
			: (MCHEmul::Address (valueRegisterIY (), true) - (size_t) (-n))[0]);

	return (executeWith (registerE (), valueAddressIY (n)));
}

// ---
_INST_IMPL (FZ80::LD_EFromIXH)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE (), valueRegisterIXH ()));
}

// ---
_INST_IMPL (FZ80::LD_EFromIXL)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE (), valueRegisterIXL ()));
}

// ---
_INST_IMPL (FZ80::LD_EFromIYH)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE (), valueRegisterIYH ()));
}

// ---
_INST_IMPL (FZ80::LD_EFromIYL)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE (), valueRegisterIYL ()));
}

// ---
_INST_IMPL (FZ80::LD_H)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerH (), parameters ()[1].value ()));
}

// ---
_INST_IMPL (FZ80::LD_HFromA)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerH (), valueRegisterA ()));
}

// ---
_INST_IMPL (FZ80::LD_HFromB)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerH (), valueRegisterB ()));
}

// ---
_INST_IMPL (FZ80::LD_HFromC)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerH (), valueRegisterC ()));
}

// ---
_INST_IMPL (FZ80::LD_HFromD)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerH (), valueRegisterD ()));
}

// ---
_INST_IMPL (FZ80::LD_HFromE)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerH (), valueRegisterE ()));
}

// ---
_INST_IMPL (FZ80::LD_HFromH)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerH (), valueRegisterH ()));
}

// ---
_INST_IMPL (FZ80::LD_HFromL)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerH (), valueRegisterL ()));
}

// ---
_INST_IMPL (FZ80::LD_HFromAddressHL)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerH (), valueAddressHL ()));
}

// ---
_INST_IMPL (FZ80::LD_HFromAddressIndexIX)
{
	assert (parameters ().size () == 3);

	char n = (char) parameters ()[2].value ();
	static_cast <FZ80::CZ80*> (_lastExecutionData._cpu) -> 
		setRWInternalRegister ((n >= 0) 
			? (MCHEmul::Address (valueRegisterIX (), true) + (size_t) n)[0]
			: (MCHEmul::Address (valueRegisterIX (), true) - (size_t) (-n))[0]);

	return (executeWith (registerH (), valueAddressIX (n)));
}

// ---
_INST_IMPL (FZ80::LD_HFromAddressIndexIY)
{
	assert (parameters ().size () == 3);

	char n = (char) parameters ()[2].value ();
	static_cast <FZ80::CZ80*> (_lastExecutionData._cpu) -> 
		setRWInternalRegister ((n >= 0) 
			? (MCHEmul::Address (valueRegisterIY (), true) + (size_t) n)[0]
			: (MCHEmul::Address (valueRegisterIY (), true) - (size_t) (-n))[0]);

	return (executeWith (registerH (), valueAddressIY (n)));
}

// ---
_INST_IMPL (FZ80::LD_L)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerL (), parameters ()[1].value ()));
}

// ---
_INST_IMPL (FZ80::LD_LFromA)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerL (), valueRegisterA ()));
}

// ---
_INST_IMPL (FZ80::LD_LFromB)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerL (), valueRegisterB ()));
}

// ---
_INST_IMPL (FZ80::LD_LFromC)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerL (), valueRegisterC ()));
}

// ---
_INST_IMPL (FZ80::LD_LFromD)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerL (), valueRegisterD ()));
}

// ---
_INST_IMPL (FZ80::LD_LFromE)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerL (), valueRegisterE ()));
}

// ---
_INST_IMPL (FZ80::LD_LFromH)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerL (), valueRegisterH ()));
}

// ---
_INST_IMPL (FZ80::LD_LFromL)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerL (), valueRegisterL ()));
}

// ---
_INST_IMPL (FZ80::LD_LFromAddressHL)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerL (), valueAddressHL ()));
}

// ---
_INST_IMPL (FZ80::LD_LFromAddressIndexIX)
{
	assert (parameters ().size () == 3);

	char n = (char) parameters ()[2].value ();
	static_cast <FZ80::CZ80*> (_lastExecutionData._cpu) -> 
		setRWInternalRegister ((n >= 0) 
			? (MCHEmul::Address (valueRegisterIX (), true) + (size_t) n)[0]
			: (MCHEmul::Address (valueRegisterIX (), true) - (size_t) (-n))[0]);

	return (executeWith (registerL (), valueAddressIX (n)));
}

// ---
_INST_IMPL (FZ80::LD_LFromAddressIndexIY)
{
	assert (parameters ().size () == 3);

	char n = (char) parameters ()[2].value ();
	static_cast <FZ80::CZ80*> (_lastExecutionData._cpu) -> 
		setRWInternalRegister ((n >= 0) 
			? (MCHEmul::Address (valueRegisterIY (), true) + (size_t) n)[0]
			: (MCHEmul::Address (valueRegisterIY (), true) - (size_t) (-n))[0]);

	return (executeWith (registerL (), valueAddressIY (n)));
}

// ---
_INST_IMPL (FZ80::LD_IndirectHL)
{
	assert (parameters ().size () == 2);

	return (executeWith (addressHL (), parameters ()[1].value ()));
}

// ---
_INST_IMPL (FZ80::LD_IndirectHLFromA)
{
	assert (parameters ().size () == 1);

	return (executeWith (addressHL (), valueRegisterA ()));
}

// ---
_INST_IMPL (FZ80::LD_IndirectHLFromB)
{
	assert (parameters ().size () == 1);

	return (executeWith (addressHL (), valueRegisterB ()));
}

// ---
_INST_IMPL (FZ80::LD_IndirectHLFromC)
{
	assert (parameters ().size () == 1);

	return (executeWith (addressHL (), valueRegisterC ()));
}

// ---
_INST_IMPL (FZ80::LD_IndirectHLFromD)
{
	assert (parameters ().size () == 1);

	return (executeWith (addressHL (), valueRegisterD ()));
}

// ---
_INST_IMPL (FZ80::LD_IndirectHLFromE)
{
	assert (parameters ().size () == 1);

	return (executeWith (addressHL (), valueRegisterE ()));
}

// ---
_INST_IMPL (FZ80::LD_IndirectHLFromH)
{
	assert (parameters ().size () == 1);

	return (executeWith (addressHL (), valueRegisterH ()));
}

// ---
_INST_IMPL (FZ80::LD_IndirectHLFromL)
{
	assert (parameters ().size () == 1);

	return (executeWith (addressHL (), valueRegisterL ()));
}

// ---
_INST_IMPL (FZ80::LD_IndirectBCFromA)
{
	assert (parameters ().size () == 1);

	return (executeWith (addressBC (), valueRegisterA ()));
}

// ---
_INST_IMPL (FZ80::LD_IndirectDEFromA)
{
	assert (parameters ().size () == 1);

	return (executeWith (addressDE (), valueRegisterA ()));
}

// ---
_INST_IMPL (FZ80::LD_IndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (addressIX ((char) parameters ()[2].value ()), parameters ()[3].value ()));
}

// ---
_INST_IMPL (FZ80::LD_IndirectIndexIXFromA)
{
	assert (parameters ().size () == 3);

	return (executeWith (addressIX ((char) parameters ()[2].value ()), valueRegisterA ()));
}

// ---
_INST_IMPL (FZ80::LD_IndirectIndexIXFromB)
{
	assert (parameters ().size () == 3);

	return (executeWith (addressIX ((char) parameters ()[2].value ()), valueRegisterB ()));
}

// ---
_INST_IMPL (FZ80::LD_IndirectIndexIXFromC)
{
	assert (parameters ().size () == 3);

	return (executeWith (addressIX ((char) parameters ()[2].value ()), valueRegisterC ()));
}

// ---
_INST_IMPL (FZ80::LD_IndirectIndexIXFromD)
{
	assert (parameters ().size () == 3);

	return (executeWith (addressIX ((char) parameters ()[2].value ()), valueRegisterD ()));
}

// ---
_INST_IMPL (FZ80::LD_IndirectIndexIXFromE)
{
	assert (parameters ().size () == 3);

	return (executeWith (addressIX ((char) parameters ()[2].value ()), valueRegisterE ()));
}

// ---
_INST_IMPL (FZ80::LD_IndirectIndexIXFromH)
{
	assert (parameters ().size () == 3);

	return (executeWith (addressIX ((char) parameters ()[2].value ()), valueRegisterH ()));
}

// ---
_INST_IMPL (FZ80::LD_IndirectIndexIXFromL)
{
	assert (parameters ().size () == 3);

	return (executeWith (addressIX ((char) parameters ()[2].value ()), valueRegisterL ()));
}

// ---
_INST_IMPL (FZ80::LD_IndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (addressIY ((char) parameters ()[2].value ()), parameters ()[3].value ()));
}

// ---
_INST_IMPL (FZ80::LD_IndirectIndexIYFromA)
{
	assert (parameters ().size () == 3);

	return (executeWith (addressIY ((char) parameters ()[2].value ()), valueRegisterA ()));
}

// ---
_INST_IMPL (FZ80::LD_IndirectIndexIYFromB)
{
	assert (parameters ().size () == 3);

	return (executeWith (addressIY ((char) parameters ()[2].value ()), valueRegisterB ()));
}

// ---
_INST_IMPL (FZ80::LD_IndirectIndexIYFromC)
{
	assert (parameters ().size () == 3);

	return (executeWith (addressIY ((char) parameters ()[2].value ()), valueRegisterC ()));
}

// ---
_INST_IMPL (FZ80::LD_IndirectIndexIYFromD)
{
	assert (parameters ().size () == 3);

	return (executeWith (addressIY ((char) parameters ()[2].value ()), valueRegisterD ()));
}

// ---
_INST_IMPL (FZ80::LD_IndirectIndexIYFromE)
{
	assert (parameters ().size () == 3);

	return (executeWith (addressIY ((char) parameters ()[2].value ()), valueRegisterE ()));
}

// ---
_INST_IMPL (FZ80::LD_IndirectIndexIYFromH)
{
	assert (parameters ().size () == 3);

	return (executeWith (addressIY ((char) parameters ()[2].value ()), valueRegisterH ()));
}

// ---
_INST_IMPL (FZ80::LD_IndirectIndexIYFromL)
{
	assert (parameters ().size () == 3);

	return (executeWith (addressIY ((char) parameters ()[2].value ()), valueRegisterL ()));
}

// ---
_INST_IMPL (FZ80::LD_AddressFromA)
{
	assert (parameters ().size () == 3);

	/** Addresses in memory are kept little - endian. */
	return (executeWith (
		_lastExecutionData._INOUTAddress = 
			MCHEmul::Address ({ 
				parameters ()[1].value (), 
				parameters ()[2].value () }, false /** Little - endian. */), valueRegisterA ()));
}

// ---
_INST_IMPL (FZ80::LD_AddressFromHL)
{
	assert (parameters ().size () == 3);

	/** Addresses in memory are kept little - endian. */
	return (executeWith (
		_lastExecutionData._INOUTAddress =
			MCHEmul::Address ({ 
				parameters ()[1].value (), 
				parameters ()[2].value () }, false /** Little - endian. */), valueRegisterHL ()));
}

// ---
_INST_IMPL (FZ80::U3LD_AddressFromHL)
{
	assert (parameters ().size () == 4);

	/** Addresses in memory are kept little - endian. */
	return (executeWith (
		_lastExecutionData._INOUTAddress =
			MCHEmul::Address ({ 
				parameters ()[2].value (), 
				parameters ()[3].value () }, false /** Little - endian. */), valueRegisterHL ()));
}

// ---
_INST_IMPL (FZ80::LD_AddressFromBC)
{
	assert (parameters ().size () == 4);

	/** Addresses in memory are kept little - endian. */
	return (executeWith (
		_lastExecutionData._INOUTAddress = 
			MCHEmul::Address ({ 
				parameters ()[2].value (), 
				parameters ()[3].value () }, false /** Little - endian. */), valueRegisterBC ()));
}

// ---
_INST_IMPL (FZ80::LD_AddressFromIX)
{
	assert (parameters ().size () == 4);

	/** Addresses in memory are kept little - endian. */
	return (executeWith (
		_lastExecutionData._INOUTAddress = 
			MCHEmul::Address ({ 
				parameters ()[2].value (), 
				parameters ()[3].value () }, false /** Little - endian. */), valueRegisterIX ()));
}

// ---
_INST_IMPL (FZ80::LD_AddressFromIY)
{
	assert (parameters ().size () == 4);

	/** Addresses in memory are kept little - endian. */
	return (executeWith (
		_lastExecutionData._INOUTAddress =
			MCHEmul::Address ({ 
				parameters ()[2].value (), 
				parameters ()[3].value () }, false /** Little - endian. */), valueRegisterIY ()));
}

// ---
_INST_IMPL (FZ80::LD_AddressFromDE)
{
	assert (parameters ().size () == 4);

	/** Addresses in memory are kept little - endian. */
	return (executeWith (
		_lastExecutionData._INOUTAddress =
			MCHEmul::Address ({ 
				parameters ()[2].value (), 
				parameters ()[3].value () }, false /** Little - endian. */), valueRegisterDE ()));
}

// ---
_INST_IMPL (FZ80::LD_AddressFromSP)
{
	assert (parameters ().size () == 4);

	/** Addresses in memory are kept big - endian. */
	MCHEmul::UBytes v = memory () -> stack () -> currentAddress ().bytes ();
	v.setMinLength (2); // It has always to be 2 bytes long...
	return (executeWith (
		_lastExecutionData._INOUTAddress =
			MCHEmul::Address ({ 
				parameters ()[2].value (), 
				parameters ()[3].value () }, false /** Little - endian. */), v));
}

// ---
_INST_IMPL (FZ80::LD_IFromA)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerI (), valueRegisterA ()));
}

// ---
_INST_IMPL (FZ80::LD_RFromA)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerR (), valueRegisterA ()));
}

// ---
_INST_IMPL (FZ80::LD_BC)
{
	assert (parameters ().size () == 3);

	// The "executeWith" method order the bytes in little - endian format... 
	return (executeWith (registerBC (), 
		MCHEmul::UBytes ({ parameters ()[1].value (), parameters ()[2].value () }, true))); 
}

// ---
_INST_IMPL (FZ80::LD_BCFromAddress)
{
	assert (parameters ().size () == 4);

	// The "executeWith" method order the bytes in little - endian format... 
	return (executeWith (registerBC (), 
		_lastExecutionData._INOUTData = 
			memory () -> values (_lastExecutionData._INOUTAddress =
				MCHEmul::Address ({ parameters ()[2].value (), parameters ()[3].value () }, 
					false /** address in little - endian. */), 2)));
}

// ---
_INST_IMPL (FZ80::LD_DE)
{
	assert (parameters ().size () == 3);

	// The "executeWith" method order the bytes in little - endian format... 
	return (executeWith (registerDE (), 
		MCHEmul::UBytes ({ parameters ()[1].value (), parameters ()[2].value () }, true)));
}

// ---
_INST_IMPL (FZ80::LD_DEFromAddress)
{
	assert (parameters ().size () == 4);

	// The "executeWith" method order the bytes in little - endian format... 
	return (executeWith (registerDE (), 
		_lastExecutionData._INOUTData =
			memory () -> values (_lastExecutionData._INOUTAddress =
				MCHEmul::Address ({ parameters ()[2].value (), parameters ()[3].value () }, 
					false /** address little - endian. */), 2)));
}

// ---
_INST_IMPL (FZ80::LD_HL)
{
	assert (parameters ().size () == 3);

	// The "executeWith" method order the bytes in little - endian format... 
	return (executeWith (registerHL (), 
		MCHEmul::UBytes ({ parameters ()[1].value (), parameters ()[2].value () }, true)));
}

// ---
_INST_IMPL (FZ80::LD_HLFromAddress)
{
	assert (parameters ().size () == 3);

	// The "executeWith" method order the bytes in little - endian format... 
	return (executeWith (registerHL (), 
		_lastExecutionData._INOUTData =
			memory () -> values (_lastExecutionData._INOUTAddress =
				MCHEmul::Address ({ parameters ()[1].value (), parameters ()[2].value () }, 
					false /** address in little - endian. */), 2)));
}

// ---
_INST_IMPL (FZ80::U3LD_HLFromAddress)
{
	assert (parameters ().size () == 4);

	// The "executeWith" method order the bytes in little - endian format... 
	return (executeWith (registerHL (), 
		_lastExecutionData._INOUTData =
			memory () -> values (_lastExecutionData._INOUTAddress =
				MCHEmul::Address ({ parameters ()[2].value (), parameters ()[3].value () }, 
					false /** address little - endian. */), 2)));
}

// ---
_INST_IMPL (FZ80::LD_IX)
{
	assert (parameters ().size () == 4);

	// The "executeWith" method order the bytes in little - endian format... 
	return (executeWith (registerIX (), 
		MCHEmul::UBytes ({ parameters ()[2].value (), parameters ()[3].value () }, true))); 
}

// ---
_INST_IMPL (FZ80::LD_IXFromAddress)
{
	assert (parameters ().size () == 4);

	// The "executeWith" method order the bytes in little - endian format... 
	return (executeWith (registerIX (), 
		_lastExecutionData._INOUTData =
			memory () -> values (_lastExecutionData._INOUTAddress =
				MCHEmul::Address ({ parameters ()[2].value (), parameters ()[3].value () }, 
					false /** address little - endian. */), 2)));
}

// ---
_INST_IMPL (FZ80::LD_IXH)
{
	assert (parameters ().size () == 3);

	return (executeWith (registerIXH (), parameters ()[2].value ()));
}

// ---
_INST_IMPL (FZ80::LD_IXHFromA)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIXH (), valueRegisterA ()));
}

// ---
_INST_IMPL (FZ80::LD_IXHFromB)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIXH (), valueRegisterB ()));
}

// ---
_INST_IMPL (FZ80::LD_IXHFromC)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIXH (), valueRegisterC ()));
}

// ---
_INST_IMPL (FZ80::LD_IXHFromD)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIXH (), valueRegisterD ()));
}

// ---
_INST_IMPL (FZ80::LD_IXHFromE)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIXH (), valueRegisterE ()));
}

// ---
_INST_IMPL (FZ80::LD_IXHFromIXH)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIXH (), valueRegisterIXH ()));
}

// ---
_INST_IMPL (FZ80::LD_IXHFromIXL)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIXH (), valueRegisterIXL ()));
}

// ---
_INST_IMPL (FZ80::LD_IXL)
{
	assert (parameters ().size () == 3);

	return (executeWith (registerIXL (), parameters ()[2].value ()));
}

// ---
_INST_IMPL (FZ80::LD_IXLFromA)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIXL (), valueRegisterA ()));
}

// ---
_INST_IMPL (FZ80::LD_IXLFromB)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIXL (), valueRegisterB ()));
}

// ---
_INST_IMPL (FZ80::LD_IXLFromC)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIXL (), valueRegisterC ()));
}

// ---
_INST_IMPL (FZ80::LD_IXLFromD)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIXL (), valueRegisterD ()));
}

// ---
_INST_IMPL (FZ80::LD_IXLFromE)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIXL (), valueRegisterE ()));
}

// ---
_INST_IMPL (FZ80::LD_IXLFromIXH)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIXL (), valueRegisterIXH ()));
}

// ---
_INST_IMPL (FZ80::LD_IXLFromIXL)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIXL (), valueRegisterIXL ()));
}

// ---
_INST_IMPL (FZ80::LD_IY)
{
	assert (parameters ().size () == 4);

	// The "executeWith" method order the bytes in little - endian format... 
	return (executeWith (registerIY (), 
		MCHEmul::UBytes ({ parameters ()[2].value (), parameters ()[3].value () }, true)));
}

// ---
_INST_IMPL (FZ80::LD_IYFromAddress)
{
	assert (parameters ().size () == 4);

	// The "executeWith" method order the bytes in little - endian format... 
	return (executeWith (registerIY (), 
		_lastExecutionData._INOUTData =
			memory () -> values (_lastExecutionData._INOUTAddress =
				MCHEmul::Address ({ parameters ()[2].value (), parameters ()[3].value () }, 
					false /** address little - endian. */), 2)));
}

// ---
_INST_IMPL (FZ80::LD_IYH)
{
	assert (parameters ().size () == 3);

	return (executeWith (registerIYH (), parameters ()[2].value ()));
}

// ---
_INST_IMPL (FZ80::LD_IYHFromA)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIYH (), valueRegisterA ()));
}

// ---
_INST_IMPL (FZ80::LD_IYHFromB)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIYH (), valueRegisterB ()));
}

// ---
_INST_IMPL (FZ80::LD_IYHFromC)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIYH (), valueRegisterC ()));
}

// ---
_INST_IMPL (FZ80::LD_IYHFromD)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIYH (), valueRegisterD ()));
}

// ---
_INST_IMPL (FZ80::LD_IYHFromE)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIYH (), valueRegisterE ()));
}

// ---
_INST_IMPL (FZ80::LD_IYHFromIYH)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIYH (), valueRegisterIYH ()));
}

// ---
_INST_IMPL (FZ80::LD_IYHFromIYL)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIYH (), valueRegisterIYL ()));
}

// ---
_INST_IMPL (FZ80::LD_IYL)
{
	assert (parameters ().size () == 3);

	return (executeWith (registerIYL (), parameters ()[2].value ()));
}

// ---
_INST_IMPL (FZ80::LD_IYLFromA)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIYL (), valueRegisterA ()));
}

// ---
_INST_IMPL (FZ80::LD_IYLFromB)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIYL (), valueRegisterB ()));
}

// ---
_INST_IMPL (FZ80::LD_IYLFromC)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIYL (), valueRegisterC ()));
}

// ---
_INST_IMPL (FZ80::LD_IYLFromD)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIYL (), valueRegisterD ()));
}

// ---
_INST_IMPL (FZ80::LD_IYLFromE)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIYL (), valueRegisterE ()));
}

// ---
_INST_IMPL (FZ80::LD_IYLFromIYH)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIYL (), valueRegisterIYH ()));
}

// ---
_INST_IMPL (FZ80::LD_IYLFromIYL)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIYL (), valueRegisterIYL ()));
}

// ---
_INST_IMPL (FZ80::LD_SP)
{
	assert (parameters ().size () == 3);

	// The position loaded is the ones located at the address pointer by the parameters,
	// that is kept in litlee - endian format and ocuppies two bytes...
	memory () -> stack () -> setPosition 
		(MCHEmul::Address ({ parameters ()[1].value (), parameters ()[2].value () }, false /** address little - endian. */).value (), 
			false /** Absolute position. */);
	return (true);
}

// ---
_INST_IMPL (FZ80::LD_SPFromHL)
{
	assert (parameters ().size () == 1);

	memory () -> stack () -> setPosition 
		(MCHEmul::Address ({ 
			registerH ().values ()[0].value (), 
			registerL ().values ()[0].value () }, true).value (), false /** Absolute position. */);
	return (true);
}

// ---
_INST_IMPL (FZ80::LD_SPFromIX)
{
	assert (parameters ().size () == 2);

	memory () -> stack () -> setPosition 
		(MCHEmul::Address ({ 
			registerIXH ().values ()[0].value (), 
			registerIXL ().values ()[0].value () }, true).value (), false /** Absolute position. */);
	return (true);
}

// ---
_INST_IMPL (FZ80::LD_SPFromIY)
{
	assert (parameters ().size () == 2);

	memory () -> stack () -> setPosition 
		(MCHEmul::Address ({ 
			registerIYH ().values ()[0].value (), 
			registerIYL ().values ()[0].value () }, true).value (), false /** Absolute position. */);
	return (true);
}

// ---
_INST_IMPL (FZ80::LD_SPFromAddress)
{
	assert (parameters ().size () == 4);

	// The position loaded is the one located at the address pointer by the parameters,
	// that is kept in little - endian format, and ocuppies 2 bytes...
	memory () -> stack () -> setPosition 
		(MCHEmul::Address (_lastExecutionData._INOUTData = 
			memory () -> values (_lastExecutionData._INOUTAddress =
				MCHEmul::Address 
					({ parameters ()[2].value (), parameters ()[3].value () }, 
						false /** little - endian. */), 2), false /** little - endian. */).value (), 
							false /** Absolute position. */);
	return (true);
}
