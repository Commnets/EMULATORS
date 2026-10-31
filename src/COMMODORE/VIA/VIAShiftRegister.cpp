#include <COMMODORE/VIA/VIAShiftRegister.hpp>
#include <COMMODORE/VIA/VIAControlLines.hpp>
#include <COMMODORE/VIA/VIATimer.hpp>

// ---
COMMODORE::VIAShiftRegister::VIAShiftRegister (int id)
	: MCHEmul::InfoClass ("VIAShiftRegister"),
	  _id (id),
	  _mode (COMMODORE::VIAShiftRegister::ShiftMode::_DISABLE),
	  _value (MCHEmul::UByte::_0),
	  _T (nullptr), _CL1 (nullptr), _CL2 (nullptr),
	  _interruptEnabled (false),
	  _interruptRequested (false)
{ 
	initialize (); // To assign the implementation variables...
}

// ---
void COMMODORE::VIAShiftRegister::setMode (COMMODORE::VIAShiftRegister::ShiftMode m)
{
	// Updating other ACR fields must not restart the current serial transfer.
	if (_mode == m)
		return;

	_mode = m;
	_numberBits = 0;
	_finishPending = false;
	_timerCounter = 0;
	_clockPhase = true;
	_shifting = (_mode == ShiftMode::_INUNDERSIGNALCL1 ||
		_mode == ShiftMode::_OUTUNDERSIGNALCL1);

	if (_mode == ShiftMode::_DISABLE)
		_interruptRequested = false;

	updateCB2Function ();

	if (_CL1 != nullptr &&
		(_mode == ShiftMode::_INUNDERTIMER || _mode == ShiftMode::_INATCLOCK ||
		 _mode == ShiftMode::_OUTFREERUNNING || _mode == ShiftMode::_OUTUNDERTIMER ||
		 _mode == ShiftMode::_OUTATCLOCK))
		_CL1 -> setValue (true);
}

// ---
void COMMODORE::VIAShiftRegister::updateCB2Function () const
{
	// The constructor initializes the SR before its control lines are connected.
	if (_CL2 == nullptr)
		return;

	using Function = COMMODORE::VIAControlLineType2::Function;
	switch (_mode)
	{
		case ShiftMode::_DISABLE:
			{
				_CL2 -> setFunction (Function::_PERIPHERAL);
			}

			break;

		case ShiftMode::_INUNDERTIMER:
		case ShiftMode::_INATCLOCK:
		case ShiftMode::_INUNDERSIGNALCL1:
			{
				_CL2 -> setFunction (Function::_SERIALINPUT);
			}

			break;

		case ShiftMode::_OUTFREERUNNING:
		case ShiftMode::_OUTUNDERSIGNALCL1:
			{
				_CL2 -> setFunction (Function::_SERIALOUTPUT);
			}

			break;

		case ShiftMode::_OUTUNDERTIMER:
		case ShiftMode::_OUTATCLOCK:
			{
				_CL2 -> setFunction (_shifting
					? Function::_SERIALOUTPUT
					: Function::_SERIALOUTPUTIDLE);
			}

			break;

		default:
			{
				assert (false);
			}

			break;
	}
}

// ---
void COMMODORE::VIAShiftRegister::startTransfer () const
{
	_numberBits = 0;
	_shifting = (_mode != ShiftMode::_DISABLE);
	_finishPending = false;
	_clockPhase = true;
	_interruptRequested = false;

	if (_mode == ShiftMode::_INUNDERTIMER || _mode == ShiftMode::_OUTFREERUNNING ||
		_mode == ShiftMode::_OUTUNDERTIMER)
	{
		assert (_T != nullptr);
		_timerCounter = (unsigned short) ((_T -> initialValue () & 0x00ff) + 1);
	}
	else
		_timerCounter = 0;

	updateCB2Function ();

	if (_CL1 != nullptr &&
		(_mode == ShiftMode::_INUNDERTIMER || _mode == ShiftMode::_INATCLOCK ||
		 _mode == ShiftMode::_OUTFREERUNNING || _mode == ShiftMode::_OUTUNDERTIMER ||
		 _mode == ShiftMode::_OUTATCLOCK))
		_CL1 -> setValue (true);

	// Both SR reads and writes can start another output transfer.
	if (_CL2 != nullptr &&
		(_mode == ShiftMode::_OUTFREERUNNING || _mode == ShiftMode::_OUTUNDERTIMER ||
		 _mode == ShiftMode::_OUTATCLOCK || _mode == ShiftMode::_OUTUNDERSIGNALCL1))
		_CL2 -> setValue (_value.bit (7));
}

// ---
const MCHEmul::UByte COMMODORE::VIAShiftRegister::value () const
{
	startTransfer ();

	return (_value);
}

// ---
void COMMODORE::VIAShiftRegister::setValue (const MCHEmul::UByte& v)
{
	_value = v;

	startTransfer ();
}

// ---
void COMMODORE::VIAShiftRegister::initialize ()
{
	_mode = COMMODORE::VIAShiftRegister::ShiftMode::_DISABLE;

	_value = MCHEmul::UByte::_0;

	_interruptEnabled = _interruptRequested = false;

	_numberBits = 0;
	_shifting = false;
	_finishPending = false;
	_timerCounter = 0;
	_clockPhase = true;

	updateCB2Function ();
}


// ---
bool COMMODORE::VIAShiftRegister::simulate (MCHEmul::CPU* cpu)
{
	assert (cpu != nullptr);

	if (_mode == ShiftMode::_DISABLE)
	{
		_interruptRequested = false;
		return (true);
	}

	assert (_CL1 != nullptr && _CL2 != nullptr);

	bool shift = false;
	bool internalClock = false;
	switch (_mode)
	{
		case ShiftMode::_INUNDERTIMER:
		case ShiftMode::_OUTFREERUNNING:
		case ShiftMode::_OUTUNDERTIMER:
			{
				assert (_T != nullptr);
				internalClock = true;

				if (_shifting)
				{
					if (_timerCounter == 0)
					{
						_timerCounter = (unsigned short) ((_T -> initialValue () & 0x00ff) + 1);
						_clockPhase = !_clockPhase;
						_CL1 -> setValue (_clockPhase);
						shift = !_clockPhase;
					}
					else
						--_timerCounter;
				}
			}
			break;

		case ShiftMode::_INATCLOCK:
		case ShiftMode::_OUTATCLOCK:
			{
				internalClock = true;
				if (_shifting)
				{
					_clockPhase = !_clockPhase;
					_CL1 -> setValue (_clockPhase);
					shift = !_clockPhase;
				}
			}
			break;

		case ShiftMode::_INUNDERSIGNALCL1:
		case ShiftMode::_OUTUNDERSIGNALCL1:
			shift = _CL1 -> peekPositiveEdge ();
			break;

		default:
			break;
	}

	// The eighth internally generated pulse still needs its rising half-cycle.
	if (internalClock && _finishPending && _clockPhase)
	{
		_finishPending = false;
		_shifting = false;

		updateCB2Function ();

		return (true);
	}

	if (!shift)
		return (true);

	bool complete = false;
	if (_mode == ShiftMode::_INUNDERTIMER || _mode == ShiftMode::_INATCLOCK ||
		_mode == ShiftMode::_INUNDERSIGNALCL1)
		complete = shiftIn (_CL2 -> value ());
	else
	{
		bool output = false;
		complete = shiftOut (output);
		_CL2 -> setValue (output);
	}

	if (!complete)
		return (true);

	if (_mode != ShiftMode::_OUTFREERUNNING)
		_interruptRequested = true;

	if (_mode == ShiftMode::_INUNDERTIMER || _mode == ShiftMode::_INATCLOCK ||
		_mode == ShiftMode::_OUTUNDERTIMER || _mode == ShiftMode::_OUTATCLOCK)
		_finishPending = true;

	return (true);
}

// ---
MCHEmul::InfoStructure COMMODORE::VIAShiftRegister::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	result.add ("MODE",			(unsigned char) _mode);
	result.add ("VALUE",		_value);
	result.add ("NUMBERBITS",	_numberBits);

	return (result);
}
