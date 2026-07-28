/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: SoundVoice.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 13/01/2023 \n
 *	Description: Any sound produced is generated using voices.
				 The voices group different types of sound waves.
 *	Versions: 1.0 Initial
 */

#ifndef __CORE_SOUNDENVELOPE__
#define __CORE_SOUNDENVELOPE__

#include <CORE/InfoClass.hpp>

namespace MCHEmul
{
	/** Represents an envelop~for the sound. \n
		There might be many different possibilities or even none. \n
		These is something that the every chip sound has to decide */
	class SoundEnvelope : public InfoClass
	{
		public:
		SoundEnvelope (unsigned int cF)
			: InfoClass ("SoundEnvelope"),
			  _active (false),
			  _chipFrequency (cF)
							{ }

		virtual ~SoundEnvelope () = default;

		/** To activate or desactivate the envelop. */
		bool active () const
							{ return (_active); }
		void setActive (bool a)
							{ if ((_active != a) && (_active = a))
								initializeInternalCounters (); }

		// To manage the way the Envelope works...
		/** To start/stop the envelope from the state it is. \n
			Usually the envelope is implemented internally like a FSM. \n
			This method is used to set up that state and the internal variables that could be needed as well. */
		virtual void setStart (bool s) = 0;
		/** To initialize the envelope. */
		virtual void initialize () = 0;
		/** The envelope usually manages many internal counters
			to control the point where it is in. This method sets up those to 0 (if any). */
		virtual void initializeInternalCounters () = 0;
		/** To change the status of the envelop accordingly to the number of clocks. */
		virtual void clock (unsigned int nC = 1) = 0;
		/** The most important method of the envelop: \n
			To produce the envelope gain. This is not an audio sample. \n
			The value, according with the situation, has to be between 0.0 and 1.0. */
		virtual double envelopeData () const = 0;

		protected:
		bool _active;
		unsigned int _chipFrequency;
	};

	/** Represents the traditional envelop = ADSR. \n
		The ADSR is about a FSM that passes through different states. 
		The time the FSM stands in each state will depend on the configuration of the different
		parameters of the ADSR. \n
		Every time those parameters are changed, the sampling data has to be recalculated
		(and the internal counters reseted. */
	class SoundADSREnvelope final : public SoundEnvelope
	{
		public:
		SoundADSREnvelope (unsigned int cF,
			unsigned short a = 0, unsigned short d = 0, unsigned short r = 0);

		// ADSR values...
		/** The values are given and returned in milliseconds. */
		unsigned short attack () const
							{ return (_attack); }
		inline void setAttack (unsigned short a);
		unsigned short decay () const
							{ return (_decay); }
		inline void setDecay (unsigned short d);
		unsigned short release () const
							{ return (_release); }
		inline void setRelease (unsigned short r);
		inline void setADR (unsigned short a, unsigned short d, unsigned short r);

		/** The sustain volumen is a number between 0 and 1 indicating 
			the %(1) over a "maximum value". */
		double sustainVolumen () const
						{ return (_sustainVolumen); }
		inline void setSustainVolumen (double s);

		/** In ADSR when starts the FSM is moved into the ATTACK state,
			and when stops the FSM is moved into the RELEASE state unless it was no in IDLE previouly in which does nothing. */
		virtual void setStart (bool s) override;
		virtual void initialize () override;
		virtual void initializeInternalCounters () override;
		virtual void clock (unsigned int nC = 1) override;
		virtual double envelopeData () const override;

		virtual InfoStructure getInfoStructure () const override;

		private:
		/** Just to advance the internal state.
			It is really an implemention method. */
		void advanceState ();

		/** To calculate the internal data needed to later "draw" the voice. \n
			It could be overloaded to include more intenal data needed
			depending on the type of voice. \n
			Any moment a key value is changed this method should be invoked. */
		void calculateSamplingData ();

		private:
		/** The variables used for the envelop. In milliseconds. */
		unsigned short _attack, _decay, _release;
		/** The volumen for sustain. From 0 to 1. */
		double _sustainVolumen;
		/** The volumen from which the release phase starts. From 0 to 1. */
		double _releaseInitialVolumen;

		// Implementation
		/** The status in which the wave is in. */
		enum class State
		{
			_ATTACK = 0,
			_DECAY = 1,
			_SUSTAIN = 2,
			_RELEASE = 3,
			_IDLE = 4
		};

		/** The state in which the full wave is. */
		State _state;

		/** The counters used to control the states _attack, _decay, 
			_sustain (not needed), and _release. */
		struct StateCounters
		{
			StateCounters ()
				: _cyclesPerState (0), _counterCyclesPerState (0),
				  _limit (false)
							{ }

			void initialize ()
							{ _counterCyclesPerState = 0;
							  _limit = false; }

			unsigned int _cyclesPerState;
			unsigned int _counterCyclesPerState;
			bool _limit;
		}; 

		mutable std::vector <StateCounters> _stateCounters;
	};

	// ---
	inline void SoundADSREnvelope::setAttack (unsigned short a)
	{ 
		if (_attack == a)
			return;

		_attack = a; 
		
		calculateSamplingData ();
	}

	// ---
	inline void SoundADSREnvelope::setDecay (unsigned short d)
	{ 
		if (_decay == d)
			return;

		_decay = d; 
		
		calculateSamplingData ();
	}

	// ---
	inline void SoundADSREnvelope::setRelease (unsigned short r)
	{ 
		if (_release == r)
			return;

		_release = r; 
		
		calculateSamplingData ();
	}

	// ---
	inline void SoundADSREnvelope::setADR (unsigned short a, unsigned short d, unsigned short r)
	{
		if (_attack == a && _decay == d && _release == r)
			return;

		_attack = a; _decay = d; _release = r; 
		
		calculateSamplingData ();
	}

	// ---
	inline void SoundADSREnvelope::setSustainVolumen (double s)
	{ 
		if (s > 1.0f) s = 1.0f;
		else if (s < 0.0f) s = 0.0f;
		if (_sustainVolumen == s)
			return;

		_sustainVolumen = s;
	}
}

#endif
  
// End of the file
/*@}*/
