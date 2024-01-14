#include <COMMODORE/VIA/VIAShiftRegister.hpp>
#include <COMMODORE/VIA/VIAControlLines.hpp>
#include <COMMODORE/VIA/VIATimer.hpp>

// ---
COMMODORE::VIAShiftRegister::VIAShiftRegister (int id)
	: MCHEmul::InfoClass ("VIAShiftRegister"),
	  _id (id),
	  _mode (COMMODORE::VIAShiftRegister::ShiftMode::_DISABLE),
	  _value (MCHEmul::UByte::_0),
	  _T (nullptr), _CL1 (nullptr), _CL2 (nullptr),
	  _interruptEnabled (false),
	  _interruptRequested (false)
{ 
	initialize (); // To assign the implementation variables...
}

// ---
void COMMODORE::VIAShiftRegister::initialize ()
{
	_mode = COMMODORE::VIAShiftRegister::ShiftMode::_DISABLE;

	_value = MCHEmul::UByte::_0;

	_interruptEnabled = _interruptRequested = false;

	_numberBits = 0;

	_disable = true;

	_justReadOrWritten = false;
}


// ---
bool COMMODORE::VIAShiftRegister::simulate (MCHEmul::CPU* cpu)
{
	switch (_mode)
	{
		case (COMMODORE::VIAShiftRegister::ShiftMode::_DISABLE):
			break;

		case (COMMODORE::VIAShiftRegister::ShiftMode::_INUNDERTIMER):
			{
				assert (_T != nullptr &&
						_CL1 != nullptr && _CL2 != nullptr);

				if (!_disable &&
					((_justReadOrWritten && 
						_interruptRequested.peekValue () /** To not change its value. */) ||
					(!_justReadOrWritten && _T -> reaches0LSB () /** Just the LSB. */)))
				{
					// Inverse the value of CL1...
					_CL1 -> setValue (!_CL1 -> value ()); 
						
					// Complete?
					if (shiftIn (_CL2 -> value ())) 
					{ 
						_disable = true; // Stops...

						_interruptRequested = true; // ...and Interrupt requested...
					}
				}
			}

			break;

		case (COMMODORE::VIAShiftRegister::ShiftMode::_INATCLOCK):
			{
				assert (_CL1 != nullptr && _CL2 != nullptr);

				if (!_disable)
				{
					// Inverse the value of CL1...
					_CL1 -> setValue (!_CL1 -> value ()); 
						
					// Complete?
					if (shiftIn (_CL2 -> value ())) 
					{ 
						_disable = true; // Stops...

						_interruptRequested = true; // ...and interrupt requested...
					}
				}
			}

			break;

		case (COMMODORE::VIAShiftRegister::ShiftMode::_INUNDERSIGNALCL1):
			{
				assert (_CL1 != nullptr && _CL2 != nullptr);

				if ((_justReadOrWritten && // Never stops (_disable is not used)
						_interruptRequested.peekValue () /** To not change its value. */) ||
					(!_justReadOrWritten && _CL1 -> peekTransition ()))
				{
					// Complete?
					if (shiftIn (_CL2 -> value ())) 
						_interruptRequested = true; // Interrupt requested...
				}
			}

			break;

		case (COMMODORE::VIAShiftRegister::ShiftMode::_OUTFREERUNNING):
			{
				assert (_T != nullptr && 
						_CL1 != nullptr && _CL2 != nullptr);

				if ((_justReadOrWritten && 
						_interruptRequested.peekValue () /** To not change its value. */) ||
					(!_justReadOrWritten && _T -> reaches0LSB () /** Just the LSB. */))
				{
					// Inverse the value of CL1...
					_CL1 -> setValue (!_CL1 -> value ()); 

					bool o;
					bool r = shiftOut (o, true);
					_CL2 -> setValue (o);

					// Complete?
					if (r)
						_interruptRequested = true; // ...an interrupt requested...
				}
			}

			break;

		case (COMMODORE::VIAShiftRegister::ShiftMode::_OUTUNDERTIMER):
			{
				assert (_T != nullptr && 
						_CL1 != nullptr && _CL2 != nullptr);

				if (!_disable &&
					((_justReadOrWritten && 
						_interruptRequested.peekValue () /** To not change its value. */) ||
					(!_justReadOrWritten && _T -> reaches0LSB () /** Just the LSB. */)))
				{
					// Inverse the value of CL1...
					_CL1 -> setValue (_CL1 -> value ()); 

					bool o;
					bool r = shiftOut (o, false);
					_CL2 -> setValue (o);

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
				assert (_CL1 != nullptr && _CL2 != nullptr);

				if (!_disable)
				{
					// Inverse the value of CB1...
					_CL1 -> setValue (!_CL1 -> value ()); 

					bool o;
					bool r = shiftOut (o, false);
					_CL2 -> setValue (o);

					// Complete?
					if (r)
					{ 
						_disable = true; // Stops...

						_interruptRequested = true; // ...and interrupt requested...
					}
				}
			}

			break;

		case (COMMODORE::VIAShiftRegister::ShiftMode::_OUTUNDERSIGNALCL1):
			{
				assert (_CL1 != nullptr && _CL2 != nullptr);

				if ((_justReadOrWritten && // Never stops. _disable is not used.
						_interruptRequested.peekValue () /** To not change its value. */) ||
					(!_justReadOrWritten && _CL1 -> peekTransition ()))
				{

					bool o;
					bool r = shiftOut (o, false);
					_CL2 -> setValue (o);

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

	_justReadOrWritten = false;

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
