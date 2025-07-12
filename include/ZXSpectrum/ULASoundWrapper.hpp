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

		/** The ULA voice can adapt three different types of waves,
			depending on the voice. */
		class Voice final : public MCHEmul::SoundVoice
		{
			public:
			friend ULASoundSimpleLibWrapper;

			Voice (int id, unsigned int cF);

			/** When active, the internal counters are reactived. */
			virtual void setActive (bool a) override
							{ if ((_active != a) && (_active = a)) 
								waves ()[0] -> initializeInternalCounters (); } // There is only one wave, remember!

			/** To get the data of the voice. */
			virtual double data () const override;
		};

		// There is only one voice of this type used by ULA!
		Voice _voice;

		// Implementation
		/** The number of clocks that a sample takes in the sound. */
		unsigned int _clocksPerSample;
		/** Counter from 0 to _clockPerSample. */
		unsigned int _counterClocksPerSample;
	};
}

#endif
  
// End of the file
/*@}*/
