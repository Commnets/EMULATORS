/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: SIDLibWrapper.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 13/01/2023 \n
 *	Description: SID Chip Wrapper Emulation.
 *				 The emulation is created using the RESID library:
 *				 http://www.zimmers.net/anonftp/pub/cbm/crossplatform/emulators/resid/index.html
 *				 This library has been integrated in many emulators like VICE (my reference).
 *				 Also a wrapper for testing purposes is created.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_SIDLIBWRAPPER__
#define __COMMODORE_SIDLIBWRAPPER__

#include <CORE/incs.hpp>
#include <resid/sid.h>

namespace COMMODORE
{
	/** The generic SIDLibWrapper. */
	class SIDLibWrapper : public MCHEmul::SoundLibWrapper
	{
		public:
		SIDLibWrapper (const MCHEmul::Attributes attrs = { })
			: MCHEmul::SoundLibWrapper (attrs),
			  _lastValueRead (MCHEmul::UByte::_0)
							{ }

		virtual void setValue (size_t p, const MCHEmul::UByte& v)
							{ /** do nothing. */ }
		virtual const MCHEmul::UByte& readValue (size_t p) const
							{ return (_lastValueRead); }
		/** Like the previous one but not affecting the value. */
		virtual const MCHEmul::UByte& peekValue (size_t p) const
							{ return (readValue (p)); }

		/** To get the full info of the wrapper, including the voices. */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;
		/** To get information about the voices from then wrapper. \n
			That infomation is not neccesary stored in the registers. */
		virtual MCHEmul::InfoStructure getVoiceInfoStructure (unsigned char nV) const = 0;

		protected:
		// Implementation
		mutable MCHEmul::UByte _lastValueRead;
	};

	/** A wrapper over the RESID Library. */
	class SoundRESIDWrapper final : public SIDLibWrapper
	{
		public:
		/** 
		  *	Constructor.
		  *	@param cF	Chip frequency.
		  *	@param sM	Sampling method admitted by the RESID library. 
		  *	@param sF	Sampling frequency. This couldn't be less than 4000Hz.
		  */
		SoundRESIDWrapper (unsigned int cF, RESID::sampling_method sM, unsigned int sF)
			: SIDLibWrapper (
				{
					{ "Name", "RESID" },
					{ "Programer", "Dag Lem" },
					{ "Year", "2004" }
				}),
			  _chipFrequency (cF),
			  _samplingFrequency (sF),
			  _resid_sid ()
							{ _resid_sid.set_sampling_parameters ((double) cF, sM, (double) sF); }

		virtual void initialize () override
							{ _resid_sid.reset (); }

		virtual void setValue (size_t p, const MCHEmul::UByte& v) override
							{ _resid_sid.write ((RESID::reg8) (p % 0x20), (RESID::reg8) v.value ()); }
		virtual const MCHEmul::UByte& readValue (size_t p) const override
							{ return (_lastValueRead = 
								MCHEmul::UByte ((unsigned char) (*(const_cast <RESID::SID*> (&_resid_sid))).read ((RESID::reg8) (p % 0x20)))); }

		virtual bool getData (MCHEmul::CPU *cpu, MCHEmul::UBytes& dt) override;

		virtual MCHEmul::InfoStructure getVoiceInfoStructure (unsigned char nV) const override;

		private:
		unsigned int _chipFrequency;
		unsigned int _samplingFrequency;
		RESID::SID _resid_sid;
	};

	/** A very simple wrapper trying to emulate 
		all voices in the simpliest way possible. \n
		All programmed by ICF after reading several articles about!!! */
	class SoundSIDSimpleWrapper final : public SIDLibWrapper
	{
		public:
		static unsigned short _ATTACKTIMES [0x10];  // In milliseconds
		static unsigned short _DECAYTIMES [0x10];   // In milliseconds
		static unsigned short _RELEASETIMES [0x10]; // In milliseconds

		/** 
		  *	Constructor.
		  *	@param cF	Chip frequency in clocks / second.
		  * @param sF	Sampling frequency in samples / second. It cannot be 0.
		  */
		SoundSIDSimpleWrapper (unsigned int cF, unsigned int sF);

		/** The volumen is a number between 0 and 1. */
		double volumen () const
						{ return (_volumen); }
		void setVolumen (double v)
						{ _volumen = v; }

		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;
		virtual const MCHEmul::UByte& readValue (size_t p) const override;

		virtual void initialize () override;

		virtual bool getData (MCHEmul::CPU *cpu, MCHEmul::UBytes& dt) override;

		virtual MCHEmul::InfoStructure getVoiceInfoStructure (unsigned char nV) const override
						{ return ((nV < 3) ? _voices [nV] -> getInfoStructure () : MCHEmul::InfoStructure ()); }

		private:
		// These methods are used in registers 0x15, 0x16, 0x17 & 0x18
		void setFrecuencyInFilters (unsigned short nR);
		void activateFiltersPerVoice 
			(const MCHEmul::UByte& vF, const MCHEmul::UByte& tF, const MCHEmul::UByte& nL);

		private:
		unsigned int _chipFrequency;
		unsigned int _samplingFrequency;
		double _volumen;

		/** Noise waveform generated as the SID does, using its phase accumulator
			and the internal 23-bit shift register. */
		class SIDNoiseSoundWave final : public MCHEmul::SoundWave
		{
			public:
			SIDNoiseSoundWave (unsigned int cF);

			void setTest (bool t);

			virtual void initialize () override;

			virtual void initializeInternalCounters () override;

			virtual void clock (unsigned int nC = 1) override;

			virtual double data () const override;

			protected:
			virtual void calculateWaveSamplingData () override;

			private:
			// Implementation...
			inline void stepShiftRegister ();
			inline unsigned char outputValue () const;

			private:
			/** SID 24-bit oscillator phase. */
			unsigned int _phaseAccumulator;
			/** 16-bit increment reconstructed from the programmed frequency. */
			unsigned int _phaseIncrement;
			/** SID 23-bit noise shift register. */
			unsigned int _shiftRegister;
		};

		/** The SID voice is made up of 4 waves and
			there is special methos to deal with the pulse one. */
		class Voice final : public MCHEmul::SoundVoice
		{
			public:
			friend SoundSIDSimpleWrapper;

			/** The sampling frecuency is needed for the filter associated to each voice. */
			Voice (int id, unsigned int cF, unsigned int sF);

			virtual void setActive (bool a) override
							{ _active = a; }

			void setTest (bool t);

			/** The other voices this one could be related with. **/
			void setRelation (Voice* v)
							{ _voiceRelated = v; }

			/** To set up / off the ring modulation with other voices. */
			bool ringModulation () const
							{ return (_ringModulation); }
			void setRingModulation (bool a)
							{ _ringModulation = a; }

			/** To synchronize the voice with the linked one. */
			void setSync (bool s)
							{ _sync = s; }

			/** To know the value of the oscilator behind.
				It is used sometimes for complex effects. \n
				It returns a number from 0 to 255 depending on the wave that is moving behind!. */
			unsigned char wavesClockValue () const
							{ return ((unsigned char) (waves ()[0] /** whatever. */ -> clockValue () * 255)); }
			unsigned char oscillatorValue () const
							{ return ((_wavesActive == 0)
								? 0 : MCHEmul::normalizedSoundSampleToU8 (wavesData ())); }
			/** Same but for the envelope. */
			unsigned char envelopeValue () const
							{ return ((unsigned char) (_envelope -> envelopeData () * 255)); }

			/** To control the percentage of the pulse wave when active. */
			double pulseUpPercentage () const
							{ return (static_cast <MCHEmul::PulseSoundWave*> 
								(waves ()[(size_t) MCHEmul::SoundWave::Type::_PULSE]) -> pulseUpPercentage ()); }
			void setPulseUpPercentage (double pU)
							{ static_cast <MCHEmul::PulseSoundWave*> 
								(waves ()[(size_t) MCHEmul::SoundWave::Type::_PULSE]) -> setPulseUpPercentage (pU); }

			virtual void initialize () override;

			/** To support the ring modulation. */
			virtual void clock (unsigned int nC = 1) override;

			bool oscillatorRestarted () const
							{ return (_oscillatorRestarted); }
			void clearOscillatorRestarted ()
							{ _oscillatorRestarted = false; }

			virtual double data () const override;

			/**
			  *	The name of the fields are: \n
			  *	RINGMODULATION	= Attribute with YES when Ring Modulation is active and NO in other case.
			  *	VOICERELATED	= Attribute with the id of the voice related with this one.
			  *	SYNC			= Attribute with YES when Syncronization is active and NO in other case.
			  */
			virtual MCHEmul::InfoStructure getInfoStructure () const override;

			private:
			void setWavesActive (unsigned char wA)
							{ _wavesActive = wA; }

			/** To apply the syncronization. */
			void applySync ();

			/** Just o reset a part of the values. */
			void initializeOscillatorCounters ();

			private:
			/** The voice used as source for hard sync and ring modulation. */
			Voice* _voiceRelated;
			/** The ring modulation. */
			bool _ringModulation;
			/** Is it sync with its voice related?. */
			bool _sync;
			/** To manage the test situation. */
			bool _test;

			// Implementation
			// Waves active..to speed up the calculus later
			unsigned char _wavesActive;
			/** To control when the oscilator has changed. */
			bool _oscillatorRestarted;

			// For the situations when a table of sounds is required...
			static const unsigned char _SAWTRIWAVE_6581 [0x100]; // The number of elements that the oscillator can take...
			static const unsigned char _PULSETRIWAVE_6581 [0x100];
			static const unsigned char _PULSESAWWAVE_6581 [0x100];
			static const unsigned char _PULSESAWTRIWAVE_6581 [0x100];
		};

		/** The different voices used by SID. \n
			They will be three defined at construction time. */
		MCHEmul::SoundVoices _voices;

		/** The registers used by the SID. */
		std::vector <MCHEmul::UByte> _registers;

		// Implementation
		double _cyclesPerSample;
		/** Fractional chip-cycle accumulator used to decide when a PCM sample must be emitted. */
		double _counterCyclesPerSample;
	};

	// ---
	inline void SoundSIDSimpleWrapper::SIDNoiseSoundWave::stepShiftRegister ()
	{
		const unsigned int feedback =
			((_shiftRegister >> 22) ^ (_shiftRegister >> 17)) & 0x01;

		_shiftRegister =
			((_shiftRegister << 1) & 0x7fffff) | feedback;
	}

	// ---
	inline unsigned char SoundSIDSimpleWrapper::SIDNoiseSoundWave::outputValue () const
	{
		return ((unsigned char)
			(((_shiftRegister & 0x400000) >> 15) |
			 ((_shiftRegister & 0x100000) >> 14) |
			 ((_shiftRegister & 0x010000) >> 11) |
			 ((_shiftRegister & 0x002000) >> 9) |
			 ((_shiftRegister & 0x000800) >> 8) |
			 ((_shiftRegister & 0x000080) >> 5) |
			 ((_shiftRegister & 0x000010) >> 3) |
			 ((_shiftRegister & 0x000004) >> 2)));
	}
}

#endif
  
// End of the file
/*@}*/
