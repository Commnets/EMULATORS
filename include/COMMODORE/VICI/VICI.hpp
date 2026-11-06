/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: VICI.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 16/12/2023 \n
 *	Description: The VICI Chip.
 *	Versions: 1.0 Initial
 *	Based on http://tinyvga.com/6561
 */

#ifndef __COMMODORE_VICI__
#define __COMMODORE_VICI__

#include <CORE/incs.hpp>
#include <COMMODORE/VICI/VICIRegisters.hpp>

namespace COMMODORE
{
	/** The chip that takes care of anything around the graphics in computers like VIC20. 
		@see GraphicalChip and @see SoundChip. \n
		The emulation has been built following: http://tinyvga.com/6561.
		\n
		Any case: \n
		This is Ignacio Cea estimation...
		Every horizontal raster line takes 64us (including horizontal retrace = 15us as PAL/NTSC standard definition). \n
		1/1.108.404 = 0,902198us per cycle in PAL. 64us/0,902198 = 71 cycles per raster line in PAL. \n
		1/1.022.727 = 0,977778us per cycle in NTSC. 64us/0,977778us = 65 cycles per raster line in NTSC. \n
		VICI clock speed input is * 14 clock CPU in NTSC (14,318MHz), and x 4 in PAL (4,433MHz). \n
		PAL number of visible columns (defined) = 233 and NTSC = 210. \n
		So for the border = (233 - (22 columns * 8 pixels)) / 2 = 28 + 29 pixels border (left/right) in PAL and 17 + 17 in NTSC. \n
		The way they are distributed will depend of the valid values for screen position adjustment. \n
		Cycles have been multiplied by 8 pixels but when read data for the screen any column is divided inlow and high nibble. \n
		\n
		Notice that VICI doesn't launch any type of interruption.
	  */
	class VICI : public MCHEmul::GraphicalChip
	{
		public:
		/** 
			VICI is both a Graphical and a Sound Chip. \n
			VICI should then inherit from both MCHEmul::GraphicalCip & MCHEmul::SoundChip,
			but this could generate ambiguity when calling some parent methods, that will be the case. \n
			The main function of the VICI is, however, the graphical one. \n
			VICI will then inherit from MCHEmul::GraphicalChip only and it will use an object inside inheriting from MCHEmul::SoundChip. 
		  */
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

			inline void synchronizeClock (unsigned int cycle);
			/** Advances sound to an exclusive cycle boundary without replaying samples. */
			bool simulateUntil (MCHEmul::CPU* cpu, unsigned int cycle);
			virtual bool simulate (MCHEmul::CPU* cpu) override
							{ return (simulateUntil (cpu, cpu -> clockCycles ())); }

			virtual MCHEmul::InfoStructure getInfoStructure () const override;

			private:
			// -----
			// Different debug methods to simplify the internal code
			// and to make simplier the modification in case it is needed...
			/** Debug special situations...
				Take care using this instructions _deepDebugFile could be == nullptr... */
			void debugVICISoundCycle (MCHEmul::CPU* cpu, unsigned int i);
			// -----

			private:
			// Implementation
			/** The number of cycles the CPU was executed once the simulated method finishes. */
			unsigned int _lastCPUCycles;
			bool _clockSynchronized;
		};

		static const unsigned int _ID = 106;

		// Sample rate in Hz.
		static const unsigned int _SOUNDSAMPLINGCLOCK		= 44100;
		// 8 bits sound data, very simple nothing complicated...
		static const unsigned short _SOUNDSAMPLINGFORMAT	= AUDIO_U8;
		// Number of channels..
		static const unsigned char _SOUNDCHANNELS			= 1;

		/** Specific classes for PAL & NTSC have been created giving this data as default. \n
			The VICI constructor receives info over the raster data, the memory view to use,
			The number of cycles of every raster line (different depending on the VICI version) 
			and additional attributes. */
		VICI (const MCHEmul::RasterData& vd, const MCHEmul::RasterData& hd, 
			int vV, unsigned short cRL, MCHEmul::SoundLibWrapper* sW, const MCHEmul::Attributes& attrs = { });

		virtual ~VICI () override;

		const SoundFunction* soundFunction () const
							{ return (_soundFunction); }
		SoundFunction* soundFunction ()
							{ return (_soundFunction); }

		virtual unsigned short numberColumns () const override
							{ return (_raster.visibleColumns ()); }
		virtual unsigned short numberRows () const override
							{ return (_raster.visibleLines ()); }
		/** Always with in the visible screen. */
		inline void screenPositions (short& x1, short& y1, short& x2, short& y2) const;
		/** The original ones. */
		inline void originalScreenPositions (short& x1, short& y1, short& x2, short& y2) const;

		/** To get the number of cycles per raster line used in this chip. */
		unsigned short cyclesPerRasterLine () const
							{ return (_cyclesPerRasterLine); }

		/** To get the raster info. */
		const MCHEmul::Raster& raster () const
							{ return (_raster); }

		/** Retained VD0..VD7 lines. */
		const MCHEmul::UByte& videoBusData () const
							{ return (_videoBusData); }
		/** Retained VD8..VD11 lines, represented in the low nibble. */
		const MCHEmul::UByte& videoBusColor () const
							{ return (_videoBusColor); }

		// Managing lightpen
		/** To activate/desactivate the lightpen. */
		void setLightPenActive (bool a)
							{ _VICIRegisters -> setLigthPenActive (a); }
		/** To know whether the light pen is active. */
		bool lightPenActive () const
							{ return (_VICIRegisters -> lightPenActive ()); }

		virtual bool initialize () override;
		/** Connects the CPU-timed consumer; buffering must already be enabled. \n
			Call after initializing the CPU, VICI and SoundFunction. */
		bool enableCPUTimedWrites
			(MCHEmul::CPU* cpu, const MCHEmul::PhysicalStorageSubsets& subsets);

		/** The raster is a critical thing in the behaviour of the VICI. \n
			The content of registers 0x03 and 0x04 is "real time". \n
			Register 0x03 contains raster bit 0 in bit 7, and register 0x04 contains raster bits 8..1. \n
			Any read instruction could read different values depending on the position of the raster 
			when that instruction happens. */
		virtual void CPUAboutToExecute
			(const MCHEmul::InstructionContextEventData* dt) override;
		virtual void CPUAboutToExecute
			(const MCHEmul::InterruptContextEventData* dt) override;

		/** Simulates cycles in the VICI. */
		virtual bool simulate (MCHEmul::CPU* cpu) override;

		/**
		  *	The name of the fields are: \n
		  * VICIRegisters	= InfoStructure: Info about the registers.
		  * Raster			= InfoStructure: Info about the raster.
		  * Sound			= InfoStructire: Info about the sound.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		// To get snapshots of the memory...
		// They are used in some commands...
		/** content of the screen. */
		MCHEmul::UBytes screenMemorySnapShot (MCHEmul::CPU* cpu) const;
		MCHEmul::UBytes colorMemorySnapShot (MCHEmul::CPU* cpu) const;

		/** The characters. */
		MCHEmul::Strings charsDrawSnapshot (MCHEmul::CPU* cpu, 
			const std::vector <size_t>& chrs = { }) const;

		protected:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		/** Invoked from initialize to create the right screen memory. */
		virtual MCHEmul::ScreenMemory* createScreenMemory () override;

		// Draw the graphics detail...
		/** To simplify the use of some of the routines dedicated to draw graphics. */
		struct DrawContext
		{
			unsigned short _ICD;	// Initial Column of the Display (Not taken into account reductions in size).
			unsigned short _LCD;	// Last Column of the Display 
			unsigned short _RC;		// Raster X (from the beginning of the visible zone)
			unsigned short _IRD;	// Initial Row Display (Not taken into account reductions of the size)
			unsigned short _LRD;	// Last Row of the Display 
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

		// Invoked from the method "simulation"...
		/** Different actions are taken attending the raster cycle. */
		void treatRasterCycle ();
		/** Read and draw the graphics. */
		void readGraphicsAndDrawVisibleZone ();

		// These all methods are invoked from the two ones above!
		// They are here just to structure better the code...
		// Read screen data
		// Acquisition helpers called by readGraphicsAndDrawVisibleZone().
		/** To read the video matrix and the RAM color. \n
			Someting that happens during a display fetch / video matrix fetch. */
		inline void readVideoMatrixAndColorRAM ();
		/** Reads and retains the complete glyph-row byte for the current character. */
		inline void readGraphicalInfo ();

		// Light pen...
		/** To treat light-pen detection at the current raster position. */
		inline void treatLightPenAtCurrentRasterPosition ();

		// Draw the screen
		// Methods linked to the drawVisibleZone method...
		/** Treat the visible zone.
			Draws the graphics and finally draw the border. */
		void drawVisibleZone ();
		/** To draw any type of graphic \n
			This method uses the ones below it \n
			The method receives: \n
			dC = other info about the raster used to finally draw. \n
			They all return a instance of DrawResult (@see above). */
		void drawGraphics (const DrawContext& dC);
		/** To decode the eight internal pixels of the current cycle in high resolution. */
		DrawResult drawHighResolutionMode ();
		/** To decode the eight internal pixels of the current cycle in multicolor. */
		DrawResult drawMulticolorMode ();
		/** Copies the decoded pixels, clipping the span to the visible frame width. \n
			The context origin must belong to the visible frame. */
		void drawResultToScreen (const DrawResult& cT, const DrawContext& dC);

		// -----
		// Different debug methods to simplify the internal code
		// and to make simplier the modification in case it is needed...
		/** Debug special situations...
			Take care using this instructions _deepDebugFile could be == nullptr... */
		void debugVICICycle (unsigned int cycle);
		/** Records the completed final cycle before line counters are reset. */
		void debugVICILineEnd (unsigned int cycle);
		/** Records the raster origin and advance to the instruction read point. */
		void debugCPUAboutToExecute
			(const MCHEmul::InstructionContextEventData* dt,
			 unsigned int cycles);
		// -----

		private:
		bool prepareCPUTransaction
			(MCHEmul::CPU* cpu, const MCHEmul::BusCycleData* busData);
		bool extractPendingWrites ();
		bool executePendingWriteAt (MCHEmul::CPU* cpu, unsigned int cycle);
		/** Advances through the video phase of the predicted CPU read cycle. */
		bool simulate (MCHEmul::CPU* cpu, unsigned int cycles);
		bool simulateRasterCycle
			(MCHEmul::CPU* cpu, unsigned int cycle, bool completeCycle);
		bool finishRasterCycle (MCHEmul::CPU* cpu, unsigned int cycle);
		/** Updates text bounds after acquisition and before drawing the current cycle. */
		inline void calculateScreenPositions ();
		/** Acquires the data byte and the corresponding color-bus nibble. */
		MCHEmul::UByte acquireVideoMemory (const MCHEmul::Address& address);

		protected:
		/** A reference to the sound part of the chip. */
		SoundFunction* _soundFunction;
		/** The memory is used also as the set of registers of the chip. */
		COMMODORE::VICIRegisters* _VICIRegisters;
		/** The number of the memory view used to read the data. */
		int _VICIView;
		/** The number of cycles per raster line as it depends on the type of Chip. */
		unsigned short _cyclesPerRasterLine;
		/** The raster. */
		MCHEmul::Raster _raster;

		// Implementation
		/** Non-owning video view, linked during initialize(). */
		MCHEmul::MemoryView* _videoMemory;
		/** Retained bus lines, independent of character output and frame boundaries. */
		MCHEmul::UByte _videoBusData;
		MCHEmul::UByte _videoBusColor;
		/** Next cycle to start. \n
			The last started cycle may still await completion during a CPU read. */
		unsigned int _lastCPUCycles;
		/** The format used to draw. 
			It has to be the same that is used by the Screen object. */
		SDL_PixelFormat* _format;
		/** When a raster line is processed, it is necessary to know which cycle is being processed. 
			The number of max cycles is get from the method (@see) "cyclesPerRasterLine". */
		unsigned short _cycleInRasterLine;
		/** Whether GRAPHICSREADY was emitted for the current initial vertical blank. */
		bool _lastVBlankEntered;
		/** Inclusive text bounds relative to the visible viewport, before clipping. */
		short _scrX1, _scrY1, _scrX2, _scrY2;
		/** A temporal variable to indicate whether a lightpen position
			has already been latched in the current frame.
			This variable is reset at the beginning of every frame. */
		bool _lightPenFrameLatched;
		/** A very temporal variable to keep when the button is pressed. */
		bool _lightPenButtonPressed;

		/** 
		  *	Structure to control how the graphics are displayed in the screen. \n
		  * based on the article: http://tinyvga.com/6561 \n
		  * During display, matrix/color and glyph acquisitions alternate on each raster line. \n
		  * Pending acquisition data is separate from the retained character output. \n
		  * The text window is something that can be relocated within the visible zone. \n
		  * Live origin comparisons open the window; display state advances independently of presentation. */
		struct VICGraphicInfo
		{
			/** Experimental delays from origin comparison, expressed in CPU cycles. */
			static constexpr unsigned char _COUNTERSTARTDELAY = 3;
			static constexpr unsigned char _FETCHSTARTDELAY = 5;
			/** Width of the experimental video-matrix counter model. */
			static constexpr unsigned short _MATRIXCOUNTERMASK = 0x0fff;

			enum class FetchPhase
			{
				_START,
				_MATRIX,
				_GLYPH
			};

			enum class VerticalState
			{
				_WAITING,
				_PENDING,
				_DISPLAY,
				_DONE
			};

			VICGraphicInfo ()
				: _matrixBaseCounter (0), _matrixCounter (0), _matrixOffset (0),
				  _matrixCarry4 (false), _matrixCarry8 (false),
				  _matrixAdvancedThisCycle (false),
				  _COL (0), _COLMAX (0), _pendingColumns (0),
				  _ROW (0), _ROWMAX (0),
				  _RC (0),
				  _horizontalOpened (false),
				  _verticalState (VerticalState::_WAITING),
				  _DRAW (false), _SCREENORIGINX (false), _SCREENORIGINY (false),
				  _screenCodeData (MCHEmul::UByte::_0),
				  _graphicByteData (MCHEmul::UByte::_0),
				  _graphicData (MCHEmul::UByte::_0),
				  _colorData (MCHEmul::UByte::_0),
				  _pendingColorData (MCHEmul::UByte::_0),
				  _fetchPhase (FetchPhase::_MATRIX),
				  _startupCyclesLeft (0),
				  _counterStartupCyclesLeft (0), _counterHalvesLeft (0),
				  _outputHalvesLeft (0),
				  _graphicDataReady (false)
							{ }

			/** Seeds register-derived state and resets the display controller. */
			inline void initialize (VICIRegisters* r);
			/** Resets frame counters without resampling register-derived dimensions. */
			inline void initializeFrame ();
			/** Samples live origin registers before the current video acquisition. */
			void beginRasterCycle
				(VICIRegisters* r, unsigned short horizontalCycle,
				 unsigned short rasterLine);
			/** Consumes one startup cycle without performing an active acquisition. */
			inline void advanceFetchStartup ();
			/** Advances the completed column independently of the matrix counter. */
			inline void completeCharacterFetch ();
			/** Prepares the next address and advances the independently timed counter. */
			inline void advanceMatrixCycle ();
			inline void nextVertical (VICIRegisters* r);
			bool draw () const
							{ return (_DRAW); }
			bool graphicDataReady () const
							{ return (_graphicDataReady); }
			bool screenOriginDetected () const
							{ return (_SCREENORIGINX && _SCREENORIGINY); }

			/** Matrix endpoint saved at a character-row boundary, in half-character units. */
			unsigned short _matrixBaseCounter;
			/** Working 12-bit matrix position in half-character units. */
			unsigned short _matrixCounter;
			/** Character-unit offset prepared by the counter for a later matrix acquisition. */
			size_t _matrixOffset;
			/** Final-cycle carries into bits 4 and 8, applied independently at the next line reload. */
			bool _matrixCarry4, _matrixCarry8;
			/** Whether the current video cycle advanced the matrix counter. */
			bool _matrixAdvancedThisCycle;

			// Horizontal acquisition
			/** Next matrix column, advanced after each complete glyph acquisition. */
			unsigned char _COL;
			/** Provisional width at opening, finalized when the matrix counter starts. */
			unsigned char _COLMAX;
			/** Width sampled on the second cycle of each internal raster line. */
			unsigned char _pendingColumns;

			// Vertical...
			/** Character-row index used to determine vertical window completion. */
			unsigned char _ROW;
			/** Row limit sampled on the third cycle of internal line 0. */
			unsigned char _ROWMAX;

			/** Raster line within the current character row, used to address the glyph byte. */
			unsigned char _RC;

			/** Whether the horizontal window has already opened on this line. */
			bool _horizontalOpened;
			/** Vertical window lifecycle within the current frame. */
			VerticalState _verticalState;

			/** Whether the current geometry permits character acquisition. \n
				Output validity is tracked separately so the final character can finish. */
			bool _DRAW;
			/** Has the origin of the screen being detected in this cycle. \n
				First time the first column or the first row of the text window is detected, these flags get set. \n
				Both pulse together when the first horizontal opening starts vertical display. */
			bool _SCREENORIGINX, _SCREENORIGINY;

			/** Character code retained between matrix and glyph reads. */
			mutable MCHEmul::UByte _screenCodeData;
			/** Complete glyph-row byte retained during character output. */
			mutable MCHEmul::UByte _graphicByteData;
			/** Four glyph bits selected for the current output cycle. */
			mutable MCHEmul::UByte _graphicData;
			/** Color and mode of the character currently being output. */
			mutable MCHEmul::UByte _colorData;
			/** Color acquired with the pending character code. */
			MCHEmul::UByte _pendingColorData;
			/** Startup, matrix or glyph phase of the acquisition controller. */
			FetchPhase _fetchPhase;
			/** Startup cycles remaining before preparing the first matrix acquisition. */
			unsigned char _startupCyclesLeft;
			/** Counter startup countdown, including its activation cycle. */
			unsigned char _counterStartupCyclesLeft;
			/** Half-cell increments still to perform with the captured width. */
			unsigned char _counterHalvesLeft;
			/** Four-bit groups still to emit from the glyph byte, high half first. */
			unsigned char _outputHalvesLeft;
			/** The current output nibble is valid for drawing. */
			bool _graphicDataReady;

			private:
			inline void setDraw ();
			inline void initializeHorizontal ();
			inline void initializeVertical ();
			inline void initializeHorizontalCounters ();
			inline void initializeVerticalCounters ();
			/** Loads the retained base and applies the preceding boundary carries. */
			inline void reloadMatrixCounter ();
			/** Advances one acquisition unit within the counter width. */
			inline void advanceMatrixCounter ();
		};

		VICGraphicInfo _vicGraphicInfo;

		private:
		// Implementation
		/** Non-owning CPU/subset links used only by the complete-instruction scheduler. */
		MCHEmul::CPU* _timedWriteCPU;
		MCHEmul::PhysicalStorageSubsets _timedWriteSubsets;
		/** Persistent instruction/interrupt metadata, never an event-context pointer. */
		const MCHEmul::BusCycleData* _writeBusCycleData;
		/** Absolute first cycle of the current CPU transaction. */
		unsigned int _writeStartCycle;
		/** Buffered writes awaiting replay on their assigned CPU bus cycles. */
		MCHEmul::SetMemoryCommands _pendingWrites;
		/** Video phases simulated before CPU execution, including the final phase awaiting completion. */
		unsigned int _cyclesSimulatedInAdvance;
		/** The final advanced cycle awaits write replay and raster advancement. */
		bool _advancedCyclePending;
	};

	// ---
	inline void VICI::SoundFunction::synchronizeClock (unsigned int cycle)
	{
		_lastCPUCycles = cycle;
		_clockSynchronized = true;
	}

	// ---
	inline void VICI::screenPositions (short& x1, short& y1, short& x2, short& y2) const
	{ 
		// Extrract the info with the limits...
		x1 = _scrX1; 
		if (x1 < 0) x1 = 0;
		if (x1 >= _raster.hData ().visiblePositions ()) x1 = _raster.hData ().visiblePositions () - 1;
		y1 = _scrY1;
		if (y1 < 0) y1 = 0;
		if (y1 >= _raster.vData ().visiblePositions ()) y1 = _raster.vData ().visiblePositions () - 1;
		x2 = _scrX2; 
		if (x2 < 0) x2 = 0;
		if (x2 >= _raster.hData ().visiblePositions ()) x2 = _raster.hData ().visiblePositions () - 1;
		y2 = _scrY2;
		if (y2 < 0) y2 = 0;
		if (y2 >= _raster.vData ().visiblePositions ()) y2 = _raster.vData ().visiblePositions () - 1;
	}

	// ---
	inline void VICI::originalScreenPositions (short& x1, short& y1, short& x2, short& y2) const
	{ 
		// The original limits, the could be negative...
		x1 = _scrX1; 
		y1 = _scrY1;
		x2 = _scrX2; 
		y2 = _scrY2;
	}

	// ---
	inline void VICI::readVideoMatrixAndColorRAM ()
	{
		_vicGraphicInfo._screenCodeData = acquireVideoMemory
			(_VICIRegisters -> screenMemory (_vicGraphicInfo._matrixOffset));

		// Character color belongs to the matrix acquisition.
		_vicGraphicInfo._pendingColorData = _videoBusColor;
	}

	// ---
	inline void VICI::readGraphicalInfo ()
	{
		const bool expanded = _VICIRegisters -> charsExpanded ();

		// A mode change must keep the row address within the selected glyph.
		_vicGraphicInfo._graphicByteData = acquireVideoMemory
			(_VICIRegisters -> charDataMemory
				((size_t)
					((_vicGraphicInfo._screenCodeData.value () <<
						(expanded ? 4 : 3)) +
					 (_vicGraphicInfo._RC & (expanded ? 0x0f : 0x07)))));
	}

	// ---
	inline void VICI::treatLightPenAtCurrentRasterPosition ()
	{
		if (!_VICIRegisters -> lightPenActive () ||
			!_lightPenButtonPressed ||
			_lightPenFrameLatched ||
			!_VICIRegisters -> isMouseInVisibleZone () ||
			!_raster.isInVisibleZone ())
			return; // Nothing to do really...

		unsigned short cv = 0;
		unsigned short rv = 0;
		_raster.currentVisiblePosition (cv, rv);
		const int mx = _VICIRegisters -> mousePositionX ();
		const int my = _VICIRegisters -> mousePositionY ();

		// The raster advances horizontally in 8-pixel blocks in this emulation.
		// And it is needed to compare visible thing with visible things...
		if (my != (int) rv || 
			!((mx >= (int) cv) && (mx < (int) (cv + _raster.step ()))))
			return;

		/** At this point the raster beam is passing through the mouse/light-pen
			position. The light pen detects the beam and drives LP low. \n
			Convert from visible mouse coordinate to the corresponding
			VIC-I raster coordinate. \n
			_raster.hData().currentPosition() is the current internal
			raster X position. mx - cv gives the pixel offset inside the
			current 8-pixel group. */
		const unsigned short internalX =
			_raster.hData ().currentPosition () + (unsigned short) (mx - (int) cv);
		// Your raster advances in 8 internal horizontal positions per VIC-I CPU cycle.
		const unsigned short rasterCycle = internalX / _raster.step ();
		// VIC-I light pen X register: approximately 2 * (cycle + 1), with bit 0 set.
		const unsigned char lpx =
			(unsigned char) ((2 * ((rasterCycle + 1) % _cyclesPerRasterLine)) | 0x01);
		const unsigned char lpy =
			(unsigned char) ((_raster.vData ().currentPosition () >> 1) & 0xff);
		_VICIRegisters -> latchLightPenPositionFromRaster (lpx, lpy);

		_lightPenFrameLatched = true;
	}

	// ---
	inline void VICI::calculateScreenPositions ()
	{
		if (_vicGraphicInfo.graphicDataReady () &&
			_vicGraphicInfo._COL == 1 &&
			_vicGraphicInfo._outputHalvesLeft == 1)
		{
			// Derive the bounds from the first group actually emitted this line.
			_scrX1 =
				(short) _raster.hData ().currentVisiblePosition ();
			_scrX2 =
				_scrX1 +
				(short) ((unsigned short) _vicGraphicInfo._COLMAX << 4) - 1;
		}

		if (_vicGraphicInfo.screenOriginDetected ())
		{
			_scrY1 =
				(short) _raster.vData ().currentVisiblePosition ();
			_scrY2 =
				_scrY1 +
				(short) ((unsigned short) _vicGraphicInfo._ROWMAX <<
					(_VICIRegisters -> charsExpanded () ? 4 : 3)) - 1;
		}
	}

	// ---
	inline void VICI::VICGraphicInfo::initialize (VICIRegisters* v)
	{
		// Seed register-derived state before the first regular sampling points.
		_pendingColumns = v -> charsWidthScreen ();
		_ROWMAX = v -> charsHeightScreen ();

		initializeFrame ();
	}

	// ---
	inline void VICI::VICGraphicInfo::initializeFrame ()
	{
		_matrixBaseCounter = 0;
		_matrixCounter = 0;
		_matrixCarry4 = false;
		_matrixCarry8 = false;
		_matrixAdvancedThisCycle = false;
		_COLMAX = 0;

		initializeHorizontal ();
		initializeVertical ();

		_screenCodeData = MCHEmul::UByte::_0;
		_graphicByteData = MCHEmul::UByte::_0;
		_graphicData = MCHEmul::UByte::_0;
		_colorData = MCHEmul::UByte::_0;
		_pendingColorData = MCHEmul::UByte::_0;

		setDraw ();
	}

	// ---
	inline void VICI::VICGraphicInfo::advanceFetchStartup ()
	{
		// Counter activation has already captured the width shared by both controllers.
		if (--_startupCyclesLeft == 0)
			_fetchPhase = FetchPhase::_MATRIX;
	}

	// ---
	inline void VICI::VICGraphicInfo::completeCharacterFetch ()
	{
		++_COL;

		setDraw ();
	}

	// ---
	inline void VICI::VICGraphicInfo::advanceMatrixCycle ()
	{
		if (!_horizontalOpened)
			return;

		if (_counterStartupCyclesLeft != 0)
		{
			if (--_counterStartupCyclesLeft != 0)
				return;

			// Capture once so counting and later acquisitions use the same width.
			_COLMAX = _pendingColumns;
			_counterHalvesLeft = (unsigned char) (_COLMAX << 1);
		}

		if (_counterHalvesLeft == 0)
			return;

		// Consume the old address before replacing it every two counter cycles.
		if ((_counterHalvesLeft & 0x01) == 0)
			_matrixOffset = (size_t)
				((_matrixCounter + 1) & _MATRIXCOUNTERMASK) >> 1;

		advanceMatrixCounter ();
		--_counterHalvesLeft;
	}

	// ---
	inline void VICI::VICGraphicInfo::nextVertical (VICIRegisters* v)
	{
		// Only an increment in the final cycle can retain a boundary carry.
		_matrixCarry4 = _matrixAdvancedThisCycle &&
			(_matrixCounter & 0x000f) == 0;
		_matrixCarry8 = _matrixAdvancedThisCycle &&
			(_matrixCounter & 0x00ff) == 0;

		// Pending vertical opening must not consume glyph rows before display starts.
		if (_verticalState == VerticalState::_DISPLAY)
		{
			const unsigned char height =
				(unsigned char) (v -> charsExpanded () ? 16 : 8);

			++_RC;

			// A height change may occur after the first matching boundary.
			if (_RC == height ||
				_RC == (unsigned char) (height << 1))
			{
				_RC = 0;

				// Save the completed endpoint before the next line reload.
				if (_horizontalOpened)
					_matrixBaseCounter = _matrixCounter;

				if (++_ROW == _ROWMAX)
					_verticalState = VerticalState::_DONE;
			}
		}

		initializeHorizontal ();
	}

	// ---
	inline void VICI::VICGraphicInfo::setDraw ()
	{
		_DRAW = _horizontalOpened &&
			_verticalState == VerticalState::_DISPLAY &&
			_fetchPhase != FetchPhase::_START &&
			_COL < _COLMAX;
	}

	// ---
	inline void VICI::VICGraphicInfo::initializeHorizontal ()
	{
		initializeHorizontalCounters ();

		setDraw ();
	}

	// ---
	inline void VICI::VICGraphicInfo::initializeVertical ()
	{
		initializeVerticalCounters ();

		setDraw ();
	}

	// ---
	inline void VICI::VICGraphicInfo::initializeHorizontalCounters ()
	{
		_COL = 0;
		_horizontalOpened = false;

		_fetchPhase = FetchPhase::_MATRIX;
		_startupCyclesLeft = 0;
		_counterStartupCyclesLeft = 0;
		_counterHalvesLeft = 0;
		_matrixOffset = 0;
		_outputHalvesLeft = 0;
		_graphicDataReady = false;

		_SCREENORIGINX = false;
		_SCREENORIGINY = false;
	}

	// ---
	inline void VICI::VICGraphicInfo::initializeVerticalCounters ()
	{
		_ROW = 0;
		_RC = 0;
		_verticalState = VerticalState::_WAITING;

		_SCREENORIGINY = false;
	}

	// ---
	inline void VICI::VICGraphicInfo::reloadMatrixCounter ()
	{
		_matrixCounter = _matrixBaseCounter;

		// Boundary carries affect their own nibble without propagating further.
		if (_matrixCarry8)
			_matrixCounter = (unsigned short)
				((_matrixCounter & 0x00ff) |
				 (((_matrixCounter & 0x0f00) + 0x0100) & 0x0f00));

		if (_matrixCarry4)
			_matrixCounter = (unsigned short)
				((_matrixCounter & 0x0f0f) |
				 (((_matrixCounter & 0x00f0) + 0x0010) & 0x00f0));

		_matrixCarry4 = false;
		_matrixCarry8 = false;
	}

	// ---
	inline void VICI::VICGraphicInfo::advanceMatrixCounter ()
	{
		_matrixCounter = (unsigned short)
			((_matrixCounter + 1) & _MATRIXCOUNTERMASK);

		_matrixAdvancedThisCycle = true;
	}

	/** The version para PAL systems. */
	class VICI_PAL final : public VICI
	{
		public:
		static const MCHEmul::RasterData _VRASTERDATA;
		static const MCHEmul::RasterData _HRASTERDATA;

		static constexpr unsigned short _CYCLESPERRASTERLINE = 71;

		VICI_PAL (int vV, MCHEmul::SoundLibWrapper* wS);
	};

	/** The version para NTSC systems. */
	class VICI_NTSC final : public VICI
	{
		public:
		static const MCHEmul::RasterData _VRASTERDATA;
		static const MCHEmul::RasterData _HRASTERDATA;

		static constexpr unsigned short _CYCLESPERRASTERLINE = 65;

		VICI_NTSC (int vV, MCHEmul::SoundLibWrapper* wS);
	};
}

#endif
  
// End of the file
/*@}*/
