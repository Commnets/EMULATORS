#include <CORE/SoundVoice.hpp>

// ---
void MCHEmul::SoundVoice::clock (unsigned int nC)
{ 
	for (auto i : _waves)
		i -> clock (nC); 

	if (!_active)
		return;

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
double MCHEmul::SoundVoice::data () const
{ 
	double r = 0; 
						 
	for (auto i : _waves) 
		if (i -> active ()) // Only when the wave active...
			r += i -> data (); 
	if (r > 1.0f)
		r = 1.0f; // It can not be bigger that 1.0f

	// Time to modify the value according with the state of the wave...
	double f = 1.0f;
	MCHEmul::SoundVoice::StateCounters& sC = _stateCounters [(int) _state];
	switch (_state)
	{
		case State::_ATTACK:
			// When the limit was reached, the state will have been move to _DECAY...
			f = MCHEmul::linearInterpolation 
				(0.0f, 0.0f, sC._cyclesPerState, 1.0f, sC._counterCyclesPerState);
			break;

		case State::_DECAY:
			// When the limit was reached, the state will have been move to _SUSTAIN...
			f = MCHEmul::linearInterpolation
				(0.0f, 1.0f, sC._cyclesPerState, _sustainVolumen, sC._counterCyclesPerState);
			break;

		case State::_SUSTAIN:
			// in _SUSTAIN there is no limits in time...
			f = _sustainVolumen;
			break;

		case State::_RELEASE:
			// When the limit is reached, the sound is back to 0...
			f = (sC._limit)
				? 0.0f 
				: MCHEmul::linearInterpolation
					(0.0f, _sustainVolumen, sC._cyclesPerState, 0.0f, sC._counterCyclesPerState);
			break;
	}

	return (r * f);
}

// ---
void MCHEmul::SoundVoice::calculateWaveSamplingData ()
{
	_stateCounters [(int) State::_ATTACK]._cyclesPerState =
		(_attack == 0) ? 0 : (unsigned int) ((double) _chipFrequency * (double) _attack / 1000.0f);
	_stateCounters [(int) State::_DECAY]._cyclesPerState =
		(_decay == 0) ? 0 : (unsigned int) ((double) _chipFrequency * (double) _decay / 1000.0f);
	// The state _SUSTAIN has no duration...
	_stateCounters [(int) State::_RELEASE]._cyclesPerState =
		(_release == 0) ? 0 : (unsigned int) ((double) _chipFrequency * (double) _release / 1000.0f);

	// Initialize the counters...
	for (size_t i = 0; i < 4; _stateCounters [i++]._counterCyclesPerState = 0);
}

