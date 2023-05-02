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
		SIDLibWrapper ()
			: MCHEmul::SoundLibWrapper (),
			  _lastValueRead (MCHEmul::UByte::_0)
							{ }

		virtual void setValue (size_t p, const MCHEmul::UByte& v)
							{ /** do nothing. */ }
		virtual const MCHEmul::UByte& readValue (size_t p) const
							{ return (_lastValueRead); }

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
		SoundRESIDWrapper (double cF, RESID::sampling_method sM, double sF)
			: SIDLibWrapper (),
			  _resid_sid ()
							{ _resid_sid.set_sampling_parameters (cF, sM, sF); }

		virtual void initialize () override
							{ _resid_sid.reset (); }

		virtual void setValue (size_t p, const MCHEmul::UByte& v) override
							{	_resid_sid.write ((RESID::reg8) (p % 0x20), (RESID::reg8) v.value ()); }
		virtual const MCHEmul::UByte& readValue (size_t p) const override;

		virtual bool getData (MCHEmul::CPU *cpu, MCHEmul::UBytes& dt) override;

		private:
		RESID::SID _resid_sid;
	};

	/** A very simple wrapper trying to emulate 
		all voices in the simpliest way possible. \n
		All programmed by ICF after reading several articles about!!! */
	class SoundSimpleWrapper final : public SIDLibWrapper
	{
		public:
		/** 
		  *	Constructor.
		  *	@param cF	Chip frequency in clocks / second.
		  * @param sF	Sampling frequency in samples / second. It cannot be 0.
		  */
		SoundSimpleWrapper (unsigned int cF, unsigned int sF)
			: SIDLibWrapper (),
			  _volumen (0.0f), // There is no volumen at the beginning...
			  _voices (
				  { new Voice (0, cF), 
				    new Voice (1, cF), 
				    new Voice (2, cF) }),
			  _registers (std::vector <MCHEmul::UByte> (0x20, MCHEmul::UByte::_0)),
			  _lastClockCycles (0),
			  _clocksPerSample ((unsigned int) ((double) cF / (double) sF)), // sF can not be 0...
			  _counterClocksPerSample (0)
							{ }

		/** The volumen is a number between 0 and 1. */
		double volumen () const
						{ return (_volumen); }
		void setVolumen (double v)
						{ _volumen = v; }

		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;
		virtual const MCHEmul::UByte& readValue (size_t p) const override
							{ return (_lastValueRead = _registers [p % 0x20]); } // Every 20 registers the value is repeated...

		virtual void initialize () override;

		virtual bool getData (MCHEmul::CPU *cpu, MCHEmul::UBytes& dt) override;

		private:
		double _volumen;

		/** The SID voice is made up of 4 waves and
			there is special methos to deal with the pulse one. */
		class Voice final : public MCHEmul::SoundVoice
		{
			public:
			Voice (int id, unsigned int cF)
				: MCHEmul::SoundVoice (id, cF,
					{
						new MCHEmul::SawSmoothSoundWave (cF),
						new MCHEmul::TriangleSoundWave (cF),
						new MCHEmul::PulseSoundWave (cF),
						new MCHEmul::NoiseSoundWave (cF)
					})
							{ }

			/** To control the percentage of the pulse wave when active. */
			double pulseUpPercentage () const
							{ return (dynamic_cast <MCHEmul::PulseSoundWave*> (waves ()[2]) -> pulseUpPercentage ()); }
			void setPulseUpPercentage (double pU)
							{ dynamic_cast <MCHEmul::PulseSoundWave*> (waves ()[2]) -> setPulseUpPercentage (pU); }
		};

		/** The different voices used by SID. \n
			They will be three defined at construction time. */
		MCHEmul::SoundVoices _voices;

		/** The registers used by the SID. */
		std::vector <MCHEmul::UByte> _registers;

		// Implementation
		unsigned int _lastClockCycles;
		/** Number of cycles per sample. */
		unsigned int _clocksPerSample;
		/** Counter from 0 to _clockPerSample. */
		unsigned int _counterClocksPerSample;

		static unsigned short _ATTACKTIMES [0x10];  // In milliseconds
		static unsigned short _DECAYTIMES [0x10];   // In milliseconds
		static unsigned short _RELEASETIMES [0x10]; // In milliseconds
	};
}

#endif
  
// End of the file
/*@}*/
