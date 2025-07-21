#include <C64/SerialPort.hpp>
#include <C64/CIA2.hpp>

// ---
void C64::SerialIOPort::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	if (evnt.id () == COMMODORE::CIARegisters::_PORTAACTUALIZED)
	{
		// TODO
	}
	else
	if (evnt.id () == COMMODORE::CIARegisters::_PORTBACTUALIZED)
	{
		// TODO
	}
}
