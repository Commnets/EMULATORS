#include <COMMODORE/CIA/CIASerialPort.hpp>
#include <COMMODORE/CIA/CIA.hpp>

// ---
void COMMODORE::CIASerialPort::initialize ()
{
	_status = Status::_READING;

	_value = MCHEmul::UByte::_0;

	_toTransmit = false;

	_numberBitsTransmitted = 0;

	_interruptEnabled = _interruptRequested = false;

	_SPPin = false;
	_generatedCNTSignal = true;
}

// ---
void COMMODORE::CIASerialPort::simulate
	(bool CNTRisingEdge, bool CNTFallingEdge, bool timerAUnderflow, bool timerAContinuous)
{
	switch (_status)
	{
		case Status::_READING:
			{
				if (CNTRisingEdge &&
					addBit (_SPPin))
					_interruptRequested = true;
			}

			break;

		case Status::_SAVING:
			{
				if (!_toTransmit ||
					!timerAUnderflow ||
					!timerAContinuous)
					break;

				bool oldCNTSignal = _generatedCNTSignal;
				_generatedCNTSignal = !_generatedCNTSignal;

				notify (MCHEmul::Event
					(COMMODORE::CIA::_CNTSIGNAL, _generatedCNTSignal ? 1 : 0));

				// Output data advances on the falling edge of the generated CNT clock.
				if (oldCNTSignal && !_generatedCNTSignal)
				{
					bool bitToTransmit = false;
					if (removeBit (bitToTransmit))
					{
						_interruptRequested = true;
						_toTransmit = false;
					}

					notify (MCHEmul::Event
						(COMMODORE::CIA::_SPSIGNAL, bitToTransmit ? 1 : 0));
				}
			}

			break;

		default:
			break;
	}
}

// ---
MCHEmul::InfoStructure COMMODORE::CIASerialPort::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	result.add ("Status", std::string ((_status == CIASerialPort::Status::_READING) ? "Read" : "Write"));
	result.add ("Buffer", _bufferValue);

	return (result);
}
