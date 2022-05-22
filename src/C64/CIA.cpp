#include <C64/CIA.hpp>
#include <C64/C64.hpp>

// ---
bool C64::CIA1::initialize ()
{
	setMemoryRef (nullptr);

	bool result = MCHEmul::Chip::initialize ();
	if (!result)
		return (false); // _lastError variable has been already set at this point...

	// TODO

	return (true);
}

// ---
bool C64::CIA1::simulate (MCHEmul::CPU*)
{
	// TODO

	return (true);
}

// ---
bool C64::CIA2::initialize ()
{
	setMemoryRef (nullptr);

	bool result = MCHEmul::Chip::initialize ();
	if (!result)
		return (false); // _lastError variable has been already set at this point...

	// TODO

	return (true);
}

// ---
bool C64::CIA2::simulate (MCHEmul::CPU*)
{
	// TODO

	return (true);
}
