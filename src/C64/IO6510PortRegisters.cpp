#include <C64/IO6510PortRegisters.hpp>
#include <COMMODORE/incs.hpp>

// ---
void C64::IO6510PortRegisters::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* ntier)
{
	// Bit 4 reflects whether some key has been pressed on the dataette...
	// When something has been pressed this bit is set to 0, 1 in the other situation.
	// The same bit in the memory position 0 hast to be defined as input (value = 0)
	if (evnt.id () == COMMODORE::DatasetteIOPort::_KEYPRESSED ||
		evnt.id () == COMMODORE::DatasetteIOPort::_NOKEYPRESSED)
		setBitPortValue (4, evnt.id () == COMMODORE::DatasetteIOPort::_NOKEYPRESSED);
}

// ---
void C64::IO6510PortRegisters::notifyPortChanges (const MCHEmul::UByte& c, const MCHEmul::UByte& v)
{
	// Send the notification when the C64 IO Port bits changes (bits 0, 1 or 2)
	if (c.value () & 0x07) // _LORAM, _HIRAM, _CHAREN
		notify (MCHEmul::Event (_C64PORTIOBITSACTUALIZED, (unsigned int) (v.value () & 0x07)));
	
	// Send the data to the casette port, if something changed!
	if (c.bit (3))
		notify (MCHEmul::Event (COMMODORE::DatasetteIOPort::_WRITE, v.bit (3) ? 1 : 0));
	
	// The bit 4 is for know whwther there is a keyboard pressed in the casette...
	
	// Modify the status of the motor of the casette, if something changed!
	if (c.bit (5))
		notify (MCHEmul::Event (v.bit (5)
			? COMMODORE::DatasetteIOPort::_MOTORSTOPPED : COMMODORE::DatasetteIOPort::_MOTORRUNNING));
}

// ---
void C64::IO6510PortRegisters::initializeInternalValues ()
{
	// This is an input from the casette and it is not automatically initialized...
	_portValue = MCHEmul::UByte::_FF;

	// Dataport. Dtermines the direction of the bytes 0x01
	// All lines for output except line bit 4 (datasette line sense) that equal to 0.
	setValue (0x00, 0x2f);

	// Very important register
	// = BASIC + KERNAL + I/O + 0 in OUTPUT + 
	// CASETTE SENSE LINE (NOTHING PRESSED) = 0 + CASETTE MOTOR STOPPED = 1; 
	// The ROM when initialize will set this variable automatically...
	setValue (0x01, 0xe7);
}
