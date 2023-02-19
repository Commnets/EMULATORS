#include <COMMODORE/DatasettePeripherals.hpp>

bool COMMODORE::DatasettePeripheral::initialize ()
{
	_valueRead = true;
	_valueToWrite = false;
	_motorOff = true;
	_noKeyPressed = true;

	return (true);
}
