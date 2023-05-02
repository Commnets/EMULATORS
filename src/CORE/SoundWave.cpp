#include <CORE/SoundWave.hpp>

// ---
void MCHEmul::SoundWave::initialize ()
{ 
	_frequency = 0;

	_active = false; // By default...
							  
	calculateWaveSamplingData (); 
}

// ---
void MCHEmul::SoundWave::calculateWaveSamplingData ()
{
	// How many cycles are needed to complet a wave?
	_cyclesPerWave= (_frequency != 0) 
		? (int) ((double) _chipFrequency / (double) _frequency) : 0;

	// Any time the calculation is done, the counter starts back from 0...
	_counterInCyclesPerWave = 0;
}

// ---
double MCHEmul::SawSmoothSoundWave::data () const
{
	if (_cyclesPerWave == 0)
		return (0.0f); // No data...

	return (MCHEmul::linearInterpolation 
		(0, 0, (double) _cyclesPerWave, 1.0f, (double) _counterInCyclesPerWave));
}

// ---
double MCHEmul::TriangleSoundWave::data () const
{
	if (_cyclesPerWave == 0)
		return (0.0f); // No data...

	return ((_counterInCyclesPerWave < (_cyclesPerWave >> 1))
		? MCHEmul::linearInterpolation (0.0f, 0.0f, (double) (_cyclesPerWave >> 1), 1.0f, (double) _counterInCyclesPerWave)
		: MCHEmul::linearInterpolation (0.0f, 1.0f, (double) (_cyclesPerWave >> 1), 
			0.0f, (double) (_counterInCyclesPerWave - (_cyclesPerWave >> 1))));
}

// ---
void MCHEmul::PulseSoundWave::initialize ()
{ 
	SoundWave::initialize (); 
	
	_pulseUpPercentage = 0.0f;
						  
	_pulseUp = false;

	_cyclesPulseUp = _counterCyclesPulseUp = 0;

	calculateWaveSamplingData ();
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
	if (_cyclesPerWave == 0)
		return (0.0f);

	return (_pulseUp ? 1.0f : 0.0f);
}

// ---
void MCHEmul::PulseSoundWave::calculateWaveSamplingData ()
{
	MCHEmul::SoundWave::calculateWaveSamplingData ();

	// How many cycles within the cyclesPerWave is the pulse up?
	_cyclesPulseUp = (int) ((double) _cyclesPerWave * (double) _pulseUpPercentage);
	// ...and down..simple:
	_cyclesPulseDown = _cyclesPerWave - _cyclesPulseUp;

	// Any time this method is invoked the counters are moved back to 0...
	_counterCyclesPulseUp = _counterCyclesPulseDown = 0;
}

// ---
double MCHEmul::NoiseSoundWave::data () const
{
	if (_cyclesPerWave == 0)
		return (0.0); // No data...

	return ((double) (std::rand () % 1000) / 1000.0f);
}
