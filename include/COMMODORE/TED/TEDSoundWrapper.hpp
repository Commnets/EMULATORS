/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: TEDSoundWrapper.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 09/02/2024 \n
 *	Description: TED Sound Wrapper Emulation.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_TEDSOUNDWRAPPER__
#define __COMMODORE_TEDSOUNDWRAPPER__

#include <CORE/incs.hpp>

namespace COMMODORE
{
	/** The generic LibWrapper for TED. \n
		Nothing special to do. */
	class TEDSoundLibWrapper : public MCHEmul::SoundLibWrapper
	{
		public:
		TEDSoundLibWrapper (const MCHEmul::Attributes& attrs = { })
			: MCHEmul::SoundLibWrapper (attrs),
			  _lastValueRead (MCHEmul::UByte::_0)
							{ }

		/** Reading & Writing the records. */
		virtual void setValue (size_t p, const MCHEmul::UByte& v) = 0;
		virtual const MCHEmul::UByte& readValue (size_t p) const = 0;
		/** Like the previous one but not affecting the value. */
		virtual const MCHEmul::UByte& peekValue (size_t p) const
							{ return (readValue (p)); }

		/** To get the full info of the wrapper, including the voices. */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;
		/** To get the info of the different voices. */
		virtual MCHEmul::InfoStructure getVoiceInfoStructure (unsigned char nV) const = 0;

		protected:
		// Implementation
		mutable MCHEmul::UByte _lastValueRead;
	};

	/** A very simple wrapper trying to emulate 
		all voices in the simpliest way possible. \n
		All programmed by ICF after reading several articles about!!! */
	class TEDSoundSimpleLibWrapper final : public TEDSoundLibWrapper
	{
		public:
		/** 
		  *	Constructor.
		  *	@param tF	TED frequency in clocks / second.
		  * @param dv	Divider value to get the cpu frequency from the TED frequency.
		  *				The TED frequency is the one leading the CPU working.
		  * @param sF	Sampling frequency in samples / second. It cannot be 0.
		  */
		TEDSoundSimpleLibWrapper (unsigned int tF, unsigned int dv, unsigned int sF);

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
						{ return ((nV < 2) ? _voices [nV] -> getInfoStructure () : MCHEmul::InfoStructure ()); }

		private:
		// To help managing the voices...
		unsigned int voice0Register () const
						{ return (((unsigned int) (_registers [0x12].value () & 0x03)) << 8) |
								   ((unsigned int) _registers [0x0e].value ()); }
		unsigned int voice1Register () const
						{ return (((unsigned int) (_registers [0x10].value () & 0x03)) << 8) |
								   ((unsigned int) _registers [0x0f].value ()); }
		inline double tedToneFrequency (unsigned int r) const;

		private:
		unsigned int _tedFrequency;
		unsigned int _dividerValue;
		unsigned int _samplingFrequency;
		double _volumen;

		/** The TED voice can adapt two different types of waves,
			depending on the voice (1 in voice 1 and 2 in voice 2 (pulse + noise). */
		class Voice final : public MCHEmul::SoundVoice
		{
			public:
			friend TEDSoundSimpleLibWrapper;

			Voice (int id, unsigned int cF);

			virtual void setActive (bool a) override
							{ _active = a; }

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

			/** To control the percentage of the pulse wave when active. */
			double pulseUpPercentage () const
							{ return (static_cast <MCHEmul::PulseSoundWave*> 
								(waves ()[(size_t) MCHEmul::SoundWave::Type::_PULSE]) -> pulseUpPercentage ()); }
			void setPulseUpPercentage (double pU)
							{ static_cast <MCHEmul::PulseSoundWave*> 
								(waves ()[(size_t) MCHEmul::SoundWave::Type::_PULSE]) -> setPulseUpPercentage (pU); }

			/** To get the current TED voice output. */
			virtual double data () const override;

			private:
			void setWavesActive (unsigned char wA)
							{ _wavesActive = wA; }

			private:
			// Implementation
			// Waves active..to speed up the calculus later
			unsigned char _wavesActive;
		};

		/** The two TED sound voices. */
		MCHEmul::SoundVoices _voices;

		/** The registers used by the TED. */
		std::vector <MCHEmul::UByte> _registers;

		// Implementation
		/** The number of cycles that a sample takes in the sound. */
		double _cyclesPerSample;
		/** Counter from 0 to _cyclesPerSample. */
		double _counterCyclesPerSample;
	};

	// ---
	inline double TEDSoundSimpleLibWrapper::tedToneFrequency (unsigned int r) const
	{
	    unsigned int divisor = 1024 - (r & 0x03ff);
		if (divisor == 0) divisor = 1;
		return ((double) _tedFrequency / (double) _dividerValue) /
				(8.0f * (double) divisor);
	}
}

#endif
  
// End of the file
/*@}*/
