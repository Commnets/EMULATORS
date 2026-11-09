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
 *  Based also on: https://hackaday.io/project/11460-fpgated
 *  (the author of this project has been very helpful to understand the TED chip).
 *
 *	Execution and CPU synchronization. \n
 *	C264::Commodore264::processEvent forwards CPU instruction and interrupt-entry
 *	notifications to the corresponding CPUAboutToExecute overload. \n
 *
 *	Instruction notification. \n
 *	1. Select the BusCycleData alternative predicted by the 7501. \n
 *	2. prepareCPUTransaction retains the CPU, bus description, initial CPU
 *	   coordinate and transaction kind. \n
 *	3. Establish the diagnostic CPU link and emit the transaction record when
 *	   deep debugging is active. \n
 *	4. With timed writes enabled, advanceToCPURead recovers the readable prefix
 *	   preceding the first write and leaves its final CPU cycle suspended
 *	   immediately before the prepared read-capture edge. \n
 *
 *	Interrupt-entry notification. \n
 *	1. Obtain the interrupt BusCycleData and prepare the transaction. \n
 *	2. Establish the diagnostic CPU link and emit the transaction record. \n
 *	3. Unlike instruction notification, do not call advanceToCPURead.
 *	   Vector reads following stack writes are not individually synchronized
 *	   by this prefix-recovery mechanism. \n
 *	This notification describes CPU interrupt entry, not a TED IRQ request. \n
 *
 *	Recovery after CPU execution. \n
 *	simulate obtains the executed CPU boundary, validates the pending
 *	transaction and any prepared prefix, and extracts buffered writes. \n
 *	It repeatedly calls simulateCPUCycle until _lastCPUCycles reaches that
 *	boundary. An already suspended cycle resumes from its retained cursor;
 *	earlier TED work must not be repeated. \n
 *	Each completed CPU cycle commits its half-pulse remainder and pacing time,
 *	publishes the diagnostic raster copy and advances _lastCPUCycles. \n
 *	After recovery, every scheduled write must have been consumed before the
 *	transaction state is released. Without timed writes, the first simulate
 *	call establishes the initial CPU synchronization boundary. \n
 *
 *	Single and double CPU speed. \n
 *	A single-speed CPU cycle spans 32 quarter-dot units, or one slow TED pulse.
 *	A double-speed CPU cycle spans 16 quarter-dot units, or half a pulse. \n
 *	With graphics enabled, these durations contain eight or four dot emissions,
 *	respectively. The profile divider is 20 for PAL and 16 for NTSC; a CPU
 *	cycle represents that divider or half of it in board crystal clocks. \n
 *	The clock mode and divider are captured at CPU-cycle start. Later register
 *	writes cannot alter that retained duration. A carried half pulse can make
 *	a CPU cycle begin in the second half of a slow TED pulse. \n
 *	advanceToCPURead stops before zero-based quarter index 23 at single speed
 *	or 7 at double speed. These are recovery cursors, not absolute TED clocks. \n
 *
 *	Work within a slow TED pulse. \n
 *	advanceCPUQuarters skips quarter edges without TED work and calls
 *	processCPUQuarter for control and dot-clock edges. \n
 *	Control edges process pending raster controls. At phase zero the graphics
 *	gate is captured and beginTEDCycle performs the existing fetch/counter
 *	work and prepares eight output entries. \n
 *	On emission edges, scheduled CPU writes precede pixel output. Raster IRQ
 *	sampling uses the operands captured before simultaneous CPU loads.
 *	The pixel is emitted before presentation and hardware-counter advancement. \n
 *	At slow-pulse completion, finishTEDCycle performs the enabled counter,
 *	timer and sound work and propagates pending IRQ requests. Other request
 *	points preserve the existing raster-event and CPU-boundary handling. \n
 *	Changing CPU speed does not multiply the timer or sound clocks. FREEZE
 *	gating is retained for the slow pulse; sound scheduling continues through
 *	the existing frozen-pulse path. \n
 *
 *	Diagnostic event markers. \n
 *	TEDEVENTS marks accepted badline conditions, accepted raster-comparator
 *	events and the first propagation of each continuous TED IRQ request. \n
 *	Markers use presentation coordinates captured before dot advancement.
 *	They do not imply CPU interrupt acceptance or physical output latency.
 */

#ifndef __COMMODORE_TED__
#define __COMMODORE_TED__

#include <array>
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
			TED will then inherit from MCHEmul::GraphicalChip only and it will use an object inside inheriting from MCHEmul::SoundChip. \n
			The sound function behaves really at a slower speed internally (/160 in a PAL TED and /128 in NTSC one). */
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

			/** Queues complete slow-clock pulses supplied by the TED scheduler. */
			void addTEDCycle (unsigned int masterCycles);
			/** Updates clock metadata without resetting generators. */
			void setClockDivider (unsigned int divider);

			/** Consumes queued pulses during regular chip simulation. */
			virtual bool simulate (MCHEmul::CPU* cpu) override
							{ return (simulatePendingCycles (cpu, cpu -> clockCycles (), false)); }
			/** Consumes queued pulses before a scheduled CPU audio write. \n
				The cycle argument identifies the write boundary for diagnostics. \n
				It does not request additional clock advancement. */
			bool simulateBeforeCPUWrite (MCHEmul::CPU* cpu, unsigned int cycle)
							{ return (simulatePendingCycles (cpu, cycle, true)); }

			/**
			  *	The name of the fields are: \n
			  * The ones coming from the parent class and: \n
			  * SoundLibWrapper	= InfoStructure: Information provided by the active sound wrapper.
			  */
			virtual MCHEmul::InfoStructure getInfoStructure () const override;

			private:
			/** Consumes only the complete pulses already queued by TED. \n
				The reference cycle and cause affect diagnostics only. */
			bool simulatePendingCycles
				(MCHEmul::CPU* cpu, unsigned int referenceCycle,
				 bool beforeCPUWrite);

			//-----
			void debugSoundBatch
				(MCHEmul::CPU* cpu, unsigned int referenceCycle,
				 bool beforeCPUWrite, unsigned int cyclesProcessed,
				 unsigned long long masterCyclesProcessed,
				 size_t samplesGenerated) const;
			//-----

			public:
			// Implementation
			/** Complete TED sound-clock pulses waiting to be simulated. */
			unsigned int _pendingSoundClockCycles;

			private:
			struct SoundClockBatch final
			{
				SoundClockBatch (unsigned int masterCycles)
					: _masterCycles (masterCycles), _cycles (1)
							{ }

				unsigned int _masterCycles;
				unsigned int _cycles;
			};

			// Implementation
			/** Consecutive pulses with equal elapsed duration share one batch. */
			std::vector <SoundClockBatch> _pendingSoundBatches;
		};

		static const unsigned int _ID = 101;

		// 44,1MHz (more or less standard in current sound cards)
		static const unsigned int _SOUNDSAMPLINGCLOCK		= 44100;
		// 8 bits sound data, very simple nothing complicated...
		static const unsigned short _SOUNDSAMPLINGFORMAT	= AUDIO_U8;
		// Number of channels..
		static const unsigned char _SOUNDCHANNELS			= 1;

		/** The position of the bad lines. */
		static const unsigned short _FIRSTBADLINE			= 0x00;
		static const unsigned short _LASTBADLINE			= 0xc7;

		/** Data about the size of the screen */
		static const unsigned short _GRAPHMAXCHARLINES		= 25;	// Not taking into account reductions in the size
		static const unsigned short _GRAPHMAXCHARCOLUMNS	= 40;
		static const unsigned short _GRAPHMAXBITMAPCOLUMNS	= 320;	// Not taking into account double coulors
		static const unsigned short _GRAPHMAXBITMAPROWS		= 200;

		/** Geometry and clock description owned by each TED instance. */
		struct Configuration final
		{
			Configuration (const MCHEmul::RasterData& vd, const MCHEmul::RasterData& hd,
				unsigned short divider, unsigned short screenFrequency);

			const MCHEmul::RasterData _vertical, _horizontal;
			const unsigned short _divider, _screenFrequency;

			// Implementation
			const unsigned short _endOfScreen, _cyclesPerRasterLine;
		};

		static const Configuration _PALCONFIGURATION, _NTSCCONFIGURATION;
		static const unsigned int _VIDEOCONFIGURATIONCHANGED = 101;

		/** Profiles may override the default presentation geometry. */
		TED (int intId, bool initialPAL, const Configuration& pal, const Configuration& ntsc,
			int vV, MCHEmul::SoundLibWrapper* sW, const MCHEmul::Attributes& attrs = { });

		virtual ~TED () override;

		/** To draw or not to draw raster interrupt positions. */
		void setDrawRasterInterruptPositions (bool d)
							{ _drawRasterInterruptPositions = d; }
		/** Enables markers for badlines, raster events and TED IRQ requests. */
		void setDrawOtherEvents (bool d)
		{
			if (_drawOtherEvents == d)
				return;

			_drawOtherEvents = d;
			_eventStatus = { };

			// Enabling diagnostics must not turn an old request into a new event.
			if (d && _TEDRegisters != nullptr)
				_eventStatus._irqRequestMarked = _TEDRegisters -> launchIRQ ();
		}

		unsigned short screenFrequency () const
							{ return (_initialConfiguration -> _screenFrequency); }
		unsigned short timesFasterThanCPU () const
							{ return (_initialConfiguration -> _divider); }

		double nominalFrameFrequency () const;

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
							{ return (_configuration -> _cyclesPerRasterLine); }

		/** Single clock active? */
		bool inSingleClockMode () const
							{ return (_inSingleClockMode); }

		/** To get the raster info. */
		const MCHEmul::Raster& raster () const
							{ return (_raster); }

		/** Returns and clears recovered time awaiting host pacing. \n
			The returned unit is one board crystal-clock period. */
		inline unsigned int takePendingPacingCycles ();

		virtual bool initialize () override;

		/** Prepares the destinations used by CPU timed-write recovery. \n
			Does not enable buffering, extract commands or execute writes. \n
			Requires CPU execution in complete-transaction mode. */
		bool prepareCPUTimedWrites (MCHEmul::CPU* cpu,
			 const MCHEmul::PhysicalStorageSubsets& subsets);
		/** Enables buffered CPU-write recovery after machine setup. */
		bool enableCPUTimedWrites ();

		/** Captures transaction metadata and prepares TED state for CPU reads. */
		virtual void CPUAboutToExecute
			(const MCHEmul::InstructionContextEventData* dt) override;
		/** Receives an accepted interrupt before its launch changes CPU state. */
		virtual void CPUAboutToExecute
			(const MCHEmul::InterruptContextEventData* dt) override;

		/** Simulates cycles in the TED. \n
			It draws the border AFTER once graphics info has been drawn within the display zone. \n
			Foreground graphics are composed with the background before the border is drawn. */
		virtual bool simulate (MCHEmul::CPU* cpu) override;

		/**
		  *	The name of the fields are: \n
		  * TEDRegisters	= InfoStructure: Information about the TED registers. \n
		  * Raster			= InfoStructure: Information about the raster. \n
		  * Sound			= InfoStructure: Information about the sound function. \n
		  * TEDTimer1		= InfoStructure: Information about timer 1. \n
		  * TEDTimer2		= InfoStructure: Information about timer 2. \n
		  * TEDTimer3		= InfoStructure: Information about timer 3. \n
		  * Execution		= InfoStructure: Live execution state, with these attributes. \n
		  * COUNTERX		= Attribute: Effective horizontal counter in dot units. \n
		  * COUNTERY		= Attribute: Effective vertical counter. \n
		  * VIDEOLINE		= Attribute: Internal vertical capture, not a framebuffer row. \n
		  * RASTERCYCLE	= Attribute: Cycle derived from the effective horizontal counter. \n
		  * SELECTEDPAL	= Attribute: Whether the selected profile is PAL. \n
		  * PRESENTATIONPAL = Attribute: Whether the applied presentation profile is PAL. \n
		  * PRESENTATIONPENDING = Attribute: Selected and presentation profiles differ. \n
		  * CLOCKDIVIDER	= Attribute: Selected profile divider, not a retained CPU-cycle duration. \n
		  * SINGLECLOCK	= Attribute: Current TED decision, not the mode of a suspended CPU cycle. \n
		  * GRAPHICSCYCLEACTIVE = Attribute: Graphics gating retained for the current slow pulse. \n
		  * VERTICALCLOCKPENDING = Attribute: Vertical clock awaiting the slow-clock boundary. \n
		  * FLASHCLOCKPENDING = Attribute: FLASH clock awaiting the slow-clock boundary. \n
		  * HORIZONTALWRITEPENDING = Attribute: Horizontal write awaiting the next dot tick. \n
		  * VC				= Attribute: Working graphics counter, distinct from CHARPOS. \n
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		// To get snapshots of the memory...
		// They are used in some commands...
		/** Current screen matrix in physical RAM, including machine aliases. */
		MCHEmul::UBytes screenMemorySnapShot (MCHEmul::CPU* cpu) const;
		/** Current attributes in physical RAM, independently of CPU overlays. */
		MCHEmul::UBytes attributeMemorySnapShot (MCHEmul::CPU* cpu) const;
		/** Current bitmap bytes using the graphics source selected by FF12. \n
			Restores the previous memory view and TED ROM selection. */
		MCHEmul::UBytes bitmapMemorySnapShot (MCHEmul::CPU* cpu);

		/** Draws character definitions using the graphics source selected by FF12. \n
			Character numbers remain raw indexes from 0 to 255. \n
			Restores the previous memory view and TED ROM selection. */
		MCHEmul::Strings charsDrawSnapshot (MCHEmul::CPU* cpu,
			const std::vector <size_t>& chrs = { });

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
			unsigned short _RCA;	// Raster X adjusted (Moves 8 by 8, so = Raster X >> 3 << 3)
			unsigned short _RR;		// Raster Y (From the beginning of the visible zone.
									// Moves 1 by 1. No adjusted needed)
		};

		/** Eight decoded output entries retained between preparation and emission. \n
			Global background colors are resolved when each pixel is emitted. */
		struct DrawResult
		{
			static const unsigned int _TRANSPARENT = ~0u;
			static const unsigned int _BACKGROUNDREFERENCE = 0x100u;

			DrawResult (unsigned int bC = _TRANSPARENT)
				: _foregroundColorData { bC, bC, bC, bC, bC, bC, bC, bC },
				  _invalid (false)
							{ }

			/** Literal palette color, or BACKGROUNDREFERENCE | background index. */
			unsigned int _foregroundColorData [8];
			bool _invalid;
		};

		/** Describes the latest CPU transaction notified to the TED. \n
			CPU and bus-cycle references are non-owning. \n
			BusCycleData belongs to the instruction or interrupt definition,
			not to the reusable notification event. */
		struct CPUTransactionContext final
		{
			CPUTransactionContext ()
				: _cpu (nullptr), _busData (nullptr),
				  _startCycle (0), _isInterrupt (false)
							{ }

			MCHEmul::CPU* _cpu;
			const MCHEmul::BusCycleData* _busData;
			unsigned int _startCycle;
			bool _isInterrupt;
		};

		/** Retained state of one CPU cycle. \n
			The clock mode and divider are captured when the cycle starts. \n
			A suspended read resumes at NextQuarter without repeating earlier work. */
		struct CPUCycleState final
		{
			CPUCycleState
				(unsigned int cycle = 0, bool singleClock = false,
				 unsigned int clockDivider = 20, unsigned int halfCarryIn = 0)
				: _cycle (cycle),
				  _clockDivider (clockDivider),
				  _halfCarryIn (halfCarryIn),
				  _nextQuarter (0),
				  _singleClock (singleClock),
				  _accessPending (false)
							{ }

			unsigned int halfTEDCycles () const
							{ return (_singleClock ? 2u : 1u); }
			/** Number of quarter-dot edges represented by this CPU cycle. */
			unsigned int quarterDots () const
							{ return (halfTEDCycles () << 4); }
			/** Crystal clocks represented by one half of a slow TED cycle. */
			unsigned int halfMasterCycles () const
							{ return (_clockDivider >> 1); }
			unsigned int masterCycles () const
							{ return (halfTEDCycles () * halfMasterCycles ()); }
			/** Current relative phase within the surrounding slow TED cycle. */
			unsigned int phase () const
							{ return (((_halfCarryIn << 4) + _nextQuarter) & 0x1f); }

			/** CPU coordinate and divider retained when this cycle starts. */
			unsigned int _cycle, _clockDivider;
			/** Input remainder and next unprocessed quarter-dot edge. */
			unsigned int _halfCarryIn, _nextQuarter;
			/** Retained clock mode and suspension before the CPU access. */
			bool _singleClock, _accessPending;
		};

		/** Diagnostic coordinates for a TED phase event. \n
			CPUQuarter / CPUQuarterLength is the exact fraction within a CPU cycle. \n
			CPUHalf identifies the containing half; it is not the exact event fraction. \n
			This structure does not drive simulation. */
		struct DebugCycleContext
		{
			unsigned int _cpuCycle, _cpuHalf;
			unsigned int _cpuQuarter, _cpuQuarterLength;
			unsigned int _clockDivider, _masterCycles;
			unsigned int _cpuBatchStart, _cpuBatchEnd;
			unsigned int _cpuBatchIndex, _cpuBatchCycles;
			unsigned int _halfCarryIn, _halfCarryOut;
			unsigned int _irqRequestCycle;
			bool _conversionSingleClock;
		};

		/** Diagnostic events associated with one presentation position. */
		struct EventMarker final
		{
			static const unsigned int _BADLINE = 0x01;
			static const unsigned int _RASTER = 0x02;
			static const unsigned int _IRQ = 0x04;

			EventMarker ()
				: _events (0), _column (0), _row (0), _visible (false)
								{ }

			unsigned int _events;
			unsigned short _column, _row;
			bool _visible;
		};

		// Invoked from the method "simulation"....
		/** Different actions are taken attending the raster cycle. */
		virtual void treatRasterCycle (const DebugCycleContext& dC);
		/** Emits one prepared pixel using the current output colors and border. */
		void drawVisibleZone (unsigned int pixel);
		/** Draws diagnostic markers over already rendered pixels. */
		void drawOtherEvents (const EventMarker& marker);

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
		  * The first method receives a flag to point out whether the cursor has or not to be drawn,
		  * and also the position to draw the cursor (useful when the previous variable was true).
		  */
		DrawResult drawMonoColorChar (int cb, bool c, int cI, int cP);
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

		// Very internal methods...
		/** Reads RAM for video-matrix and attribute fetches, independently of CPU overlays. \n */
		virtual const MCHEmul::UByte& readRAMForVideo
			(const MCHEmul::Address& a) const = 0;
		/** When the bit 2 at register 0x12 (= 18) is active, 
			the fetch of dot or char data info has to be from ROM always. \n
			The way the method are implemented will depend on the place where the chip is running. */
		virtual bool ROMActiveToFetchCharData () const = 0;
		virtual void activeROMtoFetchCharData (bool a) = 0;

		/** Applies machine wiring effects after a scheduled CPU write. */
		virtual void applyCPUWriteEffects
			(const MCHEmul::SetMemoryCommand&)
							{ }

		private:
		enum RasterWriteKind
		{
			_WRITEVERTICAL = 0x01,
			_WRITEHORIZONTAL = 0x02,
			_WRITEFLASH = 0x04,
			_ACKRASTER = 0x08
		};

		/** Advances to the final readable phase before the first write. */
		bool advanceToCPURead (MCHEmul::CPU* cpu);
		/** Recovers a CPU cycle, optionally stopping before its access. */
		bool simulateCPUCycle
			(MCHEmul::CPU* cpu,
			 unsigned int batchStart, unsigned int batchEnd,
			 bool completeCycle = true);
		/** Advances the relative recovery cursor, skipping edges without TED work. */
		bool advanceCPUQuarters
			(MCHEmul::CPU* cpu, unsigned int target,
			 unsigned int batchStart, unsigned int batchEnd);
		/** Processes the current control or dot-clock edge once. */
		bool processCPUQuarter
			(MCHEmul::CPU* cpu,
			 unsigned int batchStart, unsigned int batchEnd);
		/** Selects timing without changing an already captured CPU period. */
		void selectConfiguration ();
		/** Applies presentation geometry at its next frame origin. */
		bool applyPresentationConfiguration (const DebugCycleContext& dC);
		/** Decodes the effective horizontal counter and internal line capture. */
		bool processRasterControl (const DebugCycleContext& dC);
		/** Advances the counters, respecting simultaneous CPU-load priorities. */
		void clockRasterDot
			(unsigned int phase, unsigned int writes,
			 const DebugCycleContext& dC);
		inline bool sampleRasterIRQ (bool matched, bool acknowledge);
		/** Returns zero outside the normal horizontal traversal. */
		inline unsigned short rasterCycle () const;
		/** Retains the existing graphics processing at the beginning of a slow pulse. */
		void beginTEDCycle (const DebugCycleContext& dC);
		/** Completes slow-pulse work and draws events before image delivery. */
		void finishTEDCycle
			(MCHEmul::CPU* cpu, unsigned int requestCycle,
			 const DebugCycleContext& dC, EventMarker& marker);
		/** Propagates the current request and reports its first diagnostic mark. */
		bool requestIRQ (MCHEmul::CPU* cpu, unsigned int cycle);
		/** Extracts one complete transaction's buffered CPU writes. \n
			Requires all inventoried destinations to be buffered before execution. \n
			The scheduler must consume the commands before another transaction starts. */
		bool extractPendingWrites ();
		/** Applies the next command when its scheduled CPU cycle is reached. */
		bool executePendingWriteAt
			(MCHEmul::CPU* cpu, unsigned int cycle,
			 unsigned int& rasterWrites);
		/** Detects a new badline and retains the scroll value that caused it. */
		inline bool detectNewBadLine ();
		/** Returns the CPU clock decision for the current TED position. \n
			Does not advance the raster or modify graphics state. */
		inline bool singleClockModeForCurrentCycle () const;
		/** Distributes one slow-clock pulse. \n
			FREEZE gates timer clocks while sound retains its existing clock policy. */
		/** Divider selected by the current FF07.6 value. */
		unsigned int clockDivider () const
							{ return (_configuration -> _divider); }
		inline void clockTEDCycle (bool countersEnabled);
		/** Copies transaction coordinates without retaining the event object. */
		inline bool prepareCPUTransaction
			(MCHEmul::CPU* cpu,
			 const MCHEmul::BusCycleData& busData, bool interrupt);
		/** Returns the CPU coordinate of the next buffered write. \n
			Requires a validated queue with a pending command. */
		inline unsigned int pendingWriteCycle () const;
		/** Updates the legacy diagnostic image from the effective counters. */
		inline void publishRasterPosition ();

		//-----
		std::string debugRasterAsString () const;
		std::string debugTimingAsString (const DebugCycleContext& dC) const;
		/** Selected controls and retained slow-pulse gate; no CPU duration is inferred. */
		std::string debugClockAsString () const;
		std::string debugInternalAsString () const;
		std::string debugTimersAsString () const;
		std::string debugInterruptAsString () const
							{ return (_TEDRegisters -> debugInterruptAsString ()); }
		DebugCycleContext debugCycleContext
			(const CPUCycleState& cS,
			 unsigned int batchStart, unsigned int batchEnd) const;
		void debugTEDCycle (const DebugCycleContext& dC);
		void debugCPUAboutToExecute
			(const MCHEmul::InstructionContextEventData* dt);
		void debugCPUAboutToExecute
			(const MCHEmul::InterruptContextEventData* dt);
		std::string debugCPUTransactionAsString () const;
		void debugCPUReadAdvance
			(MCHEmul::CPU* cpu, bool after) const;
		void debugBadLine (const DebugCycleContext& dC);
		void debugReadingVideoMatrix (const DebugCycleContext& dC);
		void debugReadingGraphics (const DebugCycleContext& dC);
		/** Logs the eight prepared codes without resolving background references. */
		void debugPreparedGraphics (const DebugCycleContext& dC) const;
		void debugTEDCycleEnd (const DebugCycleContext& dC);
		void debugFrozenTEDCycle (const DebugCycleContext& dC) const;
		void debugCPUWrite
			(MCHEmul::CPU* cpu, unsigned int cycle,
			 const MCHEmul::SetMemoryCommand& command, bool after) const;
		void debugRasterEvent
			(const DebugCycleContext& dC,
			 const char* event, unsigned short sourceLine) const;
		//-----

		protected:
		/** The type of interrupt launched from the TED. */
		int _interruptId;
		const Configuration _configurations [2];
		const Configuration* const _initialConfiguration;
		/** There three timers within the TED. */
		TEDTimer _T1, _T2, _T3;
		/** A reference to the sound part of the chip. */
		SoundFunction* _soundFunction;
		/** The memory is used also as the set of registers of the chip. */
		COMMODORE::TEDRegisters* _TEDRegisters;
		/** The number of the memory view used to read the data. */
		int _TEDView;
		/** The raster. */
		MCHEmul::Raster _raster;
		/** To draw or not to draw lines at the positions where the raster interruptions are generated. */
		bool _drawRasterInterruptPositions;
		/** To draw or not other different events. */
		bool _drawOtherEvents;

		// Implementation
		const Configuration* _configuration;
		const Configuration* _presentationConfiguration;
		/** Effective counters owned by TED; register initialization must not reset them. */
		COMMODORE::TEDRegisters::RasterPosition _rasterPosition;
		/** Horizontal decodes awaiting a slow-clock boundary. */
		bool _verticalClockPending, _flashClockPending;
		/** Equality retained by the raster-interrupt detector. */
		bool _rasterIRQMatched;
		/** Counter/graphics gating retained for the complete slow-clock pulse. */
		bool _graphicsCycleActive;
		/** Latest notified transaction. \n
			It remains available after recovery and is replaced by the next
			notification; it does not indicate that execution is still active. */
		CPUTransactionContext _cpuTransaction;
		/** CPU bound to the timed-write scheduler. */
		MCHEmul::CPU* _timedWriteCPU;
		/** All CPU write destinations, including currently inactive aliases. */
		MCHEmul::PhysicalStorageSubsets _timedWriteSubsets;
		/** Extracted commands in original CPU write order. */
		MCHEmul::SetMemoryCommands _pendingWrites;
		/** Index of the next command to apply. */
		size_t _nextPendingWrite;
		/** Whether the scheduler owns buffered CPU writes. */
		bool _cpuTimedWritesEnabled;
		/** Whether the latest notified transaction awaits recovery. */
		bool _cpuTransactionPending;
		/** Retained CPU-cycle state, including a possible suspended access. */
		CPUCycleState _cpuCycleState;
		/** Prepared prefix length, including its final suspended CPU cycle. */
		unsigned int _cpuReadAdvanceCycles;
		/** Recovered time awaiting host pacing, in crystal-clock units. \n
			Consumed after each computer iteration; not an absolute TED clock. */
		unsigned int _pendingPacingCycles;
		/** Crystal clocks accumulated within the current slow sound pulse. \n
			Cleared when that pulse is queued; not an absolute TED clock. */
		unsigned int _pendingSoundMasterCycles;
		/** CPU boundary reached by recovery. \n
			The next CPU cycle to recover starts at this coordinate. \n
			It advances after each completed CPU cycle. \n
			Zero is an initial-synchronization sentinel only without timed writes enabled. */
		unsigned int _lastCPUCycles;
		/** Completed time remainder in half-TED-cycle units, always 0 or 1. \n
			Updated after completing a CPU cycle. \n
			A suspended cycle retains its input remainder and progress in CPUCycleState. */
		unsigned int _pendingHalfCyclesFromLastExecution;
		/** The format used to draw. It has to be the same that is used by the Screen object. */
		SDL_PixelFormat* _format;
		/** Derived graphics-cycle position retained at the beginning of the slow pulse. */
		unsigned short _cycleInRasterLine;
		/** When the raster is in the first bad line this variable is set taking into account what 
			is kept in the TED register about whether the video ir or not active. \n
			In any other circunstance it kepts its value. */
		bool _videoActive;
		/** Current single-clock decision produced by TED processing. \n
			It is not a history of the modes used by previous CPU cycles. \n
			Once a CPU cycle is captured, later changes to this value
			do not alter the duration represented by that context. */
		bool _inSingleClockMode;
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

		/** Graphics fetch state and buffers.
			The 10-bit character position reload counter and the 3-bit vertical
			subaddress are stored in TEDRegisters::GraphicalInfo and shared with
			the CPU-visible registers. VC is the separate working counter.
			The current fetch and counter phases are implemented in treatRasterCycle (). */
		struct TEDGraphicInfo
		{
			TEDGraphicInfo ()
				: _VC (0), _VLMI (0),
				  _idleState (true),
				  _screenCodeData { },
				  _graphicData { },
				  _colorData { },
				  _lastScreenCodeDataRead (MCHEmul::UByte::_0),
				  _lastGraphicDataRead (MCHEmul::UByte::_0),
				  _lastColorDataRead (MCHEmul::UByte::_0)
							{ }

			unsigned short _VC, _VLMI;
			bool _idleState; 

			// Implementation
			mutable std::array <MCHEmul::UByte, _GRAPHMAXCHARCOLUMNS> _screenCodeData;
			mutable std::array <MCHEmul::UByte, _GRAPHMAXCHARCOLUMNS> _graphicData;
			mutable std::array <MCHEmul::UByte, _GRAPHMAXCHARCOLUMNS> _colorData;
			/** The last info read. */
			mutable MCHEmul::UByte _lastScreenCodeDataRead;
			mutable MCHEmul::UByte _lastGraphicDataRead;
			mutable MCHEmul::UByte _lastColorDataRead;
		};

		TEDGraphicInfo _tedGraphicInfo;
		/** Graphics prepared for the current eight-dot pulse. */
		DrawResult _drawResult;

		/** Diagnostic state retained between event positions. */
		struct EventsStatus final
		{
			EventsStatus ()
				: _badLinePending (false), _irqRequestMarked (false)
								{ }

			/** Accepted badline awaiting its first emission edge. */
			bool _badLinePending;
			/** The current continuous IRQ request has already been marked. */
			bool _irqRequestMarked;
		};

		EventsStatus _eventStatus;

		private:
		static const MCHEmul::Address _MEMORYPOSIDLE1, _MEMORYPOSIDLE2;
	};

	// ---
	inline unsigned int TED::takePendingPacingCycles ()
	{
		const unsigned int result = _pendingPacingCycles;

		_pendingPacingCycles = 0;

		return (result);
	}

	// ---
	inline void TED::readVideoMatrixAndColorRAM ()
	{
		const size_t vc = ((size_t) _tedGraphicInfo._VC) & 0x03ff;

		_tedGraphicInfo._lastScreenCodeDataRead =
			_tedGraphicInfo._screenCodeData [_tedGraphicInfo._VLMI] =
				readRAMForVideo (_TEDRegisters -> screenMemory () + vc);

		_tedGraphicInfo._lastColorDataRead =
			_tedGraphicInfo._colorData [_tedGraphicInfo._VLMI] =
				readRAMForVideo (_TEDRegisters -> attributeMemory () + vc);
	}

	// ---
	inline void TED::readGraphicalInfo ()
	{
		const COMMODORE::TEDRegisters::GraphicalInfo& gI =
			_TEDRegisters -> graphicalInfo ();

		memoryRef () -> setActiveView (_TEDView);

		if (_tedGraphicInfo._idleState) // In this state the info is read from a specific place of the memory...
			_tedGraphicInfo._lastGraphicDataRead =
				_tedGraphicInfo._graphicData [_tedGraphicInfo._VLMI] = 
					_TEDRegisters -> graphicExtendedColorTextModeActive () 
						? memoryRef () -> value (_MEMORYPOSIDLE1) : memoryRef () -> value (_MEMORYPOSIDLE2);
		else // ..in the other one the info will be read attending to the situation of the memory...
		{	 // ...and from ROM or RAM...
			const bool oldTEDROM = ROMActiveToFetchCharData (); // What the configuration is...
			const bool wantedTEDROM = _TEDRegisters -> ROMActiveToFetchCharAndBitmap (); // What is wanted...
			if (oldTEDROM != wantedTEDROM) // If different activate the vision that is wanted...
				activeROMtoFetchCharData (wantedTEDROM);

			const size_t charIndex =
				(size_t) _tedGraphicInfo._screenCodeData [_tedGraphicInfo._VLMI].value () &
				_TEDRegisters -> characterIndexMask ();

			_tedGraphicInfo._lastGraphicDataRead =
				_tedGraphicInfo._graphicData [_tedGraphicInfo._VLMI] = 
					_TEDRegisters -> textMode () 
						? memoryRef () -> value (_TEDRegisters -> charDataMemory () + 
							(charIndex << 3) + gI._currentCharacterRow)
						: memoryRef () -> value (_TEDRegisters -> bitmapMemory () + 
							(((size_t) (_tedGraphicInfo._VC & 0x03ff)) << 3) + 
							  (size_t) gI._currentCharacterRow); // This is always between 0 and 7...

			if (oldTEDROM != wantedTEDROM)
				activeROMtoFetchCharData (oldTEDROM); // and finally restore it...
		}
		
		memoryRef () -> setCPUView ();
	}

	// ---
	inline bool TED::sampleRasterIRQ (bool matched, bool acknowledge)
	{
		const bool raised = matched && !_rasterIRQMatched;

		_rasterIRQMatched = matched;

		if (raised && !acknowledge)
		{
			_TEDRegisters -> activateRasterIRQ ();

			return (true);
		}

		return (false);
	}

	// ---
	inline unsigned short TED::rasterCycle () const
	{
		if (_rasterPosition._horizontal >= 456)
			return (0);

		return
			(((_rasterPosition._horizontal >= 360)
				? (_rasterPosition._horizontal - 360)
				: (_rasterPosition._horizontal + 96)) >> 3) + 1;
	}

	// ---
	inline bool TED::detectNewBadLine ()
	{
		const unsigned char scrollY = _TEDRegisters -> verticalScrollPosition ();

		const bool result =
			_videoActive &&
			_rasterPosition._videoLine >= _FIRSTBADLINE &&
			_rasterPosition._videoLine <= _LASTBADLINE &&
			(unsigned char) (_rasterPosition._videoLine & 0x07) == scrollY &&
			_lastBadLineScrollY != (int) scrollY;

		if (result)
			_lastBadLineScrollY = scrollY;

		return (result);
	}

	// ---
	inline bool TED::singleClockModeForCurrentCycle () const
	{
		const unsigned short cycle = rasterCycle ();

		return
			(_TEDRegisters -> freezeActive () ||
			 _TEDRegisters -> singleClockModeForced () ||
			 (cycle >= 49 && cycle <= 52) ||
			 (cycle >= 5 && cycle <= 48 &&
			  _rasterPosition._videoLine <= 204));
	}

	// ---
	inline void TED::clockTEDCycle (bool countersEnabled)
	{
		if (countersEnabled)
		{
			_T1.clock ();
			_T2.clock ();
			_T3.clock ();
		}

		_soundFunction -> addTEDCycle (_pendingSoundMasterCycles);
		_pendingSoundMasterCycles = 0;
	}

	// ---
	inline bool TED::prepareCPUTransaction
		(MCHEmul::CPU* cpu,
		 const MCHEmul::BusCycleData& busData, bool interrupt)
	{
		if (_cpuTimedWritesEnabled &&
			(cpu != _timedWriteCPU ||
			 _cpuTransactionPending ||
			 _cpuCycleState._accessPending ||
			 !_pendingWrites.empty () ||
			 _lastCPUCycles != cpu -> clockCycles ()))
		{
			_error = MCHEmul::_CHIP_ERROR;

			return (false);
		}

		_cpuTransaction._cpu = cpu;
		_cpuTransaction._busData = &busData;
		_cpuTransaction._startCycle = cpu -> clockCycles ();
		_cpuTransaction._isInterrupt = interrupt;
		_cpuTransactionPending = _cpuTimedWritesEnabled;
		_cpuReadAdvanceCycles = 0;

		return (true);
	}

	// ---
	inline unsigned int TED::pendingWriteCycle () const
	{
		assert (_cpuTransaction._busData != nullptr);
		assert (_nextPendingWrite < _pendingWrites.size ());

		return
			(_cpuTransaction._startCycle +
			 (unsigned int) (_cpuTransaction._busData ->
				writeCycle (_nextPendingWrite)));
	}

	// ---
	inline void TED::publishRasterPosition ()
	{
		// Preserve the diagnostic structure as a derived image.
		_TEDRegisters -> setCurrentRasterPosition
			(_rasterPosition._vertical, _rasterPosition._horizontal);
	}

	/** The version para PAL systems. */
	class TED_PAL final : public TED
	{
		public:
		// The structure of the screen...
		static const MCHEmul::RasterData _VRASTERDATA;
		static const MCHEmul::RasterData _HRASTERDATA;
		// Important data for the internal working of the chip...
		static const unsigned short _SCREENFREQUENCY			= 50;	// Hz
		static const unsigned short _DIVIDERCPUCLOCK			= 20;	// The CPU clock is /20 the TED clock
		static const unsigned short _DIVIDERSOUNDCLOCK			= 160;	// The sound clock is /160 = _DIVIDERCPUCLOCK << 3 the TED clock

		TED_PAL (int intId, int vV, MCHEmul::SoundLibWrapper* wS,
			const TED::Configuration& pal = TED::_PALCONFIGURATION,
			const TED::Configuration& ntsc = TED::_NTSCCONFIGURATION);
	};

	/** The version para NTSC systems. */
	class TED_NTSC final : public TED
	{
		public:
		// The structure of the screen...
		static const MCHEmul::RasterData _VRASTERDATA;
		static const MCHEmul::RasterData _HRASTERDATA;
		// Important data for the internal working of the chip...
		static const unsigned short _SCREENFREQUENCY			= 60;	// Hz;
		static const unsigned short _DIVIDERCPUCLOCK			= 16;	// The CPU clock is /20 the TED clock
		static const unsigned short _DIVIDERSOUNDCLOCK			= 128;	// The sound clock is /128 = _DIVIDERCPUCLOCK << 3 the TED clock

		TED_NTSC (int intId, int vV, MCHEmul::SoundLibWrapper* wS,
			const TED::Configuration& pal = TED::_PALCONFIGURATION,
			const TED::Configuration& ntsc = TED::_NTSCCONFIGURATION);
	};
}

#endif
  
// End of the file
/*@}*/
