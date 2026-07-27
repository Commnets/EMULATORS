/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: SoundWave.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 13/01/2023 \n
 *	Description: To define different types of waves than can ve used later to create sound.
 *	Versions: 1.0 Initial
 */

#ifndef __CORE_SOUNDWAVE__
#define __CORE_SOUNDWAVE__

#include <CORE/global.hpp>
#include <CORE/InfoClass.hpp>
#include <CORE/OBool.hpp>

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
			_SQUARE = 3, // Pulse with a 50% duty cycle
			_NOISE = 4,
			_PLAIN = 5 // Just a constant sound, no wave!
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
			  _frequency (0.0f),
			  _cyclesPerWave (0.0f), _counterInCyclesPerWave (0.0f),
			  _clockRestarted (false)
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
		double frequency () const
						{ return (_frequency); }
		void setFrequency (double f)
						{ _frequency = (f > 0.0) ? f : 0.0; calculateWaveSamplingData (); }

		virtual void initialize ();

		/** Just to initialize the internal counters. \n
			All the internal counters defining the state are moved back to 0. \n
			It can be overloaded for futher purposes. \n
			Initially it is also invoked from the method calculateWaveSamplingData (). */
		virtual void initializeInternalCounters ()
						{ _counterInCyclesPerWave = 0.0f; }
		/** This method has to be invoked in every cpu cycle,
			receiving as parameter the number of cycles happened from the last invocation. \n
			It can be overloaded to cover specific needs for specific sound waves. */
		virtual void clock (unsigned int nC = 1);
		
		/** To know the position from 0 to 1 of the internal clock.
			It can not be overloaded. */
		double clockValue () const
						{ return ((_cyclesPerWave <= 0.0f) 
							? 0.0f : ((double) _counterInCyclesPerWave / (double) _cyclesPerWave)); }

		/** To know whether the clock has pass over 0. \n
			When the value is read, it is set back to false. */
		bool clockRestarted () const
						{ return (_clockRestarted); }

		/** This method should always return a normalized audio sample between -1.0 and 1.0. \n
			Silence is represented by 0.0. \n
			Decimal numbers are admitted (are needed). \n
			Must be overloaded according to the type of wave. */
		virtual double data () const = 0;

		/**
		 *	The name of the fields are: \n
		 *	ACTIVE				= Attribute: YES when the wave is active and NO otherwise. \n
		 *	TYPE				= Attribute: Integer value of the wave type. \n
		 *	FREQUENCY			= Attribute: Frequency of the wave. \n
		 *	TYPEANDFREQUENCY	= Attribute: Summary containing the type and frequency.
		 */
		virtual InfoStructure getInfoStructure () const override;

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
		double _frequency;

		/** The number of computer cycles needed to complete a wave cycle. \n
			It is double to have the better accuracy possible! */
		double _cyclesPerWave;
		/** A counter for the current cycle within the _cyclesPerWave. \n
			It is double to have the better accuracy possible! */
		double _counterInCyclesPerWave;

		// Implementation
		mutable OBool _clockRestarted;
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

		/** From -1.0f to 1.0f and from 1.0f to -1.0f */
		virtual double data () const override;
	};

	/** This the one that imitates a sinusoidal wave with its harmonic the best. */
	class SawSmoothSoundWave final : public SoundWave
	{
		public:
		SawSmoothSoundWave (unsigned int cF)
			: SoundWave (Type::_SAWTOOTH, cF)
						{ }

		/** From -1.0f to 1.0f and start back. */
		virtual double data () const override;
	};

	/** A pulse. \n
		In a pulse wave is important to know how much percentage of the cycle the pulse is up
		and how much of it is down. */
	class PulseSoundWave : public SoundWave
	{
		public:
		PulseSoundWave (unsigned int cF);

		/** To manage the part of the pulse up...
			It is a number between 0 and 1% */
		double pulseUpPercentage () const
						{ return (_pulseUpPercentage); }
		inline void setPulseUpPercentage (double pU);

		/* Returns true when the pulse is in the up situation. */
		bool pulseUp () const
						{ return (_cyclesPerWave > 0.0)
								? (clockValue () < _pulseUpPercentage)
								: false; }

		virtual void initialize () override;

		virtual double data () const override;

		/**
		  *	The name of the fields are: \n
		  *	The ones from the SoundWave +
		  *	PULSEUP			= Attribute: Value from 0 to 1 of the _pulseUpPercentage.
		  */
		virtual InfoStructure getInfoStructure () const override;

		private:
		/** The percentage of the cycle that the pulse is up.
			It is a number between 0% and 1%. */
		double _pulseUpPercentage;
	};

	// ---
	void PulseSoundWave::setPulseUpPercentage (double pU)
	{
		if (pU < 0.0) _pulseUpPercentage = 0.0;
		else if (pU > 1.0) _pulseUpPercentage = 1.0;
		else _pulseUpPercentage = pU;

	    calculateWaveSamplingData ();
	}

	/** A pure square sound wave. \n
		This is a pulse sound wave but with a dutty cycle of 50%. */
	class SquareSoundWave final : public PulseSoundWave
	{
		public:
		SquareSoundWave (unsigned int cF);

		virtual void initialize () override;
	};

	/** Pseudo-random noise generated by a clocked LFSR.
		The internal LFSR is advanced from clock(), never from data().
		This makes the noise deterministic and synchronized with the emulated chip clock. */
	class NoiseSoundWave final : public SoundWave
	{
		public:
		NoiseSoundWave (unsigned int cF, unsigned int seed = 0xACE1u)
			: SoundWave (Type::_NOISE, cF),
			  _initialLFSR ((seed == 0) ? 0xACE1u : seed),
			  _lfsr ((seed == 0) ? 0xACE1u : seed),
			  _currentOutput (0.0f)
								{ setClassName ("SoundNoiseWave"); }

		virtual void initialize () override;

		virtual void initializeInternalCounters () override;

		virtual void clock (unsigned int nC = 1) override;

		virtual double data () const override;

		private:
		void stepLFSR ();

		private:
		/** Initial non-zero LFSR state. */
		unsigned int _initialLFSR;
		/** Current 16-bit LFSR state. Must never be zero. */
		unsigned int _lfsr;
		/** Last generated raw noise value, normalized from 0.0 to 1.0. */
		double _currentOutput;
	};

	/** This is not a wave in the pure sense, just a constant sound.
		It could be used, when the sound is regulated by software alternating on/off situations. */
	class PlainSoundWave final : public SoundWave
	{
		public:
		/** The frequency here is not really neede. */
		PlainSoundWave ()
			: SoundWave (Type::_PLAIN, 0)
						{ }

		/** Here the clock is mot needed really, do does nothing. */
		virtual void clock (unsigned int nC = 1) override
							{ /** Does nothing. */ }

		/** The data is always 1.0f when active and 0.0f when silent. */
		virtual double data () const override
							{ return (active () ? 1.0f : 0.0f); }
	};
}

#endif
  
// End of the file
/*@}*/
