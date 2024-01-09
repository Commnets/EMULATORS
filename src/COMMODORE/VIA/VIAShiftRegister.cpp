#include <COMMODORE/VIA/VIAShiftRegister.hpp>
#include <COMMODORE/VIA/VIAControlLine.hpp>
#include <COMMODORE/VIA/VIATimer.hpp>

// ---
void COMMODORE::VIAShiftRegister::initialize ()
{
	_mode = COMMODORE::VIAShiftRegister::ShiftMode::_DISABLE;

	_value = MCHEmul::UByte::_0;

	_interruptEnabled = _interruptRequested = false;

	_lastCPUCycles = 0;

	_numberBits = 0;

	_disable = true;

	_justReadOrWritten = false;

	_CB1Pulse.set (false);
}


// ---
bool COMMODORE::VIAShiftRegister::simulate (MCHEmul::CPU* cpu)
{
	if (_lastCPUCycles == 0)
	{
		_lastCPUCycles = cpu -> lastCPUClockCycles ();

		return (true);
	}

	for (unsigned int i = (cpu -> lastCPUClockCycles () - _lastCPUCycles); i > 0; i--)
	{
		switch (_mode)
		{
			case (COMMODORE::VIAShiftRegister::ShiftMode::_DISABLE):
				break;

			case (COMMODORE::VIAShiftRegister::ShiftMode::_INUNDERTIMER2):
				{
					if (!_disable &&
						(_justReadOrWritten && 
							_interruptRequested.peekValue () /** To not change its value. */) ||
						(!_justReadOrWritten && _timer2 -> reaches0LSB () /** Just the LSB. */))
					{
						// Inverse the value of CB1...
						_CB1 -> setValue (!_CB1 -> value ()); 
						
						// Complete?
						if (shiftIn (_CB2 -> value ())) 
						{ 
							_disable = true; // Stops...

							_interruptRequested = true; // ...and Interrupt requested...
						}
					}
				}

				break;

			case (COMMODORE::VIAShiftRegister::ShiftMode::_INATCLOCK):
				{
					if (!_disable)
					{
						// Inverse the value of CB1...
						_CB1 -> setValue (!_CB1 -> value ()); 
						
						// Complete?
						if (shiftIn (_CB2 -> value ())) 
						{ 
							_disable = true; // Stops...

							_interruptRequested = true; // ...and interrupt requested...
						}
					}
				}

				break;

			case (COMMODORE::VIAShiftRegister::ShiftMode::_INUNDERSIGNALCB1):
				{
					// To keep track of any potential pulse in _CB1 line...
					_CB1Pulse.set (_CB1 -> value ());
					if ((_justReadOrWritten && 
							_interruptRequested.peekValue () /** To not change its value. */) ||
						(!_justReadOrWritten && _CB1Pulse.transition () /** Clears the pusle. */))
					{
						// Complete?
						if (shiftIn (_CB2 -> value ())) 
							_interruptRequested = true; // Interrupt requested...
					}
				}

				break;

			case (COMMODORE::VIAShiftRegister::ShiftMode::_OUTFREERUNNING):
				{
					if ((_justReadOrWritten && 
							_interruptRequested.peekValue () /** To not change its value. */) ||
						(!_justReadOrWritten && _timer2 -> reaches0LSB () /** Just the LSB. */))
					{
						// Inverse the value of CB1...
						_CB1 -> setValue (!_CB1 -> value ()); 

						bool o;
						bool r = shiftOut (o, true);
						_CB2 -> setValue (o);

						// Complete?
						if (r)
							_interruptRequested = true; // ...an interrupt requested...
					}
				}

				break;

			case (COMMODORE::VIAShiftRegister::ShiftMode::_OUTUNDERTIMER2):
				{
					if (!_disable &&
						(_justReadOrWritten && 
							_interruptRequested.peekValue () /** To not change its value. */) ||
						(!_justReadOrWritten && _timer2 -> reaches0LSB () /** Just the LSB. */))
					{
						// Inverse the value of CB1...
						_CB1 -> setValue (!_CB1 -> value ()); 

						bool o;
						bool r = shiftOut (o, false);
						_CB2 -> setValue (o);

						// Complete?
						if (r)
						{ 
							_disable = true; // Stops...

							_interruptRequested = true; // ...and interrupt requested...
						}
					}
				}

				break;

			case (COMMODORE::VIAShiftRegister::ShiftMode::_OUTATCLOCK):
				{
					if (!_disable)
					{
						// Inverse the value of CB1...
						_CB1 -> setValue (!_CB1 -> value ()); 

						bool o;
						bool r = shiftOut (o, false);
						_CB2 -> setValue (o);

						// Complete?
						if (r)
						{ 
							_disable = true; // Stops...

							_interruptRequested = true; // ...and interrupt requested...
						}
					}
				}

				break;

			case (COMMODORE::VIAShiftRegister::ShiftMode::_OUTUNDERSIGNALCB1):
				{
					// To keep track of any potential pulse in _CB1 line...
					_CB1Pulse.set (_CB1 -> value ());
					if ((_justReadOrWritten &&
							_interruptRequested.peekValue () /** To not change its value. */) ||
						(!_justReadOrWritten && _CB1Pulse.transition () /** Clears the pusle. */))
					{

						bool o;
						bool r = shiftOut (o, false);
						_CB2 -> setValue (o);

						// Complete?
						if (r)
							_interruptRequested = true; // Interrupt requested...
					}
				}

				break;

			default:
				// It shouldn't be here...
				assert (false);
				break;
		}
	}

	_justReadOrWritten = false;

	_lastCPUCycles = cpu -> lastCPUClockCycles ();

	return (true);
}

// ---
MCHEmul::InfoStructure COMMODORE::VIAShiftRegister::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	result.add ("MODE",			(unsigned char) _mode);
	result.add ("VALUE",		_value);
	result.add ("NUMBERBITS",	_numberBits);

	return (result);
}
