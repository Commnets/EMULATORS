/** \ingroup MSX */
/*@{*/

/**	
 *	@file	
 *	File: PSG.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 27/04/2025 \n
 *	Description: PSG is an abstraction over a concrete sound chip.
 *				 The real chip might depend on the generation or on the implementation
 *	Versions: 1.0 Initial
 */

#ifndef __MSX_PSG__
#define __MSX_PSG__

#include <CORE/incs.hpp>
#include <GENERALINSTRUMENTS/incs.hpp>

namespace MSX
{
	/** The base class. \n
		It is only wrappes a SoundChip.
		What the chip is really about will depend on the implementation of PSG. */
	class PSG
	{
		public:
		PSG (MCHEmul::SoundChip* gC);

		/** PSG is not the owner of the chip received, 
			but a just in case a destructor is needed in any class inheriting this one. */
		virtual ~PSG ()
							{  }

		/** To get the id. */
		int id () const
							{ return (_soundChip -> id ()); }

		/** To get the sound chip behind. */
		const MCHEmul::SoundChip* soundChip () const
							{ return (_soundChip); }
		MCHEmul::SoundChip* soundChip ()
							{ return (_soundChip); }

		/** To get & set the registers of the chip behind. */
		virtual MCHEmul::UByte readRegister (unsigned char rId) const = 0;
		virtual MCHEmul::UByte peekRegister (unsigned char rId) const = 0;
		virtual void setRegister (unsigned char rId, const MCHEmul::UByte& v) = 0;

		/** To get information about the specific chip behind the PSG. */
		MCHEmul::InfoStructure getInfoStructure () const;

		protected:
		MCHEmul::SoundChip* _soundChip;
	};

	/** The wrapper over the GENERAL INSTRUMENT AY-3-8910. */
	class PSG_AY38910 final : public PSG
	{
		public:
		PSG_AY38910 (GENERALINSTRUMENTS::AY38910* sC)
			: PSG (sC),
			  _AY38910 (sC)
							{ /** Nothing else to do. */ }

		virtual MCHEmul::UByte readRegister (unsigned char rId) const override
							{ return (_AY38910 -> readRegister (rId)); }
		virtual MCHEmul::UByte peekRegister (unsigned char rId) const override
							{ return (_AY38910 -> peekRegister (rId)); }
		virtual void setRegister (unsigned char rId, const MCHEmul::UByte& v) override
							{ _AY38910 -> setRegister (rId, v); }

		private:
		GENERALINSTRUMENTS::AY38910* _AY38910;
	};
}

#endif
  
// End of the file
/*@}*/
