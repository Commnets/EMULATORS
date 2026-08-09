#include <COMMODORE/CIA/CIASerialPort.hpp>
#include <COMMODORE/CIA/CIA.hpp>

// ---
void COMMODORE::CIASerialPort::initialize ()
{
	_status = Status::_READING;

	_value = MCHEmul::UByte::_0;

	_numberBitsTransmitted = 0;

	_interruptEnabled = _interruptRequested = false;

	_SPPin = false;
	_shiftRegister = _transmitBuffer = MCHEmul::UByte::_0;
	_transmitBufferFull = _transmissionActive = false;
	_generatedCNTSignal = true;
}

// ---
void COMMODORE::CIASerialPort::simulate
	(bool CNTRisingEdge, bool CNTFallingEdge, bool timerAUnderflow)
{
	if (_status == Status::_READING)
	{
		if (CNTRisingEdge &&
			addBit (_SPPin))
			_interruptRequested = true;

		return;
	}

	// The combined fast path avoids serial state work on almost every
	// emulated CIA cycle.
	if (!timerAUnderflow ||
		!_transmissionActive)
		return;

	bool oldCNTSignal = _generatedCNTSignal;
	_generatedCNTSignal = !_generatedCNTSignal;

	notify (MCHEmul::Event
		(COMMODORE::CIA::_CNTSIGNAL, _generatedCNTSignal ? 1 : 0));

	// Output data advances on the falling edge of the generated CNT clock.
	if (!oldCNTSignal || _generatedCNTSignal)
		return;

	bool bitToTransmit = false;
	if (removeBit (bitToTransmit))
	{
		_interruptRequested = true;

		// Keep a buffered second byte ready without touching the completed one.
		if (_transmitBufferFull)
			startTransmission ();
		else
			_transmissionActive = false;
	}

	notify (MCHEmul::Event
		(COMMODORE::CIA::_SPSIGNAL, bitToTransmit ? 1 : 0));
}

// ---
MCHEmul::InfoStructure COMMODORE::CIASerialPort::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	result.add ("Status", std::string ((_status == CIASerialPort::Status::_READING) ? "Read" : "Write"));
	result.add ("Buffer", _shiftRegister);

	return (result);
}
