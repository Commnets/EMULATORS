#include <COMMODORE/DatasettePeripherals.hpp>

bool COMMODORE::DatasettePeripheral::initialize ()
{
	_valueRead = true;
	_valueToWrite = true;
	_motorOff = true;
	_noKeyPressed = true;

	return (true);
}
