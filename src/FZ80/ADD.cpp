#include <FZ80/ADD.hpp>

// ---
_INST_IMPL (FZ80::ADD_AWithA)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::U1ADD_AWithA)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::U2ADD_AWithA)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::ADD_AWithB)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerB ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::U1ADD_AWithB)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::U2ADD_AWithB)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::ADD_AWithC)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerC ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::U1ADD_AWithC)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::U2ADD_AWithC)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::ADD_AWithD)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerD ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::U1ADD_AWithD)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::U2ADD_AWithD)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::ADD_AWithE)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerE ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::U1ADD_AWithE)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::U2ADD_AWithE)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::ADD_AWithH)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerH ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::ADD_AWithL)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerL ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::ADD_AWithIndirectHL)
{
	assert (parameters ().size () == 1);

	return (executeWith (valueAddressHL (), false));
}

// ---
_INST_IMPL (FZ80::ADD_AWithIndirectIndexIX)
{
	assert (parameters ().size () == 3);

	return (executeWith (valueAddressIX ((char) parameters ()[2].value ()), false));
}

// ---
_INST_IMPL (FZ80::ADD_AWithIndirectIndexIY)
{
	assert (parameters ().size () == 3);

	return (executeWith (valueAddressIY ((char) parameters ()[2].value ()), false));
}

// ---
_INST_IMPL (FZ80::ADD_A)
{
	assert (parameters ().size () == 2);

	return (executeWith (parameters ()[1].value (), false));
}

// ---
_INST_IMPL (FZ80::ADD_HLWithBC)
{
	assert (parameters ().size () == 1);

	// Used maybe later in BIT instructions...
	static_cast <FZ80::CZ80*> (_lastExecutionData._cpu) -> 
		setRWInternalRegister (registerHL ()[0] -> values ()[0]);

	return (executeWith (registerHL (), valueRegisterBC (), false));
}

// ---
_INST_IMPL (FZ80::ADD_HLWithDE)
{
	assert (parameters ().size () == 1);

	static_cast <FZ80::CZ80*> (_lastExecutionData._cpu) -> 
		setRWInternalRegister (registerHL ()[0] -> values ()[0]);

	return (executeWith (registerHL (), valueRegisterDE (), false));
}

// ---
_INST_IMPL (FZ80::ADD_HLWithHL)
{
	assert (parameters ().size () == 1);

	static_cast <FZ80::CZ80*> (_lastExecutionData._cpu) -> 
		setRWInternalRegister (registerHL ()[0] -> values ()[0]);

	return (executeWith (registerHL (), valueRegisterHL (), false));
}

// ---
_INST_IMPL (FZ80::ADD_HLWithSP)
{
	assert (parameters ().size () == 1);

	static_cast <FZ80::CZ80*> (_lastExecutionData._cpu) -> 
		setRWInternalRegister (registerHL ()[0] -> values ()[0]);

	return (executeWith (registerHL (), valueRegisterSP (), false));
}

// ---
_INST_IMPL (FZ80::ADD_AWithIXH)
{
	assert (parameters ().size () == 2);

	return (executeWith (valueRegisterIXH (), false));
}

// ---
_INST_IMPL (FZ80::ADD_AWithIXL)
{
	assert (parameters ().size () == 2);

	return (executeWith (valueRegisterIXL (), false));
}

// ---
_INST_IMPL (FZ80::ADD_AWithIYH)
{
	assert (parameters ().size () == 2);

	return (executeWith (valueRegisterIYH (), false));
}

// ---
_INST_IMPL (FZ80::ADD_AWithIYL)
{
	assert (parameters ().size () == 2);

	return (executeWith (valueRegisterIYL (), false));
}

// ---
_INST_IMPL (FZ80::ADD_IXWithBC)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIX (), valueRegisterBC (), false));
}

// ---
_INST_IMPL (FZ80::ADD_IXWithDE)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIX (), valueRegisterDE (), false));
}

// ---
_INST_IMPL (FZ80::ADD_IXWithIX)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIX (), valueRegisterIX (), false));
}

// ---
_INST_IMPL (FZ80::ADD_IXWithSP)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIX (), valueRegisterSP (), false));
}

// ---
_INST_IMPL (FZ80::ADD_IYWithBC)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIY (), valueRegisterBC (), false));
}

// ---
_INST_IMPL (FZ80::ADD_IYWithDE)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIY (), valueRegisterDE (), false));
}

// ---
_INST_IMPL (FZ80::ADD_IYWithIY)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIY (), valueRegisterIY (), false));
}

// ---
_INST_IMPL (FZ80::ADD_IYWithSP)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIY (), valueRegisterSP (), false));
}

// ---
_INST_IMPL (FZ80::ADC_AWithA)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::U1ADC_AWithA)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::U2ADC_AWithA)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::ADC_AWithB)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerB ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::U1ADC_AWithB)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::U2ADC_AWithB)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::ADC_AWithC)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerC ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::U1ADC_AWithC)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::U2ADC_AWithC)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::ADC_AWithD)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerD ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::U1ADC_AWithD)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::U2ADC_AWithD)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::ADC_AWithE)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerE ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::U1ADC_AWithE)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::U2ADC_AWithE)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::ADC_AWithH)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerH ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::ADC_AWithL)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerL ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::ADC_AWithIndirectHL)
{
	assert (parameters ().size () == 1);

	return (executeWith (valueAddressHL (), true));
}

// ---
_INST_IMPL (FZ80::ADC_AWithIndirectIndexIX)
{
	assert (parameters ().size () == 3);

	return (executeWith (valueAddressIX ((char) parameters ()[2].value ()), true));
}

// ---
_INST_IMPL (FZ80::ADC_AWithIndirectIndexIY)
{
	assert (parameters ().size () == 3);

	return (executeWith (valueAddressIY ((char) parameters ()[2].value ()), true));
}

// ---
_INST_IMPL (FZ80::ADC_A)
{
	assert (parameters ().size () == 2);

	return (executeWith (parameters ()[1].value (), true));
}

// ---
_INST_IMPL (FZ80::ADC_HLWithBC)
{
	assert (parameters ().size () == 2);

	static_cast <FZ80::CZ80*> (_lastExecutionData._cpu) -> 
		setRWInternalRegister (registerHL ()[0] -> values ()[0]);

	return (executeWith (registerHL (), valueRegisterBC (), true));
}

// ---
_INST_IMPL (FZ80::ADC_HLWithDE)
{
	assert (parameters ().size () == 2);

	static_cast <FZ80::CZ80*> (_lastExecutionData._cpu) -> 
		setRWInternalRegister (registerHL ()[0] -> values ()[0]);

	return (executeWith (registerHL (), valueRegisterDE (), true));
}

// ---
_INST_IMPL (FZ80::ADC_HLWithHL)
{
	assert (parameters ().size () == 2);

	static_cast <FZ80::CZ80*> (_lastExecutionData._cpu) -> 
		setRWInternalRegister (registerHL ()[0] -> values ()[0]);

	return (executeWith (registerHL (), valueRegisterHL (), true));
}

// ---
_INST_IMPL (FZ80::ADC_HLWithSP)
{
	assert (parameters ().size () == 2);

	static_cast <FZ80::CZ80*> (_lastExecutionData._cpu) -> 
		setRWInternalRegister (registerHL ()[0] -> values ()[0]);

	return (executeWith (registerHL (), valueRegisterSP (), true));
}

// ---
_INST_IMPL (FZ80::ADC_AWithIXH)
{
	assert (parameters ().size () == 2);

	return (executeWith (valueRegisterIXH (), true));
}

// ---
_INST_IMPL (FZ80::ADC_AWithIXL)
{
	assert (parameters ().size () == 2);

	return (executeWith (valueRegisterIXL (), true));
}

// ---
_INST_IMPL (FZ80::ADC_AWithIYH)
{
	assert (parameters ().size () == 2);

	return (executeWith (valueRegisterIYH (), true));
}

// ---
_INST_IMPL (FZ80::ADC_AWithIYL)
{
	assert (parameters ().size () == 2);

	return (executeWith (valueRegisterIYL (), true));
}
