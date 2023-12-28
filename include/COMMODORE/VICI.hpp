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
	class VICI : public MCHEmul::GraphicalChip
	{
		public:
		static const unsigned int _ID = 106;

		// 44,1MHz (more or less standard in current sound cards)
		static const unsigned int _SOUNDSAMPLINGCLOCK		= 44100;
		// 8 bits sound data, very simple nothing complicated...
		static const unsigned short _SOUNDSAMPLINGFORMAT	= AUDIO_U8;
		// Number of channels..
		static const unsigned char _SOUNDCHANNELS			= 1;

		/** Specific classes for PAL & NTSC have been created giving this data as default. \n
			The VICI constructor receives info over the raster data, the memory view to use,
			The number of cycles of every raster line (different depending on the VICII version) 
			and additional attributes. */
		VICI (const MCHEmul::RasterData& vd, const MCHEmul::RasterData& hd, 
			int vV, unsigned short cRL, MCHEmul::SoundLibWrapper* sW, const MCHEmul::Attributes& attrs = { });

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

		// Draw the graphics detail...
		/** To simplify the use of some of the routines dedicated to draw graphics. */
		struct DrawContext
		{
			unsigned short _ICD;	// Initial Column of the Display (Not taken into account reductions in size).
			unsigned short _ICS;	// Initial Column Screen (Screen = Display with reductions in the size considered).
			unsigned short _LCD;	// Last Column of the Display 
			unsigned short _LCS;	// Last Column of the Screen
			unsigned short _SC;		// Scroll X
			unsigned short _RC;		// Raster X (from the beginning of the visible zone)
			unsigned short _RCA;	// Raster X adjusted (Moves 8 by 8, so = Raster X >> 3 << 3)
			unsigned short _IRD;	// Initial Row Display (Not taken into account reductions of the size)
			unsigned short _IRS;	// Initial Row Screen (Screen = Display with reductions of size taken into account).
			unsigned short _LRD;	// Last Row of the Display 
			unsigned short _LRS;	// Last Row of the Screen
			unsigned short _SR;		// Scroll Y
			unsigned short _RR;		// Raster Y (From the beginning of the visible zone.
									// Moves 1 by 1. No adjusted needed)
		};

		/** To simplify the way the result of a drawing text/bitmaps routines are managed. \n
			Any time a draw routine runs, 8 bits of info are calculated. \n
			These bits can be either foreground or background and their color are kept in 
			their respective variable _colorData. */
		struct DrawResult
		{
			/** The color of the each pixel considered as background. */
			unsigned int _backgroundColorData [8];
			/** The color of the each pixel considered as foreground. */
			unsigned int _foregroundColorData [8];

			DrawResult (unsigned int bC = ~0 /** Meaning transparent (jumped later when moving info to screen). */)
				: _backgroundColorData { bC, bC, bC, bC, bC, bC, bC, bC },
				  _foregroundColorData { bC, bC, bC, bC, bC, bC, bC, bC }
							{ }
		};

		// Invoked from the method "simulation"....
		/** Different actions are taken attending the raster cycle. \n
			Returns the number of cycles that, as a consequence of dealing with a raster line, 
			the CPU should be stopped. \n
			The way the raster cycles are treated will depend (somehow) on the type of VICI chip. */
		virtual unsigned int treatRasterCycle ();
		/** Treat the visible zone.
			Draws the graphics and finally draw the border. */
		void drawVisibleZone (MCHEmul::CPU* cpu);

		// These all methods are invoked from the three ones above!
		// They are here just to structure better the code...
		// Read screen data
		// Methods linked to the raster line to the read the graphics...
		/** To read the video matrix and the RAM color. \n
			Someting that happens during a badline. */
		inline void readVideoMatrixAndColorRAM ();
		/** To read the graphical info, considerig the info read in the previous method. \n
			This method is executed per raster cycles. */
		inline void readGraphicalInfo ();

		/** To draw any type of graphic \n
			This method uses the ones below it \n
			The method receives: \n
			dC = other info about the raster used to finally draw. \n
			They all return a instance of DrawResult (@see above). */
		DrawResult drawGraphics (const DrawContext& dC);

		// The last part...
		/** To move the graphics drawn to the screen. \n
			The info move is the text/bitmap info that has been already draw to the screen. */
		void drawResultToScreen (const DrawResult& cT, const DrawContext& dC);

		protected:
		/** VICI is both a Graphical and a Sound Chip. \n
			VICI should then inherit from both MCHEmul::GraphicalCip & MCHEmul::SoundChip,
			but this could generate ambiguity when calling some parent methods, that will be the case. \n
			The main function of the VICI is, however, the graphical one. \n
			VICI will then inherit from MCHEmul::GraphicalChip only and it will use an object inside inheriting from MCHEmul::SoundChip. */
		class SoundFunction final : public MCHEmul::SoundChip
		{
			public:
			static const unsigned int _ID = 1060;

			SoundFunction (MCHEmul::SoundLibWrapper* sW);

			/** The main characteristics of the SoundChip. */
			virtual SDL_AudioFormat type () const override
								{ return (_SOUNDSAMPLINGFORMAT); }
			virtual int maxFrequency () const override
								{ return (_SOUNDSAMPLINGCLOCK >> 1); }
			virtual unsigned char numberChannels () const override
								{ return (_SOUNDCHANNELS); }

			virtual bool initialize () override;

			virtual bool simulate (MCHEmul::CPU* cpu) override;

			virtual MCHEmul::InfoStructure getInfoStructure () const override;

			// Implementation
			/** The number of cycles the CPU was executed once the simulated method finishes. */
			unsigned int _lastCPUCycles;
		};

		/** A reference to the sound part of the chip. */
		SoundFunction* _soundFunction;
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
		/** Whether the vertical raster has entered the last VBlank zone already. */
		bool _lastVBlankEntered;
	};

	// ---
	inline void VICI::readVideoMatrixAndColorRAM ()
	{
		// TODO
	}

	// ---
	inline void VICI::readGraphicalInfo ()
	{
		// TODO
	}

	/** The version para NTSC systems. */
	class VICI_NTSC final : public VICI
	{
		public:
		static const MCHEmul::RasterData _VRASTERDATA;
		static const MCHEmul::RasterData _HRASTERDATA;

		VICI_NTSC (int vV, MCHEmul::SoundLibWrapper* wS);

		private:
		virtual unsigned int treatRasterCycle () override;
	};

	/** The version para PAL systems. */
	class VICI_PAL final : public VICI
	{
		public:
		static const MCHEmul::RasterData _VRASTERDATA;
		static const MCHEmul::RasterData _HRASTERDATA;

		static constexpr unsigned short _CYCLESPERRASTERLINE = 65;

		VICI_PAL (int vV, MCHEmul::SoundLibWrapper* wS);

		private:
		virtual unsigned int treatRasterCycle () override;
	};
}

#endif
  
// End of the file
/*@}*/
