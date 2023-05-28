#include <CORE/SoundWave.hpp>

// ---
void MCHEmul::SoundWave::initialize ()
{ 
	_active = false; // By default...

	_frequency = 0;

	calculateWaveSamplingData (); 

	_clockRestarted = false;
}

// ---
MCHEmul::InfoStructure MCHEmul::SoundWave::getInfoStructure () const
{
	MCHEmul::InfoStructure result = MCHEmul::InfoClass::getInfoStructure ();

	result.add ("ACTIVE", _active);
	result.add ("TYPE", (int) _type);
	result.add ("FREQUENCY", _frequency);

	return (result);
}

// ---
void MCHEmul::SoundWave::calculateWaveSamplingData ()
{
	// How many cycles are needed to complet a wave?
	_cyclesPerWave= (_frequency != 0) 
		? (int) ((double) _chipFrequency / (double) _frequency) : 0;

	initializeInternalCounters ();
}

// ---
double MCHEmul::TriangleSoundWave::data () const
{
	if (!_active)
		return (0.0f); // No active...

	if (_cyclesPerWave == 0)
		return (0.0f); // No data...

	return ((_counterInCyclesPerWave < (_cyclesPerWave >> 1))
		? MCHEmul::linearInterpolation (0.0f, 0.0f, (double) (_cyclesPerWave >> 1), 1.0f, (double) _counterInCyclesPerWave)
		: MCHEmul::linearInterpolation (0.0f, 1.0f, (double) (_cyclesPerWave >> 1), 
			0.0f, (double) (_counterInCyclesPerWave - (_cyclesPerWave >> 1))));
}

// ---
double MCHEmul::SawSmoothSoundWave::data () const
{
	if (!_active)
		return (0.0f); // No active...

	if (_cyclesPerWave == 0)
		return (0.0f); // No data...

	return (MCHEmul::linearInterpolation 
		(0, 0, (double) _cyclesPerWave, 1.0f, (double) _counterInCyclesPerWave));
}

// ---
MCHEmul::PulseSoundWave::PulseSoundWave (unsigned int cF)
	: MCHEmul::SoundWave (Type::_PULSE, cF),
	  _pulseUpPercentage (0),
	  _pulseUp (false),
	  _cyclesPulseUp (0), _counterCyclesPulseUp (0),
	  _cyclesPulseDown (0), _counterCyclesPulseDown (0)
{ 
	setClassName ("SoundPulseWave");
						  
	calculateWaveSamplingData (); 
}

// ---
void MCHEmul::PulseSoundWave::initialize ()
{ 
	SoundWave::initialize (); 
	
	_pulseUpPercentage = 0.0f;
						  
	_pulseUp = false;

	calculateWaveSamplingData ();
}

// ---
void MCHEmul::PulseSoundWave::initializeInternalCounters ()
{ 
	SoundWave::initializeInternalCounters ();
						  
	_counterCyclesPulseUp = _counterCyclesPulseDown = 0; 

	_pulseUp = false;
}

// --
MCHEmul::InfoStructure MCHEmul::PulseSoundWave::getInfoStructure () const
{
	MCHEmul::InfoStructure result = MCHEmul::SoundWave::getInfoStructure ();

	result.add ("PULSEUP", _pulseUpPercentage);

	return (result);
}

// ---
void MCHEmul::PulseSoundWave::clock (unsigned int nC)
{
	MCHEmul::SoundWave::clock (nC);

	if (_pulseUp)
	{
		if ((_counterCyclesPulseUp += nC) > _cyclesPulseUp)
		{
			_counterCyclesPulseDown = _counterCyclesPulseUp - _cyclesPulseUp;
			if (_counterCyclesPulseDown >= _cyclesPulseDown)
				_counterCyclesPulseDown = 0;

			_counterCyclesPulseUp = 0;

			_pulseUp = false;
		}
	}
	else
	{
		if ((_counterCyclesPulseDown += nC) > _cyclesPulseDown)
		{
			_counterCyclesPulseUp = _counterCyclesPulseDown - _cyclesPulseDown;
			if (_counterCyclesPulseUp >= _cyclesPulseUp)
				_counterCyclesPulseUp = 0;

			_counterCyclesPulseDown = 0;

			_pulseUp = true;
		}
	}
}

// ---
double MCHEmul::PulseSoundWave::data () const
{
	if (!_active)
		return (0.0f); // No active...

	if (_cyclesPerWave == 0)
		return (0.0f);

	return (_pulseUp ? 1.0f : 0.0f);
}

// ---
void MCHEmul::PulseSoundWave::calculateWaveSamplingData ()
{
	// How many cycles within the cyclesPerWave is the pulse up?
	_cyclesPulseUp = (int) ((double) _cyclesPerWave * (double) _pulseUpPercentage);
	// ...and down..simple:
	_cyclesPulseDown = _cyclesPerWave - _cyclesPulseUp;

	_pulseUp = false;

	MCHEmul::SoundWave::calculateWaveSamplingData ();
}

// ---
double MCHEmul::NoiseSoundWave::data () const
{
	if (!_active)
		return (0.0f); // No active...

	if (_cyclesPerWave == 0)
		return (0.0); // No data...

	return ((double) (std::rand () % 1000) / 1000.0f);
}
