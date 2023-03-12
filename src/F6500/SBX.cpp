#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
bool F6500::SBX_General::executeWith (MCHEmul::UByte u)
{
	// TODO

	return (true);
}

// ---
_INST_IMPL (F6500::SBX_Inmediate)
{
	return (executeWith (value_inmediate ()));
}
