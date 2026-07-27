/** \ingroup ZXSPECTRUM */
/*@{*/

/**	
 *	@file	
 *	File: ULASoundWrapper.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 05/07/2025 \n
 *	Description: ULA Sound Wrapper Emulation.
 *	Versions: 1.0 Initial
 */

#ifndef __ZXSPECTRUM_ULASOUNDWRAPPER__
#define __ZXSPECTRUM_ULASOUNDWRAPPER__

#include <CORE/incs.hpp>

namespace ZXSPECTRUM
{
	/** The generic LibWrapper for ULA in Spectrum. \n
		Nothing special to do. */
	class ULASoundLibWrapper : public MCHEmul::SoundLibWrapper
	{
		public:
		ULASoundLibWrapper (const MCHEmul::Attributes& attrs = { })
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
		  *	VOICES	= InfoStructure: Information about the ULA sound voice.
		  */
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
	class ULASoundSimpleLibWrapper final : public ULASoundLibWrapper
	{
		public:
		/** 
		  *	Constructor.
		  *	@param cF	Chip frequency in clocks / second.
		  * @param sF	Sampling frequency in samples / second. It cannot be 0.
		  */
		ULASoundSimpleLibWrapper (unsigned int cF, unsigned int sF);

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
						{ return ((nV == 0) ? _voice.getInfoStructure () : MCHEmul::InfoStructure ()); }

		private:
		unsigned int _chipFrequency;
		unsigned int _samplingFrequency;
		double _volumen;

		/** The ULA voice uses a plain wave controlled directly by the EAR output. */
		class Voice final : public MCHEmul::SoundVoice
		{
			public:
			friend ULASoundSimpleLibWrapper;

			Voice (int id, unsigned int cF);

			/** Enables or disables audible output for this voice. */
			virtual void setActive (bool a) override
							{ _active = a; }

			/** To get the normalized voice output between -1.0 and 1.0. */
			virtual double data () const override;
		};

		// There is only one voice of this type used by ULA!
		Voice _voice;

		// Implementation
		/** The number of cycles that a sample takes in the sound. */
		double _cyclesPerSample;
		/** Counter from 0 to _cyclesPerSample. */
		double _counterCyclesPerSample;
	};
}

#endif
  
// End of the file
/*@}*/
