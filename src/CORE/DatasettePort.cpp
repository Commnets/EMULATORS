#include <CORE/DatasettePort.hpp>
#include <CORE/DatasettePeripherals.hpp>
#include <CORE/CPU.hpp>

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
	// If there were another datasette previously...
	if (_datasette != nullptr)
		// No key is pressed...
		notify (MCHEmul::Event (_NOKEYPRESSED)); 

	// Assign the new datsette...
	_datasette = (p == nullptr) // It is possible to disconnect the peripheral simply...
		? nullptr : static_cast <MCHEmul::DatasettePeripheral*> (p);
	// In the parent class the peripheral is initialzed and finally connected...
	bool result = MCHEmul::IODevice::connectPeripheral (p);

	// If the datasette is finally connected, 
	// the status of the motor is transmited...
	if (_datasette != nullptr)
		_datasette -> setMotorOff (_motorStopped);

	return (result);
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

	_lastCPUCycles = cpu -> clockCycles ();

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
			_datasette -> setWrite (evnt.value () == 1, _lastCPUCycles);
	}
	else
	if (evnt.id () == _MOTORRUNNING || evnt.id () == _MOTORSTOPPED)
	{
		_motorStopped = evnt.id () == _MOTORSTOPPED;
		if (_datasette != nullptr)
			_datasette -> setMotorOff (_motorStopped);
	}
}
