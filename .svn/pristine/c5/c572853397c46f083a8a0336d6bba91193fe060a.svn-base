#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>
#include <core/CPU.hpp>
#include <core/Stack.hpp>

// ---
MCHEmul::Address F6500::Instruction::address_absolute ()
{
	assert (parameters ().size () == 3);

	return (MCHEmul::Address ({ parameters ()[1], parameters ()[2] }, false));
}

// ---
MCHEmul::Address F6500::Instruction::address_zeroPage ()
{
	assert (parameters ().size () == 2);

	return (MCHEmul::Address ({ parameters ()[1] }));
}

// ---
MCHEmul::Address F6500::Instruction::address_absoluteX ()
{
	assert (parameters ().size () == 3);

	MCHEmul::Register& x = cpu () -> internalRegister (F6500::C6510::_XREGISTER);

	MCHEmul::Address iA ({ parameters ()[1], parameters ()[2] }, false);
	MCHEmul::Address fA = iA + x [0].value ();
	if (iA [0] != fA [0]) _additionalCycles = 1; // Page jump in the address so one cycle more
	return (fA);
}

// ---
MCHEmul::Address F6500::Instruction::address_absoluteY ()
{
	assert (parameters ().size () == 3);

	MCHEmul::Register& y = cpu () -> internalRegister (F6500::C6510::_YREGISTER);

	MCHEmul::Address iA ({ parameters ()[1], parameters ()[2] }, false);
	MCHEmul::Address fA = iA + y [0].value ();
	if (iA [0] != fA [0]) _additionalCycles = 1; // Page jump in the address so one cycle more
	return (fA);
}

// ---
MCHEmul::Address F6500::Instruction::address_zeroPageX ()
{
	assert (parameters ().size () == 2);

	MCHEmul::Register& x = cpu () -> internalRegister (F6500::C6510::_XREGISTER);

	MCHEmul::Address iA ({ parameters ()[1] });
	return (iA + x [0].value ());
}

// ---
MCHEmul::Address F6500::Instruction::address_zeroPageY ()
{
	assert (parameters ().size () == 2);

	MCHEmul::Register& y = cpu () -> internalRegister (F6500::C6510::_XREGISTER);

	MCHEmul::Address iA ({ parameters ()[1] });
	return (iA + y [0].value ());
}

// ---
MCHEmul::Address F6500::Instruction::address_indirectZeroPageX ()
{
	assert (parameters ().size () == 2);

	MCHEmul::Register& x = cpu () -> internalRegister (F6500::C6510::_XREGISTER);

	// Pre - indirect zero page addressing...
	MCHEmul::Address iA = MCHEmul::Address ({ parameters ()[1] }) + x [0].value ();
	return (MCHEmul::Address (memory () -> values (iA, 2), false)); 
}

// ---
MCHEmul::Address F6500::Instruction::address_indirectZeroPageY ()
{
	assert (parameters ().size () == 2);

	MCHEmul::Register& y = cpu () -> internalRegister (F6500::C6510::_YREGISTER);

	// Post - indirect zero page addressing...
	MCHEmul::Address iA (memory () -> values (MCHEmul::Address ({ parameters ()[1] }), 2), false);
	MCHEmul::Address fA = iA + y [0].value ();
	if (iA [0] != fA [0]) _additionalCycles = 1; // Page jump in the address so one cycle more
	return (fA);
}

// ---
MCHEmul::Address F6500::Instruction::address_indirect ()
{
	assert (parameters ().size () == 3);

	return (MCHEmul::Address (memory () -> values 
		(MCHEmul::Address ({ parameters ()[1], parameters ()[2] }, false), 2), false));
}

// ---
MCHEmul::UByte F6500::Instruction::value_inmediate ()
{
	assert (parameters ().size () == 2);

	return (parameters ()[1]);
}

// ---
MCHEmul::UByte F6500::Instruction::value_relative ()
{
	assert (parameters ().size () == 2);

	return (parameters ()[1]); // The number can be interpreted as a negative number (used in jumps)...
}

_INST_IMPL (F6500::BIT_Absolute)
{
	MCHEmul::UByte v = value_absolute ();

	bool z = (v & cpu () -> internalRegister (F6500::C6510::_ACCUMULATOR).values ()[0] /** 1 byte long */) == MCHEmul::UByte::_0;

	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	st.setBitStatus ("N", v [7]);
	st.setBitStatus ("V", v [6]);
	st.setBitStatus ("Z", z);

	return (true);
}

_INST_IMPL (F6500::BIT_ZeroPage)
{
	MCHEmul::UByte v = value_zeroPage ();

	bool z = (v & cpu () -> internalRegister (F6500::C6510::_ACCUMULATOR).values ()[0] /** 1 byte long */) == MCHEmul::UByte::_0;

	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	st.setBitStatus ("N", v [7]);
	st.setBitStatus ("V", v [6]);
	st.setBitStatus ("Z", z);

	return (true);
}

_INST_IMPL (F6500::NOP)
{
	assert (parameters ().size () == 1);

	// Does nothing
	return (true);
}
