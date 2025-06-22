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

#ifndef __CORE_SOUNDVOICE__
#define __CORE_SOUNDVOICE__

#include <CORE/InfoClass.hpp>
#include <CORE/SoundWave.hpp>

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
			To produce the envelop value. 
			The value, according with the situation, has to be between 0 and 1.0 */
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
		void setAttack (unsigned short a)
							{ _attack = a; calculateSamplingData (); }
		unsigned short decay () const
							{ return (_decay); }
		void setDecay (unsigned short d)
							{ _decay = d; calculateSamplingData (); }
		unsigned short release () const
							{ return (_release); }
		void setRelease (unsigned short r)
							{ _release = r; calculateSamplingData (); }
		void setADR (unsigned short a, unsigned short d, unsigned short r)
							{ _attack = a; _decay = d; _release = r; calculateSamplingData (); }

		/** The sustain volumen is a number between 0 and 1 indicating 
			the %(1) over a "maximum value". */
		double sustainVolumen () const
						{ return (_sustainVolumen); }
		void setSustainVolumen (double s)
						{ if ((_sustainVolumen = s) > 1.0f) _sustainVolumen = 1.0f; }

		/** In ADSR when starts the FSM is moved into the ATTACK state,
			and when stops the FSM is moved into the RELEASE state unless it was no in IDLE previouly in which does nothing. */
		virtual void setStart (bool s) override;
		virtual void initialize () override;
		virtual void initializeInternalCounters () override;
		virtual void clock (unsigned int nC = 1) override;
		virtual double envelopeData () const override;

		virtual InfoStructure getInfoStructure () const override;

		private:
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

	/** Represents a voice. \n
		Ot can be overloaded for specific needs. */
	class SoundVoice : public InfoClass
	{
		public:
		/** The voice is owner of the waves. */
		SoundVoice (int id, unsigned int cF,
			const SoundWaves& sw, SoundEnvelope* sE = nullptr);

		~SoundVoice ();

		int id () const
							{ return (_id); }

		bool active () const
							{ return (_active); }
		/** It can be overloaded for special purposes. 
			This methid manages also the envelope by default. */
		virtual void setActive (bool a);
	
		// To manage the waves...
		const SoundWaves& waves () const
							{ return (_waves); }
		/** NO boundaty analysis is done. */
		const SoundWave* wave (SoundWave::Type t) const
							{ return (_wavesPlain [(size_t) t]); }
		SoundWave* wave (SoundWave::Type t)
							{ return ((SoundWave*) (((const SoundVoice*) (this)) -> wave (t))); }

		// To manage the envelop...
		/** Take care the result can be nullptr. */
		const SoundEnvelope* envelope () const
							{ return (_envelope); }
		SoundEnvelope* envelope ()
							{ return ((SoundEnvelope*) (((const SoundVoice*) (this)) -> envelope ())); }

		/** All waves being part of the voice must "vibrate" at the same frequency. \n
			Take care because no boundary analysis is done. */
		unsigned short frequency () const
							{ return (_waves [0] -> frequency ()); /** could be any, as all have the same value. */ }
		void setFrequency (unsigned short f)
							{ for (auto i : _waves) 
								i -> setFrequency (f); }


		// To manage the way the voice works...
		/** To start/stop the voice. 
			This usually starts/stops the internal parameters of the voice. \n
			By default it starts/stops the envelope, if any. */
		virtual void setStart (bool s)
							{ if (_envelope != nullptr) 
								_envelope -> setStart (s); }
		/** To initialize. */
		virtual void initialize ();
		/** To initialize the interal counters that are usually managed in the voice. \n
			By default the ones in the envelop and the ones in each of the voices are initialized. */
		virtual void initializeInternalCounters ();
		/** This method has to be invoked at every cpu cycle to 
			actualize the situation of the waves, ADSR cycle and others. \n
			It can be overloaded for specific needs (imagin a filter class is added!). */
		virtual void clock (unsigned int nC = 1);
		/** To get the output data of the voice. \n
			It is number between 0 an 1. \n
			It can be overloaded later for special purposes. \n
			The number returned is less than 1.0 if each of wavedData 
			and envelopeData is well designed and returns less than 1.0. */
		virtual double data () const
							{ return (wavesData () * 
								((_envelope != nullptr && _envelope -> active ()) 
									? _envelope -> envelopeData () : 1.0)); }

		/**
		  *	The name of the fields are: \n
		  *	ID				= Id of the voice.
		  *	ACTIVE			= Attribute with YES when the voice is active and NO in other case.
		  *	ATTACK			= Value (from to 0 to 1) of the _attack parameter.
		  *	DECAY			= Value (from to 0 to 1) of the _decay parameter.
		  *	RELEASE			= Value (from to 0 to 1) of the _release parameter.
		  *	SUSTAIN			= Value (from to 0 to 1) of the _sustain parameter.
		  */
		virtual InfoStructure getInfoStructure () const override;

		protected:
		/** To calculate the value comming from the waves. */
		double wavesData () const;

		protected:
		/** The id of the voice. */
		int _id;
		/** The frequency of the chip in cycles per second. */
		unsigned int _chipFrequency;
		/** When the voice is active. */
		bool _active;
		/** A reference to the envelope if any, so it can be nullptr. */
		SoundEnvelope* _envelope;
		/** All possible waves. */
		MCHEmul::SoundWaves _waves;

		// Implementation...
		// The waves are stored in a plain vector...
		// There could be positions with no wave...take care when looking for them...
		SoundWave** _wavesPlain;
	};

	// To facilitate the use of a set of these...
	using SoundVoices = std::vector <SoundVoice*>;
}

#endif
  
// End of the file
/*@}*/
