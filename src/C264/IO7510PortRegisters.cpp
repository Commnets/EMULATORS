#include <C264/IO7510PortRegisters.hpp>
#include <COMMODORE/incs.hpp>

// ---
C264::IO7501PortRegisters::IO7501PortRegisters (MCHEmul::PhysicalStorage* ps)
	: F6500::IO7501PortRegisters (_IO7501REGISTERS_SUBSET, ps)
{
	// The actions over this registers are not buffered, 
	// and the consequence in PLA must be inmediate...
	setBufferMemorySetCommands (false);

	// The bit 5 is not used but it is always to 0...
	// This is defined in the initial values...

	initializeInternalValues (); 
}

// ---
void C264::IO7501PortRegisters::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* ntier)
{
	// A bit is read in the datasette....
	if (evnt.id () == MCHEmul::DatasetteIOPort::_READ)
		setBitPortValue (4, evnt.value () == 1 ? true : false);
}

// ---
void C264::IO7501PortRegisters::notifyPortChanges (const MCHEmul::UByte& c, const MCHEmul::UByte& v)
{
	// Modify the status of the motor of the casette, if something changed in this bit!
	if (c.bit (3))
		notify (MCHEmul::Event (v.bit (3)
			? MCHEmul::DatasetteIOPort::_MOTORSTOPPED : MCHEmul::DatasetteIOPort::_MOTORRUNNING));
	// When something is written to the datasette, 
	// this bit is affected and it has to be notified too!
	if (c.bit (1))
		notify (MCHEmul::Event (MCHEmul::DatasetteIOPort::_WRITE, v.bit (1) ? 1 : 0));
}

// ---
void C264::IO7501PortRegisters::initializeInternalValues ()
{
	// The bit 5 is not used the rest are up by default...
	_portValue = 0xd8; 

	// The bits 0,1,2 and 3 are always output. 4, 6 and 7 are input. 5 is not used...
	setValue (0x00, 0x0f);
	// bit 3 up (motor = off) 
	// bits 0,1,2 down (there is no data in the output lines).
	// bit 5 is not used, and bits 6 and 7 are up...
	setValue (0x01, 0xd8);
}
