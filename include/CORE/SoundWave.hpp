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
#include <CORE/InfoClass.hpp>

namespace MCHEmul
{
	/** To replicate a wave. 
		The waves can only be used from a SoundVoice. */
	class SoundWave : public InfoClass
	{
		public:
		// The four different types of waves supported...
		enum class Type
		{
			_TRIANGLE = 0,
			_SAWTOOTH = 1,
			_PULSE = 2,
			_NOISE = 3
		};

		/**
		  *	Constructor.
		  *	@param t	Type of the wave (@see enum TYpe)
		  *	@param cF	Chip frequency, in cycles per second.
		  */
		SoundWave (Type t, unsigned int cF)
			: InfoClass ("SoundWave"), 
			  _type (t),
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

		/** Just to initialize the internal counters. \n
			All the internal counters defining the state are moved back to 0. \n
			It can be overloaded for futher purposes. \n
			Initially it is also invoked from the method calculateWaveSamplingData (). */
		virtual void initializeInternalCounters ()
						{ _counterInCyclesPerWave = 0; }

		virtual InfoStructure getInfoStructure () const override;

		/** This method has to be invoked in every cpu cycle,
			receiving as parameter the number of cycles happened from the last invocation. \n
			It can be overloaded to cover specific needs for specific sound waves. */
		virtual void clock (unsigned int nC = 1)
						{ if (((_counterInCyclesPerWave += nC) >= _cyclesPerWave) &&
							  ((_counterInCyclesPerWave -= _cyclesPerWave) >= _cyclesPerWave))
								_counterInCyclesPerWave = 0; } // Just in case _cyclesPerWave is 0...
		
		/** To know the position from 0 to 1 of the internal clock.
			It can not be overloaded. */
		double clockValue () const
						{ return ((_cyclesPerWave == 0) 
							? 0.0f : (double) _counterInCyclesPerWave / (double) _cyclesPerWave); }

		/** This method should return always a value between 0 and 1. \n
			Indicating the %(1) of the maximum situation achieved. \n
			Decimal numbers are admitted (are needed). \n
			Muct be overloaded according to the type of wave. */
		virtual double data () const = 0;

		protected:
		// Implementation
		/** To calculate the internal data needed to later "draw" the different waves. \n
			It could be overloaded to include more intenal data needed
			depending on the type of wave. \n
			Any moment a key value is changed this method should be invoked. ºn
			This method invokes the next one. */
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

	/** Triangle. */
	class TriangleSoundWave final : public SoundWave
	{
		public:
		TriangleSoundWave (unsigned int cF)
			: SoundWave (Type::_TRIANGLE, cF)
						{ }

		virtual double data () const override;
	};

	/** This the one that imitates a sinusoidal wave with its harmonic the best. */
	class SawSmoothSoundWave final : public SoundWave
	{
		public:
		SawSmoothSoundWave (unsigned int cF)
			: SoundWave (Type::_SAWTOOTH, cF)
						{ }

		virtual double data () const override;
	};

	/** A pulse. \n
		In a pulse wave is important to know how much percentage of the cycle the pulse is up
		and how much of it is down. */
	class PulseSoundWave final : public SoundWave
	{
		public:
		PulseSoundWave (unsigned int cF);

		/** To manage the part of the pulse up...
			It is a number between 0 and 1% */
		double pulseUpPercentage () const
						{ return (_pulseUpPercentage); }
		void setPulseUpPercentage (double pU)
						{ _pulseUpPercentage = (pU > 1.0f ? 1.0f : pU); calculateWaveSamplingData (); }

		/* Returns true when the pulse is in the up situation. */
		bool pulseUp () const
						{ return (_pulseUp); }

		virtual void initialize () override;

		virtual void initializeInternalCounters () override;

		virtual InfoStructure getInfoStructure () const override;

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

		virtual double data () const override;
	};
}

#endif
  
// End of the file
/*@}*/
