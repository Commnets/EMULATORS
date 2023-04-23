/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: SID.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 13/01/2023 \n
 *	Description: SID Chip Emulation.
 *				 The emulation is created using the RESID library:
 *				 http://www.zimmers.net/anonftp/pub/cbm/crossplatform/emulators/resid/index.html
 *				 This library has been integrated in many emulators like VICE (my reference).
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_SID__
#define __COMMODORE_SID__

#include <CORE/incs.hpp>
#include <COMMODORE/SIDRegisters.hpp>
#include <resid/sid.h>

namespace COMMODORE
{
	/** A wrapper over the RESID Library. */
	class SoundRESIDWrapper final : public MCHEmul::SoundLibWrapper
	{
		public:
		SoundRESIDWrapper (double cF, RESID::sampling_method sM, double sF)
			: MCHEmul::SoundLibWrapper (),
			  _resid_sid (),
			  _data (MCHEmul::UBytes ({ 0 }))
							{ _resid_sid.set_sampling_parameters (cF, sM, sF); }

		virtual void initialize () override
							{ _resid_sid.reset (); }

		virtual MCHEmul::UBytes getData (MCHEmul::CPU *cpu) override;

		private:
		RESID::SID _resid_sid;

		// Implementatation
		/** The last data returned. */
		MCHEmul::UBytes _data;
	};

	/** A wrapper just to test the sound is working. */
	class SoundPulseWrapper final : public MCHEmul::SoundLibWrapper
	{
		public:
		SoundPulseWrapper (int mC = 10, char pV = 100 /** by default. */)
			: MCHEmul::SoundLibWrapper (),
			  _maxCounter (mC),
			  _pulseValue (pV),
			  _status (0), _counter (0), _data ()
							{ }

		virtual void setParameters (const MCHEmul::Attributes& attrs) override;

		virtual MCHEmul::UBytes getData (MCHEmul::CPU *cpu) override;

		private:
		int _maxCounter;
		char _pulseValue;

		// Implementation
		/** = 0 no data, = 1 pulse up, = 2 pulse down. */
		int _status;
		/** Just to count up to _maxCounter value. */
		int _counter;
		/** Thelast Ubyte returned. */
		MCHEmul::UBytes _data;
	};

	/** The chip is to produce the sound. */
	class SID : public MCHEmul::SoundChip
	{
		public:
		static const unsigned int _ID = 105;

		/** The parameter is the clock speed. */
		SID (unsigned int cF);

		/** The main characteristics of the SID. */
		virtual SDL_AudioFormat type () const override
							{ return (AUDIO_S8); } // resid represents the information as a unsigned short with the MSB at the left
		virtual int maxFrequency () const override
							{ return (44100); } // The max frequecy that the SID can play is 4000Hz
		virtual unsigned char numberChannels () const override
							{ return (1); } // SID is mono...

		virtual bool initialize () override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		SIDRegisters* _SIDRegisters;

		// Implementation
		/** A reference to the SID implementation of the resid library. */
		RESID::SID _resid_sid;
		mutable unsigned int _lastClockCycles;
	};
}

#endif
  
// End of the file
/*@}*/
