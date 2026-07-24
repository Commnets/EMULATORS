#include <COMMODORE/CIA/CIARegisters.hpp>

// ---
COMMODORE::CIARegisters::CIARegisters (int id, MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: MCHEmul::ChipRegisters (id, ps, pp, a, s),
	  _timerA (nullptr), _timerB (nullptr), _clock (nullptr),
	  _lastValueRead (MCHEmul::PhysicalStorage::_DEFAULTVALUE),
	  _interruptsEnabledBack (false)
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
void COMMODORE::CIARegisters::actualizePortB ()
{
	MCHEmul::UByte outputValue (_outputRegB);

	if (_reflectTimerAAtPortDataB)
		outputValue.setBit (6, _timerAValueAtPortDataB);
	if (_reflectTimerBAtPortDataB)
		outputValue.setBit (7, _timerBValueAtPortDataB);

	// Keep the software latch independent from the timer-driven pin values.
	unsigned char outputRegisterB = _outputRegB;
	_outputRegB = outputValue.value ();
	setPortB (0xff, false);
	_outputRegB = outputRegisterB;
}

// ---
void COMMODORE::CIARegisters::setReflectTimerAAtPortDataB (bool r, bool v)
{
	if (_reflectTimerAAtPortDataB == r &&
		(!r || _timerAValueAtPortDataB == v))
		return;

	_reflectTimerAAtPortDataB = r;
	_timerAValueAtPortDataB = v;
	actualizePortB ();
}

// ---
void COMMODORE::CIARegisters::setReflectTimerBAtPortDataB (bool r, bool v)
{
	if (_reflectTimerBAtPortDataB == r &&
		(!r || _timerBValueAtPortDataB == v))
		return;

	_reflectTimerBAtPortDataB = r;
	_timerBValueAtPortDataB = v;
	actualizePortB ();
}

// ---
void COMMODORE::CIARegisters::setValue (size_t p, const MCHEmul::UByte& v)
{
	if (_timerA == nullptr || _timerB == nullptr || _clock == nullptr)
		return;

	size_t pp = p % 0x10;

	MCHEmul::PhysicalStorageSubset::setValue (pp, v);

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

				actualizePortB ();
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

				actualizePortB ();
			}

			break;

		// LSB of the Latch A
		case 0x04:
			{
				_timerA -> setLatchLow (v);
			}

			break;

		// MSB of the Latch A
		case 0x05:
			{
				_timerA -> setLatchHigh (v);
			}

			break;

		// LSB of the Latch B
		case 0x06:
			{
				_timerB -> setLatchLow (v);
			}

			break;

		// MSB of the Latch B
		case 0x07:
			{
				_timerB -> setLatchHigh (v);
			}

			break;

		// Time of Day Clock Tenths of Seconds
		// Bits 0-3: BCD Digits. Bits 4-7: Unused.
		case 0x08:
			{
				int ts = MCHEmul::UInt ({ v }, 
					true /** 1 byte...doesn't matter. */, MCHEmul::UInt::_PACKAGEDBCD).asInt (); // A BCD value to int...
				if (MCHEmul::PhysicalStorageSubset::readValue (0x0f).bit (7)) _clock -> setAlarmTenthSeconds ((unsigned char) ts); 
				else _clock -> setTenthSeconds ((unsigned char) ts);
			}

			break;

		// Time of Day Clock Seconds
		// Bits 0-3: Second BCD Digit. Bits 4-6: First BCD Digit. Bit 7: Unused.
		case 0x09:
			{
				int s = MCHEmul::UInt ({ v }, 
					true /** 1 byte...doesn't matter. */, MCHEmul::UInt::_PACKAGEDBCD).asInt (); // A BCD value to int...
				if (MCHEmul::PhysicalStorageSubset::readValue (0x0f).bit (7)) _clock -> setAlarmSeconds ((unsigned char) s); 
				else _clock -> setSeconds ((unsigned char) s);
			}

			break;

		// Time of Day Clock Minutes
		// Bits 0-3: Second BCD Digit. Bits 4-6: First BCD Digit. Bit 7: Unused.
		case 0x0a:
			{
				int m = MCHEmul::UInt ({ v }, 
					true /** 1 byte...doesn't matter. */, MCHEmul::UInt::_PACKAGEDBCD).asInt (); // A BCD value to int...
				if (MCHEmul::PhysicalStorageSubset::readValue (0x0f).bit (7)) _clock -> setAlarmMinutes ((unsigned char) m); 
				else _clock -> setMinutes ((unsigned char) m);
			}

			break;

		// Time of Day Clock Hours
		// Bits 0-3: Second BCD Digit. Bit 4: First BCD Digit. Bits 5-6: Unused. Bit 7: AM/PM Flag (PM = 1)
		case 0x0b:
			{
				MCHEmul::UByte dt = v;
				int h = MCHEmul::UInt ({ dt & 0x1f }, 
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
				_timerA -> setRunMode
					(v.bit (3) ? CIATimer::RunMode::_ONETIME : CIATimer::RunMode::_RESTART);
				_timerA -> setAffectPortDataB (v.bit (1));
				_timerA -> setPortOutputMode
					(v.bit (2) ? CIATimer::PortOutputMode::_TOGGLE : CIATimer::PortOutputMode::_PULSE);
				_timerA -> setCountMode
					(v.bit (5)
						? CIATimer::CountMode::_SIGNALSONCNTLINE
						: CIATimer::CountMode::_PROCESSORCYCLES);
				if (v.bit (4))
					_timerA -> forceLoad ();
				_timerA -> setEnabled (v.bit (0));
				_serialPort -> setStatus
					(v.bit (6) ? CIASerialPort::Status::_SAVING : CIASerialPort::Status::_READING);
			}

			break;

		// Control Register B
		case 0x0f:
			{
				_timerB -> setRunMode
					(v.bit (3) ? CIATimer::RunMode::_ONETIME : CIATimer::RunMode::_RESTART);
				_timerB -> setAffectPortDataB (v.bit (1));
				_timerB -> setPortOutputMode
					(v.bit (2) ? CIATimer::PortOutputMode::_TOGGLE : CIATimer::PortOutputMode::_PULSE);
				_timerB -> setCountMode
					((CIATimer::CountMode) ((v.value () >> 5) & 0x03));
				if (v.bit (4))
					_timerB -> forceLoad ();
				_timerB -> setEnabled (v.bit (0));
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
				unsigned int h = (unsigned int) _clock -> hours (); // Only hours is affected...
				bool pm = h > 12 || (h == 12 && _clock -> peekMinutes () > 0); // ...minutes is not affected.
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
				_interruptsEnabledBack = true; // The interrupts are allowed back!
			}

			break;

		case 0x0e:
			{
				result = MCHEmul::UByte::_0;
				result.setBit (0, _timerA -> enabled ());
				result.setBit (1, _timerA -> affectPortDataB ());
				result.setBit (2, _timerA -> portOutputMode () == CIATimer::PortOutputMode::_TOGGLE);
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
				result.setBit (1, _timerB -> affectPortDataB ());
				result.setBit (2, _timerB -> portOutputMode () == CIATimer::PortOutputMode::_TOGGLE);
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
const MCHEmul::UByte& COMMODORE::CIARegisters::peekValue (size_t p) const
{
	// It is like readvalue except for a couple of registers
	// that when red are actualized...

	if (_timerA == nullptr || _timerB == nullptr || _clock == nullptr)
		return (MCHEmul::PhysicalStorage::_DEFAULTVALUE);

	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	size_t pp = p % 0x10;

	switch (pp)
	{
		case 0x08:
			{
				result = MCHEmul::UInt::fromUnsignedInt (_clock -> peekTenthsSecond (), MCHEmul::UInt::_PACKAGEDBCD).bytes ()[0];
			}

			break;

		case 0x09:
			{
				result = MCHEmul::UInt::fromUnsignedInt (_clock -> peekSeconds (), MCHEmul::UInt::_PACKAGEDBCD).bytes ()[0];
			}

			break;

		case 0x0a:
			{
				result = MCHEmul::UInt::fromUnsignedInt (_clock -> peekMinutes (), MCHEmul::UInt::_PACKAGEDBCD).bytes ()[0];
			}

			break;

		case 0x0b:
			{
				unsigned int h = (unsigned int) _clock -> peekHours ();
				bool pm = h > 12 || (h == 12 && _clock -> peekMinutes () > 0);
				if (pm && h > 12) h -= 12;
				result = MCHEmul::UInt::fromUnsignedInt (h, MCHEmul::UInt::_PACKAGEDBCD).bytes ()[0];
				result.setBit (7, pm);
			}

			break;

		case 0x0d:
			{
				result = MCHEmul::UByte::_0;
				result.setBit (7, launchInterruption ());
				result.setBit (0, _timerA -> peekInterruptRequested ());
				result.setBit (1, _timerB -> peekInterruptRequested ());
				result.setBit (2, _clock -> peekInterruptRequested ());
				result.setBit (3, _serialPort -> peekInterruptRequested ());
				result.setBit (4, peekFlagLineInterruptRequested ());
			}

			break;

		default:
			result = readValue (pp);
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

	_reflectTimerAAtPortDataB = _reflectTimerBAtPortDataB = false;
	_timerAValueAtPortDataB = _timerBValueAtPortDataB = false;

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
	// After this the port A and B will be still FF...
	setValue (0x04, MCHEmul::UByte::_FF);
	setValue (0x05, MCHEmul::UByte::_FF);
	setValue (0x06, MCHEmul::UByte::_FF);
	setValue (0x07, MCHEmul::UByte::_FF);
	// 0x0b, 0x0a, 0x09, 0x08: In reverse order to maintain active the clock at the end...
	setValue (0x0b, MCHEmul::UByte::_0);
	setValue (0x0a, MCHEmul::UByte::_0);
	setValue (0x09, MCHEmul::UByte::_0);
	setValue (0x08, MCHEmul::UByte::_0);
	setValue (0x0c, MCHEmul::UByte::_FF); // Not connected against anything...
	setValue (0x0d, 0x1f);  // To clear all interrupts...
	setValue (0x0e, 0xe0);  // Register A: Stopped, no value appear on bit 6 port B, pulse,
							// continuous, not load, counting cycles, serial port output, and 50 hz frequecy (by default)
	setValue (0x0f, 0xa0);  // Register B: Stopped, no value appear on bit 7 port B, pulse, 
							// continuous, not load, counting cycles, writting to TOD regisers sets alarm.

	_flagLineInterruptRequested = false;

	_portA = _portB = 0xff; // They are not initially connected against anything...


	_interruptsEnabledBack = false;
}
