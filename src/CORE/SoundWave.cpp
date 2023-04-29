#include <CORE/SoundWave.hpp>

// ---
void MCHEmul::SoundWave::initialize ()
{ 
	_frequency = 0;
	_active = false; // By default...
	_attack = _release = _decay = 0;
	_sustainVolumen = _maximumVolumen = 0; 
							  
	calculateWaveSamplingData (); 
}

// ---
void MCHEmul::SoundWave::calculateWaveSamplingData ()
{
	// The counter for the waves...
	_cyclesPerWave= (_frequency != 0) 
		? (int) ((double) _chipFrequency / (double) _frequency) : 0;
	_counterInCyclesPerWave = 0;
}

// ---
double MCHEmul::SawSmoothSoundWave::data () const
{
	// Linear interpolation from 0 to 1...
	auto valueFor = [](double minx, double miny, double maxx, double maxy, double x) -> double
		{ return (((maxx == minx) || maxy == 0) ? 0 : (miny + ((x - minx) * (maxy - miny) / (maxx - minx))) / maxy); };

	return (valueFor (0, 0, _cyclesPerWave, _sustainVolumen, _counterInCyclesPerWave));
}

// ---
double MCHEmul::TriangleSoundWave::data () const
{
	// Linear interpolation from 0 to 1...
	auto valueFor = [](double minx, double miny, double maxx, double maxy, double x) -> double
		{ return (((maxx == minx) || maxy == 0) ? 0 : (miny + ((x - minx) * (maxy - miny) / (maxx - minx))) / maxy); };

	return ((_counterInCyclesPerWave > (_cyclesPerWave >> 1))
		? valueFor (0.0f, 0.0f, (double) (_cyclesPerWave >> 1), (double) _sustainVolumen, (double) _counterInCyclesPerWave)
		: valueFor (0.0f, (double) _sustainVolumen, (double) (_cyclesPerWave >> 1), 
			0.0f, (double) (_counterInCyclesPerWave - (_cyclesPerWave >> 1))));
}

// ---
double MCHEmul::PulseSoundWave::data () const
{
	// TODO
	
	return (0.0f);
}

// ---
double MCHEmul::NoiseSoundWave::data () const
{
	// Randon number from 0 to 1 (three decimals)
	return ((double) (std::rand () % 1000) / 1000.0f);
}
