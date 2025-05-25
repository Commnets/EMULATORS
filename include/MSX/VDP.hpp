/** \ingroup MSX */
/*@{*/

/**	
 *	@file	
 *	File: VDP.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 29/03/2025 \n
 *	Description: VDP is an abstraction over a concrete video chip.
 *				 The real chip will depend on the generation.
 *	Versions: 1.0 Initial
 */

#ifndef __MSX_VDP__
#define __MSX_VDP__

#include <CORE/incs.hpp>
#include <TEXASINSTRUMENTS/incs.hpp>

namespace MSX
{
	/** The base class. \n
		It is only wrappes a GraphicChip.
		What the chip is really about will depend on the implementation of VDP. */
	class VDP
	{
		public:
		VDP (MCHEmul::GraphicalChip* gC);

		/** VDP is not the owner of the chip received, 
			but a just in case a destructor is needed in any class inheriting this one. */
		virtual ~VDP ()
							{  }

		/** To get the id. */
		int id () const
							{ return (_graphicalChip -> id ()); }

		/** To get the graphical video behind. */
		const MCHEmul::GraphicalChip* graphicalChip () const
							{ return (_graphicalChip); }
		MCHEmul::GraphicalChip* graphicalChip ()
							{ return (_graphicalChip); }

		/** Size & position of the screen. 
			The specific graphical chip will implemented it in one way or another. */
		virtual unsigned short numberColumns () const  = 0;
		virtual unsigned short numberRows () const = 0;
		virtual void screenPositions (unsigned short& x1, unsigned short& y1, 
			unsigned short& x2, unsigned short& y2) = 0;

		// Manage the internal registers of the VDP
		/** To get & set the registers of the chip behind. */
		virtual MCHEmul::UByte readRegister (unsigned char rId) const = 0;
		virtual MCHEmul::UByte peekRegister (unsigned char rId) const = 0;
		virtual void setRegister (unsigned char rId, const MCHEmul::UByte& v) = 0;

		// Manages the internal video memory
		// Relies in the behaviour, because it defines the real generation of the VDP/MSX...
		// Take care because no boundaries checks are done...
		virtual size_t videoMemorySize () const = 0;
		virtual const std::vector <MCHEmul::UByte>& videoMemory () const = 0;
		virtual const MCHEmul::UByte& videoData (const MCHEmul::Address& pos) const = 0;
		virtual std::vector <MCHEmul::UByte> videoData (const MCHEmul::Address& pos, size_t nB) const = 0;
		virtual void setVideoData (const MCHEmul::Address& pos, const MCHEmul::UByte& v) = 0;
		virtual unsigned char graphicMode () const = 0;

		virtual std::vector <MCHEmul::UByte> patternNameTableSnapShot () const = 0;
		virtual std::vector <MCHEmul::UByte> patternGenerationTableSnapShot () const = 0;
		virtual std::vector <MCHEmul::UByte> colorNameTableSnapShot () const = 0;
		virtual MCHEmul::Strings spriteDrawSnapShot (size_t nS) const = 0;
		virtual MCHEmul::Strings spritesDrawSnapShot (const std::vector <size_t>& nS) const = 0;

		/** To activate or desactivate the visualization of events. 
			This is something accesed from outside and the specific implementation
			will depend on the real implementation of the chip. */
		virtual void setShowEvents (bool sE) = 0;

		/** To get information about the specific chip behind the VDP. */
		MCHEmul::InfoStructure getInfoStructure () const;

		protected:
		MCHEmul::GraphicalChip* _graphicalChip;
	};

	/** The wrapper over the TEXAS INSTRUMENT TMS99xxFamily. */
	class VDP_TMS99xxFamily final : public VDP
	{
		public:
		VDP_TMS99xxFamily (TEXASINSTRUMENTS::TMS99xxFamily* gC)
			: VDP (gC),
			  _TMS99xxFamily (gC)
							{ /** Nothing else to do. */ }

		virtual unsigned short numberColumns () const override
							{ return (_TMS99xxFamily -> raster ().visibleColumns ()); }
		virtual unsigned short numberRows () const override
							{ return (_TMS99xxFamily -> raster ().visibleLines ()); }
		/** Always within the visible screen. */
		virtual void screenPositions (unsigned short& x1, unsigned short& y1, 
			unsigned short& x2, unsigned short& y2) override
							{ _TMS99xxFamily -> raster ().displayPositions (x1, y1, x2, y2); }

		virtual MCHEmul::UByte readRegister (unsigned char rId) const override
							{ return (_TMS99xxFamily -> readRegister (rId)); }
		virtual MCHEmul::UByte peekRegister (unsigned char rId) const override
							{ return (_TMS99xxFamily -> peekRegister (rId)); }
		virtual void setRegister (unsigned char rId, const MCHEmul::UByte& v) override
							{ _TMS99xxFamily -> setRegister (rId, v); }

		virtual size_t videoMemorySize () const override
							{ return (_TMS99xxFamily -> videoMemorySize ()); }
		virtual const std::vector <MCHEmul::UByte>& videoMemory () const override
							{ return (_TMS99xxFamily -> videoMemory ()); }
		virtual const MCHEmul::UByte& videoData (const MCHEmul::Address& pos) const override
							{ return (_TMS99xxFamily -> videoData (pos)); }
		virtual std::vector <MCHEmul::UByte> videoData (const MCHEmul::Address& pos, size_t nB) const override
							{ return (_TMS99xxFamily -> videoData (pos, nB)); }
		virtual void setVideoData (const MCHEmul::Address& pos, const MCHEmul::UByte& v) override
							{ _TMS99xxFamily -> setVideoData (pos, v); }
		virtual unsigned char graphicMode () const override
							{ return (_TMS99xxFamily -> graphicMode ()); }

		virtual std::vector <MCHEmul::UByte> patternNameTableSnapShot () const override
							{ return (std::move (_TMS99xxFamily -> patternNameTableSnapShot ())); }
		virtual std::vector <MCHEmul::UByte> patternGenerationTableSnapShot () const override
							{ return (std::move (_TMS99xxFamily -> patternGenerationTableSnapShot ())); }
		virtual std::vector <MCHEmul::UByte> colorNameTableSnapShot () const override
							{ return (std::move (_TMS99xxFamily -> colorNameTableSnapShot ())); }
		virtual MCHEmul::Strings spriteDrawSnapShot (size_t nS) const override
							{ return (std::move (_TMS99xxFamily -> spriteDrawSnapShot (nS))); }
		virtual MCHEmul::Strings spritesDrawSnapShot (const std::vector <size_t>& nS) const override
							{ return (std::move (_TMS99xxFamily -> spritesDrawSnapShot (nS))); }

		virtual void setShowEvents (bool sE) override
							{ _TMS99xxFamily -> setShowEvents (sE); }

		private:
		TEXASINSTRUMENTS::TMS99xxFamily* _TMS99xxFamily;
	};
}

#endif
  
// End of the file
/*@}*/
