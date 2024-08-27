/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: SID.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 13/01/2023 \n
 *	Description: SID Chip Emulation.
 *				 The emulation is created using the RESID library:
 *				 http://www.zimmers.net/anonftp/pub/cbm/crossplatform/emulators/resid/index.html
 *				 This library has been integrated in many emulators like VICE (my reference).
 *	Versions: 1.0 Initial
 *  \n
 *	In the SID there are two pins (POTX and POTY, numbers 24 and 23 as per the official documentation says) that 
 *	convert an analogic wave comming from, usually, two "potenciometers" into a digital value between 0 and 255. \n
 *	The pair of potenciometers are simulated in MCHEmul using the 2 main axis of a joystick (axis X and axis Y). \n
 *  The simulation has been built to allow 2 different pairs (two joysticks) 
 *	of potenciometers (once at a time) to be connected to the SID. @see SIDRegisters to further info.
 */

#ifndef __COMMODORE_SID__
#define __COMMODORE_SID__

#include <CORE/incs.hpp>
#include <COMMODORE/SID/SIDRegisters.hpp>

namespace COMMODORE
{
	/** The chip is to produce the sound. */
	class SID : public MCHEmul::SoundChip
	{
		public:
		static const unsigned int _ID = 105;

		// 44,1MHz (more or less standard in current sound cards)
		static const unsigned int _SOUNDSAMPLINGCLOCK		= 44100;
		// 8 bits sound data, very simple nothing complicated...
		static const unsigned short _SOUNDSAMPLINGFORMAT	= AUDIO_U8;
		// Number of channels..
		static const unsigned char _SOUNDCHANNELS			= 1;

		/** The parameter is the clock speed. */
		SID (unsigned int cF, MCHEmul::SoundLibWrapper* sW);

		// Managing the source of the info for potenciomaters
		// POTX and POT (@see 
		size_t potenciometerGroupActive () const
							{ return (_SIDRegisters -> potenciometerGroupActive ()); }
		/** Invoking with no parameter would select none. 
			No boundaries check are done, but nG must be 0 or 1. */
		void setPotenciometerGroupActive (size_t nG = MCHEmul::_S0 /** None by default. */)
							{ _SIDRegisters -> setPotenciometerGroupActive (nG); }

		/** The main characteristics of the SID. */
		virtual SDL_AudioFormat type () const override
							{ return (_SOUNDSAMPLINGFORMAT); }
		virtual int maxFrequency () const override
							{ return (_SOUNDSAMPLINGCLOCK >> 1); }
		virtual unsigned char numberChannels () const override
							{ return (_SOUNDCHANNELS); }

		virtual bool initialize () override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		/**
		  *	The name of the fields are: \n
		  *	Registers		= Info of the SID registers (memory).
		  *	SoundLibWrapper	= Info of the sound lib wrapper used.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		/** To read the position of the joysticks 
			that could be interpreted as info related with the paddles, if any!.
			How many paddles there can be will depend on how the SID has been built into the computer. */
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		private:
		SIDRegisters* _SIDRegisters;

		// Implementation
		/** A reference to the SID implementation of the resid library. */
		mutable unsigned int _lastClockCycles;
	};
}

#endif
  
// End of the file
/*@}*/
