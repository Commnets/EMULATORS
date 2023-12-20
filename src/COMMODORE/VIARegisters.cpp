#include <COMMODORE/VIARegisters.hpp>

// ---
COMMODORE::VIARegisters::VIARegisters (int id, MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: MCHEmul::ChipRegisters (id, ps, pp, a, s),
	  _lastValueRead (MCHEmul::PhysicalStorage::_DEFAULTVALUE)
	  // At this point all internal variables will have random values...
{ 
	setClassName ("VIARegisters");

	initializeInternalValues (); 
}

// ---
void COMMODORE::VIARegisters::initialize ()
{
	MCHEmul::PhysicalStorageSubset::initialize (); 

	initializeInternalValues ();
}

// ---
void COMMODORE::VIARegisters::setValue (size_t p, const MCHEmul::UByte& v)
{
	MCHEmul::PhysicalStorageSubset::setValue (p, v);

	size_t pp = p % 0x10;

	switch (pp)
	{
		// Data Port Register A: CIAPRA
		case 0x00:
			{
				_outputRegA = v.value ();

				// The value of the portA needs to be reset...
				// If there were any previous value set in the portA, it will be overwritten...
				setPortA (0xff, false);
			}

			break;

		// Data Port Register B: CIAPRB
		case 0x01:
			{
				_outputRegB = v.value ();

				// The value of the portB needs to be reset...
				// If there were any previous value set in the portB, it will be overwritten...
				setPortB (0xff, false);
			}

			break;

		// Data Direction Register A: CIDDRA
		case 0x02:
			{
				_dataPortADir = v.value ();

				setPortA (0xff, false);
			}

			break;

		// Data Direction Register A: CIDDRB
		case 0x03:
			{
				_dataPortBDir = v.value ();

				setPortB (0xff, false);
			}

			break;

		// TODO
			
		default:
			break;
	}
}

// ---
const MCHEmul::UByte& COMMODORE::VIARegisters::readValue (size_t p) const
{
	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	size_t pp = p % 0x10;

	switch (pp)
	{
		case 0x00:
			{
				result = MCHEmul::UByte (_portA);
			}

			break;

		/** In the Data Port B a reflection of the timers could happen. */
		case 0x01:
			{
				result = MCHEmul::UByte (_portB);
			}

			break;

		case 0x02:
			{
				result = MCHEmul::UByte (_dataPortADir);
			}

			break;

		case 0x03:
			{
				result = MCHEmul::UByte (_dataPortBDir);
			}

			break;

		// TODO
			
		default:
			break;
	}

	return (_lastValueRead = result);
}

// ---
const MCHEmul::UByte& COMMODORE::VIARegisters::peekValue (size_t p) const
{
	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	size_t pp = p % 0x10;

	switch (pp)
	{
		// TODO
		
		default:
			result = readValue (pp);
			break;
	}

	return (_lastValueRead = result);
}

// ---
void COMMODORE::VIARegisters::initializeInternalValues ()
{
	//They have to be initialized in advanced as the value set depends also on the value at the beginning...
	_portA = _portB = MCHEmul::UByte::_FF; // As described in the documentation they all have a pull up resistor...
	// Same with the output registers
	_outputRegA = _outputRegB = MCHEmul::UByte::_FF;

	// The internal variables are initialized through the data in memory...

	// The direction is first set up to set up accodingly the values of the ports A and B...
	setValue (0x02, MCHEmul::UByte::_FF); // All lines output...
	setValue (0x03, MCHEmul::UByte::_0);  // All lines input...
	setValue (0x00, MCHEmul::UByte::_FF); // When they are no connected to nothing, the documentation sais that they are pulled up!
	setValue (0x01, MCHEmul::UByte::_FF); // same than previous one...

	// TODO

	_portA = _portB = 0xff; // They are not initially connected against anything...
}
