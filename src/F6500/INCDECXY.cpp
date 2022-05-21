#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>
#include <core/CPU.hpp>

// ---
_INST_IMPL (F6500::DEX)
{
	assert (parameters ().size () == 1);

	MCHEmul::Register& x = cpu () -> internalRegister (F6500::C6510::_XREGISTER);

	// Read the value, makes the operation and sets it back...
	MCHEmul::UInt v = MCHEmul::UInt (x.values ()) - MCHEmul::UInt::_1;
	// A carry could be generated, but it will be ignored...
	x.set (v.bytes ()); // 1 byte long always

	// Time of the status register
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	st.setBitStatus ("N", v.negative ());
	st.setBitStatus ("Z", v == MCHEmul::UInt::_0);

	return (true);
}

// ---
_INST_IMPL (F6500::DEY)
{
	assert (parameters ().size () == 1);

	MCHEmul::Register& y = cpu () -> internalRegister (F6500::C6510::_YREGISTER);

	// Read the value, makes the operation and sets it back...
	MCHEmul::UInt v = MCHEmul::UInt (y.values ()) - MCHEmul::UInt::_1;
	// A carry could be generated, but it will be ignored...
	y.set (v.bytes ()); // 1 byte long always

	// Time of the status register
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	st.setBitStatus ("N", v.negative ());
	st.setBitStatus ("Z", v == MCHEmul::UInt::_0);

	return (true);
}

// ---
_INST_IMPL (F6500::INX)
{
	assert (parameters ().size () == 1);

	MCHEmul::Register& x = cpu () -> internalRegister (F6500::C6510::_XREGISTER);

	// Read the value, makes the operation and sets it back...
	MCHEmul::UInt v = MCHEmul::UInt (x.values ()) + MCHEmul::UInt::_1;
	// A carry could be generated, but it will be ignored...
	x.set (v.bytes ()); // 1 byte long always

	// Time of the status register
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	st.setBitStatus ("N", v.negative ());
	st.setBitStatus ("Z", v == MCHEmul::UInt::_0);

	return (true);
}

// ---
_INST_IMPL (F6500::INY)
{
	assert (parameters ().size () == 1);

	MCHEmul::Register& y = cpu () -> internalRegister (F6500::C6510::_YREGISTER);

	// Read the value, makes the operation and sets it back...
	MCHEmul::UInt v = MCHEmul::UInt (y.values ()) - MCHEmul::UInt::_1;
	// A carry could be generated, but it will be ignored...
	y.set (v.bytes ()); // 1 byte long always

	// Time of the status register
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	st.setBitStatus ("N", v.negative ());
	st.setBitStatus ("Z", v == MCHEmul::UInt::_0);

	return (true);
}
