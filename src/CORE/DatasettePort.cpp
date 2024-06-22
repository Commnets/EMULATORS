#include <CORE/DatasettePort.hpp>
#include <CORE/DatasettePeripherals.hpp>

// ---
bool MCHEmul::DatasetteIOPort::connectPeripheral (MCHEmul::IOPeripheral* p)
{
	// If it is not of the right type, it could be connected at all!
	if (p != nullptr && dynamic_cast <MCHEmul::DatasettePeripheral*> (p) == nullptr)
	{ 
		_error = MCHEmul::_PERIPHERAL_ERROR;

		return (false);
	}

	// There can be only one peripheral connected at the same time...
	if (!peripherals ().empty ())
		MCHEmul::IODevice::disconnectAllPeripherals ();
	if (_datasette != nullptr)
		notify (MCHEmul::Event (_NOKEYPRESSED)); // just in case...
	_datasette = (p == nullptr) // It is possible to disconnect the peripheral simply...
		? nullptr : static_cast <MCHEmul::DatasettePeripheral*> (p);
	return (MCHEmul::IODevice::connectPeripheral (p));
}

// ---
bool MCHEmul::DatasetteIOPort::simulate (MCHEmul::CPU* cpu)
{
	// Only modifications in the status are communicated...
	// If nothing changed nothing is communicated
	// This is just to improve performance.

	if (_datasette != nullptr)
	{
		if (_datasette -> keysChangedStatusRequested ())
			notify (MCHEmul::Event (_datasette -> noKeyPressed () ? _NOKEYPRESSED : _KEYPRESSED));
		if (_datasette -> readChangeValueRequested ())
			notify (MCHEmul::Event (_READ, _datasette -> read () ? 1 : 0)); 
	}

	// The standard simulation is invoked to
	// invoke the simulation methds for all devices connected...
	return (MCHEmul::IODevice::simulate (cpu));
}

// ---
void MCHEmul::DatasetteIOPort::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	// The events are usually received from the special chip,
	// controlling the port!

	if (evnt.id () == _WRITE)
	{
		if (_datasette != nullptr)
			_datasette -> setWrite (evnt.value () == 1);
	}
	else
	if (evnt.id () == _MOTORRUNNING || evnt.id () == _MOTORSTOPPED)
	{
		if (_datasette != nullptr)
			_datasette -> setMotorOff (evnt.id () == _MOTORSTOPPED);
	}
}
