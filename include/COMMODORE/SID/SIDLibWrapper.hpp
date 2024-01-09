/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: SIDLibWrapper.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
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
		unsigned int _chipFrequency;
		unsigned int _samplingFrequency;
		double _volumen;

		/** The SID voice is made up of 4 waves and
			there is special methos to deal with the pulse one. */
		class Voice final : public MCHEmul::SoundVoice
		{
			public:
			friend SoundSIDSimpleWrapper;

			Voice (int id, unsigned int cF)
				: MCHEmul::SoundVoice (id, cF,
					{
						new MCHEmul::TriangleSoundWave (cF),
						new MCHEmul::SawSmoothSoundWave (cF),
						new MCHEmul::PulseSoundWave (cF),
						new MCHEmul::NoiseSoundWave (cF)
					}),
				  _voiceRelated (nullptr), // set when Emulation is built (it is guarentted that it is not nullptr when running)
				  _ringModulation (false), // Not modulated by default...
				  _sync (false), // Not sync by default...
				  _wavesActive (0)
							{ setClassName ("SIDVoice"); }

			virtual void setActive (bool a) override
							{ if ((_active != a) && (_active = a)) 
								for (auto i : _waves) // Initialize only the counters of the waves...
									i -> initializeInternalCounters (); }

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
			bool wavesClockRestarted () const
							{ bool r = true; 
							  for (auto i : waves ()) r &= i -> clockRestarted (); /** One will be enought, but just to set all rest to 0. */
							  return (r); }
			unsigned char oscillatorValue () const
							{ return ((unsigned char) (wavesData () * 255)); }
			/** Same but for the envelope. */
			unsigned char envelopeValue () const
							{ return ((unsigned char) (ADSRData () * 255)); }

			/** To control the percentage of the pulse wave when active. */
			double pulseUpPercentage () const
							{ return (static_cast <MCHEmul::PulseSoundWave*> 
								(waves ()[(size_t) MCHEmul::SoundWave::Type::_PULSE]) -> pulseUpPercentage ()); }
			void setPulseUpPercentage (double pU)
							{ static_cast <MCHEmul::PulseSoundWave*> 
								(waves ()[(size_t) MCHEmul::SoundWave::Type::_PULSE]) -> setPulseUpPercentage (pU); }

			virtual void initialize () override;

			/** To support the ring modulation. */
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

			private:
			/** The voice related. */
			Voice* _voiceRelated;
			/** The ring modulation. */
			bool _ringModulation;
			/** Is it sync with its voice related?. */
			bool _sync;

			// Implementation
			// Waves active..to speed up the calculus later
			unsigned char _wavesActive;

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
		unsigned int _clocksPerSample;
		/** Counter from 0 to _clockPerSample. */
		unsigned int _counterClocksPerSample;
	};
}

#endif
  
// End of the file
/*@}*/
