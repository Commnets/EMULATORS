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
			void addTEDCycles (unsigned int nC)
							{ _pendingSoundClockCycles += nC; }

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
				 size_t samplesGenerated) const;
			//-----

			public:
			// Implementation
			/** Complete TED sound-clock pulses waiting to be simulated. */
			unsigned int _pendingSoundClockCycles;
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
		static const unsigned short _LASTBADLINE	= 0xc7;

		/** Data about the size of the screen */
		static const unsigned short _GRAPHMAXCHARLINES		= 25;	// Not taking into account reductions in the size
		static const unsigned short _GRAPHMAXCHARCOLUMNS	= 40;
		static const unsigned short _GRAPHMAXBITMAPCOLUMNS	= 320;	// Not taking into account double coulors
		static const unsigned short _GRAPHMAXBITMAPROWS		= 200;

		/** Specific classes for PAL & NTSC have been created giving this data as default. \n
			The TED constructor receives info over the raster data, the frequency it works, the memory view to use,
			The number of cycles of every raster line (different depending on the TED version)
			and additional attributes. */
		TED (int intId, unsigned short clkcpum, unsigned short sfq,
			 const MCHEmul::RasterData& vd, const MCHEmul::RasterData& hd,
			 int vV, MCHEmul::SoundLibWrapper* sW, const MCHEmul::Attributes& attrs = { });

		virtual ~TED () override;

		/** To draw or not to draw raster interrupt positions. */
		void setDrawRasterInterruptPositions (bool d)
							{ _drawRasterInterruptPositions = d; }
		/** Whether to draw other events related with the TED raster.
			Borders, bad lines mainly. */
		void setDrawOtherEvents (bool d)
							{ _drawOtherEvents = d; }

		unsigned short screenFrequency () const
							{ return (_screenfrequency); }
		unsigned short timesFasterThanCPU () const
							{ return (_timesFasterThanCPU); }

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

		/** Single clock active? */
		bool inSingleClockMode () const
							{ return (_inSingleClockMode); }

		/** To get the raster info. */
		const MCHEmul::Raster& raster () const
							{ return (_raster); }
		/** Returns and clears recovered time awaiting host pacing. \n
			The returned unit is one fast CPU-clock period. */
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
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		// To get snapshots of the memory...
		// They are used in some commands...
		/** content of the screen. */
		MCHEmul::UBytes screenMemorySnapShot (MCHEmul::CPU* cpu) const;
		/** The content of the attribute memory. */
		MCHEmul::UBytes attributeMemorySnapShot (MCHEmul::CPU* cpu) const;
		/** The content of the bitmap memory. */
		MCHEmul::UBytes bitmapMemorySnapShot (MCHEmul::CPU* cpu) const;

		// To print out the form of the different graphic elements
		// taking into account the graphic mode active
		/** The characters. */
		MCHEmul::Strings charsDrawSnapshot (MCHEmul::CPU* cpu, 
			const std::vector <size_t>& chrs = { }) const;

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

		/** Describes one CPU cycle, not an entire instruction. \n
			The selected clock mode remains fixed throughout this CPU cycle. */
		struct CPUCycleContext final
		{
			CPUCycleContext (unsigned int cycle, bool singleClock)
				: _cycle (cycle), _singleClock (singleClock)
							{ }

			unsigned int halfTEDCycles () const
							{ return (_singleClock ? 2u : 1u); }

			/** CPU clock coordinate of the cycle being processed. */
			unsigned int _cycle;
			/** Clock mode applied to this cycle, independently of later requests. */
			bool _singleClock;
		};

		/** Recovery stages for one CPU cycle. \n
			These are scheduler stages, not names of physical TED clock signals. */
		enum class CPUCyclePhase
		{
			_NOTSTARTED,
			_BEFOREACCESS,
			_COMPLETE
		};

		/** CPU-cycle recovery contract. \n
			1. Select and retain the duration of the CPU cycle. \n
			2. Perform TED work preceding the CPU access. \n
			3. Perform or apply the CPU access. \n
			4. Complete the remaining TED work. \n
			5. Advance the CPU recovery cursor only when the cycle is complete. \n
			_NOTSTARTED has all work pending; _BEFOREACCESS awaits the CPU access
			and subsequent work; _COMPLETE has no work pending for that CPU cycle. \n
			A cycle suspended before its access must resume without repeating
			the preceding TED work. Completing a fast CPU cycle does not necessarily
			complete the surrounding slow TED cycle. */

		/** Retains a CPU cycle suspended before its access. \n
			The selected duration and already executed graphics work must survive
			until CPU execution returns. \n
			For a TED pulse completed after the CPU access, _graphicsStarted
			retains whether its counters and graphics were enabled before access. */
		struct CPUCycleRecovery final
		{
			CPUCycleRecovery ()
				: _context (0, false),
				  _phase (CPUCyclePhase::_NOTSTARTED),
				  _halfCarryIn (0), _graphicsStarted (false)
							{ }

			CPUCycleContext _context;
			CPUCyclePhase _phase;
			unsigned int _halfCarryIn;
			/** Graphics work already performed for a pulse awaiting completion. \n
				That pulse must finish even if the access enables FREEZE. \n
				A frozen pulse must not start graphics after the access. */
			bool _graphicsStarted;
		};

		/** Diagnostic coordinates for a completed TED cycle. \n
			CPUHalf expresses half a CPU-clock unit. \n
			The timestamp identifies TED-cycle completion, not its beginning. \n
			This structure does not drive simulation. */
		struct DebugCycleContext
		{
			unsigned int _cpuCycle, _cpuHalf;
			unsigned int _cpuBatchStart, _cpuBatchEnd;
			unsigned int _cpuBatchIndex, _cpuBatchCycles;
			unsigned int _halfCarryIn, _halfCarryOut;
			unsigned int _irqRequestCycle;
			bool _conversionSingleClock;
		};

		// Invoked from the method "simulation"....
		/** Different actions are taken attending the raster cycle. */
		virtual void treatRasterCycle (const DebugCycleContext& dC);
		/** Treat the visible zone.
			Draws the graphics, detect collions, and finally draw the border. */
		void drawVisibleZone (MCHEmul::CPU* cpu);
		/** Invoked from the previous one, just to draw. \n
		  *	The parameters are the context of the draw. \n
		  *	@see also DrawContext and DrawResult structures. */
		void drawGraphicsAndMoveToScreen (const DrawContext& dC);
		/** To draw events that happened inside the TED, if the variable _drawOtherEvents is set. */
		void drawOtherEvents ();

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

		// The last part...
		/** To move the graphics drawn to the screen. \n
			The info move is the text/bitmap info that has been already draw to the screen. \n
			The text/bitmap foreground pixels are composed with background and border colors. */
		void drawResultToScreen (const DrawResult& cT, const DrawContext& dC);

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
		/** Advances to the final readable phase before the first write. */
		bool advanceToCPURead (MCHEmul::CPU* cpu);
		/** Recovers a CPU cycle, optionally stopping before its access. */
		bool simulateCPUCycle
			(MCHEmul::CPU* cpu,
			 unsigned int batchStart, unsigned int batchEnd,
			 bool completeCycle = true);
		/** Executes the work preceding the current CPU access. */
		void beginCPUCycle
			(MCHEmul::CPU* cpu,
			 unsigned int batchStart, unsigned int batchEnd);
		/** Applies the access and completes the retained CPU cycle. */
		bool finishCPUCycle
			(MCHEmul::CPU* cpu,
			 unsigned int batchStart, unsigned int batchEnd);
		/** Processes graphics fetches before the scheduled CPU access. \n
			Drawing and raster advancement follow in finishTEDCycle(). */
		void beginTEDCycle (const DebugCycleContext& dC);
		/** Draws and completes the current cycle, including raster, timers and IRQ. */
		void finishTEDCycle
			(MCHEmul::CPU* cpu, unsigned int requestCycle,
			 const DebugCycleContext& dC);
		/** Extracts one complete transaction's buffered CPU writes. \n
			Requires all inventoried destinations to be buffered before execution. \n
			The scheduler must consume the commands before another transaction starts. */
		bool extractPendingWrites ();
		/** Applies the next command when its scheduled CPU cycle is reached. */
		bool executePendingWriteAt
			(MCHEmul::CPU* cpu, unsigned int cycle);
		/** Detects a new badline and retains the scroll value that caused it. */
		inline bool detectNewBadLine ();
		/** Returns the CPU clock decision for the current TED position. \n
			Does not advance the raster or modify graphics state. */
		inline bool singleClockModeForCurrentCycle () const;
		/** Distributes one slow-clock pulse. \n
			FREEZE gates timer clocks while sound retains its existing clock policy. */
		inline void clockTEDCycle (bool countersEnabled);
		/** Copies transaction coordinates without retaining the event object. */
		inline bool prepareCPUTransaction
			(MCHEmul::CPU* cpu,
			 const MCHEmul::BusCycleData& busData, bool interrupt);
		/** Returns the CPU coordinate of the next buffered write. \n
			Requires a validated queue with a pending command. */
		inline unsigned int pendingWriteCycle () const;

		//-----
		std::string debugRasterAsString () const;
		std::string debugInternalAsString () const;
		std::string debugTimersAsString () const;
		std::string debugInterruptAsString () const
							{ return (_TEDRegisters -> debugInterruptAsString ()); }
		DebugCycleContext debugCycleContext
			(const CPUCycleContext& cC,
			 unsigned int batchStart, unsigned int batchEnd,
			 unsigned int halfCarryIn, unsigned int halfCarryOut) const;
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
		void debugTEDCycleEnd (const DebugCycleContext& dC);
		void debugFrozenTEDCycle (const DebugCycleContext& dC) const;
		void debugCPUWrite
			(MCHEmul::CPU* cpu, unsigned int cycle,
			 const MCHEmul::SetMemoryCommand& command, bool after) const;
		//-----

		protected:
		/** The type of interrupt launched from the TED. */
		int _interruptId;
		/** How faster the TED is on respect the CPU. */
		unsigned short _timesFasterThanCPU;
		/** The frequency of the TED: */
		unsigned short _screenfrequency;
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
		/** To draw or not to draw lines at the positions where the raster interruptions are generated. */
		bool _drawRasterInterruptPositions;
		/** To draw or not other different events. */
		bool _drawOtherEvents;

		// Implementation
		/** Internal counter that increments in 1 when the frame is drawn. 
			This internal counetr is used to determine whether the internal cursor must blink. 
			It reaches always just half of the frequency. */
		unsigned short _timesFrameDrawn;
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
		CPUCycleRecovery _cpuCycleRecovery;
		/** Prepared prefix length, including its final suspended CPU cycle. */
		unsigned int _cpuReadAdvanceCycles;
		/** Recovered time awaiting host pacing, in fast CPU-cycle units. \n
			Consumed after each computer iteration; not an absolute TED clock. */
		unsigned int _pendingPacingCycles;
		/** CPU boundary reached by recovery. \n
			The next CPU cycle to recover starts at this coordinate. \n
			It advances after each completed CPU cycle. \n
			Zero is an initial-synchronization sentinel only without timed writes enabled. */
		unsigned int _lastCPUCycles;
		/** Time remainder in half-TED-cycle units, always 0 or 1. \n
			One fast CPU cycle contributes one unit; one slow CPU cycle contributes two. \n
			For a CPU cycle suspended before access, this already contains its output
			remainder; CPUCycleRecovery retains the work still awaiting completion. */
		unsigned int _pendingHalfCyclesFromLastExecution;
		/** The format used to draw. It has to be the same that is used by the Screen object. */
		SDL_PixelFormat* _format;
		/** When a raster line is processed, it is necessary to know which cycle is being processed. 
			The number of max cycles is get from the method (@see) "cyclesPerRasterLine". */
		unsigned short _cycleInRasterLine;
		/** When the raster is in the first bad line this variable is set taking into account what 
			is kept in the TED register about whether the video ir or not active. \n
			In any other circunstance it kepts its value. */
		bool _videoActive;
		/** Current single-clock decision produced by TED processing. \n
			It is not a history of the modes used by previous CPU cycles. \n
			Once a CPUCycleContext is constructed, later changes to this value
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
				  _cursorHardwareStatus (false),
				  _screenCodeData (std::vector <MCHEmul::UByte> (40, MCHEmul::UByte::_0)),
				  _graphicData (std::vector <MCHEmul::UByte> (40, MCHEmul::UByte::_0)),
				  _colorData (std::vector <MCHEmul::UByte> (40, MCHEmul::UByte::_0)),
				  _lastScreenCodeDataRead (MCHEmul::UByte::_0),
				  _lastGraphicDataRead (MCHEmul::UByte::_0),
				  _lastColorDataRead (MCHEmul::UByte::_0)
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


			unsigned short _VC, _VLMI;
			bool _idleState; 
			bool _cursorHardwareStatus;

			// Implementation...
			// This one doesn't actually exist "in" the TED chip,
			// but is used when he left border has to be partially drawn.
			// After doing so, the _ffMBorder will become false...
			mutable MCHEmul::UBytes _screenCodeData;
			mutable MCHEmul::UBytes _graphicData; 
			mutable MCHEmul::UBytes _colorData;
			/** The last info read. */
			mutable MCHEmul::UByte _lastScreenCodeDataRead;
			mutable MCHEmul::UByte _lastGraphicDataRead;
			mutable MCHEmul::UByte _lastColorDataRead;
		};

		TEDGraphicInfo _tedGraphicInfo;

		/** The events that can be drawn
			associated to the movement of the raster line. */
		struct EventsStatus
		{
			/** The bad line to hightlight. */
			unsigned short _badLine;
		};

		mutable EventsStatus _eventStatus;

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

			// How many char are reacheable?
			unsigned char characterIndexMask = 0xff; // 255 by default...
			if (_TEDRegisters -> graphicExtendedColorTextModeActive ()) 
				characterIndexMask = 0x3f; // But in this mode only 64 ones are...
			else if (_TEDRegisters -> reverseVideoActive ()) 
				characterIndexMask = 0x7f; // Or 128 in this other one...
			// The index to access to the mempry is the position in the charDataMemory filetered by the mask...
			const size_t charIndex = (size_t) _tedGraphicInfo._screenCodeData [_tedGraphicInfo._VLMI].value () & 
				characterIndexMask;

			_tedGraphicInfo._lastGraphicDataRead =
				_tedGraphicInfo._graphicData [_tedGraphicInfo._VLMI] = 
					_TEDRegisters -> textMode () 
						? memoryRef () -> value (_TEDRegisters -> charDataMemory () + 
							(charIndex /** See above. */ << 3) + gI._currentCharacterRow)
						: memoryRef () -> value (_TEDRegisters -> bitmapMemory () + 
							(((size_t) (_tedGraphicInfo._VC & 0x03ff)) << 3) + 
							  (size_t) gI._currentCharacterRow); // This is always between 0 and 7...

			if (oldTEDROM != wantedTEDROM)
				activeROMtoFetchCharData (oldTEDROM); // and finally restore it...
		}
		
		memoryRef () -> setCPUView ();
	}

	// ---
	inline bool TED::detectNewBadLine ()
	{
		const unsigned char scrollY = _TEDRegisters -> verticalScrollPosition ();

		const bool result =
			_videoActive &&
			_raster.currentLine () >= _FIRSTBADLINE &&
			_raster.currentLine () <= _LASTBADLINE &&
			(unsigned char) (_raster.currentLine () & 0x07) == scrollY &&
			_lastBadLineScrollY != (int) scrollY;

		if (result)
			_lastBadLineScrollY = scrollY;

		return (result);
	}

	// ---
	inline bool TED::singleClockModeForCurrentCycle () const
	{
		return
			(_TEDRegisters -> freezeActive () ||
			 _TEDRegisters -> singleClockModeForced () ||
			 (_cycleInRasterLine >= 49 && _cycleInRasterLine <= 52) ||
			 (_cycleInRasterLine >= 5 && _cycleInRasterLine <= 48 &&
			  _raster.currentLine () >= 0 && _raster.currentLine () <= 204));
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

		_soundFunction -> addTEDCycles (1);
	}

	// ---
	inline bool TED::prepareCPUTransaction
		(MCHEmul::CPU* cpu,
		 const MCHEmul::BusCycleData& busData, bool interrupt)
	{
		if (_cpuTimedWritesEnabled &&
			(cpu != _timedWriteCPU ||
			 _cpuTransactionPending ||
			 _cpuCycleRecovery._phase == CPUCyclePhase::_BEFOREACCESS ||
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
		static constexpr unsigned short _CYCLESPERRASTERLINE	= 71;

		TED_PAL (int intId, unsigned short clkcpum, int vV, MCHEmul::SoundLibWrapper* wS);
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
		static constexpr unsigned short _CYCLESPERRASTERLINE	= 65;

		TED_NTSC (int intId, unsigned short clkcpum, int vV, MCHEmul::SoundLibWrapper* wS);
	};
}

#endif
  
// End of the file
/*@}*/
