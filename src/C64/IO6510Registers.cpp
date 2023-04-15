#include <C64/IO6510Registers.hpp>

// ---
C64::IO6510Registers::IO6510Registers 
		(MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: MCHEmul::ChipRegisters (_IO6510REGISTERS_SUBSET, ps, pp, a, s),
	  _lastValueRead (MCHEmul::PhysicalStorage::_DEFAULTVALUE)
	  // At this point all internal variables will have random values...
{ 
	setClassName ("IO6510Registers");

	initializeInternalValues ();
}

// ---
void C64::IO6510Registers::initialize ()
{
	MCHEmul::PhysicalStorageSubset::initialize ();

	initializeInternalValues ();
}

// ---
void C64::IO6510Registers::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* ntier)
{
	// Bit 4 reflects whether some key has been pressed on the dataette...
	// When something has been pressed this bit is set to 0, 1 in the other situation.
	// The same bit in the memory position 0 hast to be defined as input (value = 0)
	if (evnt.id () == COMMODORE::DatasetteIOPort::_KEYPRESSED ||
		evnt.id () == COMMODORE::DatasetteIOPort::_NOKEYPRESSED)
		setCasetteNoKeyPressed (evnt.id () == COMMODORE::DatasetteIOPort::_NOKEYPRESSED);
}

// ---
void C64::IO6510Registers::setValue (size_t p, const MCHEmul::UByte& v)
{
	MCHEmul::ChipRegisters::setValue (p, v);

	switch (p)
	{
		// Direction Dataport 6510 (0x00) is stored as it comes...
		// Its value is taken into account when the Dataport 6510 is either written or read

		// Dataport 6510
		case 0x01:
			{
				MCHEmul::UByte val0		= MCHEmul::ChipRegisters::readValue (0x00);

				// The following bits are only taking into account when the register 0 is defined as output direction...
				_LORAM					= val0.bit (0) ? v.bit (0) : false; 
				_HIRAM					= val0.bit (1) ? v.bit (1) : false; 
				_CHAREN					= val0.bit (2) ? v.bit (2) : false;
				_casetteData			= val0.bit (3) ? v.bit (3) : false; 
				// The bit 4 is important when reading, because is something comming from the casette...
				_casetteMotorStopped	= val0.bit (5) ? v.bit (5) : false;
				// The bits 6 & 7 are not used...
				// The action has to be notified...

				// Send the notification when the C64 IO Port bits changes (Byte 1, bits 0. 1 or 2)
				notify (MCHEmul::Event (_C64PORTIOBITSACTUALIZED, 
					(_LORAM ? 1 : 0) + (_HIRAM ? 2 : 0) + (_CHAREN ? 4 : 0)));
				// Send the data to the casette port...
				notify (MCHEmul::Event (COMMODORE::DatasetteIOPort::_WRITE, 
					_casetteData ? 1 : 0));
				// Modify the status of the motor of the casette...
				notify (MCHEmul::Event (_casetteMotorStopped
					? COMMODORE::DatasetteIOPort::_MOTORSTOPPED : COMMODORE::DatasetteIOPort::_MOTORRUNNING));
			}

			break;
			
		default:
			break;
	}
}

// ---
const MCHEmul::UByte& C64::IO6510Registers::readValue (size_t p) const
{
	MCHEmul::UByte result = MCHEmul::ChipRegisters::readValue (p);

	switch (p)
	{
		// Direction Dataport 6510 (0x00) is read as it is stored...
		// Its value is taken into account when the Dataport 6510 is either written or read

		case 0x01:
			{
				MCHEmul::UByte val0 = MCHEmul::ChipRegisters::readValue (0x00);

				result = MCHEmul::UByte::_0;
				result.setBit (0, _LORAM);
				result.setBit (1, _HIRAM);
				result.setBit (2, _CHAREN);
				result.setBit (3, _casetteData);
				// The bit 4 is taken into account only when the bit 4 in the register 0 is defined as input...
				result.setBit (4, !val0.bit (4) ? _casetteNoKeyPressed : true);
				result.setBit (5, _casetteMotorStopped);
				// Bits 6 & 7 are not used...
			}

			break;
			
		default:
			break;
	}

	return (_lastValueRead = result);
}

// ---
void C64::IO6510Registers::initializeInternalValues ()
{
	// Dataport. Dtermines the direction of the bytes 0x01
	// All lines for output except line bit 4 (datasette line sense) that equal to 0.
	setValue (0x00, 0x2f);
	// Very important register
	// = BASIC + KERNAL + I/O + 0 in OUTPUT + 
	// CASETTE SENSE LINE (NOTHING PRESSED) = 0 + CASETTE MOTOR STOPPED = 1; 
	// The ROM when initialize will set this variable automatically...
	setValue (0x01, 0xe7);

	// This is an input from the casette and it is not automatically initialized...
	_casetteNoKeyPressed = true;
}
