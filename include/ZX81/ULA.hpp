/** \ingroup ZX81 */
/*@{*/

/**	
 *	@file	
 *	File: ULA.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 25/03/2024 \n
 *	Description: The "ULA" Chip.
 *				 This chip is used to produce the signal video and to read the keyboard.
 *	Versions: 1.0 Initial
 *	Based on https://8bit-museum.de/heimcomputer-2/sinclair/sinclair-scans/scans-zx81-video-display-system/
 */

#ifndef __ZX81_ULA__
#define __ZX81_ULA__

#include <CORE/incs.hpp>

namespace ZX81
{
	class SinclairZX81;
	class PortFE;

	/** The chip that takes care of anything around the graphics in ZX81.
		It is also accountable for reading the status of the keyboard.
		@see GraphicalChip. */
	class ULA : public MCHEmul::GraphicalChip
	{
		public:
		friend SinclairZX81;

		static const unsigned int _ID = 106;

		/** Specific classes for PAL & NTSC have been created giving this data as default. \n
			The ULA constructor receives info over the raster data, the memory view to use,
			The number of cycles of every raster line (different depending on the ULA version),
			a reference to the portFE that is used to read the keyboard,
			and additional attributes. */
		ULA (const MCHEmul::RasterData& vd, const MCHEmul::RasterData& hd, 
			int vV, unsigned short cRL, const MCHEmul::Attributes& attrs = { });

		virtual ~ULA () override;

		virtual unsigned short numberColumns () const override
							{ return (_raster.visibleColumns ()); }
		virtual unsigned short numberRows () const override
							{ return (_raster.visibleLines ()); }
		/** Always with in the visible screen. */
		inline void screenPositions (short& x1, short& y1, short& x2, short& y2);

		/** To get the number of cycles per raster line used in this chip. */
		unsigned short cyclesPerRasterLine () const
							{ return (_cyclesPerRasterLine); }

		/** To get the raster info. */
		const MCHEmul::Raster& raster () const
							{ return (_raster); }

		virtual bool initialize () override;

		/** Simulates cycles in the VICI. */
		virtual bool simulate (MCHEmul::CPU* cpu) override;

		/**
		  *	The name of the fields are: \n
		  * Raster			= InfoStructure: Info about the raster.
		  * Sound			= InfoStructire: Info about the sound.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		/** Invoked from initialize to create the right screen memory. */
		virtual MCHEmul::ScreenMemory* createScreenMemory () override;

		// Implementation
		/** invoked from SinclairZX81 class. */
		void linkToPortFE (PortFE* pFE) // Nothing is checked...
							{ _portFE = pFE; }

		protected:
		/** A reference to the port FE. */
		PortFE* _portFE;
		/** The number of the memory view used to read the data. */
		int _ULAView;
		/** The number of cycles per raster line as it depends on the type of Chip. */
		unsigned short _cyclesPerRasterLine;
		/** The raster. */
		MCHEmul::Raster _raster;

		// Implementation
		/** The number of cycles the CPU was executed once the simulated method finishes. */
		unsigned int _lastCPUCycles;
		/** The format used to draw. 
			It has to be the same that is used by the Screen object. */
		SDL_PixelFormat* _format;
	};

	// ---
	inline void ULA::screenPositions (short& x1, short& y1, short& x2, short& y2)
	{
		// TODO

		x1 = 0; y1 = 0; x2 = 0; y2 = 0;
	}

	/** The version para PAL systems. */
	class ULA_PAL final : public ULA
	{
		public:
		static const MCHEmul::RasterData _VRASTERDATA;
		static const MCHEmul::RasterData _HRASTERDATA;

		static constexpr unsigned short _CYCLESPERRASTERLINE = 71;

		ULA_PAL ();
	};

	/** The version para NTSC systems. */
	class ULA_NTSC final : public ULA
	{
		public:
		static const MCHEmul::RasterData _VRASTERDATA;
		static const MCHEmul::RasterData _HRASTERDATA;

		static constexpr unsigned short _CYCLESPERRASTERLINE = 65;

		ULA_NTSC ();
	};
}

#endif
  
// End of the file
/*@}*/
