/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: VICISoundWrapper.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 24/12/2023 \n
 *	Description: VICI Sound Wrapper Emulation.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_VICISOUNDWRAPPER__
#define __COMMODORE_VICISOUNDWRAPPER__

#include <CORE/incs.hpp>

namespace COMMODORE
{
	/** The generic LibWrapper for VIC20. \n
		Nothing special to do. */
	class VICISoundLibWrapper : public MCHEmul::SoundLibWrapper
	{
		public:
		VICISoundLibWrapper (const MCHEmul::Attributes& attrs = { })
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
	class VICISoundSimpleLibWrapper final : public VICISoundLibWrapper
	{
		public:
		/** 
		  *	Constructor.
		  *	@param cF	Chip frequency in clocks / second.
		  * @param sF	Sampling frequency in samples / second. It cannot be 0.
		  */
		VICISoundSimpleLibWrapper (unsigned int cF, unsigned int sF);

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

		/** The VICI voice can adapt three different types of waves,
			depending on the voice. */
		class Voice final : public MCHEmul::SoundVoice
		{
			public:
			friend VICISoundSimpleLibWrapper;

			Voice (int id, unsigned int cF)
				: MCHEmul::SoundVoice (id, cF,
					{
						new MCHEmul::SawSmoothSoundWave (cF),
						new MCHEmul::PulseSoundWave (cF),
						new MCHEmul::NoiseSoundWave (cF)
					}),
				  _wavesActive (0)
							{ setClassName ("VICIVoice"); }

			virtual void setActive (bool a) override
							{ if ((_active != a) && (_active = a)) 
								for (auto i : _waves) // Initialize only the counters of the waves...
									i -> initializeInternalCounters (); }

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

			/** To support the ring modulation. */
			virtual double data () const override;

			private:
			void setWavesActive (unsigned char wA)
							{ _wavesActive = wA; }

			private:
			// Implementation
			// Waves active..to speed up the calculus later
			unsigned char _wavesActive;
		};

		/** The different voices used by SID. \n
			They will be three defined at construction time. */
		MCHEmul::SoundVoices _voices;

		/** The registers used by the VICI. */
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
