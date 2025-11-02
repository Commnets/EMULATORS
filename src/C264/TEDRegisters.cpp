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
				_register8 = v;
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
const MCHEmul::UByte& C264::TEDRegisters::readValue (size_t p) const
{
	MCHEmul::UByte result = MCHEmul::UByte::_0;

	size_t pp = p % 0x20;

	switch (pp)
	{
		// This register read finally the status of the keyboard entry, 
		// but the situation of the joystick mist also be taken into account...
		case 0x08:
			{
				result =
					(_register8 == 0xfb) // The selector has pointed the joystick 1
						? _keyboardEntry & ~_joystickStatus [0]
						: ((_register8 == 0xfd) // The seelctor just pointed the joystick 2
							? _keyboardEntry & ~_joystickStatus [1]
							: _keyboardEntry); // No joystick pointed...
			}

			break;

		default:
			{
				result = COMMODORE::TEDRegisters::readValue (pp);
			}

			break;
	}

	return (_lastValueRead = result);
}
	
// ---
void C264::TEDRegisters::initializeInternalValues ()
{
	COMMODORE::TEDRegisters::initializeInternalValues ();

	// Pull up by default...
	_keyboardEntry = MCHEmul::UByte::_FF;
	// Nothing pressed by default...
	_joystickStatus [0] = _joystickStatus [1] = MCHEmul::UByte::_0;
}
