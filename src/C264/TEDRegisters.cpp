#include <C264/TEDRegisters.hpp>

// ---
C264::TEDRegisters::TEDRegisters 
		(MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: COMMODORE::TEDRegisters (ps, pp, a, s),
	  _register8 (MCHEmul::UByte::_0)
	  // The rest of the value sare initialized under the method initializeInternalValues...
{ 
	initializeInternalValues (); 
}

// ---
void C264::TEDRegisters::initialize ()
{
	COMMODORE::TEDRegisters::initialize ();

	_register8 = MCHEmul::UByte::_0;

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
				// This code is similar to the one in COMMODORE::TEDRegisters,
				// but adapted to C264 behaviour...
				// ...because the joysticks are connected directly to the TED keyboard port too!
				MCHEmul::UByte latched = _keyboardPins;
				// $FB = 11111011: bit 2 low selects Joy Lo.
				if (!v.bit (2))
					latched &= ~_joystickStatus [0];
				// $FD = 11111101: bit 1 low selects Joy Hi.
				if (!v.bit (1))
					latched &= ~_joystickStatus [1];
				_keyboardLatch = latched;

				// It is said in some forums (https://plus4world.powweb.com/forum/42748#42810)
				// that if the TED register 8 is broken, the system read the value of the keyboard port
				// with no need to latch it before...
				// However this code doesn't reflect this behaviour, because it is not the standard one!
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

	// Nothing pressed by default...
	_joystickStatus [0] = _joystickStatus [1] = MCHEmul::UByte::_0;
}
