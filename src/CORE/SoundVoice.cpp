#include <CORE/SoundVoice.hpp>

// ---
MCHEmul::SoundADSREnvelope::SoundADSREnvelope (unsigned int cF,
			unsigned short a, unsigned short d, unsigned short r)
	: MCHEmul::SoundEnvelope (cF),
	  _attack (a), _decay (d), _release (r),
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
	if (_state == State::_SUSTAIN)
		return;

	MCHEmul::SoundADSREnvelope::StateCounters& sCA = _stateCounters [(int) _state];
	if (++sCA._counterCyclesPerState > sCA._cyclesPerState)
	{
		switch (_state)
		{
			case MCHEmul::SoundADSREnvelope::State::_ATTACK: 
				_state = State::_DECAY;
				break;

			case MCHEmul::SoundADSREnvelope::State::_DECAY:
				_state = State::_SUSTAIN;
				break;

			/** This situation can not happen,
				as the _SUSTAIN state can only be exit desactivating the signal. \n
				This is the reason to be "SUSTIAN"! */
			case MCHEmul::SoundADSREnvelope::State::_SUSTAIN:
				break;

			/** When the limit is reached the _RELEASE state is maintained. */
			case MCHEmul::SoundADSREnvelope::State::_RELEASE:
				_state = MCHEmul::SoundADSREnvelope::State::_IDLE;
				break;

			default:
				break;
		}

		// Actualize the counters...
		MCHEmul::SoundADSREnvelope::StateCounters& sCN = _stateCounters [(int) _state];
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
double MCHEmul::SoundADSREnvelope::envelopeData () const
{
	double result = 1.0f;

	if (!_active)
		return (result);

	MCHEmul::SoundADSREnvelope::StateCounters& sC = _stateCounters [(int) _state];
	switch (_state)
	{
		case MCHEmul::SoundADSREnvelope::State::_ATTACK:
			// When the limit was reached, the state will have been move to _DECAY...
			result = MCHEmul::linearInterpolation 
				(0.0f, 0.0f, sC._cyclesPerState, 1.0f, sC._counterCyclesPerState);
			break;

		case MCHEmul::SoundADSREnvelope::State::_DECAY:
			// When the limit was reached, the state will have been move to _SUSTAIN...
			result = MCHEmul::linearInterpolation
				(0.0f, 1.0f, sC._cyclesPerState, _sustainVolumen, sC._counterCyclesPerState);
			break;

		case MCHEmul::SoundADSREnvelope::State::_SUSTAIN:
			// in _SUSTAIN there is no limits in time...
			result = _sustainVolumen;
			break;

		case MCHEmul::SoundADSREnvelope::State::_RELEASE:
			// When the limit was reached, the state will have been move to _IDLE...
			result = MCHEmul::linearInterpolation
				(0.0f, _sustainVolumen, sC._cyclesPerState, 0.0f, sC._counterCyclesPerState);
			break;

		case MCHEmul::SoundADSREnvelope::State::_IDLE:
			result = 0.0f;

		default:
			break;
	}

	return (result);
}

// ---
MCHEmul::InfoStructure MCHEmul::SoundADSREnvelope::getInfoStructure () const
{
	MCHEmul::InfoStructure result;

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

// ---
MCHEmul::SoundVoice::SoundVoice (int id, unsigned int cF, 
		const MCHEmul::SoundWaves& sw, MCHEmul::SoundEnvelope* sE)
	: MCHEmul::InfoClass ("SoundVoice"),
	  _id (id),
	  _chipFrequency (cF),
	  _active (false), // No active by default...
	  _envelope (sE),
	  _waves (sw),
	  _wavesPlain (nullptr)
{ 
	// Plain the waves vector to facilitate later looking for them...
	size_t mW = 0;
	for (const auto& i : _waves) 
		if ((size_t) i -> type () > mW) mW = (size_t) i -> type ();
	_wavesPlain = new MCHEmul::SoundWave* [mW + 1];
	for (size_t i = 0; i <= mW; ++i) _wavesPlain [i] = nullptr;
	for (const auto& i : _waves) _wavesPlain [(size_t) i -> type ()] = i;
}

// ---
MCHEmul::SoundVoice::~SoundVoice ()
{ 
	for (auto i : _waves) 
		delete (i); 

	delete [] _wavesPlain; // Delete the plain waves...

	// Delete the envelope...
	delete (_envelope);
}

// ---
void MCHEmul::SoundVoice::initialize ()
{ 
	_active = false; // By default...
	
	for (const auto& i : _waves) 
		i -> initialize (); 

	if (_envelope != nullptr)
		_envelope -> initialize ();
}

// ---
void MCHEmul::SoundVoice::initializeInternalCounters ()
{ 
	if (_envelope != nullptr) 
		_envelope -> initializeInternalCounters ();
	
	for (auto i : _waves) 
		i -> initializeInternalCounters ();
}

// ---
void MCHEmul::SoundVoice::clock (unsigned int nC)
{ 
	if (!_active)
		return;

	for (const auto& i : _waves)
		i -> clock (nC);

	if (_envelope != nullptr)
		_envelope -> clock (nC);
}

// ---
MCHEmul::InfoStructure MCHEmul::SoundVoice::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	result.add ("ID", _id);
	result.add ("ACTIVE", _active);
	result.add ("SoundEnvelope", std::move ((_envelope != nullptr) 
		? _envelope -> getInfoStructure () : MCHEmul::InfoStructure ())); // Empty when there is no envelope...

	unsigned char ct = 0;
	MCHEmul::InfoStructure wDt;
	for (auto i : _waves)
		wDt.add (std::to_string (ct++), std::move (i -> getInfoStructure ()));
	result.add ("WAVES", wDt);

	return (result);
}

// ---
double MCHEmul::SoundVoice::wavesData () const
{
	double result = 0.0; 
						 
	for (auto i : _waves) 
		if (i -> active ()) // Only when the wave active...
			result += i -> data ();

	// It can not be bigger that 1.0f...
	return ((result > 1.0f) ? 1.0f : result);
}
