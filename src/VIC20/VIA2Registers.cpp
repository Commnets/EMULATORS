#include <VIC20/VIA2Registers.hpp>

// ---
VIC20::VIA2Registers::VIA2Registers (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: COMMODORE::VIARegisters (_VIA2_SUBSET, ps, pp, a, s)
	  // At this point all internal variables will have random values...
{ 
	setClassName ("VIA2Registers");

	initializeInternalValues ();
}

// ---
const MCHEmul::UByte& VIC20::VIA2Registers::readValue (size_t p) const
{
	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	if (_CA1 == nullptr ||
		_CA2 == nullptr ||
		_CB1 == nullptr ||
		_CB2 == nullptr ||
		_T1  == nullptr ||
		_T2  == nullptr ||
		_SR  == nullptr ||
		_PA  == nullptr ||
		_PB  == nullptr)
		return (_lastValueRead);

	size_t pp = p % 0x10;

	switch (pp)
	{
		// Port B...
		// Where the keyboard and the joystick right/east position is managed...
		case 0x00:
			{
				unsigned char m = 0x01;
				unsigned char dt = MCHEmul::UByte::_0;
				unsigned char msk = (_PA -> OR ().value () | ~_PA -> DDR ().value ()) &
					(_joystickStatus | 0x7f /** Only the bit 7 (right/east) is managed in this VIA, 
												so the rest of the bits are considered as switched on. */);
				for (size_t i = 0; i < 8; m <<= 1, i++)
					if ((~msk & m) != 0x00)
						dt |= ~_rev_keyboardStatusMatrix [i].value (); // 1 if clicked...
				_PB -> setPortValue ((_PB -> OR ().value () | ~_PB -> DDR ().value ()) & ~dt);
				// To considere the impact in the "ControlLines" when reading this record!
				result = _PB -> value (true);
			}

			break;

		case 0x01:
		case 0x0f:
			{
				// First of all, treat to get what should be in the Port B,
				// this is dt, that includes the usual value (considering the situation of the timer)
				MCHEmul::UByte o = _PB -> OR ();
				if ((_T2 -> runMode () == COMMODORE::VIATimer::RunMode::_ONESHOOTSIGNAL ||
					 _T2 -> runMode () == COMMODORE::VIATimer::RunMode::_CONTINUOUSSIGNAL) &&
					_PB -> DDR ().bit (7))
					o.setBit (7, _PB -> p7 ());
				unsigned char msk = (o.value () | ~_PB -> DDR ().value ());
				unsigned char m = 0x01;
				unsigned char dt = MCHEmul::UByte::_0;
				for (size_t i = 0; i < 8; m <<= 1, i++)
					if ((~msk & m) != 0x00)
						dt |= ~_keyboardStatusMatrix [i].value ();  // 1 if clicked...
				_PA -> setPortValue (result = ((_PA -> OR ().value () | ~_PA -> DDR ().value ()) & ~dt));
				// To considere (or not) the impact in the "ControlLines" when reading this record!
				result = _PA -> value (pp == 0x01);
			}

			break;
			
		default:
			{
				result = COMMODORE::VIARegisters::readValue (pp);
			}

			break;
	}

	return (_lastValueRead = result);
}

// ---
void VIC20::VIA2Registers::initializeInternalValues ()
{
	COMMODORE::VIARegisters::initializeInternalValues ();

	if (_CA1 == nullptr ||
		_CA2 == nullptr ||
		_CB1 == nullptr ||
		_CB2 == nullptr ||
		_T1  == nullptr ||
		_T2  == nullptr ||
		_SR  == nullptr ||
		_PA  == nullptr ||
		_PB  == nullptr)
		return;
	
	// Data Port B all input...
	setValue (0x00, MCHEmul::UByte::_FF); 
	// Data Port A all output...
	setValue (0x01, MCHEmul::UByte::_0);
	// Just to be able to read well the keyboard...

	_joystickStatus = 0xff; // No switches clicked, no fire buttons pressed...
	for (size_t i = 0; i < 8; i++)
		_keyboardStatusMatrix [i] = _rev_keyboardStatusMatrix [i] = MCHEmul::UByte::_FF; // No keys pressed...
}
