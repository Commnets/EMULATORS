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
	/** To replicate a wave. */
	class SoundWave
	{
		public:
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
			  _attack (0), _decay (0), _release (0),
			  _sustainVolumen (0),
			  _maximumVolumen (0),
			  _state (State::_ATTACK)
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

		void setStart (bool s)
						{ _state = s ? State::_ATTACK : State::_RELEASE; }

		unsigned short frequency () const
						{ return (_frequency); }
		void setFrequency (unsigned short f)
						{ _frequency = f; calculateWaveSamplingData (); }

		/** The values are returned in milliseconds. */
		unsigned short attack () const
						{ return (_attack); }
		/** But they are set based on the values of the register. */
		void setAttack (unsigned short a)
						{ _attack = a; calculateWaveSamplingData (); }
		unsigned short decay () const
						{ return (_decay); }
		void setDecay (unsigned short d)
						{ _decay = d; calculateWaveSamplingData (); }
		unsigned short release () const
						{ return (_release); }
		void setRelease (unsigned short r)
						{ _release = r; calculateWaveSamplingData (); }

		unsigned char sustainVolumen () const // From 0 to 0x0f...
						{ return (_sustainVolumen); }
		void setSustainVolumen (unsigned char s)
						{ _sustainVolumen = s; calculateWaveSamplingData (); }

		unsigned char maximumVolumen () const // From 0 to 0x0f
						{ return (_maximumVolumen); }
		void setMaximumVolumen (unsigned char v)
						{ _maximumVolumen = v; }

		virtual void initialize ();

		void clock (unsigned int nC)
						{ if (((_counterInCyclesPerWave += nC) >= _cyclesPerWave) &&
							  ((_counterInCyclesPerWave -= _cyclesPerWave) >= _cyclesPerWave))
								_counterInCyclesPerWave = 0; } // Just in case _cyclesPerWave is 0...

		/** When a data is requested a value from 0% to 100% is returned. 
			The value returned has to be the equivalent to the _counterInCyclesPerWave location. */
		virtual double data () const = 0;

		protected:
		/** To calculate the internal data needed to 
			later "draw" the different waves. \n
			It could be overloaded later to include more intenal data needed
			depending on the type of wave. \n
			Any moment a key value is changed this method should be invoked. */
		virtual void calculateWaveSamplingData ();

		protected:
		/** Tipo de forma de onda. */
		Type _type;
		/** Chip frequency. */
		unsigned int _chipFrequency;
		/** When the wave is active within a voice. */
		bool _active;
		/** The frequency of the wave. In milliseconds. */
		unsigned short _frequency;
		/** The variables used for the envelop. In milliseconds. */
		unsigned short _attack, _decay, _release;
		/** The volumen for sustain. From 0 to 10. */
		unsigned char _sustainVolumen;
		/** The maximum volumen. From 0 to 10. */
		unsigned char _maximumVolumen;

		// Implementation
		/** The status in which the wave is in. */
		enum class State
		{
			_ATTACK = 0,
			_DECAY = 1,
			_SUSTAIN = 2,
			_RELEASE = 3
		};

		/** The state in which the full wave is. */
		State _state;

		/** The number of computer cycles needed to complete a wave cycle. */
		unsigned int _cyclesPerWave;
		/** A counter for the current cycle within the _cyclesPerWave. */
		unsigned int _counterInCyclesPerWave;
	};

	/** To sumplify managing a list of sound waves. */
	using SoundWaves = std::vector <SoundWave*>;

	/** The most typical sound wave used in the C64. */
	class SawSmoothSoundWave final : public SoundWave
	{
		public:
		SawSmoothSoundWave (unsigned int cF)
			: SoundWave (Type::_SAWTOOTH, cF)
						{ }

		virtual double data () const override;
	};

	class TriangleSoundWave final : public SoundWave
	{
		public:
		TriangleSoundWave (unsigned int cF)
			: SoundWave (Type::_SAWTOOTH, cF)
						{ }

		virtual double data () const override;
	};

	class PulseSoundWave final : public SoundWave
	{
		public:
		PulseSoundWave (unsigned int cF)
			: SoundWave (Type::_PULSE, cF),
			  _dutyCycle (0),
			  _pulseUp (false)
						{ }

		unsigned short dutyCycle () const
						{ return (_dutyCycle); }
		void setDutyCycle (unsigned short dC)
						{ _dutyCycle = dC; }

		virtual void initialize () override 
						{ SoundWave::initialize (); 
						  _pulseUp = false; }

		virtual double data () const override;

		private:
		unsigned short _dutyCycle;

		// Implementation
		/** = false, pulse down. = true, pulse up. */
		bool _pulseUp;
	};

	class NoiseSoundWave final : public SoundWave
	{
		public:
		NoiseSoundWave (unsigned int cF)
			: SoundWave (Type::_NOISE, cF)
						{ }

		virtual double data () const override;
	};
}

#endif
  
// End of the file
/*@}*/
