/** \ingroup GENERALINSTRUMENTS */
/*@{*/

/**	
 *	@file	
 *	File: AY38910LibWrapper.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 01/06/2025 \n
 *	Description: AY38910 Chip Wrapper Emulation.
 *	Versions: 1.0 Initial
 */

#ifndef __GENERALINSTRUMENTS_AY38910LIBWRAPPER__
#define __GENERALINSTRUMENTS_AY38910LIBWRAPPER__

#include <CORE/incs.hpp>

namespace GENERALINSTRUMENTS
{
	/** The generic AY38910LibWrapper. */
	class AY38910LibWrapper : public MCHEmul::SoundLibWrapper
	{
		public:
		AY38910LibWrapper (const MCHEmul::Attributes attrs = { })
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

	/** A very simple wrapper trying to emulate 
		all voices in the simpliest way possible. \n
		All programmed by ICF after reading several articles about!!! */
	class AY38910SimpleLibWrapper final : public AY38910LibWrapper
	{
		public:
		/** 
		  *	Constructor.
		  *	@param cF	Chip frequency in clocks / second.
		  * @param sF	Sampling frequency in samples / second. It cannot be 0.
		  */
		AY38910SimpleLibWrapper (unsigned int cF, unsigned int sF);

		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;
		virtual const MCHEmul::UByte& readValue (size_t p) const override;

		virtual void initialize () override;

		virtual bool getData (MCHEmul::CPU *cpu, MCHEmul::UBytes& dt) override;

		virtual MCHEmul::InfoStructure getVoiceInfoStructure (unsigned char nV) const override
						{ return ((nV < 3) ? _voices [nV] -> getInfoStructure () : MCHEmul::InfoStructure ()); }

		private:
		unsigned int _chipFrequency;
		unsigned int _samplingFrequency;

		/** The SID voice is made up of 4 waves and
			there is special methos to deal with the pulse one. */
		class Voice final : public MCHEmul::SoundVoice
		{
			public:
			friend AY38910SimpleLibWrapper;

			Voice (int id, unsigned int cF);

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

			virtual void initialize () override;

			/** To support the ring modulation. */
			virtual double data () const override;

			/**
			  *	The name of the fields are: \n
			  */
			virtual MCHEmul::InfoStructure getInfoStructure () const override;

			private:
			/** Be careful because there is no boundaries verification, 
				The entry value (parameter) can be either 0 or 1, no more. */
			void setWavesActive (unsigned char wA)
							{ _wavesActive = wA; }

			private:
			// Implementation
			// Waves active..to speed up the calculus later
			unsigned char _wavesActive;
		};

		/** The different voices used by AY38910. \n
			They will be three defined at construction time. */
		MCHEmul::SoundVoices _voices;

		/** The registers used by the AY38910. */
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
