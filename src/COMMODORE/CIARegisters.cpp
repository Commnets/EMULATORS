#include <COMMODORE/CIARegisters.hpp>

// ---
COMMODORE::CIARegisters::CIARegisters (int id, MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: MCHEmul::ChipRegisters (id, ps, pp, a, s),
	  _timerA (nullptr), _timerB (nullptr), _clock (nullptr),
	  _lastValueRead (MCHEmul::PhysicalStorage::_DEFAULTVALUE)
	  // At this point all internal variables will have random values...
{ 
	setClassName ("CIARegisters");

	initializeInternalValues (); 
}

// ---
void COMMODORE::CIARegisters::initialize ()
{
	MCHEmul::PhysicalStorageSubset::initialize (); 

	initializeInternalValues ();
}

// ---
void COMMODORE::CIARegisters::setValue (size_t p, const MCHEmul::UByte& v)
{
	if (_timerA == nullptr || _timerB == nullptr || _clock == nullptr)
		return;

	MCHEmul::PhysicalStorageSubset::setValue (p, v);

	size_t pp = p % 0x10;

	switch (pp)
	{
		// Data Port Register A: CIAPRA
		case 0x00:
			{
				setPortA (MCHEmul::UByte (
					v.value () | ~_dataPortADir));
				// _dataPortADir: bits 0 = input, bits 1 = output.
				// So input positions are maintained to 1, and output positions the value of v is maintained....
			}

			break;

		// Data Port Register B: CIAPRB
		case 0x01:
			{
				MCHEmul::UByte cV = v;
				// The result of the timer could take into account...
				if (_reflectTimerAAtPortDataB)
					cV.setBit (6, _timerAValueAtPortDataB ? true : false);
				if (_reflectTimerBAtPortDataB)
					cV.setBit (7, _timerBValueAtPortDataB ? true : false);

				setPortB (MCHEmul::UByte (
					v.value () | ~_dataPortBDir));
				// @see 0x00. Same but with PortB instead...
			}

			break;

		// Data Direction Register A: CIDDRA
		case 0x02:
			{
				_dataPortADir = v.value ();
			}

			break;

		// Data Direction Register A: CIDDRB
		case 0x03:
			{
				_dataPortBDir = v.value ();
			}

			break;

		// LSB of the Latch A
		case 0x04:
			{
				_timerA -> setInitialValue ((_timerA -> initialValue () & 0xff00) | (unsigned short) v.value ());
			}

			break;

		// MSB of the Latch A
		case 0x05:
			{
				_timerA -> setInitialValue ((_timerA -> initialValue () & 0x00ff) | (unsigned short) (v.value () << 8));
			}

			break;

		// LSB of the Latch B
		case 0x06:
			{
				_timerB -> setInitialValue ((_timerB -> initialValue () & 0xff00) | (unsigned short) v.value ());
			}

			break;

		// MSB of the Latch B
		case 0x07:
			{
				_timerB -> setInitialValue ((_timerB -> initialValue () & 0x00ff) | (unsigned short) (v.value () << 8));
			}

			break;

		// Time of Day Clock Tenths of Seconds
		// Bits 0-3: BCD Digits. Bits 4-7: Unused.
		case 0x08:
			{
				int ts = MCHEmul::UInt ({ MCHEmul::PhysicalStorageSubset::readValue (0x08) }, 
					true /** 1 byte...doesn't matter. */, MCHEmul::UInt::_PACKAGEDBCD).asInt (); // A BCD value to int...
				if (MCHEmul::PhysicalStorageSubset::readValue (0x0f).bit (7)) _clock -> setAlarmTenthSeconds ((unsigned char) ts); 
				else _clock -> setTenthSeconds ((unsigned char) ts);
			}

			break;

		// Time of Day Clock Seconds
		// Bits 0-3: Second BCD Digit. Bits 4-6: First BCD Digit. Bit 7: Unused.
		case 0x09:
			{
				int s = MCHEmul::UInt ({ MCHEmul::PhysicalStorageSubset::readValue (0x08) }, 
					true /** 1 byte...doesn't matter. */, MCHEmul::UInt::_PACKAGEDBCD).asInt (); // A BCD value to int...
				if (MCHEmul::PhysicalStorageSubset::readValue (0x0f).bit (7)) _clock -> setSeconds ((unsigned char) s); 
				else _clock -> setSeconds ((unsigned char) s);
			}

			break;

		// Time of Day Clock Minutes
		// Bits 0-3: Second BCD Digit. Bits 4-6: First BCD Digit. Bit 7: Unused.
		case 0x0a:
			{
				int m = MCHEmul::UInt ({ MCHEmul::PhysicalStorageSubset::readValue (0x08) }, 
					true /** 1 byte...doesn't matter. */, true /** BCD */).asInt (); // A BCD value to int...
				if (MCHEmul::PhysicalStorageSubset::readValue (0x0f).bit (7)) _clock -> setAlarmMinutes ((unsigned char) m); 
				else _clock -> setMinutes ((unsigned char) m);
			}

			break;

		// Time of Day Clock Hours
		// Bits 0-3: Second BCD Digit. Bit 4: First BCD Digit. Bits 5-6: Unused. Bit 7: AM/PM Flag (PM = 1)
		case 0x0b:
			{
				MCHEmul::UByte dt = MCHEmul::PhysicalStorageSubset::readValue (0x08);
				int h = MCHEmul::UInt ({ dt & 0x0f }, 
					true /** 1 byte...doesn't matter. */, MCHEmul::UInt::_PACKAGEDBCD).asInt (); // A BCD value to int...
				if (dt.bit (7)) h += 12; // PM...
				if (MCHEmul::PhysicalStorageSubset::readValue (0x0f).bit (7)) _clock -> setAlarmHours ((unsigned char) h); 
				else _clock -> setHours ((unsigned char) h);
			}

			break;

		// Serial Data Port
		case 0x0c:
			{
				_serialPort -> setValue (v.value ());
			}

			break;

		// Interrupt Control Register
		// Depending on the bit 7 the behaviour is different: 
		// 1 = bits with 1 are set, 0 = bits with 1 are cleared... 
		// In any case, bits with 0 are left as they were...
		case 0x0d:
			{
				if (v.bit (7)) 
				{
					if (v.bit (0)) _timerA -> setInterruptEnabled (true);
					if (v.bit (1)) _timerB -> setInterruptEnabled (true);
					if (v.bit (2)) _clock  -> setInterruptEnabled (true);
					if (v.bit (3)) _serialPort -> setInterruptEnabled (true);
					if (v.bit (4)) _flagLineInterruptEnabled = true;
				}
				else
				{
					if (v.bit (0)) _timerA -> setInterruptEnabled (false);
					if (v.bit (1)) _timerB -> setInterruptEnabled (false);
					if (v.bit (2)) _clock  -> setInterruptEnabled (false);
					if (v.bit (3)) _serialPort -> setInterruptEnabled (false);
					if (v.bit (4)) _flagLineInterruptEnabled = false;
				}
			}

			break;

		// Control Register A
		case 0x0e:
			{
				_timerA -> setEnabled (v.bit (0));
				_timerA -> setAffectPortDataB (_reflectTimerAAtPortDataB = v.bit (1));
				_timerA -> setPulseAtPortDataB (v.bit (2));
				_timerA -> setRunMode (v.bit (3) ? CIATimer::RunMode::_ONETIME : CIATimer::RunMode::_RESTART);
				if (v.bit (4)) _timerA -> reset ();
				_timerA -> setCountMode (v.bit (5)
					? COMMODORE::CIATimer::CountMode::_SIGNALSONCNTLINE : COMMODORE::CIATimer::CountMode::_PROCESSORCYCLES);
				_serialPort -> setStatus (v.bit (6) 
					? COMMODORE::CIASerialPort::Status::_SAVING : COMMODORE::CIASerialPort::Status::_READING); // true when output and false when input...
				// The bit 7 to select whether TOD ic actualized under 50Hz or 60Hz is not emulated...
			}

			break;

		// Control Register B
		case 0x0f:
			{
				_timerB -> setEnabled (v.bit (0));
				_timerB -> setAffectPortDataB (_reflectTimerBAtPortDataB = v.bit (1));
				_timerB -> setPulseAtPortDataB (v.bit (2));
				_timerB -> setRunMode (v.bit (3) ? CIATimer::RunMode::_ONETIME : CIATimer::RunMode::_RESTART);
				if (v.bit (4)) _timerB -> reset ();
				// bits 5 & 6 indicates the mode...
				_timerB -> setCountMode ((COMMODORE::CIATimer::CountMode) ((v.value () >> 5) & 0x03));
				// The bit 7 is taken into account when managing TOD...
			}

			break;
			
		default:
			break;
	}
}

// ---
const MCHEmul::UByte& COMMODORE::CIARegisters::readValue (size_t p) const
{
	if (_timerA == nullptr || _timerB == nullptr || _clock == nullptr)
		return (MCHEmul::PhysicalStorage::_DEFAULTVALUE);

	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	size_t pp = p % 0x10;

	switch (pp)
	{
		case 0x00:
			{
				// In _dataPortADir bits to 1 mean output and bits to 0 mean input, so...
				result = MCHEmul::UByte (
					_portA & ~_dataPortADir | // ...only the ones at 0 are read...
					_portA & _dataPortADir);  // ...and the rest are maintained as they were!	
				// at the end result == _portA! (but in this way is better explained)
				// as it is described in page 5 of the 6526 datasheet
				// http://archive.6502.org/datasheets/mos_6526_cia_recreated.pdf
			}

			break;

		/** In the Data Port B a reflection of the timers could happen. */
		case 0x01:
			{
				// In _dataPortBDir bits to 1 mean output and bits to 0 mean input, so...
				result = MCHEmul::UByte (
					_portB & ~_dataPortBDir | // ...only the ones at 0 are read...
					_portB & _dataPortBDir);  // ...and the rest are maintained as they were!
				// at the end result == _portB! (but in this way is better explained)
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

		case 0x04:
			{
				result = MCHEmul::UByte ((unsigned char) (_timerA -> currentValue () & 0x00ff));
			}

			break;

		case 0x05:
			{
				result = MCHEmul::UByte ((unsigned char) ((_timerA -> currentValue () & 0xff00) >> 8));
			}

			break;

		case 0x06:
			{
				result = MCHEmul::UByte ((unsigned char) (_timerB -> currentValue () & 0x00ff));
			}

			break;

		case 0x07:
			{
				result = MCHEmul::UByte ((unsigned char) ((_timerB -> currentValue () & 0xff00) >> 8));
			}

			break;

		case 0x08:
			{
				// Only 1 digit from 0 to 9 (4 bits). Rest unused.
				result = MCHEmul::UInt::fromUnsignedInt (_clock -> tenthsSecond (), MCHEmul::UInt::_PACKAGEDBCD).bytes ()[0];
			}

			break;

		case 0x09:
			{
				// 2 digits. 1º: from 0 to 9 (4 bits), 2º: from 0 to 6 (3 bits). Bit 7 unused
				result = MCHEmul::UInt::fromUnsignedInt (_clock -> seconds (), MCHEmul::UInt::_PACKAGEDBCD).bytes ()[0];
			}

			break;

		case 0x0a:
			{
				// 2 digits. 1º: from 0 to 9 (4 bits), 2º: from 0 to 6 (3 bits). Bit 7 unused
				result = MCHEmul::UInt::fromUnsignedInt (_clock -> minutes (), MCHEmul::UInt::_PACKAGEDBCD).bytes ()[0];
			}

			break;

		case 0x0b:
			{
				// 2 digits. 1º: from 0 to 9 (4 bits), 2º: either 0 or 1 (1 bit). Bits 5,6 unused. Bit 7 AM/PM flag (PM = 1)
				unsigned int h = (unsigned int) _clock -> hours ();
				bool pm = h > 12 || (h == 12 && _clock -> minutes () > 0);
				if (pm && h > 12) h -= 12; // The value 12 is kept...
				result = MCHEmul::UInt::fromUnsignedInt (h, MCHEmul::UInt::_PACKAGEDBCD).bytes ()[0];
				result.setBit (7, pm);
			}

			break;

		case 0x0c:
			{
				result = _serialPort -> value ();
			}

			break;

		case 0x0d:
			{
				result = MCHEmul::UByte::_0;
				result.setBit (7, launchInterruption ()); // Any Interrupt?
				result.setBit (0, _timerA -> interruptRequested ()); // in Timer A?
				result.setBit (1, _timerB -> interruptRequested ()); // in Timer B?
				result.setBit (2, _clock -> interruptRequested ()); // in Clock?
				result.setBit (3, _serialPort -> interruptRequested ()); // in Serial Port?
				result.setBit (4, flagLineInterruptRequested ()); // In the Flag Line?
			}

			break;

		case 0x0e:
			{
				result = MCHEmul::UByte::_0;
				result.setBit (0, _timerA -> enabled ());
				result.setBit (1, _reflectTimerAAtPortDataB);
				result.setBit (2, _timerA -> pulseAtPortDataB ());
				result.setBit (3, (_timerA -> runMode () == CIATimer::RunMode::_ONETIME) ? true : false);
				// Bit 4 is always 0 when read...
				// In timer A there can only run PROCESSORCYCLES OR SIGNALONCNTLINE...
				result.setBit (5, (_timerA -> countMode () == CIATimer::CountMode::_SIGNALSONCNTLINE) ? true : false);
				result.setBit (6, (_serialPort -> status () == COMMODORE::CIASerialPort::Status::_SAVING) ? true : false);
				// Bit 7 to select whether the TOD is actualized under 50Hz or 60Hz is not still implemented...
				// By default it is left to 0 when read.
			}

			break;

		case 0x0f:
			{
				result = MCHEmul::UByte::_0;
				result.setBit (0, _timerB -> enabled ());
				result.setBit (1, _reflectTimerBAtPortDataB);
				result.setBit (2, _timerB -> pulseAtPortDataB ());
				result.setBit (3, (_timerB -> runMode () == CIATimer::RunMode::_ONETIME) ? true : false);
				// Bit 4 is always 0 when read...
				result.setBit (5, (_timerB -> countMode () == CIATimer::CountMode::_SIGNALSONCNTLINE ||
								   _timerB -> countMode () == CIATimer::CountMode::_0ONCNTPULSES) ? true : false);
				result.setBit (6, (_timerB -> countMode () == CIATimer::CountMode::_TIMERCOUNTSDOWNTO0 || 
								   _timerB -> countMode () == CIATimer::CountMode::_0ONCNTPULSES) ? true : false);
				// Bit 7 is always 0 when read...
			}

			break;
			
		default:
			break;
	}

	return (_lastValueRead = result);
}

// ---
void COMMODORE::CIARegisters::initializeInternalValues ()
{
	// At this point the timers and the clock might not be set yet...
	if (_timerA == nullptr || _timerB == nullptr || _clock == nullptr)
		return;

	//They have to be initialized in advanced as the value set depends also on the value at the beginning...
	_portA = _portB = MCHEmul::UByte::_FF; // As described in the documentation they all have a pull up resistor...

	// The internal variables are initialized through the data in memory...

	// The direction is first set up to set up accodingly the values of the ports A and B...
	setValue (0x02, MCHEmul::UByte::_FF); // All lines output...
	setValue (0x03, MCHEmul::UByte::_0);  // All lines input...
	setValue (0x00, MCHEmul::UByte::_FF); // When they are no connected to nothing, the documentation sais that they are pulled up!
	setValue (0x01, MCHEmul::UByte::_FF); // same than previous one...
	// After this the port A and B will be still FF...
	setValue (0x04, MCHEmul::UByte::_FF);
	setValue (0x05, MCHEmul::UByte::_FF);
	setValue (0x06, MCHEmul::UByte::_FF);
	setValue (0x07, MCHEmul::UByte::_FF);
	setValue (0x08, MCHEmul::UByte::_0); 
	setValue (0x09, MCHEmul::UByte::_0); 
	setValue (0x0a, MCHEmul::UByte::_0); 
	setValue (0x0b, MCHEmul::UByte::_0); 
	setValue (0x0c, MCHEmul::UByte::_FF); // Not connected against anything...
	setValue (0x0d, 0x1f);  // To clear all interrupts...
	setValue (0x0e, 0xe0);  // Register A: Stopped, no value appear on bit 6 port B, pulse,
							// continuous, not load, counting cycles, serial port output, and 50 hz frequecy (by default)
	setValue (0x0f, 0xa0);  // Register B: Stopped, no value appear on bit 7 port B, pulse, 
							// continuous, not load, counting cycles, writting to TOD regisers sets alarm.

	_flagLineInterruptRequested = false;

	_portA = _portB = 0xff; // They are not initially connected against anything...

	_reflectTimerAAtPortDataB = _reflectTimerBAtPortDataB = false; // Do not do anything...

	_timerAValueAtPortDataB = _timerBValueAtPortDataB = false; // It is the same, but just in case...
}
