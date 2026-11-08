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

		/**
		  *	The name of the fields are: \n
		  *	The attributes and infostructures of the parent class, plus: \n
		  *	VOICES	= InfoStructure: Information about the two TED sound voices.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;
		/** To get the info of the different voices. */
		virtual MCHEmul::InfoStructure getVoiceInfoStructure (unsigned char nV) const = 0;

		protected:
		// Implementation
		mutable MCHEmul::UByte _lastValueRead;
	};

	/** TED tone divider. Frequency writes replace the reload without rescaling the active count. */
	class TEDPulseSoundWave final : public MCHEmul::PulseSoundWave
	{
		public:
		TEDPulseSoundWave (unsigned int cF);

		void setFrequencyRegister (unsigned short value);
		inline void setDCActive (bool active);
		bool dcActive () const
							{ return (_dcActive); }
		/** True if the latest clock call contained a divider edge. */
		bool dividerEdge () const
							{ return (_dividerEdge); }

		virtual void initialize () override;
		virtual void initializeInternalCounters () override;
		virtual void clock (unsigned int nC = 1) override;
		virtual double clockValue () const override;
		virtual bool pulseUp () const override
							{ return (_output); }
		virtual double data () const override
							{ return (_active ? (_output ? 1.0 : -1.0) : 0.0); }

		/**
		  * The parent fields and the following attributes. \n
		  * REGISTER = Programmed 10-bit frequency value. \n
		  * RELOAD = Next divider reload, from 1 to 1024. \n
		  * COUNTER = Remaining divider ticks in the current half-cycle. \n
		  * HALFLENGTH = Reload used when the current half-cycle started. \n
		  * PRESCALER = Position in the divide-by-four stage, from 0 to 3. \n
		  * LEVEL = Current raw output bit, independently of output enable. \n
		  * DC = Reload/DC control level. \n
		  * OVERFLOW = Previous divider-overflow level. \n
		  * FREQUENCY is nominal inspection data; timing uses the integer divider.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		// TED registers control frequency and duty is fixed at 50 percent.
		using MCHEmul::SoundWave::setFrequency;
		using MCHEmul::PulseSoundWave::setPulseUpPercentage;

		// Implementation
		unsigned short _frequencyRegister;
		unsigned short _reloadValue;
		unsigned short _counter;
		unsigned short _halfLength;
		unsigned char _prescaler;
		bool _output;
		bool _previousOverflow;
		bool _dividerEdge;
		bool _dcActive;
	};

	/** Binary TED noise, advanced by the second tone divider rather than a free-running phase. */
	class TEDNoiseSoundWave final : public MCHEmul::SoundWave
	{
		public:
		TEDNoiseSoundWave (unsigned int cF);

		virtual void initialize () override;
		virtual void initializeInternalCounters () override;
		/** No autonomous advance; Voice supplies divider events through shift (). */
		virtual void clock (unsigned int nC = 1) override
							{ }
		inline void shift ();
		void shiftOne ()
							{ _shiftRegister = (unsigned char) ((_shiftRegister << 1) | 0x01); }
		bool output () const
							{ return ((_shiftRegister & 0x01) == 0); }
		virtual double data () const override
							{ return (_active ? (output () ? 1.0 : -1.0) : 0.0); }

		/**
		  * The parent fields and the following attributes. \n
		  * LFSR = Current 8-bit shift-register value. \n
		  * LEVEL = Current raw output bit, independently of output enable. \n
		  * CLOCKSOURCE = Channel2Divider; no independent noise frequency is programmed.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		// Implementation
		unsigned char _shiftRegister;
	};

	/** TED digital generators using framework voices and specialized pulse/noise waves. */
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
		virtual ~TEDSoundSimpleLibWrapper () override;

		/** The volumen is a number between 0 and 1. */
		double volumen () const
							{ return (_volumen); }
		inline void setVolumen (double v);

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
		void updateControl ();

		private:
		unsigned int _tedFrequency;
		unsigned int _dividerValue;
		unsigned int _samplingFrequency;
		double _volumen;

		/** The voice coordinates divider events and output selection; SoundVoice owns its waves. */
		class Voice final : public MCHEmul::SoundVoice
		{
			public:
			Voice (int id, unsigned int cF);

			virtual void setActive (bool active) override
							{ _active = active; }
			void setFrequencyRegister (unsigned short value)
							{ _pulse -> setFrequencyRegister (value); }
			void configure (bool toneEnabled, bool noiseEnabled, bool dcActive);

			virtual void initialize () override;
			virtual void clock (unsigned int nC = 1) override;
			/** Selected digital output, normalized to 0 or 1; disabled output is 0. */
			virtual double data () const override;

			private:
			// Implementation
			/** Non-owning references; the base voice owns the waves. */
			TEDPulseSoundWave* _pulse;
			TEDNoiseSoundWave* _noise;
			bool _noiseSelected;
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
	inline void TEDPulseSoundWave::setDCActive (bool active)
	{
		_dcActive = active;

		if (_dcActive)
			_output = true;
	}

	// ---
	inline void TEDNoiseSoundWave::shift ()
	{
		// 8-bit feedback used by the reference digital model (taps 7, 5, 4 and 1).
		const unsigned char feedback =
			((_shiftRegister >> 7) ^ (_shiftRegister >> 5) ^
			 (_shiftRegister >> 4) ^ (_shiftRegister >> 1)) & 0x01;

		_shiftRegister = (unsigned char) ((_shiftRegister << 1) | feedback);
	}

	// ---
	inline void TEDSoundSimpleLibWrapper::setVolumen (double v)
	{
		if (v < 0.0f) _volumen = 0.0f;
		else if (v > 1.0f) _volumen = 1.0f;
		else _volumen = v;
	}

}

#endif
  
// End of the file
/*@}*/
