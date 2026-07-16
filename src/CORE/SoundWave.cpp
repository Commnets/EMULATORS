#include <CORE/SoundWave.hpp>

// ---
void MCHEmul::SoundWave::initialize ()
{ 
	_active = false; // By default...

	_frequency = 0.0f;

	calculateWaveSamplingData (); 

	_clockRestarted = false;
}

// ---
void MCHEmul::SoundWave::clock (unsigned int nC)
{ 
	if (_cyclesPerWave <= 0.0)
		return;

	_counterInCyclesPerWave += (double) nC;
	if (_counterInCyclesPerWave >= _cyclesPerWave)
	{
		_clockRestarted = true;
		_counterInCyclesPerWave =
			std::fmod (_counterInCyclesPerWave, _cyclesPerWave);
	}
}

// ---
MCHEmul::InfoStructure MCHEmul::SoundWave::getInfoStructure () const
{
	MCHEmul::InfoStructure result = MCHEmul::InfoClass::getInfoStructure ();

	result.add ("ACTIVE", _active);
	result.add ("TYPE", (int) _type);
	result.add ("FREQUENCY", _frequency);
	result.add ("TYPEANDFREQUENCY", std::to_string ((int) _type) + "(" + 
		std::to_string (_frequency) + ")"); // like a summary used sometimes...

	return (result);
}

// ---
void MCHEmul::SoundWave::calculateWaveSamplingData ()
{
	double oldClockValue = clockValue ();

	// How many cycles are needed to complet a wave?
    // Number of chip cycles needed to complete one wave period.
    // It is intentionally fractional to avoid pitch error.
	_cyclesPerWave = (_frequency != 0.0f) 
		? ((double) _chipFrequency / _frequency)
		: 0.0f;

	// To preserve the position in the wave when the frequency is changed, 
	// we calculate the new counter value in cycles per wave.
	_counterInCyclesPerWave = (_cyclesPerWave > 0.0)
		? oldClockValue * _cyclesPerWave
		: 0.0f;

	// The internal counters are not changed...
}

// ---
double MCHEmul::TriangleSoundWave::data () const
{
	if (!_active)
		return (0.0f);

	if (_cyclesPerWave <= 0.0f)
		return (0.0f);

	double half = _cyclesPerWave * 0.5;
	if (_counterInCyclesPerWave < half)
		return (_counterInCyclesPerWave / half);
	return (1.0 - ((_counterInCyclesPerWave - half) / half));
}

// ---
double MCHEmul::SawSmoothSoundWave::data () const
{
	if (!_active)
		return (0.0f); // No active...

	if (_cyclesPerWave <= 0.0f)
		return (0.0f);

	return (_counterInCyclesPerWave / _cyclesPerWave);
}

// ---
MCHEmul::PulseSoundWave::PulseSoundWave (unsigned int cF)
	: MCHEmul::SoundWave (Type::_PULSE, cF),
	  _pulseUpPercentage (0)
{ 
	setClassName ("SoundPulseWave");
						  
	calculateWaveSamplingData (); 
}

// ---
void MCHEmul::PulseSoundWave::initialize ()
{ 
	SoundWave::initialize (); 

	calculateWaveSamplingData ();
}

// ---
double MCHEmul::PulseSoundWave::data () const
{
	if (!_active)
		return (0.0f);

	if (_cyclesPerWave <= 0.0f)
		return (0.0f);

	return ((clockValue () < _pulseUpPercentage) ? 1.0f : 0.0f);
}

// --
MCHEmul::InfoStructure MCHEmul::PulseSoundWave::getInfoStructure () const
{
	MCHEmul::InfoStructure result = MCHEmul::SoundWave::getInfoStructure ();

	result.add ("PULSEUP", _pulseUpPercentage);

	return (result);
}

// ---
MCHEmul::SquareSoundWave::SquareSoundWave (unsigned int cF)
	: MCHEmul::PulseSoundWave (cF)
{ 
	_type = Type::_SQUARE; // Set the type to square wave

	setClassName ("SoundSquareWave"); 

	setPulseUpPercentage (0.5f); // 50% of the time up, 50% down
};

// ---
void MCHEmul::SquareSoundWave::initialize ()
{
	PulseSoundWave::initialize ();

	// To avoid to eave it to 0.0f (as per default) 
	// and to be a square wave, we set it to 50% of the time up.
	setPulseUpPercentage (0.5f);
}

// ---
void MCHEmul::NoiseSoundWave::initialize ()
{
	MCHEmul::SoundWave::initialize ();

	_lfsr = _initialLFSR;

	_currentOutput = 0.0f;

	stepLFSR (); // Generate the first noise value immediately....
}

// ---
void MCHEmul::NoiseSoundWave::clock (unsigned int nC)
{
	if (_cyclesPerWave <= 0.0f)
		return;

	double remainingCycles = (double) nC;
	while (remainingCycles > 0.0f)
	{
		double cyclesToNextStep =
			_cyclesPerWave - _counterInCyclesPerWave;
		if (remainingCycles < cyclesToNextStep)
		{
			_counterInCyclesPerWave += remainingCycles;
			remainingCycles = 0.0f;
		}
		else
		{
			remainingCycles -= cyclesToNextStep;
			_counterInCyclesPerWave = 0.0f;
			_clockRestarted = true;

			stepLFSR ();
		}
	}
}

// ---
void MCHEmul::NoiseSoundWave::stepLFSR ()
{
	// 16-bit Galois LFSR.
	// Polynomial: x^16 + x^14 + x^13 + x^11 + 1
	// Feedback mask: 0xB400.
	unsigned int lsb = _lfsr & 0x0001u;
	_lfsr >>= 1;
	if (lsb != 0)
		_lfsr ^= 0xB400u;
	_lfsr &= 0xffffu;
	// Avoid lock-up in zero state.
	if (_lfsr == 0)
		_lfsr = _initialLFSR;

	// Convert some LFSR bits to a normalized value between 0 and 1.
	// Using 8 bits gives a less binary, more noise-like output than using only one bit.
	_currentOutput = (double) (_lfsr & 0x00ffu) / 255.0f;
}

// ---
double MCHEmul::NoiseSoundWave::data () const
{
	if (!_active)
		return (0.0f);

	if (_cyclesPerWave <= 0.0f)
		return (0.0f);

	return (_currentOutput);
}
