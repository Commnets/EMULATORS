#include <C264/TEDRegisters.hpp>

// ---
C264::TEDRegisters::TEDRegisters 
		(MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: COMMODORE::TEDRegisters (ps, pp, a, s),
	  _ROMAccessChanged (false) // It is mandatory...
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
	size_t pp = p % 0x40;

	// The 64 first bytes will keep the right value...
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

		// Empty registers, nothing special is done...
		// They are treated as RAM positions!
		case 0x20:
		case 0x21:
		case 0x22:
		case 0x23:
		case 0x24:
		case 0x25:
		case 0x26:
		case 0x27:
		case 0x28:
		case 0x29:
		case 0x2a:
		case 0x2b:
		case 0x2c:
		case 0x2d:
		case 0x2e:
		case 0x2f:
		case 0x30:
		case 0x31:
		case 0x32:
		case 0x33:
		case 0x34:
		case 0x35:
		case 0x36:
		case 0x37:
		case 0x38:
		case 0x39:
		case 0x3a:
		case 0x3b:
		case 0x3c:
		case 0x3d:
			break;

		case 0x3e:
			{
				_HIROMSelected = true;
				_ROMAccessChanged = true;
			}

			break;

		case 0x3f:
			{
				_HIROMSelected = false;
				_ROMAccessChanged = true;
			}

			break;

		default:
			COMMODORE::TEDRegisters::setValue (pp, v);
			break;
	}
}       

// ---
const MCHEmul::UByte& C264::TEDRegisters::readValue (size_t p) const
{
	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	size_t pp = p % 0x40;

	switch (pp)
	{
		case 0x20:
		case 0x21:
		case 0x22:
		case 0x23:
		case 0x24:
		case 0x25:
		case 0x26:
		case 0x27:
		case 0x28:
		case 0x29:
		case 0x2a:
		case 0x2b:
		case 0x2c:
		case 0x2d:
		case 0x2e:
		case 0x2f:
		case 0x30:
		case 0x31:
		case 0x32:
		case 0x33:
		case 0x34:
		case 0x35:
		case 0x36:
		case 0x37:
		case 0x38:
		case 0x39:
		case 0x3a:
		case 0x3b:
		case 0x3c:
		case 0x3d:
		case 0x3e:
		case 0x3f:
			{
				result = MCHEmul::PhysicalStorageSubset::readValue (pp);
			}

			break;

		// The very TED registers...
		default:
			result = COMMODORE::TEDRegisters::readValue (pp);
			break;
	}

	return (_lastValueRead = result);
}

// ---
void C264::TEDRegisters::initializeInternalValues ()
{
	COMMODORE::TEDRegisters::initializeInternalValues ();

	// The RAM values...
	for (size_t i = 0x20; i < 0x40; 
		setValue (i++, MCHEmul::UByte::_0));

	// The last two position are not initialized using the setValue method
	// as they will launch later a change in the status of the machine....
	// The old method is used instead.
	_ROMSourceActive = false;
	_ROMAccessChanged = false;

	// No info from joysticks...
	_joystickPins [0] = 
	_joystickPins [1] = MCHEmul::UByte::_FF;
}
