#include <CORE/SoundVoice.hpp>

// ---
void MCHEmul::SoundVoice::initialize ()
{ 
	_active = false; // By default...

	_attack = _decay = _release = 0; 
	
	_sustainVolumen = 0.0f; 

	_state = State::_ATTACK;
		  
	calculateVoiceSamplingData ();
	
	for (auto i : _waves) 
		i -> initialize (); 
}

// ---
void MCHEmul::SoundVoice::clock (unsigned int nC)
{ 
	if (!_active)
		return;

	for (auto i : _waves)
		i -> clock (nC);

	if (_state == State::_SUSTAIN)
		return;

	MCHEmul::SoundVoice::StateCounters& sCA = _stateCounters [(int) _state];
	if (++sCA._counterCyclesPerState > sCA._cyclesPerState)
	{
		switch (_state)
		{
			case State::_ATTACK: 
				_state = State::_DECAY;
				break;

			case State::_DECAY:
				_state = State::_SUSTAIN;
				break;

			/** This situation can not happen,
				as the _SUSTAIN state can only be exit desactivating the signal. */
			case State::_SUSTAIN:
				break;

			/** When the limit is reached the _RELEASE state is maintained. */
			case State::_RELEASE:
				_state = State::_IDLE;
				break;

			default:
				break;
		}

		// Actualize the counters...
		MCHEmul::SoundVoice::StateCounters& sCN = _stateCounters [(int) _state];
		if ((sCN._counterCyclesPerState = sCA._counterCyclesPerState - sCA._cyclesPerState) >= sCN._cyclesPerState)
			sCN._counterCyclesPerState = 0;

		// The limit of the previous state was reached...
		sCA._limit = true;
		// ...and its counter has to been set back to 0...
		sCA._counterCyclesPerState = 0;

		// This is only valid when origin and destination states are different...
		if (&sCA != &sCN)
			// ...and the limit for the new state has not been reached...
			sCN._limit = false;
			// The its counter was set above...
	}
}

// ---
MCHEmul::InfoStructure MCHEmul::SoundVoice::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	result.add ("ID", _id);
	result.add ("ACTIVE", _active);
	result.add ("ATTACK", _attack);
	result.add ("DECAY", _decay);
	result.add ("RELEASE", _release);
	result.add ("SUSTAIN", _sustainVolumen);
	result.add ("ADSR", "+" + std::to_string (_attack) + "," +
						"-" + std::to_string (_decay) + "," +
						"=" + std::to_string (_sustainVolumen) + "," +
						"--" + std::to_string (_release)); // Like a resume used sometimes...

	unsigned char ct = 0;
	MCHEmul::InfoStructure wDt;
	for (auto i : _waves)
		wDt.add (std::to_string (ct++), std::move (i -> getInfoStructure ()));
	result.add ("WAVES", wDt);

	return (result);
}

// ---
void MCHEmul::SoundVoice::calculateVoiceSamplingData ()
{
	_stateCounters [(int) State::_ATTACK]._cyclesPerState =
		(_attack == 0) ? 0 : (unsigned int) ((double) _chipFrequency * (double) _attack / 1000.0f);
	_stateCounters [(int) State::_DECAY]._cyclesPerState =
		(_decay == 0) ? 0 : (unsigned int) ((double) _chipFrequency * (double) _decay / 1000.0f);
	// The state _SUSTAIN has no duration...
	_stateCounters [(int) State::_RELEASE]._cyclesPerState =
		(_release == 0) ? 0 : (unsigned int) ((double) _chipFrequency * (double) _release / 1000.0f);
	// The state _IDLE has a duration of 0.0....

	initializeInternalCounters ();
}

// ---
double MCHEmul::SoundVoice::wavesData () const
{
	double result = 0; 
						 
	for (auto i : _waves) 
		if (i -> active ()) // Only when the wave active...
			result += i -> data ();

	// It can not be bigger that 1.0f...
	return ((result > 1.0f) ? 1.0f : result);
}

// ---
double MCHEmul::SoundVoice::ADSRData () const
{
	double result = 1.0f;

	if (!_ADSRActive)
		return (result);

	MCHEmul::SoundVoice::StateCounters& sC = _stateCounters [(int) _state];
	switch (_state)
	{
		case State::_ATTACK:
			// When the limit was reached, the state will have been move to _DECAY...
			result = MCHEmul::linearInterpolation 
				(0.0f, 0.0f, sC._cyclesPerState, 1.0f, sC._counterCyclesPerState);
			break;

		case State::_DECAY:
			// When the limit was reached, the state will have been move to _SUSTAIN...
			result = MCHEmul::linearInterpolation
				(0.0f, 1.0f, sC._cyclesPerState, _sustainVolumen, sC._counterCyclesPerState);
			break;

		case State::_SUSTAIN:
			// in _SUSTAIN there is no limits in time...
			result = _sustainVolumen;
			break;

		case State::_RELEASE:
			// When the limit was reached, the state will have been move to _IDLE...
			result = MCHEmul::linearInterpolation
				(0.0f, _sustainVolumen, sC._cyclesPerState, 0.0f, sC._counterCyclesPerState);
			break;

		case State::_IDLE:
			result = 0.0f;

		default:
			break;
	}

	return (result);
}
