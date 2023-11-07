#include <FZ80/Instructions.hpp>

// ---
_INST_IMPL (FZ80::LD_A)
{
	assert (parameters().size () == 2);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> aRegister (),
		parameters ()[1].value ()));
}

// ---
_INST_IMPL (FZ80::LD_AFromA)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> aRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> aRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_AFromB)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> aRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> bRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_AFromC)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> aRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> cRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_AFromD)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> aRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> dRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_AFromE)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> aRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> eRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_AFromF)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> aRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> fRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_AFromL)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> aRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> lRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_AFromI)
{
	assert (parameters().size () == 2);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> aRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> iRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_AFromR)
{
	assert (parameters().size () == 2);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> aRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> rRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_AFromAddressHL)
{
	assert (parameters().size () == 1);

	MCHEmul::RefRegisters& hl = dynamic_cast <FZ80::CZ80*> (cpu ()) -> hlRegister ();
	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> aRegister (),
		memory () -> value (MCHEmul::Address (MCHEmul::UBytes ({ hl [0] -> values ()[0], hl [1] -> values ()[0] }), true))));
}

// ---
_INST_IMPL (FZ80::LD_AFromAddressBC)
{
	assert (parameters().size () == 1);

	MCHEmul::RefRegisters& bc = dynamic_cast <FZ80::CZ80*> (cpu ()) -> bcRegister ();
	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> aRegister (),
		memory () -> value (MCHEmul::Address (MCHEmul::UBytes ({ bc [0] -> values ()[0], bc [1] -> values ()[0] }), true))));
}

// ---
_INST_IMPL (FZ80::LD_AFromAddressDE)
{
	assert (parameters().size () == 1);

	MCHEmul::RefRegisters& de = dynamic_cast <FZ80::CZ80*> (cpu ()) -> deRegister ();
	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> aRegister (),
		memory () -> value (MCHEmul::Address (MCHEmul::UBytes ({ de [0] -> values ()[0], de [1] -> values ()[0] }), true))));
}

// ---
_INST_IMPL (FZ80::LD_AFromAddressIndexIX)
{
	assert (parameters().size () == 3);

	MCHEmul::RefRegisters& ix = dynamic_cast <FZ80::CZ80*> (cpu ()) -> ixRegister ();
	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> aRegister (),
		memory () -> value (MCHEmul::Address (MCHEmul::UBytes ({ ix [0] -> values ()[0], ix [1] -> values ()[0] }), true) + 
			(size_t) (parameters ()[2].value ()))));
}

// ---
_INST_IMPL (FZ80::LD_AFromAddressIndexIY)
{
	assert (parameters().size () == 3);

	MCHEmul::RefRegisters& iy = dynamic_cast <FZ80::CZ80*> (cpu ()) -> iyRegister ();
	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> aRegister (),
		memory () -> value (MCHEmul::Address (MCHEmul::UBytes ({ iy [0] -> values ()[0], iy [1] -> values ()[0] }), true) + 
			(size_t) (parameters ()[2].value ()))));
}

// ---
_INST_IMPL (FZ80::LD_AFromAddress)
{
	assert (parameters().size () == 3);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> aRegister (),
		memory () -> value (MCHEmul::Address (MCHEmul::UBytes ({ parameters ()[1].value (), parameters ()[2].value () }), true))));
}

// ---
_INST_IMPL (FZ80::LD_B)
{
	assert (parameters().size () == 2);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> bRegister (),
		parameters ()[1].value ()));
}

// ---
_INST_IMPL (FZ80::LD_BFromA)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> bRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> aRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_BFromB)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> bRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> bRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_BFromC)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> bRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> cRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_BFromD)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> bRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> dRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_BFromE)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> bRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> eRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_BFromF)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> bRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> fRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_BFromL)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> bRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> lRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_BFromAddressIndexIX)
{
	assert (parameters().size () == 3);

	MCHEmul::RefRegisters& ix = dynamic_cast <FZ80::CZ80*> (cpu ()) -> ixRegister ();
	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> bRegister (),
		memory () -> value (MCHEmul::Address (MCHEmul::UBytes ({ ix [0] -> values ()[0], ix [1] -> values ()[0] }), true) + 
			(size_t) (parameters ()[2].value ()))));
}

// ---
_INST_IMPL (FZ80::LD_BFromAddressIndexIY)
{
	assert (parameters().size () == 3);

	MCHEmul::RefRegisters& iy = dynamic_cast <FZ80::CZ80*> (cpu ()) -> iyRegister ();
	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> bRegister (),
		memory () -> value (MCHEmul::Address (MCHEmul::UBytes ({ iy [0] -> values ()[0], iy [1] -> values ()[0] }), true) + 
			(size_t) (parameters ()[2].value ()))));
}

// ---
_INST_IMPL (FZ80::LD_C)
{
	assert (parameters().size () == 2);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> cRegister (),
		parameters ()[1].value ()));
}

// ---
_INST_IMPL (FZ80::LD_CFromA)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> cRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> aRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_CFromB)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> cRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> bRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_CFromC)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> cRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> cRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_CFromD)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> cRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> dRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_CFromE)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> cRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> eRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_CFromF)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> cRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> fRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_CFromL)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> cRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> lRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_CFromAddressIndexIX)
{
	assert (parameters().size () == 3);

	MCHEmul::RefRegisters& ix = dynamic_cast <FZ80::CZ80*> (cpu ()) -> ixRegister ();
	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> cRegister (),
		memory () -> value (MCHEmul::Address (MCHEmul::UBytes ({ ix [0] -> values ()[0], ix [1] -> values ()[0] }), true) + 
			(size_t) (parameters ()[2].value ()))));
}

// ---
_INST_IMPL (FZ80::LD_CFromAddressIndexIY)
{
	assert (parameters().size () == 3);

	MCHEmul::RefRegisters& iy = dynamic_cast <FZ80::CZ80*> (cpu ()) -> iyRegister ();
	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> cRegister (),
		memory () -> value (MCHEmul::Address (MCHEmul::UBytes ({ iy [0] -> values ()[0], iy [1] -> values ()[0] }), true) + 
			(size_t) (parameters ()[2].value ()))));
}

// ---
_INST_IMPL (FZ80::LD_D)
{
	assert (parameters().size () == 2);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> dRegister (),
		parameters ()[1].value ()));
}

// ---
_INST_IMPL (FZ80::LD_DFromA)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> dRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> aRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_DFromB)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> dRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> bRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_DFromC)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> dRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> cRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_DFromD)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> dRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> dRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_DFromE)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> dRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> eRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_DFromF)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> dRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> fRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_DFromL)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> dRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> lRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_DFromAddressIndexIX)
{
	assert (parameters().size () == 3);

	MCHEmul::RefRegisters& ix = dynamic_cast <FZ80::CZ80*> (cpu ()) -> ixRegister ();
	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> dRegister (),
		memory () -> value (MCHEmul::Address (MCHEmul::UBytes ({ ix [0] -> values ()[0], ix [1] -> values ()[0] }), true) + 
			(size_t) (parameters ()[2].value ()))));
}

// ---
_INST_IMPL (FZ80::LD_DFromAddressIndexIY)
{
	assert (parameters().size () == 3);

	MCHEmul::RefRegisters& iy = dynamic_cast <FZ80::CZ80*> (cpu ()) -> iyRegister ();
	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> dRegister (),
		memory () -> value (MCHEmul::Address (MCHEmul::UBytes ({ iy [0] -> values ()[0], iy [1] -> values ()[0] }), true) + 
			(size_t) (parameters ()[2].value ()))));
}

// ---
_INST_IMPL (FZ80::LD_EFromA)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> eRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> aRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_E)
{
	assert (parameters().size () == 2);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> eRegister (),
		parameters ()[1].value ()));
}

// ---
_INST_IMPL (FZ80::LD_EFromB)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> eRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> bRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_EFromC)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> eRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> cRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_EFromD)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> eRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> dRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_EFromE)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> eRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> eRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_EFromF)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> eRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> fRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_EFromL)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> aRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> lRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_EFromAddressIndexIX)
{
	assert (parameters().size () == 3);

	MCHEmul::RefRegisters& ix = dynamic_cast <FZ80::CZ80*> (cpu ()) -> ixRegister ();
	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> eRegister (),
		memory () -> value (MCHEmul::Address (MCHEmul::UBytes ({ ix [0] -> values ()[0], ix [1] -> values ()[0] }), true) + 
			(size_t) (parameters ()[2].value ()))));
}

// ---
_INST_IMPL (FZ80::LD_EFromAddressIndexIY)
{
	assert (parameters().size () == 3);

	MCHEmul::RefRegisters& iy = dynamic_cast <FZ80::CZ80*> (cpu ()) -> iyRegister ();
	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> eRegister (),
		memory () -> value (MCHEmul::Address (MCHEmul::UBytes ({ iy [0] -> values ()[0], iy [1] -> values ()[0] }), true) + 
			(size_t) (parameters ()[2].value ()))));
}

// ---
_INST_IMPL (FZ80::LD_H)
{
	assert (parameters().size () == 2);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> hRegister (),
		parameters ()[1].value ()));
}

// ---
_INST_IMPL (FZ80::LD_HFromA)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> hRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> aRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_HFromB)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> hRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> bRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_HFromC)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> hRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> cRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_HFromD)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> hRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> dRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_HFromE)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> hRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> eRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_HFromF)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> hRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> fRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_HFromL)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> hRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> lRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_HFromAddressIndexIX)
{
	assert (parameters().size () == 3);

	MCHEmul::RefRegisters& ix = dynamic_cast <FZ80::CZ80*> (cpu ()) -> ixRegister ();
	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> hRegister (),
		memory () -> value (MCHEmul::Address (MCHEmul::UBytes ({ ix [0] -> values ()[0], ix [1] -> values ()[0] }), true) + 
			(size_t) (parameters ()[2].value ()))));
}

// ---
_INST_IMPL (FZ80::LD_HFromAddressIndexIY)
{
	assert (parameters().size () == 3);

	MCHEmul::RefRegisters& iy = dynamic_cast <FZ80::CZ80*> (cpu ()) -> iyRegister ();
	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> hRegister (),
		memory () -> value (MCHEmul::Address (MCHEmul::UBytes ({ iy [0] -> values ()[0], iy [1] -> values ()[0] }), true) + 
			(size_t) (parameters ()[2].value ()))));
}

// ---
_INST_IMPL (FZ80::LD_L)
{
	assert (parameters().size () == 2);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> lRegister (),
		parameters ()[1].value ()));
}

// ---
_INST_IMPL (FZ80::LD_LFromA)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> lRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> aRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_LFromB)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> lRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> bRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_LFromC)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> lRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> cRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_LFromD)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> lRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> dRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_LFromE)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> lRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> eRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_LFromF)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> lRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> fRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_LFromL)
{
	assert (parameters().size () == 1);

	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> lRegister (),
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> lRegister ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::LD_LFromAddressIndexIX)
{
	assert (parameters().size () == 3);

	MCHEmul::RefRegisters& ix = dynamic_cast <FZ80::CZ80*> (cpu ()) -> ixRegister ();
	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> lRegister (),
		memory () -> value (MCHEmul::Address (MCHEmul::UBytes ({ ix [0] -> values ()[0], ix [1] -> values ()[0] }), true) + 
			(size_t) (parameters ()[2].value ()))));
}

// ---
_INST_IMPL (FZ80::LD_LFromAddressIndexIY)
{
	assert (parameters().size () == 3);

	MCHEmul::RefRegisters& iy = dynamic_cast <FZ80::CZ80*> (cpu ()) -> iyRegister ();
	return (executeWith (
		dynamic_cast <FZ80::CZ80*> (cpu ()) -> lRegister (),
		memory () -> value (MCHEmul::Address (MCHEmul::UBytes ({ iy [0] -> values ()[0], iy [1] -> values ()[0] }), true) + 
			(size_t) (parameters ()[2].value ()))));
}
