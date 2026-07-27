#include <CORE/SoundEnvelope.hpp>

// ---
MCHEmul::SoundADSREnvelope::SoundADSREnvelope (unsigned int cF,
			unsigned short a, unsigned short d, unsigned short r)
	: MCHEmul::SoundEnvelope (cF),
	  _attack (a), _decay (d), _release (r),
	  _sustainVolumen (0.0f), _releaseInitialVolumen (0.0f),
	  _state (State::_ATTACK),
	  _stateCounters (5 /** The five internal states of the wave = ADSR+Idle */, StateCounters ()) // All to 0...
{
	calculateSamplingData ();
}

// ---
void MCHEmul::SoundADSREnvelope::setStart (bool s)
{ 
	bool iC = false;

	if (s) 
	{ 
		_state = MCHEmul::SoundADSREnvelope::State::_ATTACK; 

		iC = true; 
	}
	else /** Only when no idle. */ 
	if (_state != MCHEmul::SoundADSREnvelope::State::_IDLE) 
	{ 
		// The value from which the release phase starts.
		_releaseInitialVolumen = envelopeData (); 

		_state = State::_RELEASE; 

		iC = true; 
	} 
	
	if (iC) 
		initializeInternalCounters ();
}

// ---
void MCHEmul::SoundADSREnvelope::initialize ()
{ 
	_attack = _decay = _release = 0; 
	
	_sustainVolumen = 0.0f;
	_releaseInitialVolumen = 0.0f;

	_state = State::_ATTACK;

	calculateSamplingData ();
	// It also initializes the internal counters...
}

// ---
void MCHEmul::SoundADSREnvelope::initializeInternalCounters ()
{ 
	for (size_t i = 0; i < 5; i++) // The states of the ADSR (Just 4)...
		_stateCounters [i].initialize ();
}

// ---
void MCHEmul::SoundADSREnvelope::clock (unsigned int nC)
{
	if (nC == 0)
		return;

	if (_state == State::_SUSTAIN || _state == State::_IDLE)
		return;

	unsigned int remainingCycles = nC;
	while (remainingCycles > 0 &&
		   _state != State::_SUSTAIN &&
		   _state != State::_IDLE)
	{
		State currentState = _state;
		StateCounters& sC = _stateCounters [(int) currentState];

		// If the state has zero duration, 
		// it is completed immediately....
		if (sC._cyclesPerState == 0)
		{
			sC._counterCyclesPerState = 0;
			sC._limit = true;

			advanceState ();
			if (_state != State::_SUSTAIN && _state != State::_IDLE)
				_stateCounters [(int) _state].initialize ();

			continue;
		}

		unsigned int cyclesLeftInState =
			sC._cyclesPerState - sC._counterCyclesPerState;
		if (remainingCycles < cyclesLeftInState)
		{
			sC._counterCyclesPerState += remainingCycles;
			remainingCycles = 0;
		}
		else
		{
			// The current state reaches its limit.
			remainingCycles -= cyclesLeftInState;

			sC._counterCyclesPerState = sC._cyclesPerState;
			sC._limit = true;

			advanceState ();
			if (_state != State::_SUSTAIN && _state != State::_IDLE)
				_stateCounters [(int) _state].initialize ();
		}
	}
}

// ---
double MCHEmul::SoundADSREnvelope::envelopeData () const
{
	double result = 1.0f;

	if (!_active)
		return (result);

	MCHEmul::SoundADSREnvelope::StateCounters& sC = _stateCounters [(int) _state];
	switch (_state)
	{
		case MCHEmul::SoundADSREnvelope::State::_ATTACK:
			{
				// When the limit was reached, the state will have been move to _DECAY...
				result = (sC._cyclesPerState == 0) 
					? 1.0f
					: MCHEmul::linearInterpolation 
						(0.0f, 0.0f, sC._cyclesPerState, 1.0f, sC._counterCyclesPerState);
			}

			break;

		case MCHEmul::SoundADSREnvelope::State::_DECAY:
			{
				// When the limit was reached, the state will have been move to _SUSTAIN...
				result = (sC._cyclesPerState == 0) 
					? _sustainVolumen
					: MCHEmul::linearInterpolation
						(0.0f, 1.0f, sC._cyclesPerState, _sustainVolumen, sC._counterCyclesPerState);
			}

			break;

		case MCHEmul::SoundADSREnvelope::State::_SUSTAIN:
			{
				// in _SUSTAIN there is no limits in time...
				result = _sustainVolumen;
			}

			break;

		case MCHEmul::SoundADSREnvelope::State::_RELEASE:
			{
				// When the limit was reached, the state will have been move to _IDLE...
				result = (sC._cyclesPerState == 0) 
					? 0.0f
					: MCHEmul::linearInterpolation
						(0.0f, _releaseInitialVolumen, sC._cyclesPerState, 0.0f, sC._counterCyclesPerState);
			}

			break;

		case MCHEmul::SoundADSREnvelope::State::_IDLE:
			{
				result = 0.0f;
			}

			break;

		// Not needed, but to avoid warnings...
		default:
			break;
	}

	return (result);
}

// ---
MCHEmul::InfoStructure MCHEmul::SoundADSREnvelope::getInfoStructure () const
{
	MCHEmul::InfoStructure result = MCHEmul::SoundEnvelope::getInfoStructure ();

	result.add ("ATTACK", _attack);
	result.add ("DECAY", _decay);
	result.add ("SUSTAIN", _sustainVolumen);
	result.add ("RELEASE", _release);
	result.add ("ADSR", "+" + std::to_string (_attack) + "," +
						"-" + std::to_string (_decay) + "," +
						"=" + std::to_string (_sustainVolumen) + "," +
						"--" + std::to_string (_release)); // Like a resume used sometimes...

	return (result);
}

// ---
void MCHEmul::SoundADSREnvelope::advanceState ()
{
	switch (_state)
	{
		case State::_ATTACK:
			_state = State::_DECAY;
			break;

		case State::_DECAY:
			_state = State::_SUSTAIN;
			break;

		case State::_RELEASE:
			_state = State::_IDLE;
			break;

		default:
			break;
	}
}

// ---
void MCHEmul::SoundADSREnvelope::calculateSamplingData ()
{
	_stateCounters [(int) State::_ATTACK]._cyclesPerState =
		(_attack == 0) ? 0 : (unsigned int) ((double) _chipFrequency * (double) _attack / 1000.0f);
	_stateCounters [(int) State::_DECAY]._cyclesPerState =
		(_decay == 0) ? 0 : (unsigned int) ((double) _chipFrequency * (double) _decay / 1000.0f);
	// The state _SUSTAIN has no duration...
	_stateCounters [(int) State::_RELEASE]._cyclesPerState =
		(_release == 0) ? 0 : (unsigned int) ((double) _chipFrequency * (double) _release / 1000.0f);
	// The state _IDLE has a duration of 0.0....

	// To start back!
	initializeInternalCounters ();
}
