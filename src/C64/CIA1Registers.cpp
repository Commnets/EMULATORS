#include <C64/CIA1Registers.hpp>

// ---
C64::CIA1Registers::CIA1Registers (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: COMMODORE::CIARegisters (_CIA1_SUBSET, ps, pp, a, s),
	  _sid (nullptr)
	  // At this point all internal variables will have random values...
{ 
	setClassName ("CIA1Registers");

	initializeInternalValues (); 
}

// ---
void C64::CIA1Registers::linkToSID (COMMODORE::SID* sid)
{ 
	assert (sid != nullptr); 
	
	_sid = sid;
}

// ---
void C64::CIA1Registers::setValue (size_t p, const MCHEmul::UByte& v)
{
	// Normal way...
	COMMODORE::CIARegisters::setValue (p, v);

	// ..but when the register accesed is the 0...
	if (_sid != nullptr && (p % 0x10) == 0x00)
		_sid -> setPotenciometerGroupActive 
		((v.value () & 0xc0) == 0x80 // bit 7 on and bit 6 off...
			? (size_t) 1 
			: ((v.value () & 0xc0) == 0x40 // bit 7 off and bit 6 on...
				? (size_t) 0
				: MCHEmul::_S0));
}

// ---
const MCHEmul::UByte& C64::CIA1Registers::readValue (size_t p) const
{
	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	size_t pp = p % 0x10;

	switch (pp)
	{
		// Data Port Register A: CIA1PRA
		case 0x00:
			{
				// In CIA 1 the value of the port can be affected 
				// by the keyboard and the joystick 1 (or paddle buttons if paddles are connected instead)
				// What is in the portB make conexions (see .hpp) in the portA
				// determining what is shown there...
				unsigned char dtA = MCHEmul::UByte::_0;

				unsigned char msk = 
					(_outputRegB | ~_dataPortBDir) & 
						(_paddleConnected [0] // Paddle connected?
							? paddleDigitalLines (0)
							: _joystickStatus [0]);

				unsigned char m = 0x01;
				for (size_t i = 0; i < 8; m <<= 1, i++)
					if ((~msk & m) != 0x00)
						dtA |= ~_rev_keyboardStatusMatrix [i].value (); // true if clicked...

				// but affected by the keys and joystick switches pressed... 
				// ...or paddle buttons if paddles are connected instead.
				unsigned char portA = 
					(_outputRegA | ~_dataPortADir) /** What it should go to portA as internal configuration determines. */ & 
						(~dtA & 
							(_paddleConnected [1] // Paddle connected?
								? paddleDigitalLines (1)
								: _joystickStatus [1]));
				unsigned char portB =
					(_outputRegB | ~_dataPortBDir) &
						(_paddleConnected [0]
							? paddleDigitalLines (0)
							: _joystickStatus [0]);

				if (numberOfPressedKeys () > 1 || hasAnyJoystickOrPaddleLineLow ())
					propagateKeyboardLowLines (portA, portB);

				_portA = portA;
				_portB = portB;

				result = MCHEmul::UByte (_portA);
			}

			break;

		// Data Port Register B: CIA1PRB
		case 0x01:
			{
				// @see above
				// Simetrically to portA, 
				// the value of portB can be affected by the keyboard and the joystick 2 
				// (or paddle buttons if paddles are connected instead)
				unsigned char dtB = MCHEmul::UByte::_0;

				unsigned char msk = 
					(_outputRegA | ~_dataPortADir) &
						((_paddleConnected [1]) // Paddle connected?
							? paddleDigitalLines (1)
							: _joystickStatus [1]);

				unsigned char m = 0x01;
				for (size_t i = 0; i < 8; m <<= 1, i++)
					if ((~msk & m) != 0x00)
						dtB |= ~_keyboardStatusMatrix [i].value ();  // true if clicked...

				unsigned char portB	= 
					(_outputRegB | ~_dataPortBDir) & 
						(~dtB & 
							((_paddleConnected [0])
								? paddleDigitalLines (0)
								: _joystickStatus [0]));
				unsigned char portA =
					(_outputRegA | ~_dataPortADir) &
						(_paddleConnected [1]
							? paddleDigitalLines (1)
							: _joystickStatus [1]);

				if (numberOfPressedKeys () > 1 || hasAnyJoystickOrPaddleLineLow ())
					propagateKeyboardLowLines (portA, portB);

				_portA = portA;
				_portB = portB;

				result = MCHEmul::UByte (_portB);
			}

			break;
			
		default:
			{
				result = COMMODORE::CIARegisters::readValue (pp);
			}

			break;
	}

	return (_lastValueRead = result);
}

// ---
void C64::CIA1Registers::initializeInternalValues ()
{
	COMMODORE::CIARegisters::initializeInternalValues ();
	
	// Data Port A all output...
	setValue (0x02, MCHEmul::UByte::_FF); 
	// Data Port B all input...
	setValue (0x03, MCHEmul::UByte::_0);
	// Just to be able to read well the keyboard...

	// The status of the different controls managed from the CIA1...
	// No switches clicked, no fire buttons pressed...
	_joystickStatus [0] = _joystickStatus [1] = 0xff; 
	// No keys pressed...
	for (size_t i = 0; i < 8; i++)
		_keyboardStatusMatrix [i] = _rev_keyboardStatusMatrix [i] = MCHEmul::UByte::_FF; 
	// No paddle connected...
	disconnectAllPaddles ();
}

// ---
void C64::CIA1Registers::propagateKeyboardLowLines (unsigned char& portA, unsigned char& portB) const
{
	bool changed = true;
	while (changed)
	{
		changed = false;
	
		// If a line on Port A is low, propagate to Port B
		// using your code's original orientation:
		// _keyboardStatusMatrix[i] is used when scanning from Port A.
		for (size_t i = 0; i < 8; i++)
		{
			if ((portA & (1 << i)) == 0x00) // If key pressed...
			{
				const unsigned char newPortB = 
					(portB & _keyboardStatusMatrix [i].value ());
				if (newPortB != portB)
				{ 
					portB = newPortB;

					changed = true;
				}
			}
		}

		// If a line on Port B is low, propagate to Port A
		// using the inverse array that the implementation already maintains.
		for (size_t i = 0; i < 8; i++)
		{
			if ((portB & (1 << i)) == 0x00) // If key pressed...
			{
				const unsigned char newPortA = 
					(portA & _rev_keyboardStatusMatrix [i].value ());
				if (newPortA != portA)
				{
					portA = newPortA;

					changed = true;
				}
			}
		}
	}
}
