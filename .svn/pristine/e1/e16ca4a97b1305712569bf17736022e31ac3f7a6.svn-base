#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>
#include <core/CPU.hpp>

// ---
_INST_IMPL (F6500::DEX)
{
	assert (parameters ().size () == 1);

	MCHEmul::Register& x = cpu () -> internalRegister (F6500::C6510::_XREGISTER);

	// Set the value...
	MCHEmul::UBytes b = MCHEmul::UInt (x.values ()).substract (MCHEmul::UInt::_1).bytes ();
	if (b.size () > 1) b = MCHEmul::UBytes ({ b [1] });
	x.set (b);

	// Time of the status register
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	st.setBitStatus ("N", b [0][7]);
	st.setBitStatus ("Z", b [0] == MCHEmul::UByte::_0);

	return (true);
}

// ---
_INST_IMPL (F6500::DEY)
{
	assert (parameters ().size () == 1);

	MCHEmul::Register& y = cpu () -> internalRegister (F6500::C6510::_YREGISTER);

	// Set the value...
	MCHEmul::UBytes b = MCHEmul::UInt (y.values ()).substract (MCHEmul::UInt::_1).bytes ();
	if (b.size () > 1) b = MCHEmul::UBytes ({ b [1] });
	y.set (b);

	// Time of the status register
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	st.setBitStatus ("N", b [0][7]);
	st.setBitStatus ("Z", b [0] == MCHEmul::UByte::_0);

	return (true);
}

// ---
_INST_IMPL (F6500::INX)
{
	assert (parameters ().size () == 1);

	MCHEmul::Register& x = cpu () -> internalRegister (F6500::C6510::_XREGISTER);

	// Set the value...
	MCHEmul::UBytes b = MCHEmul::UInt (x.values ()).add (MCHEmul::UInt::_1).bytes ();
	if (b.size () > 1) b = MCHEmul::UBytes ({ b [1] });
	x.set (b);

	// Time of the status register
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	st.setBitStatus ("N", b [0][7]);
	st.setBitStatus ("Z", b [0] == MCHEmul::UByte::_0);

	return (true);
}

// ---
_INST_IMPL (F6500::INY)
{
	assert (parameters ().size () == 1);

	MCHEmul::Register& y = cpu () -> internalRegister (F6500::C6510::_YREGISTER);

	// Set the value...
	MCHEmul::UBytes b = MCHEmul::UInt (y.values ()).add (MCHEmul::UInt::_1).bytes ();
	if (b.size () > 1) b = MCHEmul::UBytes ({ b [1] });
	y.set (b);

	// Time of the status register
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	st.setBitStatus ("N", b [0][7]);
	st.setBitStatus ("Z", b [0] == MCHEmul::UByte::_0);

	return (true);
}
