#include <C264/TEDRegisters.hpp>

// ---
C264::TEDRegisters::TEDRegisters 
		(MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: COMMODORE::TEDRegisters (ps, pp, a, s)
{ 
	initializeInternalValues (); 
}

// ---
void C264::TEDRegisters::initialize ()
{
	COMMODORE::TEDRegisters::initialize ();

	initializeInternalValues ();
}

// ---
void C264::TEDRegisters::setValue (size_t p, const MCHEmul::UByte& v)
{
	size_t pp = p % 0x20;

	// The 32 first bytes will keep the right value...
	MCHEmul::PhysicalStorageSubset::setValue (p, v);

	switch (pp)
	{
		// The TED keyboard port is linked to the joystick situation...
		case 0x08:
			{
				COMMODORE::TEDRegisters::setValue (pp, v);
				
				if (v.value () == 0xfb && 
					_joystickPins [0].value () != MCHEmul::UByte::_FF)
					_keyboardLatch &= _joystickPins [0].value ();
				if (v.value () == 0xfd &&
					_joystickPins [1].value () != MCHEmul::UByte::_FF)
					_keyboardLatch &= _joystickPins [1].value ();
			}

			break;

		default:
			{
				COMMODORE::TEDRegisters::setValue (pp, v);
			}

			break;
	}
}       

// ---
void C264::TEDRegisters::initializeInternalValues ()
{
	COMMODORE::TEDRegisters::initializeInternalValues ();

	// The RAM values...
	for (size_t i = 0x20; i < 0x40; 
		setValue (i++, MCHEmul::UByte::_0));

	// No info from joysticks...
	_joystickPins [0] = 
	_joystickPins [1] = MCHEmul::UByte::_FF;
}
