#include <CORE/SoundFilter.hpp>
#include <CORE/LogChannel.hpp>
#include <cmath>

// ----
MCHEmul::SoundFilter::SoundFilter (unsigned int sF, MCHEmul::SoundFilter::Type t)
	: MCHEmul::InfoClass ("SoundFilter"),
	  _active (false), // Not active by default...
	  _type (t),
	  _lowCutFrequency	(1000.0f), // 1 KHz by default...
	  _highCutFrequency (1000.0f), // 1 KHz by default...
	  _centerFrequency	(1000.0f), // 1 KHz by default...
	  _q (0.70710678118),		   // 1/sqrt(2), Butterworth response by default...
	  _samplingFrequency (sF)
{
	// Nothing else to do...
}

// ---
void MCHEmul::SoundFilter::setLowPassCutFrequency (double f)
{
	_type = Type::_LOWPASS;

	const double nyquist = (double) _samplingFrequency * 0.5f;
	const double maxF = nyquist * 0.99f;
	if (_samplingFrequency == 0 || maxF <= 1.0f)
	{
		_lowCutFrequency	= 0.0f;
		_highCutFrequency	= 0.0f;
		_centerFrequency	= 0.0f;

		calculateCoefficients ();

		return;
	}

	if (f < 1.0f) f = 1.0f;
	if (f > maxF) f = maxF;

	_lowCutFrequency	= 0.0f;
	_highCutFrequency	= f;
	_centerFrequency	= f;

	calculateCoefficients ();
}

// ---
void MCHEmul::SoundFilter::setHighPassCutFrequency (double f)
{
	_type = Type::_HIGHPASS;

	double nyquist = (double) _samplingFrequency * 0.5f;
	if (f < 0.0) f = 0.0f;
	if (f > nyquist * 0.99f) f = nyquist * 0.99f;

	_lowCutFrequency	= f;
	_highCutFrequency	= nyquist;
	_centerFrequency	= f;

	calculateCoefficients ();
} 

// ---
void MCHEmul::SoundFilter::setBandPassFrequency (double centerF)
{
	_type = Type::_BANDPASS;

	const double nyquist = (double) _samplingFrequency * 0.5;
	const double maxF = nyquist * 0.99;

	if (_samplingFrequency == 0 || maxF <= 1.0)
	{
		_lowCutFrequency    = 0.0;
		_highCutFrequency   = 0.0;
		_centerFrequency    = 0.0;

		calculateCoefficients ();

		return;
	}

	if (centerF < 1.0) centerF = 1.0;
	if (centerF > maxF) centerF = maxF;
	_centerFrequency = centerF;

	// In this mode, the real band width is controlled by Q.
	// These values are approximated only for information/debugging.
	const double bandwidth = (_q > 0.0)
		? (_centerFrequency / _q) : 0.0;
	_lowCutFrequency	= _centerFrequency - (bandwidth * 0.5);
	_highCutFrequency	= _centerFrequency + (bandwidth * 0.5);
	if (_lowCutFrequency < 0.0) _lowCutFrequency = 0.0;
	if (_highCutFrequency > nyquist) _highCutFrequency = nyquist;

	calculateCoefficients ();
}

// ---
void MCHEmul::SoundFilter::setBandPassFrequencies (double lowF, double highF)
{
	_type = Type::_BANDPASS;

	const double nyquist = (double) _samplingFrequency * 0.5f;
	const double maxF = nyquist * 0.99f;
	if (_samplingFrequency == 0 || maxF <= 1.0f)
	{
		_lowCutFrequency	= 0.0f;
		_highCutFrequency	= 0.0f;
		_centerFrequency	= 0.0f;
		_q					= 0.70710678118f;

		calculateCoefficients ();

		return;
	}

	if (lowF < 1.0f) lowF = 1.0f;
	if (lowF > maxF - 1.0f) lowF = maxF - 1.0f;
	if (highF > maxF) highF = maxF;
	if (highF <= lowF) highF = lowF + 1.0f;
	if (highF > maxF) { highF = maxF; lowF = highF - 1.0f; }

	_lowCutFrequency	= lowF;
	_highCutFrequency	= highF;
	_centerFrequency	= std::sqrt (_lowCutFrequency * _highCutFrequency);
	const double bandwidth = _highCutFrequency - _lowCutFrequency;
	_q = (_centerFrequency > 0.0f && bandwidth > 0.0f)
		? (_centerFrequency / bandwidth)
		: 0.70710678118f;

	calculateCoefficients ();
}

// ---
void MCHEmul::SoundFilter::setNotchFrequency (double f)
{
	_type = Type::_NOTCH;

	const double nyquist = (double) _samplingFrequency * 0.5;
	const double maxF = nyquist * 0.99;

	if (_samplingFrequency == 0 || maxF <= 1.0)
	{
		_lowCutFrequency    = 0.0;
		_highCutFrequency   = 0.0;
		_centerFrequency    = 0.0;

		calculateCoefficients ();

		return;
	}

	if (f < 1.0) f = 1.0;
	if (f > maxF) f = maxF;

	// These two values are informational in notch mode.
	// The effective width is controlled by Q.
	_lowCutFrequency	= f;
	_highCutFrequency	= f;
	_centerFrequency	= f;

	calculateCoefficients ();
}

// ---
MCHEmul::InfoStructure MCHEmul::SoundFilter::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	result.add ("TYPE",					(int) _type);
	result.add ("LOWCUTFREQUENCY",		_lowCutFrequency);
	result.add ("HIGHCUTFREQUENCY",		_highCutFrequency);
	result.add ("CENTERFREQUENCY",		_centerFrequency); // The one that really matters to filter!
	result.add ("Q",					_q);
	result.add ("SAMPLINGFREQUENCY",	_samplingFrequency);

	return (result);
}

// ---
MCHEmul::BiquadSoundFilter::BiquadSoundFilter (unsigned int sF, 
		MCHEmul::SoundFilter::Type t)
	: MCHEmul::SoundFilter (sF, t),
	  _b0 (0.0f), _b1 (0.0f), _b2 (0.0f),
	  _a1 (0.0f), _a2 (0.0f),
	  _x1 (0.0f), _x2 (0.0f), _y1 (0.0f), _y2 (0.0f)
{
	setClassName ("BiquadSoundFilter");

	calculateCoefficients ();
}

// ---
void MCHEmul::BiquadSoundFilter::initialize ()
{
	MCHEmul::SoundFilter::initialize ();

	_x1 = 0.0f; _x2 = 0.0f; _y1 = 0.0f; _y2 = 0.0f;

	calculateCoefficients ();
}

// ---
double MCHEmul::BiquadSoundFilter::process (double x)
{
	if (!_active)
		return (x);

	double y =
		(_b0 * x) +
		(_b1 * _x1) +
		(_b2 * _x2) -
		(_a1 * _y1) -
		(_a2 * _y2);

	_x2 = _x1;
	_x1 = x;

	_y2 = _y1;
	_y1 = y;

	return (y);
}

// ---
void MCHEmul::BiquadSoundFilter::calculateCoefficients ()
{
	if (_samplingFrequency == 0 || 
		_centerFrequency <= 0.0f || _q <= 0.0f)
	{
		_b0 = 1.0f; _b1 = 0.0f; _b2 = 0.0f;
		_a1 = 0.0f; _a2 = 0.0f;

		return;
	}

	const double nyquist = (double) _samplingFrequency * 0.5f;
	double f = _centerFrequency;
	if (f >= nyquist)
		f = nyquist * 0.99f;

	if (f <= 0.0f)
	{
		_b0 = 1.0f; _b1 = 0.0f; _b2 = 0.0f;
		_a1 = 0.0f; _a2 = 0.0f;

		return;
	}

	const double w0 = 2.0f * 3.14159265358979323846f * f /
		(double) _samplingFrequency;
	const double c = std::cos (w0);
	const double s = std::sin (w0);
	const double alpha = s / (2.0f * _q);

	switch (_type)
	{
		case MCHEmul::SoundFilter::Type::_LOWPASS:
			{
				double b0	= (1.0f - c) / 2.0f;
				double b1	= 1.0f - c;
				double b2	= (1.0f - c) / 2.0f;
				double a0	= 1.0f + alpha;
				double a1	= -2.0f * c;
				double a2	= 1.0f - alpha;

				_b0			= b0 / a0;
				_b1			= b1 / a0;
				_b2			= b2 / a0;
				_a1			= a1 / a0;
				_a2			= a2 / a0;
			}

			break;

		case MCHEmul::SoundFilter::Type::_HIGHPASS:
			{
				double b0	= (1.0f + c) / 2.0f;
				double b1	= -(1.0f + c);
				double b2	= (1.0f + c) / 2.0f;
				double a0	= 1.0f + alpha;
				double a1	= -2.0f * c;
				double a2	= 1.0f - alpha;

				_b0			= b0 / a0;
				_b1			= b1 / a0;
				_b2			= b2 / a0;
				_a1			= a1 / a0;
				_a2			= a2 / a0;
			}

			break;

		case MCHEmul::SoundFilter::Type::_BANDPASS:
			{
				double b0	= alpha;
				double b1	= 0.0f;
				double b2	= -alpha;
				double a0	= 1.0f + alpha;
				double a1	= -2.0f * c;
				double a2	= 1.0f - alpha;

				_b0 =		b0 / a0;
				_b1 =		b1 / a0;
				_b2 =		b2 / a0;
				_a1 =		a1 / a0;
				_a2 =		a2 / a0;
			}

			break;

		case MCHEmul::SoundFilter::Type::_NOTCH:
			{
				double b0   = 1.0;
				double b1   = -2.0 * c;
				double b2   = 1.0;
				double a0   = 1.0 + alpha;
				double a1   = -2.0 * c;
				double a2   = 1.0 - alpha;

				_b0         = b0 / a0;
				_b1         = b1 / a0;
				_b2         = b2 / a0;
				_a1         = a1 / a0;
				_a2         = a2 / a0;
			}

			break;

		// It shouldn't be here, just in case...
		default:
			{
				_LOG ("BiquadSoundFilter::calculateCoefficients: Unknown filter type: " + 
					std::to_string ((int) _type) + "\n");
			}

			break; 
	}
}

// ---
MCHEmul::InfoStructure MCHEmul::BiquadSoundFilter::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::SoundFilter::getInfoStructure ());

	result.add ("B0", _b0);
	result.add ("B1", _b1);
	result.add ("B2", _b2);
	result.add ("A1", _a1);
	result.add ("A2", _a2);

	return (result);
}
