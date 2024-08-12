/** \ingroup ZXSPECTRUM */
/*@{*/

/**	
 *	@file	
 *	File: ULA.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 09/08/2024 \n
 *	Description: The "ULA" Chip.
 *				 This chip is used to produce the video signal, the sound, and also to read the keyboard.
 *	Versions: 1.0 Initial
 *	Based on: https://pdfcoffee.com/the-zx-spectrum-ula-how-to-design-a-micro-chris-smith-pdf-free.html
 */

#ifndef __ZXSPECTRUM_ULA__
#define __ZXSPECTRUM_ULA__

#include <CORE/incs.hpp>
#include <ZXSpectrum/ULARegisters.hpp>

namespace ZXSPECTRUM
{
	/** The ULA is loaded with a 7MHz clock. \n
		In the PAL/NTSC mode, the horizontal raster takes 64us to go through a line, which is 448 7MH clock cycles. \n
		In the PAL mode, the vertical raster go through 312 lines to produce images 50 per second, 
		whilst in NTSC does 264 lines to produce images 60 per second. \n
		The ULA generates the clock for the CPU reducing the speed half, so the CPU is loaded with a 3.5 MHz clock. */
	class ULA : public MCHEmul::GraphicalChip
	{
		public:
		static const unsigned int _ID = 210;

		/** Specific classes for PAL & NTSC have been created giving this data as default. \n
			The ULA constructor receives info over the raster data, the memory view to use,
			The number of cycles of every raster line (different depending on the ULA version),
			a reference to the portFE that is used to read the keyboard,
			and additional attributes. */
		ULA (const MCHEmul::RasterData& vd, const MCHEmul::RasterData& hd, 
			int vV, const MCHEmul::Attributes& attrs = { });

		virtual ~ULA () override;

		/** To get a reference to the ULARegisters. */
		const ULARegisters* registers () const
							{ return (_ULARegisters); }
		ULARegisters* registers ()
							{ return (_ULARegisters); }

		// Size & position of the scren...
		virtual unsigned short numberColumns () const override
							{ return (_raster.visibleColumns ()); }
		virtual unsigned short numberRows () const override
							{ return (_raster.visibleLines ()); }
		/** Always with in the visible screen. */
		inline void screenPositions (unsigned short& x1, unsigned short& y1, 
			unsigned short& x2, unsigned short& y2);

		/** To get the raster info. */
		const MCHEmul::Raster& raster () const
							{ return (_raster); }
		MCHEmul::Raster& raster ()
							{ return (_raster); }

		/** To activate or desactivate the visualization of events. */
		void setShowEvents (bool sE)
							{ _showEvents = sE; }

		virtual bool initialize () override;

		/** Simulates cycles in the ULA. */
		virtual bool simulate (MCHEmul::CPU* cpu) override;

		/**
		  *	The name of the fields are: \n
		  * ULARegisters	= InfoStructire: Info about the registers.
		  * Raster			= InfoStructure: Info about the raster.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		/** Invoked from initialize to create the right screen memory. */
		virtual MCHEmul::ScreenMemory* createScreenMemory () override;

		// Invoked from the method "simulation"...
		/** Read and draw the graphics. */
		void readGraphicsAndDrawVisibleZone (MCHEmul::CPU* cpu);

		protected:
		/** A reference to the ULA registers. */
		ULARegisters* _ULARegisters;
		/** The number of the memory view used to read the data. */
		int _ULAView;
		/** The raster. */
		MCHEmul::Raster _raster;
		/** To show or no the main events that affects the visualization. */
		bool _showEvents;

		// Implementation
		/** The number of cycles the CPU was executed once the simulated method finishes. */
		unsigned int _lastCPUCycles;
		/** The format used to draw. 
			It has to be the same that is used by the Screen object. */
		SDL_PixelFormat* _format;
		/** Whether the vertical raster has entered the first VBlank zone already. */
		bool _firstVBlankEntered;
	};

	// ---
	inline void ULA::screenPositions (unsigned short& x1, unsigned short& y1, 
		unsigned short& x2, unsigned short& y2)
	{
		_raster.displayPositions (x1, y1, x2, y2);
	}

	/** The version para PAL systems. */
	class ULA_PAL final : public ULA
	{
		public:
		static const MCHEmul::RasterData _VRASTERDATA;
		static const MCHEmul::RasterData _HRASTERDATA;

		ULA_PAL (int vV);
	};

	/** The version para NTSC systems. */
	class ULA_NTSC final : public ULA
	{
		public:
		static const MCHEmul::RasterData _VRASTERDATA;
		static const MCHEmul::RasterData _HRASTERDATA;

		ULA_NTSC (int vV);
	};
}

#endif
  
// End of the file
/*@}*/
