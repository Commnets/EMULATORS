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
		// The Port B is used for many other things not still implemented...

		// Port A
		case 0x01:
			{
				// First of all, treat to get what should be in the Port B,
				// this is dt, that includes the usual value (considering the situation of the timer)
				// and also the situation of the joystick except the right/east position that is managed in VIA2...
				MCHEmul::UByte o = _PB -> OR ();
				if ((_T2 -> runMode () == COMMODORE::VIATimer::RunMode::_ONESHOOTSIGNAL ||
					 _T2 -> runMode () == COMMODORE::VIATimer::RunMode::_CONTINUOUSSIGNAL) &&
					_PB -> DDR ().bit (7))
					o.setBit (7, _PB -> p7 ());
				unsigned char dt = (o.value () | ~_PB -> DDR ().value ()) &
					(_joystickStatus | 0x80	/** The bit 7 (right/east) is managed in VIA2, 
												so here it is always considered switched on. */);
				// The value in the portA...
				_PA -> setPortValue ((_PA -> OR ().value () | ~_PA -> DDR ().value ()) & ~dt);
				// This method is invoked, 
				// to considere the impact in the ControlLines when reading this record!
				result = _PA -> value (true);
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

