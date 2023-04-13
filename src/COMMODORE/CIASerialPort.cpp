#include <COMMODORE/CIASerialPort.hpp>
#include <COMMODORE/CIATimer.hpp>

// ---
void COMMODORE::CIASerialPort::initialize ()
{
	_status = Status::_READING;

	_value = MCHEmul::UByte::_0;

	_toTransmit = false;

	_numberBitsTransmitted = 0;

	_interruptEnabled = _interruptRequested = false;

	_CNTPin = _SPPin = false;

	_CNTRaisingEdge = _CNTPulse = false;
}

// ---
void COMMODORE::CIASerialPort::simulate (MCHEmul::CPU* cpu, COMMODORE::CIATimer* t)
{
	bool rE = CNTRaisingEdge ();
	bool cP = CNTPulse ();

	switch (_status)
	{
		case (Status::_READING):
			{
				if (rE &&
					addBit (_SPPin) && // When the buffer is filled up, 
									   // true is returned and the bufferValue is moved into the _value variable!
					_interruptEnabled) // If interruptions are active one will be generated!...
					cpu -> interrupt (_interruptId) -> setActive (_interruptRequested = true);
			}

			break;

		case (Status::_SAVING):
			{
				// The value might only be transmitted if it has been configurated
				// the timer is running in continious mode and it has reach out half of the value...
				if (!_toTransmit ||
					!t -> reachesHalf () || 
					t -> runMode () != CIATimer::RunMode::_RESTART)
					break;

				// It is transmited just in the falling eded of the CNT line...
				bool bT = false;
				if (cP)
				{
					if (removeBit (bT))
					{ 
						if (_interruptEnabled)
						{
							_interruptRequested = true;

							cpu -> interrupt (_interruptId) -> setActive (true);
						}
					}
					// If the output hasn't finished, just a notification is needed
					// for the ouput devices to take this into account...
					else
						notify (MCHEmul::Event (0, bT ? 1 : 0));

					// The CNT is push back up!
					setCNTSignal (true);
					// ...and notified...
					notify (MCHEmul::Event (0, 1));
				}
			}

			break;

		/** Any other status does nothing. */
		default:
			break;
	}
}

// ---
MCHEmul::InfoStructure COMMODORE::CIASerialPort::getInfoStructure () const
{
	MCHEmul::InfoStructure result = MCHEmul::InfoClass::getInfoStructure ();

	result.add ("Status", std::string ((_status == CIASerialPort::Status::_READING) ? "Read" : "Write"));
	result.add ("Buffer", _bufferValue);

	return (result);
}
