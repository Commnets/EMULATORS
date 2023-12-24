/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: VICI.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 16/12/2023 \n
 *	Description: The VICI Chip.
 *	Versions: 1.0 Initial
 *	Based on http://tinyvga.com/6561
 */

#ifndef __COMMODORE_VICI__
#define __COMMODORE_VICI__

#include <CORE/incs.hpp>
#include <COMMODORE/VICIRegisters.hpp>

namespace COMMODORE
{
	/** The chip that takes care of anything around the graphics in computers like VIC20. 
		@see GraphicalChip and @see SoundChip */
	class VICI : public MCHEmul::GraphicalChip, public MCHEmul::SoundChip
	{
		public:
		static const unsigned int _ID = 106;

		/** Specific classes for PAL & NTSC have been created giving this data as default. \n
			The VICI constructor receives info over the raster data, the memory view to use,
			The number of cycles of every raster line (different depending on the VICII version) 
			and additional attributes. */
		VICI (const MCHEmul::RasterData& vd, const MCHEmul::RasterData& hd, 
			int vV, unsigned short cRL, const MCHEmul::Attributes& attrs = { });

		virtual ~VICI () override;

		virtual unsigned short numberColumns () const override
							{ return (_raster.visibleColumns ()); }
		virtual unsigned short numberRows () const override
							{ return (_raster.visibleLines ()); }

		/** To get the number of cycles per raster line used in this chip. */
		unsigned short cyclesPerRasterLine () const
							{ return (_cyclesPerRasterLine); }

		/** To get the raster info. */
		const MCHEmul::Raster& raster () const
							{ return (_raster); }

		/** To set the position of the light - pen. \n
			The position received must be relative within the display zone. */
		void lightPenPosition (unsigned short& x, unsigned short& y) const
							{ _VICIRegisters -> currentLightPenPosition (x, y); }
		void setLightPenPosition (unsigned short x, unsigned short y)
							{ _VICIRegisters -> setCurrentLightPenPosition (x, y); }

		/** To know whether the light pen is active. */
		bool lightPenActive () const
							{ return (_VICIRegisters -> lightPenActive ()); }
		void setLightPenActive (bool lP)
							{ _VICIRegisters -> setLigthPenActive (lP); }

		virtual bool initialize () override;

		/** Simulates cycles in the VICI. */
		virtual bool simulate (MCHEmul::CPU* cpu) override;

		/**
		  *	The name of the fields are: \n
		  * VICIIRegisters	= InfoStructure: Info about the registers.
		  * Raster			= InfoStructure: Info about the raster.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		/** Invoked from initialize to create the right screen memory. */
		virtual MCHEmul::ScreenMemory* createScreenMemory () override;

		protected:
		/** The memory is used also as the set of registers of the chip. */
		COMMODORE::VICIRegisters* _VICIRegisters;
		/** The number of the memory view used to read the data. */
		int _VICIView;
		/** The number of cycles per raster line as it depends on the type of Chip. */
		unsigned short _cyclesPerRasterLine;
		/** The difference with the PAL System. */
		unsigned short _incCyclesPerRasterLine;
		/** The raster. */
		MCHEmul::Raster _raster;

		// Implementation
		/** The number of cycles the CPU was executed once the simulated method finishes. */
		unsigned int _lastCPUCycles;
		/** The format used to draw. 
			It has to be the same that is used by the Screen object. */
		SDL_PixelFormat* _format;
		/** When a raster line is processed, it is necessary to know which cycle is being processed. 
			The number of max cycles is get from the method (@see) "cyclesPerRasterLine". */
		unsigned short _cycleInRasterLine;
	};

	/** The version para NTSC systems. */
	class VICI_NTSC final : public VICI
	{
		public:
		static const MCHEmul::RasterData _VRASTERDATA;
		static const MCHEmul::RasterData _HRASTERDATA;

		VICI_NTSC (int vV);
	};

	/** The version para PAL systems. */
	class VICI_PAL final : public VICI
	{
		public:
		static const MCHEmul::RasterData _VRASTERDATA;
		static const MCHEmul::RasterData _HRASTERDATA;

		static constexpr unsigned short _CYCLESPERRASTERLINE = 65;

		VICI_PAL (int vV);
	};
}

#endif
  
// End of the file
/*@}*/
