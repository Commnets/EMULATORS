#include <COMMODORE/VIA/VIAPort.hpp>
#include <COMMODORE/VIA/VIAControlLines.hpp>
#include <COMMODORE/VIA/VIATimer.hpp>

// ---
COMMODORE::VIAPort::VIAPort (int id)
	: MCHEmul::InfoClass ("VIAPort"),
	  _id (id), 
	  _port (0, 1 /** 1 byte wide. */),
	  _T (nullptr), _CL1 (nullptr), _CL2 (nullptr),
	  _p6 (false)
{ 
	initialize (); /** To initialize the rest of the values. */ 
}

// ---
const MCHEmul::UByte& COMMODORE::VIAPort::value (bool r) const
{ 
	_IR = (_latch ? _valueLatched : _port.value ()[0]); 

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
	_p7 = false;
	_p6 = false;
}

// ---
MCHEmul::InfoStructure COMMODORE::VIAPort::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	result.add ("OR",	_OR);
	result.add ("IR",	_IR);
	result.add ("DDR",	_DDR);
	result.add ("Port",	_port.value ()[0]);

	return (result);
}

// ---
bool COMMODORE::VIAPort::simulate (MCHEmul::CPU* cpu)
{
	MCHEmul::UByte o = _OR;
	if (_T != nullptr &&
		(_T -> runMode () == VIATimer::RunMode::_ONESHOOTSIGNAL ||
			_T -> runMode () == VIATimer::RunMode::_CONTINUOUSSIGNAL))
		o.setBit (7, _p7); // _p7 always "above", is the timer run mode is the right one...
	{
		MCHEmul::UByte r = (o.value () | ~_DDR.value ()) & _port.value ()[0].value ();
		if (r != _lastPortValue)
			setPortValue (_lastPortValue = r);
	}
	
	// For pulse purposes, keep track of the value at bit 6...
	// Depending on the instance of the port (A or B) this value 
	// will be or not finally used!
	_p6.set (_port.value ()[0].bit (6));
	
	return (true);
}
