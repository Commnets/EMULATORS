#include <COMMODORE/CIARegisters.hpp>
#include <COMMODORE/CIATimer.hpp>
#include <COMMODORE/CIAClock.hpp>

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
		// Data Port Register A
		case 0x00:
		// Data Port Register B
		case 0x01:
		// Data Direction Register A
		case 0x02:
		// Data Direction Register B
		case 0x03:
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
					if (v.bit (4)) _flagLineInterruptRequested =   true;
				}
				else
				{
					if (v.bit (0)) _timerA -> setInterruptEnabled (false);
					if (v.bit (1)) _timerB -> setInterruptEnabled (false);
					if (v.bit (2)) _clock  -> setInterruptEnabled (false);
					if (v.bit (4)) _flagLineInterruptRequested =   false;
				}
			}

			break;

		// Control Register A
		case 0x0e:
			{
				_timerA -> setEnabled (v.bit (0));
				_timerA -> setAffectPortDataB (v.bit (1));
				_timerA -> setPulseAtPortDataB (v.bit (2));
				_timerA -> setRunMode (v.bit (3) ? CIATimer::RunMode::_ONETIME : CIATimer::RunMode::_RESTART);
				if (v.bit (4)) _timerA -> reset ();
				_timerA -> setCountMode (v.bit (5)
					? COMMODORE::CIATimer::CountMode::_SIGNALSONCNTLINE : COMMODORE::CIATimer::CountMode::_PROCESSORCYCLES);
				// The bit 7 to select whether TOD ic actualized under 50Hz or 60Hz is not emulated...
			}

			break;

		// Control Register B
		case 0x0f:
			{
				_timerB -> setEnabled (v.bit (0));
				_timerB -> setAffectPortDataB (v.bit (1));
				_timerB -> setPulseAtPortDataB (v.bit (2));
				_timerB -> setRunMode (v.bit (3) ? CIATimer::RunMode::_ONETIME : CIATimer::RunMode::_RESTART);
				if (v.bit (4)) _timerB -> reset ();
				// bits 5 & 6 indicates the mode...
				_timerB -> setCountMode ((COMMODORE::CIATimer::CountMode) ((v.value () >> 5) & 0x03));
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
		// When reading no special behaviour...but when setting, take a look!
		case 0x00:
		case 0x02:
		case 0x03:
			{
				result = MCHEmul::PhysicalStorageSubset::readValue (p);
			}

			break;

		/** In the Data Port B a reflection of the timers could happen. */
		case 0x01:
			{
				result = MCHEmul::PhysicalStorageSubset::readValue (p);

				if (_reflectTimerAAtPortDataB != 0) 
					result.setBit (6, _reflectTimerAAtPortDataB == 1 ? true : false);
				if (_reflectTimerBAtPortDataB != 0)
					result.setBit (7, _reflectTimerBAtPortDataB == 1 ? true : false);
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
				result = MCHEmul::PhysicalStorageSubset::readValue (p);
			}

			break;

		case 0x0d:
			{
				bool IA = _timerA -> interruptRequested (); // It is set back to false after reading...
				bool IB = _timerB -> interruptRequested (); // Same...
				bool IC = _clock -> interruptRequested ();  // Same...
				bool ID = flagLineInterruptRequested ();    // Same...
				result = MCHEmul::UByte::_0;
				result.setBit (0, IA); // in Timer A?
				result.setBit (1, IB); // in Timer B?
				result.setBit (2, IC); // in Clock?
				result.setBit (4, ID); // In the Flag Line?
				result.setBit (7, IA || IB || IC || ID); // Any Interrupt?
			}

			break;

		case 0x0e:
		case 0x0f:
			{
				result = MCHEmul::PhysicalStorageSubset::readValue (p);
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

	// The internal variables are initialized through the data in memory...

	setValue (0x00, MCHEmul::UByte::_0); 
	setValue (0x01, MCHEmul::UByte::_0);
	setValue (0x02, MCHEmul::UByte::_0);
	setValue (0x03, MCHEmul::UByte::_0);
	setValue (0x04, MCHEmul::UByte::_0); // No timer A active
	setValue (0x05, MCHEmul::UByte::_0);
	setValue (0x06, MCHEmul::UByte::_0); // No timer B active
	setValue (0x07, MCHEmul::UByte::_0);
	setValue (0x08, MCHEmul::UByte::_0); 
	setValue (0x09, MCHEmul::UByte::_0); 
	setValue (0x0a, MCHEmul::UByte::_0); 
	setValue (0x0b, MCHEmul::UByte::_0); 
	setValue (0x0c, MCHEmul::UByte::_0); 
	setValue (0x0d, MCHEmul::UByte::_0); // No interupts allowed from the early beginning...so stopped!
	setValue (0x0e, MCHEmul::UByte::_0); // No value in timer A
	setValue (0x0f, MCHEmul::UByte::_0); // No value in timer B

	_flagLineInterruptRequested = false;

	_reflectTimerAAtPortDataB = _reflectTimerBAtPortDataB = 0; // Do not do anything...
}
