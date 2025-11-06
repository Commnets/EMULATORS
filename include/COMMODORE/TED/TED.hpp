/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: TED.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 09/02/2024 \n
 *	Description: The TED Chip.
 *	Versions: 1.0 Initial
 *	Based on https://www.pagetable.com/docs/ted/TED%207360R0%20Preliminary%20Data%20Sheet.pdf.
 */

#ifndef __COMMODORE_TED__
#define __COMMODORE_TED__

#include <CORE/incs.hpp>
#include <COMMODORE/C6529B/C6529B.hpp>
#include <COMMODORE/TED/TEDTimer.hpp>
#include <COMMODORE/TED/TEDRegisters.hpp>

namespace COMMODORE
{
	/** The chip that takes care of anything around the graphics in computers like 
		Commodore 16 or Commodore Plus 4 when emulating 116.
		@see GraphicalChip. */
	class TED : public MCHEmul::GraphicalChip
	{
		public:
		/** TED is both a Graphical and a Sound Chip. \n
			TED should then inherit from both MCHEmul::GraphicalCip & MCHEmul::SoundChip,
			but this could generate ambiguity when calling some parent methods, that will be the case. \n
			The main function of the TED is, however, the graphical one. \n
			TED will then inherit from MCHEmul::GraphicalChip only and it will use an object inside inheriting from MCHEmul::SoundChip. */
		class SoundFunction final : public MCHEmul::SoundChip
		{
			public:
			static const unsigned int _ID = 1010;

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

		static const unsigned int _ID = 101;

		// 44,1MHz (more or less standard in current sound cards)
		static const unsigned int _SOUNDSAMPLINGCLOCK		= 44100;
		// 8 bits sound data, very simple nothing complicated...
		static const unsigned short _SOUNDSAMPLINGFORMAT	= AUDIO_U8;
		// Number of channels..
		static const unsigned char _SOUNDCHANNELS			= 1;

		/** The position of the bad lines. */
		static const unsigned short _FIRSTBADLINE	= 0x00;
		static const unsigned short _LASTBADLINE	= 0xc8;

		/** Data about the size of the screen */
		static const unsigned short _GRAPHMAXCHARLINES		= 25;	// Not taking into account reductions in the size
		static const unsigned short _GRAPHMAXCHARCOLUMNS	= 40;
		static const unsigned short _GRAPHMAXBITMAPCOLUMNS	= 320;	// Not taking into account double coulors
		static const unsigned short _GRAPHMAXBITMAPROWS		= 200;

		/** Specific classes for PAL & NTSC have been created giving this data as default. \n
			The TED constructor receives info over the raster data, the frequency it works, the memory view to use,
			The number of cycles of every raster line (different depending on the VICII version) 
			and additional attributes. */
		TED (int intId, unsigned short fq, 
			 const MCHEmul::RasterData& vd, const MCHEmul::RasterData& hd,
			 int vV, MCHEmul::SoundLibWrapper* sW, const MCHEmul::Attributes& attrs = { });

		virtual ~TED () override;

		/** Whether to draw other events related with the raster of VICII. 
			Borders, bad lines mainly. */
		void setDrawOtherEvents (bool d)
							{ _drawOtherEvents = d; }

		const SoundFunction* soundFunction () const
							{ return (_soundFunction); }
		SoundFunction* soundFunction ()
							{ return (_soundFunction); }

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

		virtual bool initialize () override;

		/** Simulates cycles in the TED. \n
			It draws the border AFTER once graphics info has been drawn within the display zone. \n
			So sprites can be drawn behing the border and collisions could take place out of the visible zone. */
		virtual bool simulate (MCHEmul::CPU* cpu) override;

		/**
		  *	The name of the fields are: \n
		  * VICIIRegisters	= InfoStructure: Info about the registers.
		  * Raster			= InfoStructure: Info about the raster.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		/** Invoked from initialize to create the right screen memory. \n
			It also creates the Palette used by a TED computer (_format variable). */
		virtual MCHEmul::ScreenMemory* createScreenMemory () override;

		// Draw the graphics in detail...
		/** To simplify the use of some of the routines dedicated to draw graphics. */
		struct DrawContext
		{
			unsigned short _ICD;	// Initial Column of the Display (Not taken into account reductions in size).
			unsigned short _SC;		// Scroll X
			unsigned short _RC;		// Raster X (from the beginning of the visible zone)
			unsigned short _RCA;	// Raster X adjusted (Moves 8 by 8, so = Raster X >> 3 << 3)
			unsigned short _RR;		// Raster Y (From the beginning of the visible zone.
									// Moves 1 by 1. No adjusted needed)
		};

		/** To simplify the way the result of a drawing text/bitmaps routines are managed. \n
			Any time a draw routine runs, 8 bits of info are calculated. \n
			These bits can be either foreground or background and their color are kept in 
			their respective variable _colorData. */
		struct DrawResult
		{
			/** The color of the each pixel considered as foreground. */
			unsigned int _foregroundColorData [8];
			/** To indicate whether it is or not consecuencia of a "bad mode",
				and the has to be everything draw in black, but taking into account the real graphics behaind. */
			bool _invalid;

			DrawResult (unsigned int bC = ~0 /** Meaning transparent (jumped later when moving info to screen). */)
				: _foregroundColorData { bC, bC, bC, bC, bC, bC, bC, bC },
				  _invalid (false)
							{ }
		};

		// Invoked from the method "simulation"....
		/** Different actions are taken attending the raster cycle. \n
			Returns the number of cycles that, as a consequence of dealing with a raster line, 
			the CPU should be stopped. \n
			The way the raster cycles are treated will depend (somehow) on the type of VICII chip. */
		virtual unsigned int treatRasterCycle ();
		/** Treat the visible zone.
			Draws the graphics, detect collions, and finally draw the border. */
		void drawVisibleZone (MCHEmul::CPU* cpu);
		/** Invoked from the previous one, just to draw. \n
		  *	The parameters are the context of the draw. \n
		  *	@see also DrawContext and DrawResult structures. */
		void drawGraphicsAndMoveToScreen (const DrawContext& dC);

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
		/**
		  *	Draws a monocolor char. \n
		  *	All methods receive: \n
		  *	cb	= window column adjusted by the scrollX value where to start to draw 8 pixels. \n
		  * Some of them receive: \n
		  * inv = to point aout that the method is invoked from an invalid graphic mode. \n
		  * The first method receives a flag to point out whether the cursor has or not to be drawn. 
		  */
		DrawResult drawMonoColorChar (int cb, bool c);
		/** Draws a multicolor char. \n
			The mode can be used also as an invalid mode. */
		DrawResult drawMultiColorChar (int cb, bool inv = false);
		/** Draws an enhaced multicolor char. */
		DrawResult drawMultiColorExtendedChar (int cb);
		/** Draws a monocolor bitmap. \n
			The mode can be used also as an invalid mode. */
		DrawResult drawMonoColorBitMap (int cb, bool inv = false);
		/** Draws a multicolor bitmap. \n
			The mode can be used as an invalid mode. */
		DrawResult drawMultiColorBitMap (int cb, bool inv = false);

		// The last part...
		/** To move the graphics drawn to the screen. \n
			The info move is the text/bitmap info that has been already draw to the screen. \n
			That text/bitmap info included the sprite data as well ordered. */
		void drawResultToScreen (const DrawResult& cT, const DrawContext& dC);

		// Very internal methods...
		/** When the bit 2 at register 0x12 (= 18) is active, 
			the fetch of dot or char data info has to be from ROM always. \n
			The way the method are implemented will depend on the place where the chip is running. */
		virtual bool ROMActiveToFetchCharData () const = 0;
		virtual void activeROMtoFecthCharData (bool a) = 0;

		// -----
		// Different debug methods to simplify the internal code
		// and to make simplier the modification in case it is needed...
		/** Debug special situations...
			Take care using this instructions _deepDebugFile could be == nullptr... */
		void debugTEDCycle (MCHEmul::CPU* cpu, unsigned int i);
		// -----

		protected:
		/** The type of interrupt launched from the TED. */
		int _interruptId;
		/** The frequency of the TED: */
		unsigned short _frequency;
		/** There three timers within the TED. */
		TEDTimer _T1, _T2, _T3;
		/** A reference to the sound part of the chip. */
		SoundFunction* _soundFunction;
		/** The memory is used also as the set of registers of the chip. */
		COMMODORE::TEDRegisters* _TEDRegisters;
		/** The number of the memory view used to read the data. */
		int _TEDView;
		/** The number of cycles per raster line as it depends on the type of Chip. */
		unsigned short _cyclesPerRasterLine;
		/** The raster. */
		MCHEmul::Raster _raster;
		/** To draw or not other different events. */
		bool _drawOtherEvents;

		// Implementation
		/** Internal counter that increments in 1 when the frame is drawn. 
			This internal counetr is used to determine whether the internal cursor must blink. 
			It reaches always just half of the frequency. */
		unsigned short _timesFrameDrawn;
		/** When the CPU is not stopped (sometimes the VIC requires to stop it). \n 
			and a instruction is executed, the number of cycles that that instruction required, has to be taken into account
			to define what the VICII has to do. */
		unsigned int _lastCPUCycles;
		/** The format used to draw. It has to be the same that is used by the Screen object. */
		SDL_PixelFormat* _format;
		/** When a raster line is processed, it is necessary to know which cycle is being processed. 
			The number of max cycles is get from the method (@see) "cyclesPerRasterLine". */
		unsigned short _cycleInRasterLine;
		/** When the raster is in the first bad line this variable is set taking into account what 
			is kept in the VICII register about whether the video ir or not active. \n
			In any other circunstance it kepts its value. */
		bool _videoActive;
		/** Whether the vertical raster has entered the last VBlank zone already. */
		bool _lastVBlankEntered;
		/** The last value of the SCROLLY variable when a bad line was detected.
			This value is gathered anytime the bad line condition is checked (usually between raster cycles 12 and 52). 
			-1 will mean no previous value to be taken into account. \n
			This variable is set to -1 at the beginning of every raster line. */
		mutable int _lastBadLineScrollY;
		/** When the situation of a new bad line araises, is latched in this variable. \n
			This variabe is desactivated at the end of the line, and when the graphical info is finally read. */
		mutable bool _newBadLineCondition;
		/** This very simple variable manages only when the additional stop bad line related cycles applies. */
		mutable bool _badLineStopCyclesAdded;

		/** 
		  *	Structure to control how the graphics are displayed in the screen. \n
		  *	https://www.cebix.net/VIC-Article.txt. (points 3.7.1 & 3.7.2): \n
		  *	The VICII can be in either "idle" state or "screen" state... \n
		  *	In the idle state c and g accesses (code and color matrix and graphics data) takes place one, 
		  *	Whilest in the second one only access to $3fff memory takes place
		  *	(or $39ff when ECM = multicolor bit in register $d016 is set). \n
		  *	The screen state is set as soon as a bad condition comes. \n
		  *	The idle state is set at cycle 58 if RC == 7 and ther eis no bad condition. \n
		  * There are a couple of important things that happen as the raster line moves accross the line. \n
		  *	The rules to manipulate every value are decribed below attending to the cycle in the raster. \n
		  * There are 3 important registers within the VICII related with the graphics: \n
		  * VCBASE moves from 0 to 1000 (40 * 25) in a 40 step length. \n
		  * VC moves from 0 to 1000 1 by 1. \n
		  * Both will help to determine the position within the video matrix/color RAM to read. \n
		  * RC counts from 0 to 7 one by one and determines the line the graphics data RAM where to find the final info. \n
		  * RASTER LINE 0:			VCBASE is set to 0. \n
		  * RASTER LINE $30 - $f7: \n
		  *		CYCLE 14:			VC = VCBASE. If Bad Line => RC = 0 \n
		  *		CYCLES 15 - 54:		VC = VC + 1 \n
		  *		CYCLE 58:			if RC == 7 => VCBASE = VC; RC = RC + 1 \n
		  * So VC goes from 0 to 40 7 times before VBASE incrementes in 40,
		  * then 7 times more from 40 to 80 before VCBASE moves to 80 and so on and so forth.
		  */
		struct TEDGraphicInfo
		{
			TEDGraphicInfo ()
				: _VCBASE (0), _VC (0), _VLMI (0),
				  _RC (0),
				  _idleState (true),
				  _cursorHardwareStatus (false),
				  _screenCodeData (std::vector <MCHEmul::UByte> (40, MCHEmul::UByte::_0)),
				  _graphicData (std::vector <MCHEmul::UByte> (40, MCHEmul::UByte::_0)),
				  _colorData (std::vector <MCHEmul::UByte> (40, MCHEmul::UByte::_0))
							{ }

			void emptyVideoMatrixAndColorRAMData ()
							{ _screenCodeData	= std::vector <MCHEmul::UByte> (40, MCHEmul::UByte::_0);
							  _colorData		= std::vector <MCHEmul::UByte> (40, MCHEmul::UByte::_0); }
			
			void emptyGraphicData ()
							{ _graphicData		= std::vector <MCHEmul::UByte> (40, MCHEmul::UByte::_0); }

			void changeCursorHardwareStatus ()
							{ _cursorHardwareStatus = !_cursorHardwareStatus; }
			bool cursorHardwareStatus () const
							{ return (_cursorHardwareStatus); }


			unsigned short _VCBASE, _VC, _VLMI;
			unsigned char _RC;
			bool _idleState; 
			bool _cursorHardwareStatus;
			mutable MCHEmul::UBytes _screenCodeData;
			mutable MCHEmul::UBytes _graphicData; 
			mutable MCHEmul::UBytes _colorData;
		};

		TEDGraphicInfo _tedGraphicInfo;

		private:
		static const MCHEmul::Address _MEMORYPOSIDLE1, _MEMORYPOSIDLE2;
	};

	inline void TED::readVideoMatrixAndColorRAM ()
	{
		memoryRef () -> setActiveView (_TEDView);
		
		_tedGraphicInfo._screenCodeData [_tedGraphicInfo._VLMI] =
			memoryRef () -> value (_TEDRegisters -> screenMemory () + (size_t) _tedGraphicInfo._VC); 
		_tedGraphicInfo._colorData [_tedGraphicInfo._VLMI] = 
			memoryRef () -> value (_TEDRegisters -> attributeMemory () + (size_t) _tedGraphicInfo._VC);
		
		memoryRef () -> setCPUView ();
	}

	// ---
	inline void TED::readGraphicalInfo ()
	{
		memoryRef () -> setActiveView (_TEDView);

		if (_tedGraphicInfo._idleState) // In this state the info is read from a specific place of the memory...
			_tedGraphicInfo._graphicData [_tedGraphicInfo._VLMI] = 
			_TEDRegisters -> graphicExtendedColorTextModeActive () 
				? memoryRef () -> value (_MEMORYPOSIDLE1) : memoryRef () -> value (_MEMORYPOSIDLE2);
		else // ..in the other one the info will be read attending to the situation of the memory...
		{	 // ...and from ROM or RAM...

			bool aR = ROMActiveToFetchCharData ();
			if (_TEDRegisters -> ROMSourceActive () && !aR)
				activeROMtoFecthCharData (true); // Active ROM to read the data...

			_tedGraphicInfo._graphicData [_tedGraphicInfo._VLMI] = _TEDRegisters -> textMode () 
				? memoryRef () -> value (_TEDRegisters -> charDataMemory () + 
					(((size_t) _tedGraphicInfo._screenCodeData [_tedGraphicInfo._VLMI].value () & 
						(_TEDRegisters -> graphicExtendedColorTextModeActive () ? 0x3f : 0xff)) 
						/** In the extended graphics mode there is only 64 chars possible. */ << 3) + _tedGraphicInfo._RC)
				: memoryRef () -> value (_TEDRegisters -> bitmapMemory () + 
					(_tedGraphicInfo._VC << 3) + _tedGraphicInfo._RC);

			if (_TEDRegisters -> ROMSourceActive () && !aR)
				activeROMtoFecthCharData (false); // Activethe RAM to read the data...
		}
		
		memoryRef () -> setCPUView ();
	}

	/** The version para PAL systems. */
	class TED_PAL final : public TED
	{
		public:
		static const MCHEmul::RasterData _VRASTERDATA;
		static const MCHEmul::RasterData _HRASTERDATA;

		static constexpr unsigned short _CYCLESPERRASTERLINE = 71;

		TED_PAL (int intId, int vV, MCHEmul::SoundLibWrapper* wS);
	};

	/** The version para NTSC systems. */
	class TED_NTSC final : public TED
	{
		public:
		static const MCHEmul::RasterData _VRASTERDATA;
		static const MCHEmul::RasterData _HRASTERDATA;

		static constexpr unsigned short _CYCLESPERRASTERLINE = 65;

		TED_NTSC (int intId, int vV, MCHEmul::SoundLibWrapper* wS);
	};
}

#endif
  
// End of the file
/*@}*/
