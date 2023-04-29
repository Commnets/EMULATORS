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
			  _voices (
				  { new Voice (0, cF), 
				    new Voice (1, cF), 
				    new Voice (2, cF) }),
			  _registers (std::vector <MCHEmul::UByte> (0x20, MCHEmul::UByte::_0)),
			  _lastClockCycles (0),
			  _clocksPerSample ((unsigned int) ((double) cF / (double) sF)), // sF can not be 0...
			  _counterClocksPerSample (0)
							{ }

		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;
		virtual const MCHEmul::UByte& readValue (size_t p) const override
							{ return (_lastValueRead = _registers [p % 0x20]); } // Every 20 registers the value is repeated...

		virtual void initialize () override;

		virtual bool getData (MCHEmul::CPU *cpu, MCHEmul::UBytes& dt) override;

		private:
		/** SID Is able to manage three different voices. */
		class Voice final
		{
			public:
			Voice (int id, unsigned int cF)
				: _id (id),
				  _active (false), // No active by default...
				  _volumen (0),
				  _waves ({ new MCHEmul::SawSmoothSoundWave (cF), // Wave number 0
							new MCHEmul::TriangleSoundWave (cF),  // Wave number 1
							new MCHEmul::PulseSoundWave (cF),     // Wave number 2
							new MCHEmul::NoiseSoundWave (cF) })   // Wave number 3 in the list of waves...
							{ }

			~Voice ()
							{ for (auto i : _waves) delete (i); }

			int id () const
							{ return (_id); }

			bool active () const
							{ return (_active); }
			void setActive (bool a)
							{ _active = a; }

			void setStart (bool s)
							{ for (auto i : _waves) i -> setStart (s); }
			
			const MCHEmul::SoundWaves& waves () const
							{ return (_waves); }
			const MCHEmul::SoundWave* wave (MCHEmul::SoundWave::Type t) const
							{ return (_waves [(int) t]); }
			MCHEmul::SoundWave* wave (MCHEmul::SoundWave::Type t)
							{ return ((MCHEmul::SoundWave*) (((const Voice*) (this)) -> wave (t))); }

			unsigned short frequency () const
							{ return (_waves [0] -> frequency ()); /** could be any, as all have the same value. */ }
			void setFrequency (unsigned short f)
							{ for (auto i : _waves) 
								i -> setFrequency (f); }

			unsigned short attack () const
							{ return (_waves [0] -> attack ()); /** could be any, as all have the same value. */ }
			void setAttack (unsigned short a)
							{ for (auto i : _waves) 
								i -> setAttack (a); }
			unsigned short decay () const
							{ return (_waves [0] -> decay ()); /** could be any, as all have the same value. */ }
			void setDecay (unsigned short d)
							{ for (auto i : _waves) 
								i -> setDecay (d); }
			unsigned short release () const
							{ return (_waves [0] -> release ()); /** could be any, as all have the same value. */ }
			void setRelease (unsigned short r)
							{ for (auto i : _waves) 
								i -> setRelease (r); }

			unsigned char sustainVolumen () const
							{ return (_waves [0] -> sustainVolumen ()); /** could be any, as all have the same value. */ }
			void setSustainVolumen (unsigned char s)
							{ for (auto i : _waves) 
								i -> setSustainVolumen (s); }

			unsigned short dutyCycle () const
							{ return (static_cast <MCHEmul::PulseSoundWave*> 
								(_waves [(int) MCHEmul::SoundWave::Type::_PULSE]) -> dutyCycle ()); }
			void setDutyCycle (unsigned short dC)
							{ static_cast <MCHEmul::PulseSoundWave*> 
								(_waves [(int) MCHEmul::SoundWave::Type::_PULSE]) -> setDutyCycle (dC); }

			unsigned char volumen () const
							{ return (_volumen); } // From 0 to 0x0f...
			void setVolumen (unsigned char v)
							{ _volumen = v; 
							  for (auto i : _waves) 
								i -> setMaximumVolumen (v); }

			void clock (unsigned int nC)
							{ for (auto i : _waves)
								i -> clock (nC); }

			/** The data of the voice comes from all active waves. */
			virtual double data () const
							{ double r = 0; 
							  for (auto i : _waves) 
								  r += i -> data (); 
							  return (r); }

			protected:
			/** The id of the voice. */
			int _id;
			/** When the voice is active. */
			bool _active;
			/** The volumen. */
			unsigned char _volumen;
			/** All possible waves. */
			MCHEmul::SoundWaves _waves;
		};

		std::vector <Voice*> _voices;
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
