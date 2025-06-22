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
		  *	@param cF	Chip frequency (sound) in clocks / second.
		  *				In a MXS1 e,g, the speed of this chip is half the speed of the CPU...
		  *				This is also the speed used to calculate the waveforms. \n
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

		/** The envelope of the chip is different that a classic ADSR. */
		class Envelope final : public MCHEmul::SoundEnvelope
		{
			public:
			/** The type of wave the system is reproducing. */
			enum class Type
			{
				_TYPE0  = 0,
				_TYPE1  = 1,
				_TYPE8  = 8,
				_TYPE9  = 9,
				_TYPE10 = 10,
				_TYPE11 = 11,
				_TYPE12 = 12,
				_TYPE13 = 13,
				_TYPE14 = 14,
				_TYPE15 = 15
			};

			friend AY38910SimpleLibWrapper;

			/** The type by default is _TYPE0 and the frequency is 0 by default. */
			Envelope (unsigned short cF);

			/** Change the type of envelope drawn. \n
				Any type the envelope form is changed the sampling data is recalculated. */
			Type type () const
							{ return (_type); }
			void setType (Type t)
							{ _type =  t, calculateSamplingData (); }
			/** Same, but from the value of a register directly...
				Transmit the changes directly to the previous method. */
			void setType (const MCHEmul::UByte& v); 

			/** Sets directly the frequency of the envelope. */
			unsigned short frequency () const
							{ return (_frequency); }
			void setFrequency (unsigned short f)
							{ _frequency = f; calculateSamplingData (); }

			virtual void setStart (bool s) override;
			virtual void initialize () override;
			virtual void initializeInternalCounters () override;
			virtual void clock (unsigned int nC = 1) override;
			virtual double envelopeData () const override;

			virtual MCHEmul::InfoStructure getInfoStructure () const override;

			private:
			/** To calculate the internal data needed to later "draw" the voice. 
				Anytme the sampling data is recalculated the internal counter are also recualculated and 
				the State turns back into attack directly. */
			void calculateSamplingData ();

			private:
			// Implementation
			/** The status in which the wave is in. \n
				That status will depend on which type of wave the envelope system is reproducing. */
			enum class State
			{
				_ATTACK = 0,
				_SUSTAIN = 1,
				_DECAY = 2
			};

			/** The type of envelope wave. */
			Type _type;
			/** The state in which the full wave is. */
			State _state;
			/** The internal frequency of the wave. */
			unsigned short _frequency;

			/** The counters used to control the states _increase, _sustian and _decrease. */
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

		/** Every AY voice is made up only of a either single pulse or noise wave. \n 
			No voice has its own ADSR. It is common to all voices, 
			but it can be used or not depending on the configuration. 
			SO here the envelope is nullptr. */
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
			They will be three defined at construction time. \n
			There are 3 square voices and a noise one. \n
			The noise one can be mixed with the voices attending the value of register 7. \n 
			The envelope is common for all voices, so there is no envelope assigned to te individual voices. */
		MCHEmul::SoundVoices _voices;
		Envelope _envelope; // The envelope affects, all voices at the same time...
		bool _useEnvelope [3]; // To determine whether the envelope has to be used or not...
		bool _mixNoise [3]; // Mix noise with the square waves. false by default...
		double _volumen [3]; // The volumen can be between 0 and 1, per voice...

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
