#include <COMMODORE/VIA/VIAPort.hpp>
#include <COMMODORE/VIA/VIAControlLines.hpp>
#include <COMMODORE/VIA/VIATimer.hpp>

// ---
COMMODORE::VIAPort::VIAPort (int id)
	: MCHEmul::InfoClass ("VIAPort"),
	  // It is not needed to invoke the notifier constructor...
	  _id (id), 
	  _port (0, 1 /** 1 byte wide. */),
	  _CL1 (nullptr), _CL2 (nullptr)
{ 
	initialize (); /** To initialize the rest of the values. */ 
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
		_CL1 -> whenReadWritePort (true /** Meaning read. */); 
		_CL2 -> whenReadWritePort (true); 
	}

	return (_IR);
}

// ---
void COMMODORE::VIAPort::setValue (const MCHEmul::UByte& v, bool r)
{ 
	_OR = v; 

	// Writting the VIAPort, 
	// the interrupt of the two control lines are deleted...
	// But there could have been a possibility to do that without restarting...
	if (r)
	{ 
		assert (_CL1 != nullptr && 
				_CL2 != nullptr);

		// What to do will depend of the control line itself,
		// and in the mode it is in...
		_CL1 -> whenReadWritePort (false /** Meaning write. */); 
		_CL2 -> whenReadWritePort (false); 
	}
}

// ---
void COMMODORE::VIAPort::initialize ()
{
	_latch = false;

	_valueLatched = MCHEmul::UByte::_0;

	_port.setValue (MCHEmul::UBytes ({ MCHEmul::UByte::_0 })); // A notification might be done...

	_DDR = MCHEmul::UByte::_0;
	_OR  = MCHEmul::UByte::_0;
	_IR  = MCHEmul::UByte::_0;

	// The implementation values...
	_lastPortValue = MCHEmul::UByte::_0;
}

// ---
bool COMMODORE::VIAPort::simulate (MCHEmul::CPU* cpu)
{
	MCHEmul::UByte o = _OR;
	MCHEmul::UByte r = (o | ~_DDR) & portValue ();
	if (r != _lastPortValue)
	{
		notifyPortChanges (r, r ^ _lastPortValue);
		setPortValue (_lastPortValue = r);
	}

	return (true);
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
	: COMMODORE::VIAPort (id),
	  _T (nullptr),
	  _p6 (true), _p7 (true)
{
	initialize (); /** To initialize the rest of the values. */ 
}

// ---
const MCHEmul::UByte& COMMODORE::VIAPortB::value (bool r) const
{
	// The standard value of _IR is assigned ans returned in this invocation,
	// but it is changed as the behaviour of the PortB is a little bit different.
	// See the header of the class for more details...
	COMMODORE::VIAPort::value (r);
	// The real value is either the one latched or the last one sent to the port...
	return (_IR = ((_IR & ~_DDR /** Pins with 1 = output become 0 and viceversa. */) | _OR));
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
	MCHEmul::UByte o = _OR;
	if ((_T != nullptr &&
		 // The next two modes are only possible in Timer1 and not in Timer2...
		 (_T -> runMode () == VIATimer::RunMode::_ONESHOOTSIGNAL ||
		  _T -> runMode () == VIATimer::RunMode::_CONTINUOUSSIGNAL)) && _DDR.bit (7) /** as ouput. */) // The timer has priority over value...
		o.setBit (7, _p7); // _p7 always "above", is the timer run mode is the right one...

	MCHEmul::UByte r = (o | ~_DDR) & portValue ();
	if (r != _lastPortValue)
		setPortValue (_lastPortValue = r);
	
	// For pulse purposes, keep track of the value at bit 6...
	// This is tracked in Timer 2, when the mode is _PULSEDETECTOR.
	_p6.set (portValue ().bit (6));
	
	return (true);
}

// ---
void COMMODORE::VIAPortB::notifyPortChanges 
	(const MCHEmul::UByte& c, const MCHEmul::UByte& v)
{ 
	if (c.bit (3)) // Bit 3 is the data line of the datasette...
		notify (MCHEmul::Event (_VIAPORTN3ACTUALIZED, (unsigned int) (v.bit (3) ? 1 : 0)));
	else
		COMMODORE::VIAPort::notifyPortChanges (c, v); // The rest of the bits are managed by the parent class...
}
