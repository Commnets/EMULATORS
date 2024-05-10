#include <FZ80/EX.hpp>

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

	bool result = true;
	result &= executeWith (registerBC (), registerBCP ()); 
	// Couldn't link in the same instruction just one false finishes...
	result &= executeWith (registerDE (), registerDEP ());
	result &= executeWith (registerHL (), registerHLP ());
	return (result);
}

// ---
_INST_IMPL (FZ80::EX_SPWithHL)
{
	assert (parameters ().size () == 1);

	return (executeWithSP (registerHL ()));
}

// ---
_INST_IMPL (FZ80::EX_SPWithIX)
{
	assert (parameters ().size () == 2);

	return (executeWithSP (registerIX ()));
}

// ---
_INST_IMPL (FZ80::EX_SPWithIY)
{
	assert (parameters ().size () == 2);

	return (executeWithSP (registerIY ()));
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
	static_cast <FZ80::CZ80*> (cpu ()) -> incrementRegisterR (); // Block instructions increment 2 * BC...
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
	static_cast <FZ80::CZ80*> (cpu ()) -> incrementRegisterR (); // Block instructions increment 2 * BC...
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
	static_cast <FZ80::CZ80*> (cpu ()) -> incrementRegisterR (); // Block instructions increment 2 * BC...
	// The instruction only finish when reaches the limit or A has been found!
	if (result && (_FINISH = (_bc0 || cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_ZEROFLAG))) == false)
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
	static_cast <FZ80::CZ80*> (cpu ()) -> incrementRegisterR (); // Block instructions increment 2 * BC...
	// The instruction only finish when reaches the limit or A has been found!
	if (result && (_FINISH = (_bc0 || cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_ZEROFLAG))) == false) 
		_additionalCycles = 5; // ...and if not, it costs 5 additional cycles always!

	return (result);
}
