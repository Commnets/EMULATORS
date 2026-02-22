#include <COMMODORE/SerialPort.hpp>
#include <COMMODORE/SerialIOPeripherals.hpp>

// ---
COMMODORE::SerialIOPort::SerialIOPort (COMMODORE::SerialIOPeripheral* nS)
	: MCHEmul::IODevice (MCHEmul::IODevice::Type::_INPUTOUTPUT, _ID, 
		{ { "Name", "Serial Port" },
		  { "Type", "Input/Output" },
		  { "Manufacturer", "Commodore Business Machines CBM" } })
{ 
	if (nS != nullptr)
	{
		// The id has to be the max possible...
		assert (nS -> id () == 1000);

		connectPeripheral (nS);
	}
}

// ---
bool COMMODORE::SerialIOPort::connectPeripheral (MCHEmul::IOPeripheral* p)
{
	// If it is not of the right type, it could be connected at all!
	// and also if the limit od devices connected has been reached!
	if (dynamic_cast <COMMODORE::SerialIOPeripheral*> (p) == nullptr ||
		peripherals ().size () == 32)
	{
		_error = MCHEmul::_PERIPHERAL_ERROR;

		return (false);
	}

	// There cannot be but just a single peripheral with the same device number...
	COMMODORE::SerialIOPeripheral* pSIOP = 
		static_cast <COMMODORE::SerialIOPeripheral*> (p);
	for (const auto& i : peripherals ())
	{
		if ((static_cast <COMMODORE::SerialIOPeripheral*> (i.second) -> deviceNumber () ==
			pSIOP -> deviceNumber ()) && pSIOP != i.second)
		{
			_error = MCHEmul::_PERIPHERAL_ERROR;

			return (false);
		}
	}

	// Once at this point, 
	// the port admit several peripherals connected...
	return (MCHEmul::IODevice::connectPeripheral (p));
}
