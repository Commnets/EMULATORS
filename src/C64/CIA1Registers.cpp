#include <C64/CIA1Registers.hpp>
#include <C64/CIATimer.hpp>
#include <C64/CIAClock.hpp>

// ---
C64::CIA1Registers::CIA1Registers (int id, MCHEmul::PhysicalStorage* pS)
	: C64::ChipRegisters (id, pS, 0xdc00, MCHEmul::Address ({ 0x00, 0xdc }, false), 0x0100),
	  _timerA (nullptr), _timerB (nullptr),
	  _lastValueRead (MCHEmul::PhysicalStorage::_DEFAULTVALUE)
	  // At this point all internal variables will have random values...
{ 
	setClassName ("CIA1Registers");

	initializeInternalValues (); 
}

// ---
void C64::CIA1Registers::initialize ()
{
	MCHEmul::PhysicalStorageSubset::initialize ();

	initializeInternalValues ();
}

// ---
void C64::CIA1Registers::setValue (size_t p, const MCHEmul::UByte& v)
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
				// To indicate the row of the key matrix to be read (later)
				// It will be affected by the communication direction (CIDDRA)
				_keyboardRowToRead = v.value ();
			}

			break;

		// Data Port Register B: CIAPRB
		case 0x01:
			// No special meaning when set (but very important when read!)
			break;

		// Data Direction Register A: CIDDRA
		case 0x02:
			{
				_dataPortADir = v.value ();
			}

			break;

		// Data Direction Register B: CIDDRB
		case 0x03:
			{
				_dataPortBDir = v.value ();
			}

			break;

		// LSB of the Latch A: TIMALO
		case 0x04:
			{
				_timerA -> setInitialValue ((_timerA -> initialValue () & 0xff00) | (unsigned short) v.value ());
			}

			break;

		// MSB of the Latch A: TIMAHI
		case 0x05:
			{
				_timerA -> setInitialValue ((_timerA -> initialValue () & 0x00ff) | (unsigned short) (v.value () << 8));
			}

			break;

		// LSB of the Latch B: TIMBLO
		case 0x06:
			{
				_timerB -> setInitialValue ((_timerB -> initialValue () & 0xff00) | (unsigned short) v.value ());
			}

			break;

		// MSB of the Latch B: TIMBHI
		case 0x07:
			{
				_timerB -> setInitialValue ((_timerB -> initialValue () & 0x00ff) | (unsigned short) (v.value () << 8));
			}

			break;

		// Time of Day Clock Tenths of Seconds: TODTEN
		// Bits 0-3: BCD Digits. Bits 4-7: Unused.
		case 0x08:
			{
				int ts = MCHEmul::UInt ({ MCHEmul::PhysicalStorageSubset::readValue (0x08) }, 
					true /** 1 byte...doesn't matter. */, true /** BCD */).asInt (); // A BCD value to int...
				if (MCHEmul::PhysicalStorageSubset::readValue (0x0f).bit (7)) _clock -> setAlarmTenthSeconds ((unsigned char) ts); 
				else _clock -> setTenthSeconds ((unsigned char) ts);
			}

			break;

		// Time of Day Clock Seconds: TODSEC
		// Bits 0-3: Second BCD Digit. Bits 4-6: First BCD Digit. Bit 7: Unused.
		case 0x09:
			{
				int s = MCHEmul::UInt ({ MCHEmul::PhysicalStorageSubset::readValue (0x08) }, 
					true /** 1 byte...doesn't matter. */, true /** BCD */).asInt (); // A BCD value to int...
				if (MCHEmul::PhysicalStorageSubset::readValue (0x0f).bit (7)) _clock -> setSeconds ((unsigned char) s); 
				else _clock -> setSeconds ((unsigned char) s);
			}

			break;

		// Time of Day Clock Minutes: TODMIN
		// Bits 0-3: Second BCD Digit. Bits 4-6: First BCD Digit. Bit 7: Unused.
		case 0x0b:
			{
				int m = MCHEmul::UInt ({ MCHEmul::PhysicalStorageSubset::readValue (0x08) }, 
					true /** 1 byte...doesn't matter. */, true /** BCD */).asInt (); // A BCD value to int...
				if (MCHEmul::PhysicalStorageSubset::readValue (0x0f).bit (7)) _clock -> setAlarmMinutes ((unsigned char) m); 
				else _clock -> setMinutes ((unsigned char) m);
			}

			break;

		// Time of Day Clock Hours: TODHRS
		// Bits 0-3: Second BCD Digit. Bit 4: First BCD Digit. Bits 5-6: Unused. Bit 7: AM/PM Flag (PM = 1)
		case 0x0a:
			{
				int h = MCHEmul::UInt ({ MCHEmul::PhysicalStorageSubset::readValue (0x08) }, 
					true /** 1 byte...doesn't matter. */, true /** BCD */).asInt (); // A BCD value to int...
				if (MCHEmul::PhysicalStorageSubset::readValue (0x0f).bit (7)) _clock -> setAlarmHours ((unsigned char) h); 
				else _clock -> setHours ((unsigned char) h);
			}

			break;

		// Serial Data Port CIASDR
		case 0x0c:
			break;

		// Interrupt Control Register: CIAICR
		// Depending on the bit 7 the behaviour is different: 1 = bits with 1 are set, 0 = bits with 1 are cleared... 
		case 0x0d:
			{
				if (v.bit (0)) _timerA -> setIRQEnabled (v.bit (7));
				if (v.bit (1)) _timerB -> setIRQEnabled (v.bit (7));
				if (v.bit (2)) _clock -> setIRQEnabled (v.bit (7));
				// TODO: To manage the serial port...
			}

			break;

		// Control Register A: CIACRA
		case 0x0e:
			{
				_timerA -> setEnabled (v.bit (0));
				// TODO: Sending signals to serial port...(bits 1, 2 & 6)
				_timerA -> setRunMode (v.bit (3) ? CIATimer::RunMode::_ONETIME : CIATimer::RunMode::_RESTART);
				if (v.bit (4)) _timerA -> reset ();
				_timerA -> setCountMode (v.bit (5)
					? C64::CIATimer::CountMode::_SIGNALSONCNTLINE : C64::CIATimer::CountMode::_PROCESSORCYCLES);
				// TODO: Pending to control based on 60Hz or 50 Hz...(bit 7)
			}

			break;

		// Control Register B: CIACRB
		case 0x0f:
			{
				_timerB -> setEnabled (v.bit (0));
				// TODO: Sending signals to serial port B...(bits 1, 2 & 6)
				_timerB -> setRunMode (v.bit (3) ? CIATimer::RunMode::_ONETIME : CIATimer::RunMode::_RESTART);
				if (v.bit (4)) _timerB -> reset ();
				// bits 5 & 6 indicates the mode...
				_timerB -> setCountMode ((C64::CIATimer::CountMode) ((v.value () >> 5) & 0x03));
				// _PROCESSORCYCLES = 0, _SIGNALSONCNTLINE = 1,...
			}

			break;
			
		default:
			break;
	}
}

// ---
const MCHEmul::UByte& C64::CIA1Registers::readValue (size_t p) const
{
	if (_timerA == nullptr || _timerB == nullptr || _clock == nullptr)
		return (MCHEmul::PhysicalStorage::_DEFAULTVALUE);

	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	size_t pp = p % 0x10;

	switch (pp)
	{
		case 0x00:
			result = MCHEmul::UByte (_joystick2Status);
			break;

		case 0x01:
			{
				if (_keyboardRowToRead == 0xff)
				{ 
					if (_joystick1InputPending)
						result = _keyboardStatusMatrix [7] & ~_dataPortBDir;
					else
						result = 0xff;
				}
				else
				if (_keyboardRowToRead != 0x00)
				{
					// Identify the row to read...
					// Take into account that the row is identified writting 0 in its bit (so it is necessary to inverse it first)
					unsigned char bRV = ~_keyboardRowToRead & _dataPortADir; // 0xff all output...
					size_t c = 0; while ((bRV >>= 1) != 0) c++;
					// If more than 1 row will be possible, the greatest one will be choosen...
					// The columns (from keyboard or joystick 1) connected will have its bits to 0!...
					result = _keyboardStatusMatrix [c] & ~_dataPortBDir; // 0x00 all input...
				}
			}
			
			break;

		// When reading no special behaviour...but when setting!
		case 0x02:
		case 0x03:
			{
				result = MCHEmul::PhysicalStorageSubset::readValue (p);
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
			// TODO: 
			// To simulate the Serial Port.
			// The directions of the information has to be taken into account
			// When the serial port is configurated for input, the data is read in the pint 5 when the pin 4 (CNT) let you know
			// the info is ready for reading (this is connected with the timer).
			break;

		case 0x0d:
			{
				bool IA = _timerA -> IRQRequested (); // It is set back to false after reading...
				bool IB = _timerB -> IRQRequested (); // Same...
				bool IC = _clock -> IRQRequested (); // Same...
				result = MCHEmul::UByte::_0;
				result.setBit (7, IA || IB || IC); // IRQ?
				result.setBit (0, IA); // in Timer A?
				result.setBit (1, IB); // in Timer B?
				result.setBit (2, IC); // in Clock?
				// TODO: To manage the serial port...
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
void C64::CIA1Registers::initializeInternalValues ()
{
	// At this point the timers and the clock might not be set yet...
	if (_timerA == nullptr || _timerB == nullptr || _clock == nullptr)
		return;

	// Data Port A all input...
	setValue (0x02, MCHEmul::UByte::_FF); 
	// Data Port B all output...
	setValue (0x03, MCHEmul::UByte::_0);
	// Just to be able to read well the keyboard...

	setValue (0x00, MCHEmul::UByte::_FF); // No row to be read so far...
	setValue (0x01, MCHEmul::UByte::_0);
	setValue (0x04, MCHEmul::UByte::_0);  // No timer A active
	setValue (0x05, MCHEmul::UByte::_0);
	setValue (0x06, MCHEmul::UByte::_0);  // No timer B active
	setValue (0x07, MCHEmul::UByte::_0);
	setValue (0x08, MCHEmul::UByte::_0);
	setValue (0x09, MCHEmul::UByte::_0);
	setValue (0x0a, MCHEmul::UByte::_0);
	setValue (0x0b, MCHEmul::UByte::_0);
	setValue (0x0c, MCHEmul::UByte::_0);
	setValue (0x0d, MCHEmul::UByte::_0);  // No interupts allowed from the early beginning...so stopped!
	setValue (0x0e, MCHEmul::UByte::_0);  // No value in timer A
	setValue (0x0f, MCHEmul::UByte::_0);  // No value in timer B

	_joystick1InputPending= false; // Nothing pending from the joystick 1...
	_joystick2Status = 0xff; // No switches clicked, no fire buttons pressed...
	for (size_t i = 0; i < 8; _keyboardStatusMatrix [i++] = MCHEmul::UByte::_FF); // No keys pressed...
}
