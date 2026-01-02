#include <C264/C16SenseTapeRegisters.hpp>

// ---
C264::C16SenseTapeRegisters::C16SenseTapeRegisters 
		(int id, MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: MCHEmul::PhysicalStorageSubset (id, ps, pp, a, s),
	  _lastValueRead (MCHEmul::PhysicalStorage::_DEFAULTVALUE)
{ 
	initializeInternalValues ();
}

// ---
void C264::C16SenseTapeRegisters::initialize ()
{
	initializeInternalValues ();
}

// ---
const MCHEmul::UByte& C264::C16SenseTapeRegisters::readValue (size_t p) const
{ 
	MCHEmul::UByte result = MCHEmul::UByte::_FF;
	result.setBit (2, _cassetteKeyPressed ? false : true);
	return (_lastValueRead = result);
}

// ---
void C264::C16SenseTapeRegisters::initializeInternalValues ()
{
	_cassetteKeyPressed = false;
}

// ---
void C264::C16SenseTapeRegisters::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	if (evnt.id () == MCHEmul::DatasetteIOPort::_KEYPRESSED ||
		evnt.id () == MCHEmul::DatasetteIOPort::_NOKEYPRESSED)
		_cassetteKeyPressed = (evnt.id () == MCHEmul::DatasetteIOPort::_KEYPRESSED);
}
