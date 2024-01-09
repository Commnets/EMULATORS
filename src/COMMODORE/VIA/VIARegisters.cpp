#include <COMMODORE/VIA/VIARegisters.hpp>
#include <COMMODORE/VIA/VIATimer.hpp>
#include <COMMODORE/VIA/VIAShiftRegister.hpp>

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
		// Data Port Register B: VIA?PB
		case 0x00:
			{
				_outputRegB = v.value ();

				// The value of the portB needs to be reset...
				// If there were any previous value set in the portB, it will be overwritten...
				setPortB (0xff, false);
			}

			break;

		// Data Port Register A: VIA?PA1 & VIA?PA2
		// The behaviour is almost the same.
		// The only difference is in the reflection of the CA? lines...
		case 0x01:
		case 0x0f:
			{
				_outputRegA = v.value ();

				// The value of the portA needs to be reset...
				// If there were any previous value set in the portA, it will be overwritten...
				setPortA (0xff, false);
			}

			break;

		// Data Direction Register B: VIA?DDRB
		case 0x02:
			{
				_dataPortBDir = v.value ();

				setPortB (0xff, false);
			}

			break;

		// Data Direction Register B: VIA?DDRA
		case 0x03:
			{
				_dataPortADir = v.value ();

				setPortA (0xff, false);
			}

			break;

		// LSB of the latch Timer A: VIA?T1CL
		case 0x04:
		// LSB of the counter Timer A: VIA?T1LL
		// Notice that setting this register or 0x06 the result is the same...
		case 0x06: 
			{
				_timerA -> setInitialValue 
					((_timerA -> initialValue () & 0xff00) | (unsigned short) v.value ());
			}

			break;

		// MSB of the latch Timer A: VIA?T1CH
		case 0x05:
		// MSB of the latch Timer A: VIA1T1LH
			// Notice that in this case, the behaciour is only partially equivalent!
		case 0x07:
			{
				_timerA -> setInitialValue 
					((_timerA -> initialValue () & 0x00ff) | (unsigned short) (v.value () << 8));

				// The flag of the interruption associated to the timer is cleared...
				_timerA -> interruptRequested (); // (obool) Just doing this is done...
				// ...and also when the register accesed is the 0x05, the counter is initialized...
				if (pp == 0x05)
					_timerA -> reset ();
			}

			break;

		// LSB of the latch Timer B: VIA?T2CL
		case 0x08:
			{
				_timerB -> setInitialValue 
					((_timerB -> initialValue () & 0xff00) | (unsigned short) v.value ());
			}

		// MSB of the latch Timer B: VIA?T2CH
		case 0x09:
			{
				_timerB -> setInitialValue 
					((_timerB -> initialValue () & 0x00ff) | (unsigned short) (v.value () << 8));

				// The flag of the interruption requested is cleared...
				_timerB -> interruptRequested (); // Just doing this is done...
				// ...and also the counter is initialized...
				_timerB -> reset ();
			}

			break;

		// Shift Register: VIA?SR
		case 0x0a:
			{
				// What happens there will depen on the shifting mode...
				// e.g if it disable won't happen nothing...
				_shiftRegister -> setValue (v);
			}

			break;

		// The auxiliary control register: VIA?ACR
		// Control timers & shift register behaviour and also whether CB1 and CA1 status reflects in the ports
		case 0x0b:
			{
				_timerA -> setCountMode (COMMODORE::VIATimer::CountMode::_PROCESSORCYCLES); // Always...
				_timerA -> setRunMode ((COMMODORE::VIATimer::RunMode) 
					((unsigned char) ((v.value () & 0xd0) >> 6))); // the bit 7 and 6 determines the RunMode...

				// For the Timer B the behaviour will be different
				// depending onthe value of the bit 5...
				if (v.bit (5))
				{ 
					// Count pulse...
					_timerB -> setCountMode 
						(COMMODORE::VIATimer::CountMode::_PULSERECEIVED);
					_timerB -> setRunMode 
						(COMMODORE::VIATimer::RunMode::_CONTINUOUS);
				}
				else
				{
					// Like the basic behaviour of the timer A...
					_timerB -> setCountMode 
						(COMMODORE::VIATimer::CountMode::_PROCESSORCYCLES);
					_timerB -> setRunMode 
						(COMMODORE::VIATimer::RunMode::_ONESHOOT);
				}

				// The way the shift register works is controlled with bit 2 - 4
				_shiftRegister -> setMode 
					(COMMODORE::VIAShiftRegister::ShiftMode ((v.value () >> 2) && 0x07 /** bits 2, 3 and 4 */));

				// The info of the ports might ot not be latched...
				_latchBEnabled = v.bit (1);
				_latchAEnabled = v.bit (0);
			}

			break;

		// Peripheral control register for handshaking: VIA?PCR
		// Optios to control CA1, CA2, CB1 y CB2 lines....
		case 0x0c:
			{
				_CA1 -> setMode (v.bit (0) ? 1 : 0); // 1 or 0
				_CA2 -> setMode ((v.value () & 0x0f /** bits 1,2,3 */) >> 1); // From 0 to 7
				_CB1 -> setMode (v.bit (4) ? 1 : 0); // 1 or 0
				_CB2 -> setMode ((v.value () & 0xf0 /** bits 5,6,7 */) >> 5); // From 0 to 7
			}

			break;

		// Interrupt flag register: VIA?IFR
		// Only makes sense when reading...
		// The register bits are "set" when IRQ conditions happens!
		case 0x0d:
			break;

		// Interrupt enable register: VIA?IER
		case 0x0e:
			{
				if (v.bit (7)) 
				{
					if (v.bit (6)) _timerA -> setInterruptEnabled (true);
					if (v.bit (5)) _timerB -> setInterruptEnabled (true);
					if (v.bit (4)) _CB1 -> setInterruptEnabled (true);
					if (v.bit (3)) _CB2 -> setInterruptEnabled (true);
					if (v.bit (2)) _shiftRegister -> setInterruptEnabled (true);
					if (v.bit (1)) _CA1 -> setInterruptEnabled (true);
					if (v.bit (0)) _CA2 -> setInterruptEnabled (true);
				}
				else
				{
					if (v.bit (6)) _timerA -> setInterruptEnabled (false);
					if (v.bit (5)) _timerB -> setInterruptEnabled (false);
					if (v.bit (4)) _CB1 -> setInterruptEnabled (false);
					if (v.bit (3)) _CB2 -> setInterruptEnabled (false);
					if (v.bit (2)) _shiftRegister -> setInterruptEnabled (false);
					if (v.bit (1)) _CA1 -> setInterruptEnabled (false);
					if (v.bit (0)) _CA2 -> setInterruptEnabled (false);
				}
			}

			break;

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
				result = MCHEmul::UByte (_portB);
			}

			break;

		case 0x01:
		// Reading this register the CA lines won't be reflected into the rasult...
		case 0x0f:
			{
				result = MCHEmul::UByte (_portA);
			}

			break;

		case 0x02:
			{
				result = MCHEmul::UByte (_dataPortBDir);
			}

			break;

		case 0x03:
			{
				result = MCHEmul::UByte (_dataPortADir);
			}

			break;

		case 0x04:
			{
				result = MCHEmul::UByte 
					((unsigned char) (_timerA -> currentValue () & 0x00ff));

				// The interrupt flag is also cleared when reading...
				_timerA -> interruptRequested (); 
			}

			break;

		case 0x05:
			{
				result = MCHEmul::UByte 
					((unsigned char) ((_timerA -> currentValue () & 0xff00) >> 8));
			}

			break;

		case 0x06:
			{
				result = MCHEmul::UByte 
					((unsigned char) (_timerA -> initialValue () & 0x00ff));
			}

			break;

		case 0x07:
			{
				result = MCHEmul::UByte 
					((unsigned char) ((_timerA -> initialValue () & 0xff00) >> 8));
			}

			break;

		case 0x08:
			{
				result = MCHEmul::UByte 
					((unsigned char) (_timerB -> currentValue () & 0x00ff));

				// The interrupt flag is also cleared when reading...
				_timerB -> interruptRequested (); 
			}

			break;

		case 0x09:
			{
				result = MCHEmul::UByte 
					((unsigned char) ((_timerB -> currentValue () & 0xff00) >> 8));
			}

			break;

		case 0x0a:
			{
				result = _shiftRegister -> value ();
			}

			break;

		case 0x0b:
			{
				result = 
					(((unsigned char) (_timerA -> runMode ())) << 6) | // Bits 7 & 6
					((_timerB -> runMode () == COMMODORE::VIATimer::RunMode::_CONTINUOUS) ? 0x20 : 0x00) | // Bit 5
					(((unsigned char) (_shiftRegister -> mode ())) << 2) | // Bits 4, 3, 2
					(_latchBEnabled ? 0x02 : 0x00) | // Bit 1
					(_latchAEnabled ? 0x01 : 0x00); // Bit 0
			}

			break;

		case 0x0c:
			{
				result = 
					((_CA1 -> mode () == 1) ? 0x1 : 0x00) | // bit 0
					(_CA2 -> mode () << 1) | // bits 1,2,3
					((_CB1 -> mode () == 1) ? 0x20 : 0x00) | // bit 4
					(_CB2 -> mode () << 5); // bits 5,6,7
			}

			break;

		case 0x0d:
			{
				result = MCHEmul::UByte::_0;
				result.setBit (7, launchInterrupt ()); // Any Interrupt?
				result.setBit (6, _timerA -> interruptRequested ()); // in Timer A?
				result.setBit (5, _timerB -> interruptRequested ()); // in Timer B?
				result.setBit (4, _CB1 -> interruptRequested ()); // CB1 transition?
				result.setBit (3, _CB2 -> interruptRequested ()); // CB2 transition?
				result.setBit (2, _shiftRegister -> interruptRequested ()); // In the shift register?
				result.setBit (1, _CA1 -> interruptRequested ()); // CA1 transition?
				result.setBit (0, _CA2 -> interruptRequested ()); // CA2 transition?
			}

		case 0x0e:
			{
				result = MCHEmul::UByte::_0;
				result.setBit (6, _timerA -> interruptEnabled ());
				result.setBit (5, _timerB -> interruptEnabled ());
				result.setBit (4, _CB1 -> interruptEnabled ());
				result.setBit (3, _CB2 -> interruptEnabled ());
				result.setBit (2, _shiftRegister -> interruptEnabled ());
				result.setBit (1, _CA1 -> interruptEnabled ());
				result.setBit (0, _CA2 -> interruptEnabled ());
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
				result = MCHEmul::UByte ((unsigned char) (_timerA -> currentValue () & 0x00ff));
			}

			break;

		case 0x08:
			{
				result = MCHEmul::UByte ((unsigned char) (_timerB -> currentValue () & 0x00ff));
			}

			break;

		case 0x0d:
			{
				result = MCHEmul::UByte::_0;
				result.setBit (7, launchInterrupt ());
				result.setBit (6, _timerA -> peekInterruptRequested ());
				result.setBit (5, _timerB -> peekInterruptRequested ());

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
	setValue (0x04, MCHEmul::UByte::_0);
	/**
		This instruction can not be done, becuase it would start the Timer A! 
		setValue (0x05, MCHEmul::UByte::_0);
	*/
	setValue (0x06, MCHEmul::UByte::_0);
	setValue (0x07, MCHEmul::UByte::_0);
	setValue (0x08, MCHEmul::UByte::_0);
	/**
		This instruction can not be done, becuase it would start the Timer A! 
		setValue (0x09, MCHEmul::UByte::_0);
	*/
	setValue (0x0a, MCHEmul::UByte::_0);
	setValue (0x0b, MCHEmul::UByte::_0);
	setValue (0x0c, MCHEmul::UByte::_0);
	setValue (0x0d, MCHEmul::UByte::_0);
	setValue (0x0e, MCHEmul::UByte::_0);
	setValue (0x0f, MCHEmul::UByte::_0);

	_portA = _portB = 0xff; // They are not initially connected against anything...
}
