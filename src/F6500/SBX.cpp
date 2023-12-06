#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
_INST_IMPL (F6500::SBX_Inmediate)
{
	return (executeWith (value_inmediate ()));
}
