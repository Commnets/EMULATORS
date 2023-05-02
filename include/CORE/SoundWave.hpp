/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: SoundWave.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 13/01/2023 \n
 *	Description: To define different types of waves than can ve used later to create sound.
 *	Versions: 1.0 Initial
 */

#ifndef __CORE_SOUNDWAVE__
#define __CORE_SOUNDWAVE__

#include <CORE/global.hpp>

namespace MCHEmul
{
	class SoundVoice;

	/** To replicate a wave. 
		The waves can only be used from a SoundVoice. */
	class SoundWave
	{
		public:
		friend SoundVoice;

		// The four different types of waves supported...
		enum class Type
		{
			_SAWTOOTH = 0,
			_TRIANGLE = 1,
			_PULSE = 2,
			_NOISE = 3
		};

		/**
		  *	Constructor.
		  *	@param t	Type of the wave (@see enum TYpe)
		  *	@param cF	Chip frequency, in cycles per second.
		  */
		SoundWave (Type t, unsigned int cF)
			: _type (t),
			  _chipFrequency (cF),
			  _active (false), // to indicate whether the wave s or not active...
			  _frequency (0),
			  _cyclesPerWave (0), _counterInCyclesPerWave (0)
						{ calculateWaveSamplingData (); }

		/** Just in case. The default behaviour is doing nothing. */
		virtual ~SoundWave ()
						{ }

		/** These data is just for consulting. */
		Type type () const
						{ return (_type); }
		unsigned int chipFrequency () const
						{ return (_chipFrequency); }

		bool active () const
						{ return (_active); }
		void setActive (bool a)
						{ _active = a; }

		/** The frequency. */
		unsigned short frequency () const
						{ return (_frequency); }
		void setFrequency (unsigned short f)
						{ _frequency = f; calculateWaveSamplingData (); }

		virtual void initialize ();

		protected:
		// Tese two method can only be invoked from a Voice.
		/** This method has to be invoked in every cpu cycle,
			receiving as parameter the number of cycles happened from the last invocation. \n
			It can be overloaded to cover specific needs for specific sound waves. \n
			The method is invoked from SoundVoice. */
		virtual void clock (unsigned int nC = 1)
						{ if (((_counterInCyclesPerWave += nC) >= _cyclesPerWave) &&
							  ((_counterInCyclesPerWave -= _cyclesPerWave) >= _cyclesPerWave))
								_counterInCyclesPerWave = 0; } // Just in case _cyclesPerWave is 0...

		/** This method should return a value between 0 and 1. \n
			Indicating the %(1) of the maximum situation achieved. \n
			Decimal numbers are admitted (are needed). */
		virtual double data () const = 0;

		// Implementation
		/** To calculate the internal data needed to later "draw" the different waves. \n
			It could be overloaded to include more intenal data needed
			depending on the type of wave. \n
			Any moment a key value is changed this method should be invoked. */
		virtual void calculateWaveSamplingData ();

		protected:
		/** Tipo de forma de onda. */
		Type _type;
		/** Chip frequency in cycles per second. */
		unsigned int _chipFrequency;
		/** When the wave is active within a voice. */
		bool _active;
		/** The frequency of the wave. In waves per second. */
		unsigned short _frequency;

		/** The number of computer cycles needed to complete a wave cycle. */
		unsigned int _cyclesPerWave;
		/** A counter for the current cycle within the _cyclesPerWave. */
		unsigned int _counterInCyclesPerWave;
	};

	/** To sumplify managing a list of sound waves. */
	using SoundWaves = std::vector <SoundWave*>;

	/** This the one that imitates a sinusoidal wave with its harmonic the best. */
	class SawSmoothSoundWave final : public SoundWave
	{
		public:
		SawSmoothSoundWave (unsigned int cF)
			: SoundWave (Type::_SAWTOOTH, cF)
						{ }

		private:
		virtual double data () const override;
	};

	/** Triangle. */
	class TriangleSoundWave final : public SoundWave
	{
		public:
		TriangleSoundWave (unsigned int cF)
			: SoundWave (Type::_SAWTOOTH, cF)
						{ }

		private:
		virtual double data () const override;
	};

	/** A pulse. \n
		In a pulse wave is important to know how much percentage of the cycle the pulse is up
		and how much of it is down. */
	class PulseSoundWave final : public SoundWave
	{
		public:
		PulseSoundWave (unsigned int cF)
			: SoundWave (Type::_PULSE, cF),
			  _pulseUpPercentage (0),
			  _pulseUp (false),
			  _cyclesPulseUp (0), _counterCyclesPulseUp (0),
			  _cyclesPulseDown (0), _counterCyclesPulseDown (0)
						{ calculateWaveSamplingData (); }

		/** To manage the part of the pulse up...
			It is a number between 0 and 1% */
		double pulseUpPercentage () const
						{ return (_pulseUpPercentage); }
		void setPulseUpPercentage (double pU)
						{ _pulseUpPercentage = (pU > 1.0f ? 1.0f : pU); calculateWaveSamplingData (); }

		virtual void initialize () override;

		private:
		virtual void clock (unsigned int nC) override;

		virtual double data () const override;

		private:
		virtual void calculateWaveSamplingData () override;

		private:
		/** The percentage of the cycle that the pulse is up.
			It is a number between 0% and 1%. */
		double _pulseUpPercentage;

		// Implementation
		/** = false, pulse down. = true, pulse up. */
		bool _pulseUp;
		/** Number of cycles within _cyclesPerWave (@see paent class) that the pulse is up, and down. */
		unsigned int _cyclesPulseUp, _cyclesPulseDown;
		/** A counter for the cycles when the pulse is up and down. */
		unsigned int _counterCyclesPulseUp, _counterCyclesPulseDown;
		
	};

	/** Random. */
	class NoiseSoundWave final : public SoundWave
	{
		public:
		NoiseSoundWave (unsigned int cF)
			: SoundWave (Type::_NOISE, cF)
						{ }

		private:
		virtual double data () const override;
	};
}

#endif
  
// End of the file
/*@}*/
