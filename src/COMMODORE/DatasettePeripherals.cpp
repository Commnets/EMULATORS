#include <COMMODORE/DatasettePeripherals.hpp>

bool COMMODORE::DatasettePeripheral::initialize ()
{
	_valueRead = true;
	_valueToWrite = false;
	_motorOn = false;
	_noKeyPressed = true;

	return (true);
}
