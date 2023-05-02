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

#include <CORE/SoundWave.hpp>

namespace MCHEmul
{
	/** Represents a voice. \n
		Ot can be overloaded for specific needs. */
	class SoundVoice
	{
		public:
		/** The voice is owner of the waves. */
		SoundVoice (int id, unsigned int cF, const SoundWaves& sw)
			: _id (id),
			  _chipFrequency (cF),
			  _active (false), // No active by default...
			  _attack (0), _decay (0), _release (0),
			  _sustainVolumen (0.0f),
			  _waves (sw),
			  _state (State::_ATTACK),
			  _stateCounters (4, StateCounters ()) // All to 0...
						{ calculateWaveSamplingData (); }

		~SoundVoice ()
						{ for (auto i : _waves) 
							delete (i); }

		int id () const
						{ return (_id); }

		bool active () const
						{ return (_active); }
		void setActive (bool a)
						{ _active = a; }

		/** To define the state of the voice. */
		void setStart (bool s)
						{ _state = s ? State::_ATTACK : State::_RELEASE; }
	
		// ADSR values...
		/** The values are given and returned in milliseconds. */
		unsigned short attack () const
						{ return (_attack); }
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

		/** The sustain volumen is a number between 0 and 1 indicating 
			the %(1) over a "maximum value". */
		double sustainVolumen () const
						{ return (_sustainVolumen); }
		void setSustainVolumen (double s)
						{ if ((_sustainVolumen = s) > 1.0f) 
							_sustainVolumen = 1.0f; 
						  calculateWaveSamplingData (); }
	
		const SoundWaves& waves () const
						{ return (_waves); }
		const SoundWave* wave (SoundWave::Type t) const
						{ return (_waves [(int) t]); }
		SoundWave* wave (SoundWave::Type t)
						{ return ((SoundWave*) (((const SoundVoice*) (this)) -> wave (t))); }

		/** All waves being part of the voce must "follow the same"vibrate" at the same frequency. */
		unsigned short frequency () const
						{ return (_waves [0] -> frequency ()); /** could be any, as all have the same value. */ }
		void setFrequency (unsigned short f)
						{ for (auto i : _waves) 
							i -> setFrequency (f); }

		/** This method has to be invoked at every cpu cycle to 
			actualize the situation of the waves, ADSR cycle and others. \n
			It can be overloaded for specific needs (imagin a filter class is added!). */
		virtual void clock (unsigned int nC = 1);

		/** To get the output data of the voice. \n
			It is number between 0 an 1. \n
			It can be overloaded later for special purposes. */
		virtual double data () const;

		protected:
		// Implementation
		/** To calculate the internal data needed to later "draw" the voice. \n
			It could be overloaded to include more intenal data needed
			depending on the type of voice. \n
			Any moment a key value is changed this method should be invoked. */
		virtual void calculateWaveSamplingData ();

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
			_RELEASE = 3
		};

		/** The state in which the full wave is. */
		State _state;

		/** The counters used to control the states _attack, _decay, 
			_sustain (not needed), and _release. */
		struct StateCounters
		{
			StateCounters ()
				: _cyclesPerState (0), _counterCyclesPerState (0)
							{ }

			unsigned int _cyclesPerState;
			unsigned int _counterCyclesPerState;
		}; 

		mutable std::vector <StateCounters> _stateCounters;
	};

	// To facilitate the use of a set of these...
	using SoundVoices = std::vector <SoundVoice*>;
}

#endif
  
// End of the file
/*@}*/
