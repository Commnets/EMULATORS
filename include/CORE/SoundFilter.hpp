/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: SoundFilter.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 13/06/2026 \n
 *	Description: Represents a filter. \n
 *				 It can have several representations, but the most common is the biquad filter. \n
 *	Versions: 1.0 Initial
 */

#ifndef __CORE_SOUNDFILTER__
#define __CORE_SOUNDFILTER__

#include <CORE/InfoClass.hpp>
#include <CORE/InfoStructure.hpp>

namespace MCHEmul
{
	/** Represents a digital sound filter applied to a stream of sound samples.
	 *
	 *  A SoundFilter receives consecutive samples in the time domain and returns
	 *  filtered samples, also in the time domain. The filter does not perform an
	 *  explicit Fourier transform. Instead, concrete implementations usually
	 *  implement a digital transfer function by means of a recurrence equation
	 *  with internal state.
	 *
	 *  The filter can be configured as:
	 *  - Low-pass: frequencies below the configured upper cut frequency are kept.
	 *  - High-pass: frequencies above the configured lower cut frequency are kept.
	 *  - Band-pass: frequencies between the configured lower and upper cut
	 *    frequencies are kept.
	 *
	 *  The sampling frequency is the frequency at which process() is called, not
	 *  necessarily the emulated chip frequency. When the filter is applied in a
	 *  SoundLibWrapper just before producing PCM data, this value must normally be
	 *  the audio sampling frequency.
	 *
	 *  The class stores the generic filter configuration. The actual filtering
	 *  algorithm and coefficient calculation are implemented by derived classes.
	 */
	class SoundFilter : public InfoClass
	{
		public:
		/** Three types of filter are now supported. */
		enum class Type
		{
			_LOWPASS = 0,
			_HIGHPASS = 1,
			_BANDPASS = 2,
			_NOTCH = 3,
		};

		SoundFilter (unsigned int sF, Type t = Type::_HIGHPASS);

		/** Just in case. \n
			Initially nothing to be destroyed is needed. */
		virtual ~SoundFilter () = default;

		virtual void initialize ()
							{ } // By default it does nothing...

		/** Processes a bipolar audio sample. The filter can produce transients
			outside [-1.0, 1.0]; clipping belongs at the final voice or mixer boundary. */
		virtual double process (double x) = 0;

		/** To activate or desactivate the filter. */
		bool active () const
							{ return (_active); }
		void setActive (bool a)
							{ if (_active != a && a) { initialize (); } _active = a; }

		// Managing the type of the filter...
		Type type () const
							{ return (_type); }
		void setType (Type t)
							{ _type = t; calculateCoefficients (); }

		// Managing the filter parameters...
		// When using this method, the type is fixed!
		void setLowPassCutFrequency (double f);
		void setHighPassCutFrequency (double f);
		void setBandPassFrequency (double centerF);
		void setBandPassFrequency (double centerF, double q)
							{ setQ (q); setBandPassFrequency (centerF); }
		void setBandPassFrequencies (double lowF, double highF);
		void setNotchFrequency (double f);
		void setNotchFrequency (double f, double q)
							{ setQ (q); setNotchFrequency (f); }

		// General parameters
		// Assigning them the type of filter doesn't change, 
		// but the internal way of working changes...
		void setQ (double q)
							{ _q = (q > 0.001f) ? q : 0.001f; calculateCoefficients (); }
		void setSamplingFrequency (unsigned int f)
							{ _samplingFrequency = f; calculateCoefficients (); }

		// To get the value of the filter...
		double lowPassCutFrequency () const
							{ return (_highCutFrequency); }
		double highPassCutFrequency () const
							{ return (_lowCutFrequency); }
		double bandPassLowFrequency () const
							{ return (_lowCutFrequency); }
		double bandPassHighFrequency () const
							{ return (_highCutFrequency); }
		double q () const 
							{ return (_q); }
		unsigned int samplingFrequency () const 
							{ return (_samplingFrequency); }

		virtual InfoStructure getInfoStructure () const override;

		protected:
		virtual void calculateCoefficients () = 0;

		protected:
		/** The filter is or no active. */
		bool _active;
		/** The type. */
		Type _type;
		/** Internal working parameters.
			The one finally used will depend on the type of filer. */
		double _lowCutFrequency;
		double _highCutFrequency;
		double _centerFrequency;
		/** General parameters. */
		double _q;
		unsigned int _samplingFrequency;
	};

	// To manage a set of filter in a simple way...
	using SoundFilters = std::vector <SoundFilter*>;

	/** Second-order IIR digital filter based on a biquad structure.
	 *
	 *  This class implements a sample-by-sample filter using the standard biquad
	 *  difference equation:
	 *
	 *      y[n] = b0*x[n] + b1*x[n-1] + b2*x[n-2]
	 *             - a1*y[n-1] - a2*y[n-2]
	 *
	 *  The filter keeps the previous input and output samples as internal state.
	 *  Therefore, every call to process() advances the state of the filter and
	 *  must be performed in strict sample order.
	 *
	 *  The supported modes are low-pass, high-pass and band-pass. For low-pass and
	 *  high-pass filters, the configured cut frequency is used directly as the
	 *  characteristic biquad frequency. For band-pass filters, the lower and upper
	 *  cut frequencies are converted internally into a center frequency and a Q
	 *  factor.
	 *
	 *  This implementation is intended for real-time emulation use, where applying
	 *  an FFT/DFT per block would add unnecessary latency and computational cost.
	 *  It is suitable for filtering the output of a SoundVoice or the final mixed
	 *  output of a sound wrapper.
	 */
	class BiquadSoundFilter final : public SoundFilter
	{
		public:
		BiquadSoundFilter (unsigned int sF, Type t);

		virtual void initialize () override;

		virtual double process (double x) override;

		virtual InfoStructure getInfoStructure () const override;

		protected:
		virtual void calculateCoefficients () override;

		private:
		// Implementation
		/** The coefficients of the filter. */
		double _b0, _b1, _b2;
		double _a1, _a2;
		/** The last two values introduced in the filter
			and the last two generated. */
		double _x1, _x2, _y1, _y2;
	};
}

#endif
  
// End of the file
/*@}*/
