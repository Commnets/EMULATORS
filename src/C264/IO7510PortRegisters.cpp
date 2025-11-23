#include <C264/IO7510PortRegisters.hpp>
#include <COMMODORE/incs.hpp>

// ---
C264::IO7501PortRegisters::IO7501PortRegisters (MCHEmul::PhysicalStorage* ps)
	: F6500::IO7501PortRegisters (_IO7501REGISTERS_SUBSET, ps)
{
	// The actions over this registers are not buffered, 
	// and the consequence in PLA must be inmediate...
	setBufferMemorySetCommands (false);

	initializeInternalValues (); 
}

// ---
void C264::IO7501PortRegisters::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* ntier)
{
	// A bit is read in the datasette....
	if (evnt.id () == MCHEmul::DatasetteIOPort::_READ)
		setBitPortValue (7, evnt.value () == 1 ? true : false);
	// Bit 6 reflects whether some key has been pressed on the datasette...
	// When something has been pressed this bit is set to 0, 1 in the other situation...
	// The same bit in the memory position 0 hast to be defined as input (value = 0)
	if (evnt.id () == MCHEmul::DatasetteIOPort::_KEYPRESSED ||
		evnt.id () == MCHEmul::DatasetteIOPort::_NOKEYPRESSED)
		setBitPortValue (6, evnt.id () == MCHEmul::DatasetteIOPort::_NOKEYPRESSED);
}

// ---
void C264::IO7501PortRegisters::notifyPortChanges (const MCHEmul::UByte& c, const MCHEmul::UByte& v)
{
	// Modify the status of the motor of the casette, if something changed in this bit!
	if (c.bit (5))
		notify (MCHEmul::Event (v.bit (5)
			? MCHEmul::DatasetteIOPort::_MOTORSTOPPED : MCHEmul::DatasetteIOPort::_MOTORRUNNING));
	// When something is written to the datasette, 
	// this bit is affected and it has to be notified too!
	if (c.bit (4))
		notify (MCHEmul::Event (MCHEmul::DatasetteIOPort::_WRITE, v.bit (4) ? 1 : 0));
}

// ---
void C264::IO7501PortRegisters::initializeInternalValues ()
{
	// This is an input from the casette and it is not automatically initialized...
	_portValue = MCHEmul::UByte::_FF;

	setValue (0x00, 0x00);
	setValue (0x01, 0x00);
}
