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
#include <CORE/SoundEnvelope.hpp>
#include <CORE/SoundFilter.hpp>

namespace MCHEmul
{
	/** Represents a voice. \n
		Ot can be overloaded for specific needs. */
	class SoundVoice : public InfoClass
	{
		public:
		/** The voice is owner of the waves. */
		SoundVoice (int id, unsigned int cF,
			const SoundWaves& sw, 
			SoundEnvelope* sE = nullptr, 
			const SoundFilters& sF = { });

		~SoundVoice ();

		int id () const
							{ return (_id); }

		bool active () const
							{ return (_active); }
		/** It can be overloaded for special purposes. 
			This methid manages also the envelope by default. */
		virtual void setActive (bool a)
							{ _active = a; }
	
		// To manage the waves...
		const SoundWaves& waves () const
							{ return (_waves); }
		/** NO boundaty analysis is done. */
		const SoundWave* wave (SoundWave::Type t) const
							{ return (_wavesPlain [(size_t) t]); }
		SoundWave* wave (SoundWave::Type t)
							{ return (const_cast <SoundWave*> 
								(const_cast <const SoundVoice*> (this) -> wave (t))); }

		// To manage the envelop...
		/** Take care the result can be nullptr. */
		const SoundEnvelope* envelope () const
							{ return (_envelope); }
		SoundEnvelope* envelope ()
							{ return (const_cast <SoundEnvelope*> 
								(const_cast <const SoundVoice*> (this) -> envelope ())); }
		void setEnvelopeActive (bool a) // To activate / desactivate the evelope (if any) only...
							{ if (_envelope != nullptr) _envelope -> setActive (a); }

		// To manage the filter...
		/** Take care the result can be nullptr. */
		const SoundFilters filters () const
							{ return (_filters); }
		const SoundFilter* filter (size_t nF) const // No boundary check is done...be carefull (for performance reasons)
							{ return (_filters [nF]); }
		SoundFilter* filter (size_t nF)
							{ return (const_cast <SoundFilter*> 
								(const_cast <const SoundVoice*> (this) -> filter (nF))); }
		// To activate / desactivate the filter (if any) only...
		// No boundary check is done at all... so be carefull!
		void setFilterActive (size_t nF, bool a) 
							{ filters () [nF] -> setActive (a); }
		// To activate / desactivate all filters...
		void setFiltersActive (bool a) 
							{ for (auto f : _filters) f -> setActive (a); }

		/** All waves being part of the voice must "vibrate" at the same frequency. \n
			Take care because no boundary analysis is done. */
		double frequency () const
							{ return (_waves [0] -> frequency ()); /** could be any, as all have the same value. */ }
		void setFrequency (double f)
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
		/** To get the normalized output data of the voice. \n
			It is a number between -1.0 and 1.0, with 0.0 representing silence. \n
			It can be overloaded later for special purposes. \n
			The envelope, when present, is applied as a gain between 0.0 and 1.0. */
		virtual double data () const;

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
		/** To calculate the value comming from the waves. 
			The values of the different waves are combined to produce the final output. 
			That value is a normalized sample between -1.0 and 1.0. */
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
		/** A reference to the filters if any, so it can be empty. */
		SoundFilters _filters;
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
