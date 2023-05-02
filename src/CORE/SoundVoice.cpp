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

			// This situation will never happen...(@see above)
			case State::_SUSTAIN:
				_state = State::_RELEASE;
				break;

			case State::_RELEASE:
				_state = State::_ATTACK; /** Starts back. */
				break;

			default:
				break;
		}

		// Actualize the counters...
		MCHEmul::SoundVoice::StateCounters& sCN = _stateCounters [(int) _state];
		if ((sCN._counterCyclesPerState = sCA._counterCyclesPerState - sCA._cyclesPerState) >= sCN._cyclesPerState)
			sCN._counterCyclesPerState = 0;
		sCA._counterCyclesPerState = 0;
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
			f = MCHEmul::linearInterpolation 
				(0.0f, 0.0f, sC._cyclesPerState, 1.0f, sC._counterCyclesPerState);
			break;

		case State::_DECAY:
			f = MCHEmul::linearInterpolation
				(0.0f, 1.0f, sC._cyclesPerState, _sustainVolumen, sC._counterCyclesPerState);
			break;

		case State::_SUSTAIN:
			f = _sustainVolumen;
			break;

		case State::_RELEASE:
			f = MCHEmul::linearInterpolation
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

