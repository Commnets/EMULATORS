/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: SoundVoice.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
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
	/** Represents a voice. \n
		Ot can be overloaded for specific needs. */
	class SoundVoice : public InfoClass
	{
		public:
		/** The voice is owner of the waves. */
		SoundVoice (int id, unsigned int cF, const SoundWaves& sw)
			: InfoClass ("SoundVoice"),
			  _id (id),
			  _chipFrequency (cF),
			  _active (false), // No active by default...
			  _attack (0), _decay (0), _release (0),
			  _sustainVolumen (0.0f),
			  _waves (sw),
			  _state (State::_ATTACK),
			  _stateCounters (5 /** The five internal states of the wave = ADSR+Idle */, StateCounters ()) // All to 0...
						{ calculateVoiceSamplingData (); }

		~SoundVoice ()
						{ for (auto i : _waves) 
							delete (i); }

		int id () const
						{ return (_id); }

		bool active () const
						{ return (_active); }
		/** It can be overloaded for special purposes. */
		virtual void setActive (bool a)
						{ if ((_active != a) && (_active = a))
							initializeInternalCounters (); }

		/** To define the state of the voice. */
		void setStart (bool s)
						{ bool iC = false;
						  if (s) { iC = true; _state = State::_ATTACK; }
						  else /** Only when no idle. */ { if (_state != State::_IDLE) { iC = true; _state = State::_RELEASE; } }
						  if (iC) for (auto& i : _stateCounters) i.initialize (); }

		// ADSR values...
		/** The values are given and returned in milliseconds. */
		unsigned short attack () const
						{ return (_attack); }
		void setAttack (unsigned short a)
						{ _attack = a; calculateVoiceSamplingData (); }
		unsigned short decay () const
						{ return (_decay); }
		void setDecay (unsigned short d)
						{ _decay = d; calculateVoiceSamplingData (); }
		unsigned short release () const
						{ return (_release); }
		void setRelease (unsigned short r)
						{ _release = r; calculateVoiceSamplingData (); }

		/** The sustain volumen is a number between 0 and 1 indicating 
			the %(1) over a "maximum value". */
		double sustainVolumen () const
						{ return (_sustainVolumen); }
		void setSustainVolumen (double s)
						{ if ((_sustainVolumen = s) > 1.0f) 
							_sustainVolumen = 1.0f; 
						  calculateVoiceSamplingData (); }
	
		const SoundWaves& waves () const
						{ return (_waves); }
		const SoundWave* wave (SoundWave::Type t) const
						{ return (_waves [(size_t) t]); }
		SoundWave* wave (SoundWave::Type t)
						{ return ((SoundWave*) (((const SoundVoice*) (this)) -> wave (t))); }

		/** All waves being part of the voice must "vibrate" at the same frequency. */
		unsigned short frequency () const
						{ return (_waves [0] -> frequency ()); /** could be any, as all have the same value. */ }
		void setFrequency (unsigned short f)
						{ for (auto i : _waves) 
							i -> setFrequency (f); }

		/** To initialize. */
		virtual void initialize ();

		/** To initialize the interal counter of the waves only. */
		void initializeWavesInternalCounters ()
							{ for (auto i : _waves) 
								i -> initializeInternalCounters (); }

		/** To initialize the internal counters. \n
			All counters about the internal state are move back to initial values. \n
			Can be overloaded for further purposes. \n
			By default it is also used from the method calculateVoiceSamplingData. */
		virtual void initializeInternalCounters ()
							{ initializeWavesInternalCounters ();
							  for (size_t i = 0; i < 5; // The states of the ADSR (Just 4)...
								_stateCounters [i++]._counterCyclesPerState = 0); }

		/** This method has to be invoked at every cpu cycle to 
			actualize the situation of the waves, ADSR cycle and others. \n
			It can be overloaded for specific needs (imagin a filter class is added!). */
		virtual void clock (unsigned int nC = 1);

		/** To get the output data of the voice. \n
			It is number between 0 an 1. \n
			It can be overloaded later for special purposes. */
		virtual double data () const
							{ return (wavesData () * ADSRData ()); }

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
		// Implementation
		/** To calculate the internal data needed to later "draw" the voice. \n
			It could be overloaded to include more intenal data needed
			depending on the type of voice. \n
			Any moment a key value is changed this method should be invoked. */
		virtual void calculateVoiceSamplingData ();

		/** To calculate the value comming from the waves. */
		double wavesData () const;
		/** Same with the value of the ADSR. */
		double ADSRData () const;

		protected:
		/** The id of the voice. */
		int _id;
		/** The frequency of the chip in cycles per second. */
		unsigned int _chipFrequency;
		/** When the voice is active. */
		bool _active;
		/** The variables used for the envelop. In milliseconds. */
		unsigned short _attack, _decay, _release;
		/** The volumen for sustain. From 0 to 1. */
		double _sustainVolumen;
		/** All possible waves. */
		MCHEmul::SoundWaves _waves;

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

	// To facilitate the use of a set of these...
	using SoundVoices = std::vector <SoundVoice*>;
}

#endif
  
// End of the file
/*@}*/
