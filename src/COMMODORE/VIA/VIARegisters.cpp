#include <COMMODORE/VIA/VIARegisters.hpp>
#include <COMMODORE/VIA/VIATimer.hpp>
#include <COMMODORE/VIA/VIAShiftRegister.hpp>
#include <COMMODORE/VIA/VIAPort.hpp>

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

	size_t pp = p % 0x10;

	switch (pp)
	{
		// Data Port Register B: VIA?PB
		case 0x00:
			{
				_PB -> setValue (v.value ());
			}

			break;

		// Data Port Register A: VIA?PA1
		case 0x01:
			{
				_PA -> setValue (v.value ());
			}

			break;

		// Data Direction Register B: VIA?DDRB
		case 0x02:
			{
				_PB -> setDDR (v.value ());
			}

			break;

		// Data Direction Register B: VIA?DDRA
		case 0x03:
			{
				_PA -> setDDR (v.value ());
			}

			break;

		// LSB of the latch Timer A: VIA?T1CL
		case 0x04:
		// LSB of the counter Timer A: VIA?T1LL
		// Notice that setting this register or 0x06 the result is the same...
		case 0x06: 
			{
				_T1 -> setInitialValue 
					((_T1 -> initialValue () & 0xff00) | (unsigned short) v.value ());
			}

			break;

		// MSB of the latch Timer A: VIA?T1CH
		case 0x05:
		// MSB of the latch Timer A: VIA1T1LH
		// Notice that in this case, the behaviour is only partially equivalent!
		case 0x07:
			{
				_T1 -> setInitialValue 
					((_T1 -> initialValue () & 0x00ff) | (unsigned short) (v.value () << 8));

				// The flag of the interruption associated to the timer is cleared...
				_T1 -> interruptRequested (); // (obool) Just doing this is done...
				// ...and also when the register accesed is the 0x05, the counter is initialized...
				if (pp == 0x05)
				{ 
					_T1 -> reset ();
					_T1 -> start ();
				}
			}

			break;

		// LSB of the latch Timer B: VIA?T2CL
		case 0x08:
			{
				_T2 -> setInitialValue 
					((_T2 -> initialValue () & 0xff00) | (unsigned short) v.value ());
			}

			break;

		// MSB of the latch Timer B: VIA?T2CH
		case 0x09:
			{
				_T2 -> setInitialValue 
					((_T2 -> initialValue () & 0x00ff) | (unsigned short) (v.value () << 8));

				// The flag of the interruption requested is cleared...
				_T2 -> interruptRequested (); // Just doing this is done...
				// ...and also the counter is initialized...
				_T2 -> reset ();
				// ...and start to count...
				_T2 -> start ();
			}

			break;

		// Shift Register: VIA?SR
		case 0x0a:
			{
				// What happens there will depen on the shifting mode...
				// e.g if it disable won't happen nothing...
				_SR -> setValue (v);
			}

			break;

		// The auxiliary control register: VIA?ACR
		// Control timers & shift register behaviour and also whether CB1 and CA1 status reflects in the ports
		case 0x0b:
			{
				//The behaviour of the Timer 1 is controlled with bits 7 and 6...
				_T1 -> setCountAndRunMode (
					COMMODORE::VIATimer::CountMode::_PROCESSORCYCLES, // Always...
					(COMMODORE::VIATimer::RunMode) ((unsigned char) ((v.value () & 0xd0) >> 6 /** From 0 to 3 */)));
				if (_T1 -> runMode () == COMMODORE::VIATimer::RunMode::_ONESHOOTSIGNAL ||
					_T1 -> runMode () == COMMODORE::VIATimer::RunMode::_CONTINUOUSSIGNAL)
					_PB -> setP7 (true); // Reflect the status in the bit 7 of the port linked...
										 // The initial vwlue is true. The pulse is when false!

				// For the Timer 2 the behaviour will be different
				// depending onthe value of the bit 5 only...
				if (v.bit (5))
					_T2 -> setCountAndRunMode (
						COMMODORE::VIATimer::CountMode::_PULSERECEIVED, // Count pulse...
						COMMODORE::VIATimer::RunMode::_CONTINUOUS); // ...in a continuous way...
				else
					_T2 -> setCountAndRunMode (
						COMMODORE::VIATimer::CountMode::_PROCESSORCYCLES, // Count every processor cycle...
						COMMODORE::VIATimer::RunMode::_ONESHOOT); // ...and just once.

				// The way the shift register works is controlled with bit 2 - 4
				_SR -> setMode 
					(COMMODORE::VIAShiftRegister::ShiftMode ((v.value () >> 2) && 0x07 /** bits 2, 3 and 4 */));

				// The info of the ports might or not be latched...
				_PB -> setLatchIR (v.bit (1));
				_PA -> setLatchIR (v.bit (0));
			}

			break;

		// Peripheral control register for handshaking: VIA?PCR
		// Options to control CA1, CA2, CB1 y CB2 lines....
		case 0x0c:
			{
				_CA1 -> setMode (v.bit (0) ? 1 : 0); // 0 or 1
				_CA2 -> setMode ((v.value () & 0x0f) >> 1); // From 0 to 7
				_CB1 -> setMode (v.bit (4) ? 1 : 0); // 0 or 1
				_CB2 -> setMode ((v.value () & 0xf0 /** bits 7,6,5 */) >> 5); // From 0 to 7
			}

			break;

		// Interrupt flag register: VIA?IFR
		// The register bits are "set" when IRQ conditions happens!
		// The register 
		case 0x0d:
			{
				// The interrupt flag is cleared writting down a 1 into it...
				if (v.bit (6)) _T1  -> interruptRequested ();
				if (v.bit (5)) _T2  -> interruptRequested ();
				if (v.bit (4)) _CB1 -> interruptRequested ();
				if (v.bit (3)) _CB2 -> interruptRequested ();
				if (v.bit (2)) _SR  -> interruptRequested ();
				if (v.bit (1)) _CA1 -> interruptRequested ();
				if (v.bit (0)) _CA2 -> interruptRequested ();
			}

			break;

		// Interrupt enable register: VIA?IER
		case 0x0e:
			{
				// Bit 7 is the "set" bit, to enable or disable the interrupts...
				bool s = v.bit (7); 
				if (v.bit (6)) _T1  -> setInterruptEnabled (s);
				if (v.bit (5)) _T2  -> setInterruptEnabled (s);
				if (v.bit (4)) _CB1 -> setInterruptEnabled (s);
				if (v.bit (3)) _CB2 -> setInterruptEnabled (s);
				if (v.bit (2)) _SR  -> setInterruptEnabled (s);
				if (v.bit (1)) _CA1 -> setInterruptEnabled (s);
				if (v.bit (0)) _CA2 -> setInterruptEnabled (s);
			}

			break;

		// VIA?PA2
		// Same tha 0x01, but not affecting the status of the "ControlLines"...
		case 0x0f:
			{
				_PA -> setValue (v.value (), false /** Not to affect "ControlLines". */);
			}

		default:
			break;
	}
}

// ---
const MCHEmul::UByte& COMMODORE::VIARegisters::readValue (size_t p) const
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
				result = _PB -> value ();
			}

			break;

		case 0x01:
			{
				result = _PA -> value ();
			}

			break;

		case 0x02:
			{
				result = _PB -> DDR ();
			}

			break;

		case 0x03:
			{
				result = _PA -> DDR ();
			}

			break;

		case 0x04:
			{
				result = MCHEmul::UByte 
					((unsigned char) (_T1 -> currentValue () & 0x00ff));

				// The interrupt flag is also cleared when reading...
				_T1 -> interruptRequested (); 
			}

			break;

		case 0x05:
			{
				result = MCHEmul::UByte 
					((unsigned char) ((_T1 -> currentValue () & 0xff00) >> 8));
			}

			break;

		case 0x06:
			{
				result = MCHEmul::UByte 
					((unsigned char) (_T1 -> initialValue () & 0x00ff));
			}

			break;

		case 0x07:
			{
				result = MCHEmul::UByte 
					((unsigned char) ((_T1 -> initialValue () & 0xff00) >> 8));
			}

			break;

		case 0x08:
			{
				result = MCHEmul::UByte 
					((unsigned char) (_T2 -> currentValue () & 0x00ff));

				// The interrupt flag is also cleared when reading...
				_T2 -> interruptRequested (); 
			}

			break;

		case 0x09:
			{
				result = MCHEmul::UByte 
					((unsigned char) ((_T2 -> currentValue () & 0xff00) >> 8));
			}

			break;

		case 0x0a:
			{
				result = _SR -> value ();
			}

			break;

		case 0x0b:
			{
				result = 
					(((unsigned char) (_T1 -> runMode ())) << 6) | // Bits 7 & 6
					((_T2 -> runMode () == COMMODORE::VIATimer::RunMode::_CONTINUOUS) ? 0x20 : 0x00) | // Bit 5
					(((unsigned char) (_SR -> mode ())) << 2) | // Bits 4, 3, 2
					(_PB -> latchIR () ? 0x02 : 0x00) | // Bit 1
					(_PA -> latchIR () ? 0x01 : 0x00); // Bit 0
			}

			break;

		case 0x0c:
			{
				result = 
					((_CA1 -> mode () == 1 ) ? 0x1 : 0x00) | // bit 0
					(_CA2 -> mode () << 1) | // bits 1,2,3
					((_CB1 -> mode () == 1) ? 0x20 : 0x00) | // bit 4
					(_CB2 -> mode () << 5); // bits 5,6,7
			}

			break;

		case 0x0d:
			{
				result = MCHEmul::UByte::_0;
				result.setBit (7, launchInterrupt ()); // Any Interrupt?
				// Reading any bit clears the flag...
				result.setBit (6, _T1  -> interruptRequested ()); // in Timer A?
				result.setBit (5, _T2  -> interruptRequested ()); // in Timer B?
				result.setBit (4, _CB1 -> interruptRequested ()); // CB1 transition?
				result.setBit (3, _CB2 -> interruptRequested ()); // CB2 transition?
				result.setBit (2, _SR  -> interruptRequested ()); // In the shift register?
				result.setBit (1, _CA1 -> interruptRequested ()); // CA1 transition?
				result.setBit (0, _CA2 -> interruptRequested ()); // CA2 transition?
			}

		case 0x0e:
			{
				result = MCHEmul::UByte::_0;
				// Bit 7 is always 0...
				result.setBit (6, _T1  -> interruptEnabled ());
				result.setBit (5, _T2  -> interruptEnabled ());
				result.setBit (4, _CB1 -> interruptEnabled ());
				result.setBit (3, _CB2 -> interruptEnabled ());
				result.setBit (2, _SR  -> interruptEnabled ());
				result.setBit (1, _CA1 -> interruptEnabled ());
				result.setBit (0, _CA2 -> interruptEnabled ());
			}

			break;

		case 0x0f:
			{
				result = _PA -> value (false); // Not to clear the control lines interrupt status...
			}

			break;
			
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
		case 0x04:
			{
				result = MCHEmul::UByte ((unsigned char) (_T1 -> currentValue () & 0x00ff));
			}

			break;

		case 0x08:
			{
				result = MCHEmul::UByte ((unsigned char) (_T2 -> currentValue () & 0x00ff));
			}

			break;

		case 0x0d:
			{
				result = MCHEmul::UByte::_0;
				result.setBit (7, launchInterrupt ());
				result.setBit (6, _T1 -> peekInterruptRequested ());
				result.setBit (5, _T2 -> peekInterruptRequested ());

				// TODO
			}

			break;
		
		default:
			result = readValue (pp);
			break;
	}

	return (_lastValueRead = result);
}

// ---
void COMMODORE::VIARegisters::initializeInternalValues ()
{
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

	// The internal variables are initialized through the data in memory...

	// The direction is first set up to set up accodingly the values of the ports A and B...
	setValue (0x02, 0xff);	// All lines output...
	setValue (0x03, 0x00);	// All lines input...
	setValue (0x00, 0xff);	// When they are no connected to nothing, the documentation says that they are pulled up!
	setValue (0x01, 0xff);	// same than previous one...
	setValue (0x04, 0x00);  // T1 LSB
	/**
		T1 MSB
		This instruction can not be done, becuase it would start the Timer 1! 
		setValue (0x05, 0x00);
	*/
	setValue (0x06, 0x00);	// T1 Latch LSB
	setValue (0x07, 0x00);	// T1 Latch MSB
	setValue (0x08, 0x00);	// T2 LSB and Latch LSB
	/**
		T2 MSB
		This instruction can not be done, becuase it would start the Timer 2! 
		setValue (0x09, 0x00);
	*/
	setValue (0x0a, 0x00);	// SR = 0
	setValue (0x0b, 0x40);	// ACR (T1 = free running, T2 = single interval timing, SR disabled, PA and PB no latch)
	setValue (0x0c, 0xef);  // PCR (CB2 = output mode manual, CB1 = interrupt high to low transition,... 
							// ...CA2 = output mode normal, CA1 = high to low transition)
	setValue (0x0d, 0x00);	// IFR. No interrupts so far...
	setValue (0x0e, 0x00);	// IER. No interrupts so far...
	setValue (0x0f, 0x00);	// Has no impact when setting...

	_T1 -> stop ();
	_T2 -> stop ();
}
