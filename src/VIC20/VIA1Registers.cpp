#include <VIC20/VIA1Registers.hpp>

// ---
VIC20::VIA1Registers::VIA1Registers (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: COMMODORE::VIARegisters (_VIA1_SUBSET, ps, pp, a, s)
{ 
	setClassName ("VIA1Registers");

	initializeInternalValues ();
}

// ---
const MCHEmul::UByte& VIC20::VIA1Registers::readValue (size_t p) const
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
		case 0x00:
			{
				MCHEmul::UByte o = _PA -> OR ();
				if (_T2 -> runMode () == COMMODORE::VIATimer::RunMode::_ONESHOOTSIGNAL ||
					_T2 -> runMode () == COMMODORE::VIATimer::RunMode::_CONTINUOUSSIGNAL)
					o.setBit (7, _PA -> p7 ());
				unsigned char dt = (o.value () | ~_PA -> DDR ().value ()) &
					(_joystickStatus | 0x7f /** Only bit 7 is read in this port, so the rest of the bits for joystick should be set. */);
				_PB -> setPortValue ((_PB -> OR ().value () | ~_PB -> DDR ().value ()) & ~dt);
				// To considere the impact in the ControlLines when reading this record!
				result = _PB -> value (true);
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
void VIC20::VIA1Registers::initializeInternalValues ()
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
}

