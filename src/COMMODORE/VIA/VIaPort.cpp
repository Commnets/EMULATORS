#include <COMMODORE/VIA/VIAPort.hpp>
#include <COMMODORE/VIA/VIAControlLines.hpp>
#include <COMMODORE/VIA/VIATimer.hpp>

// ---
COMMODORE::VIAPort::VIAPort (int id, bool portA)
	: MCHEmul::InfoClass ("VIAPort"),
	  // It is not needed to invoke the notifier constructor...
	  _id (id), 
	  _port (0, 1 /** 1 byte wide. */),
	  _externalPins (MCHEmul::UByte::_FF),
	  _portA (portA),
	  _CL1 (nullptr), _CL2 (nullptr)
{ 
	initialize (); /** To initialize the rest of the values. */ 
}

// ---
void COMMODORE::VIAPort::latchValue ()
{
	// A control-line edge can precede the periodic port simulation after OR/DDR writes.
	captureExternalPins ();
	refreshPortValue ();
	_valueLatched = portValue ();
}

// ---
const MCHEmul::UByte& COMMODORE::VIAPort::value (bool r) const
{
	_IR = (_latch ? _valueLatched : portValue ()); 

	// Reading the VIAPort,
	// The ControlLine linked might affected if r = true...
	if (r)
	{ 
		assert (_CL1 != nullptr && 
				_CL2 != nullptr);

		// What to do will depend of the control line itself,
		// and in the mode it is in...
		_CL1 -> whenReadWritePort (true /** Meaning read. */, _portA);
		_CL2 -> whenReadWritePort (true, _portA);
	}

	return (_IR);
}

// ---
void COMMODORE::VIAPort::setValue (const MCHEmul::UByte& v, bool r)
{ 
	_OR = v; 

	// Writting the VIAPort, 
	// Applicable C1/C2 flags are acknowledged according to their modes.
	// Non-handshake accesses skip those side effects.
	if (r)
	{ 
		assert (_CL1 != nullptr && 
				_CL2 != nullptr);

		// What to do will depend of the control line itself,
		// and in the mode it is in...
		_CL1 -> whenReadWritePort (false /** Meaning write. */, _portA);
		_CL2 -> whenReadWritePort (false, _portA);
	}
}

// ---
void COMMODORE::VIAPort::initialize ()
{
	_latch = false;

	_valueLatched = MCHEmul::UByte::_0;

	_externalPins = MCHEmul::UByte::_FF;
	_port.setValue (MCHEmul::UBytes ({ _externalPins })); // A notification might be done...

	_DDR = MCHEmul::UByte::_0;
	_OR  = MCHEmul::UByte::_0;
	_IR  = MCHEmul::UByte::_0;

	// The implementation values...
	_lastPortValue = MCHEmul::UByte::_0;
}

// ---
bool COMMODORE::VIAPort::simulate (MCHEmul::CPU* cpu)
{
	captureExternalPins ();
	refreshPortValue ();

	return (true);
}

// ---
void COMMODORE::VIAPort::captureExternalPins ()
{
	if (portValue () != _lastPortValue)
	{
		MCHEmul::UByte direction = effectiveDDR ();

		// Alternate outputs must not be captured as externally supplied inputs.
		_externalPins = (_externalPins & direction) | (portValue () & ~direction);
	}
}

// ---
MCHEmul::UByte COMMODORE::VIAPort::resolvedValue
	(const MCHEmul::UByte& external) const
{
	MCHEmul::UByte direction = effectiveDDR ();

	return ((outputValue () & direction) | (external & ~direction));
}

// ---
void COMMODORE::VIAPort::refreshPortValue ()
{
	MCHEmul::UByte r = resolvedValue (_externalPins);
	if (r != _lastPortValue)
	{
		notifyPortChanges (r ^ _lastPortValue, r);

		setPortValue (_lastPortValue = r);
	}
}

// ---
MCHEmul::InfoStructure COMMODORE::VIAPort::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	result.add ("OR",	_OR);
	result.add ("IR",	_IR);
	result.add ("DDR",	_DDR);
	result.add ("Port",	portValue ());

	return (result);
}

// ---
void COMMODORE::VIAPort::notifyPortChanges 
	(const MCHEmul::UByte& c, const MCHEmul::UByte& v)
{ 
	notify (MCHEmul::Event (_VIAPORTIOBITSACTUALIZED, (unsigned int) v.value ()));
}

// ---
COMMODORE::VIAPortB::VIAPortB (int id)
	: COMMODORE::VIAPort (id, false),
	  _T (nullptr),
	  _p6 (true), _p7 (true)
{
	initialize (); /** To initialize the rest of the values. */ 
}

// ---
bool COMMODORE::VIAPortB::timerControlsP7 () const
{
	if (_T == nullptr)
		return (false);

	VIATimer::RunMode mode = _T -> runMode ();
	return (mode == VIATimer::RunMode::_ONESHOOTSIGNAL ||
		mode == VIATimer::RunMode::_CONTINUOUSSIGNAL);
}

// ---
MCHEmul::UByte COMMODORE::VIAPortB::effectiveDDR () const
{
	MCHEmul::UByte result = _DDR;

	// ACR7 enables the timer output independently of the stored DDRB7.
	if (timerControlsP7 ())
		result.setBit (7, true);

	return (result);
}

// ---
const MCHEmul::UByte& COMMODORE::VIAPortB::value (bool r) const
{
	// Preserve input selection and the existing control-line side effects.
	COMMODORE::VIAPort::value (r);

	MCHEmul::UByte direction = effectiveDDR ();
	MCHEmul::UByte out = outputValue ();

	return (_IR = ((_IR & ~direction) | (out & direction)));
}

// ---
void COMMODORE::VIAPortB::initialize ()
{
	COMMODORE::VIAPort::initialize ();

	_p7 = true;
	_p6 = true;
}

// ---
bool COMMODORE::VIAPortB::simulate (MCHEmul::CPU* cpu)
{
	captureExternalPins ();
	refreshPortValue ();

	// Timer 2 counts negative-going levels sampled on PB6.
	_p6.set (portValue ().bit (6));
	
	return (true);
}

// ---
MCHEmul::UByte COMMODORE::VIAPortB::outputValue () const
{
	MCHEmul::UByte result = _OR;
	if (timerControlsP7 ())
		result.setBit (7, _p7);

	return (result);
}

// ---
void COMMODORE::VIAPortB::notifyPortChanges 
	(const MCHEmul::UByte& c, const MCHEmul::UByte& v)
{ 
	if (c.bit (3)) // Bit 3 is the data line of the datasette...
		notify (MCHEmul::Event (_VIAPORTN3ACTUALIZED, (unsigned int) (v.bit (3) ? 1 : 0)));

	// A simultaneous PB3 change must not hide changes on the other seven pins.
	COMMODORE::VIAPort::notifyPortChanges (c, v);
}
