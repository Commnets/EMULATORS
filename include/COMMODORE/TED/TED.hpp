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
 *	   immediately before the prepared read-capture edge. DMA waits advance
 *	   TED time without consuming the retained logical CPU bus cycle. \n
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
 *	simulate validates nominal CPU duration and extracts buffered writes once.
 *	The transaction can remain pending across subsequent CPU stop ticks. \n
 *	It calls simulateCPUCycle only for time not already recovered by the
 *	read prefix. A prefix may include DMA waits beyond the observed CPU clock;
 *	those periods are not simulated again while the stopped CPU catches up. \n
 *	An already suspended cycle resumes from its retained cursor. Each physical
 *	period either consumes a logical bus cycle or accumulates one DMA wait;
 *	writes use their declared bus position plus the preceding wait count. \n
 *	Each completed CPU cycle commits its half-pulse remainder and pacing time,
 *	publishes the diagnostic raster copy and advances _cpuRecovery._nextCycle. \n
 *	updateCPUStop requests only the remaining compensation through setStop,
 *	outside CPUAboutToExecute. At the instruction end, BA can also retain the
 *	next implicit opcode fetch. The transaction is released after its effective
 *	CPU boundary, all writes and that next-fetch hold have been consumed. \n
 *	Without timed writes, the first simulate
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
 *	advanceCPUQuarters visits control and dot-clock edges directly and calls
 *	processCPUQuarter once for each relevant position. The retained cursor
 *	stops before the read-access edge and resumes there without repeating work. \n
 *	Control edges process pending raster controls. At phase zero the graphics
 *	gate is captured and beginTEDCycle performs retained row-end counter
 *	work. Memory captures and retained-cell transfers run separately at phases
 *	15 and 31, before coincident CPU writes. \n
 *	On emission edges, scheduled CPU writes precede pixel output. Raster IRQ
 *	sampling uses the operands captured before simultaneous CPU loads.
 *	clockBorders updates the retained vertical permission and horizontal
 *	window after CPU writes and before output, including invisible dots.
 *	RasterData supplies presentation geometry rather than the border mask.
 *	The pixel is selected from the output shift register before shifting or
 *	loading its next cell and before presentation/hardware-counter advancement. \n
 *	ECM, bitmap, multicolor and reverse controls are accepted after output at
 *	the end of the slow pulse, including frozen pulses. The preceding controls
 *	govern that edge's pixel, shift, cell load and address preparation. \n
 *	At slow-pulse completion, finishTEDCycle performs the enabled counter,
 *	timer and sound work and propagates pending IRQ requests. Other request
 *	points preserve the existing raster-event and CPU-boundary handling. \n
 *	CPU clock selection uses retained external-fetch and refresh windows. \n
 *	H=296 closes external fetching and opens refresh, H=336 closes refresh,
 *	and H=400 opens external fetching. Display enable qualifies only the
 *	external-fetch request. The slow-boundary decision affects following
 *	CPU periods; the current period retains its captured duration. \n
 *	Changing CPU speed does not multiply the timer or sound clocks. FREEZE
 *	gating is retained for the slow pulse; sound scheduling continues through
 *	the existing frozen-pulse path. \n
 *
 *	DMA arbitration. \n
 *	The selected FPGATED timing profile opens the request window after H=407
 *	and closes it after the H=295 capture. Three slow-clock lead intervals
 *	allow pending writes to finish; reads wait from the first BA request.
 *	DMA ownership qualifies matrix/attribute RAM capture, independently of
 *	the H=432..296 address-generation window and graphics-half fetches. \n
 *	CPUTransactionContext retains logical CPU progress and pending work. \n
 *	CPUTimedWrites retains the CPU binding, destinations and ordered writes. \n
 *	CPURecoveryState retains elapsed time and a possibly suspended CPU cycle,
 *	using the CPU recovery coordinate, with no absolute TED clock. \n
 *	This remains instruction-granular CPU execution. Reads after writes,
 *	interrupt vector reads and semantic side effects are not microcoded.
 *	A frozen, asserted DMA request yields from prefix recovery to the CPU
 *	stop scheduler instead of spinning indefinitely in the notification. \n
 *
 *	Diagnostic event markers. \n
 *	TEDEVENTS draws accepted raster events as complete presentation rows,
 *	using the captured raster number after that row has finished rendering.
 *	Actual matrix/attribute captures mark two already emitted pixels each.
 *	The first propagated TED IRQ request retains its separate point marker.
 *	EventsStatus retains pending rows, labels and the current output-edge Marker.
 *	The marker is reset before each emission phase with TEDEVENTS enabled.
 *	A single-row mask preserves points under the completed raster line.
 *	Top labels wait for their five glyph rows; geometry changes clear this state.
 *	The diagnostic context is owned by TED and captured before each phase.
 *	Internal methods consume that snapshot without passing it through the call chain.
 *	Phase recovery is sequential and must not be recursively entered by callbacks.
 *	These diagnostics do not imply CPU interrupt acceptance or physical latency.
 */

#ifndef __COMMODORE_TED__
#define __COMMODORE_TED__

#include <algorithm>
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
		/** Enables transfer groups, accepted raster lines and TED IRQ request markers. */
		inline void setDrawOtherEvents (bool d);

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
		unsigned int takePendingPacingCycles ()
							{ return (_cpuRecovery.takePendingPacingCycles ()); }

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
		/** Retained operands belonging to one graphics cell. */
		struct VideoCell final
		{
			VideoCell ()
				: _character (0), _attribute (0), _graphics (0),
				  _cursor (false), _valid (false)
							{ }

			unsigned char _character, _attribute, _graphics;
			bool _cursor, _valid;
		};

		/** Transfers between memory capture and the live pixel shift register. */
		struct VideoOutputState final
		{
			VideoOutputState ()
				: _next (), _current (), _waiting (), _output (),
				  _pixel (0), _scrollX (0), _control1 (0), _control2 (0),
				  _doubleShift (false), _shiftActive (false),
				  _verticalOpen (false), _horizontalOpen (false)
							{ }

			bool multicolor () const
							{ return ((_control2 & 0x10) != 0 && (_control1 != 0 || (_output._attribute & 0x08) != 0)); }

			VideoCell _next, _current, _waiting, _output;
			unsigned char _pixel, _scrollX;
			/** Effective FF06 bits 6..5 and FF07 bits 7,4, accepted at slow-pulse end. */
			unsigned char _control1, _control2;
			bool _doubleShift, _shiftActive;
			/** Vertical permission and retained horizontal border window. */
			bool _verticalOpen, _horizontalOpen;
		};

		/** Describes the latest CPU transaction notified to the TED. \n
			CPU and bus-cycle references are non-owning. \n
			BusCycleData belongs to the instruction or interrupt definition,
			not to the reusable notification event. \n
			Completed transaction metadata remains available for diagnostics. */
		struct CPUTransactionContext final
		{
			CPUTransactionContext
				(MCHEmul::CPU* cpu = nullptr,
				 const MCHEmul::BusCycleData* busData = nullptr,
				 bool interrupt = false, bool pending = false)
				: _cpu (cpu), _busData (busData),
				  _startCycle (cpu == nullptr ? 0 : cpu -> clockCycles ()),
				  _isInterrupt (interrupt),
				  _logicalCycle (0), _waitCycles (0), _firstStopPosition (0),
				  _writesExtracted (false), _stopRequested (false),
				  _pending (pending), _readAdvanceCycles (0)
							{ }

			/** Releases pending work while retaining the last transaction for diagnostics. */
			inline void finish ();
			/** Accounts for one recovered logical cycle or DMA wait. */
			inline void advance (bool held, unsigned int recoveryCycle);

			MCHEmul::CPU* _cpu;
			const MCHEmul::BusCycleData* _busData;
			unsigned int _startCycle;
			bool _isInterrupt;
			/** Bus-cycle progress excludes recovered DMA wait periods. */
			unsigned int _logicalCycle, _waitCycles, _firstStopPosition;
			bool _writesExtracted, _stopRequested;
			/** Whether bus work or wait compensation remains pending. */
			bool _pending;
			/** Planned logical prefix length; waits do not consume its bus cycles. */
			unsigned int _readAdvanceCycles;
		};

		/** CPU binding, buffered destinations and ordered write commands. */
		struct CPUTimedWrites final
		{
			CPUTimedWrites ()
				: _cpu (nullptr), _subsets (), _commands (),
				  _next (0), _enabled (false)
							{ }

			/** Consumes the queue while preserving its allocated capacity. */
			inline void clearCommands ();

			MCHEmul::CPU* _cpu;
			/** Includes currently inactive aliases. */
			MCHEmul::PhysicalStorageSubsets _subsets;
			/** Commands retain their original CPU write order. */
			MCHEmul::SetMemoryCommands _commands;
			size_t _next;
			bool _enabled;
		};

		/** BA lead intervals followed by TED ownership of the CPU bus phase. */
		enum class DMAState : unsigned char
		{
			_IDLE, _WARNING1, _WARNING2, _WARNING3, _ACTIVE
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
				  _accessPending (false), _held (false)
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
			bool _singleClock, _accessPending, _held;
		};

		/** Recovered CPU time and the possibly suspended CPU cycle. */
		struct CPURecoveryState final
		{
			CPURecoveryState
				(unsigned int nextCycle = 0, unsigned int halfCarry = 0)
				: _cycle (), _nextCycle (nextCycle),
				  _halfCarry (halfCarry), _pendingPacingCycles (0)
							{ }

			/** Commits elapsed time only after the retained cycle has completed. */
			inline void commitElapsedTime ();
			/** Returns and clears crystal clocks awaiting host pacing. */
			inline unsigned int takePendingPacingCycles ();

			CPUCycleState _cycle;
			/** The next CPU cycle to recover starts at this coordinate. \n
				Zero is an initial-synchronization sentinel only without timed writes. */
			unsigned int _nextCycle;
			/** Completed remainder in half-TED-cycle units, always zero or one. \n
				A suspended cycle retains its input remainder in CPUCycleState. */
			unsigned int _halfCarry;
			/** Recovered crystal clocks awaiting host pacing, not an absolute clock. */
			unsigned int _pendingPacingCycles;
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

		/** Capture boundaries within one slow TED pulse. \n
			Values use CPUCycleState::phase (), not CPU-cycle indexes. \n
			The opposite memory operation prepares its address at each boundary. */
		enum class VideoFetchPhase : unsigned int
		{
			_GRAPHICS = 15,
			_MATRIX = 31
		};

		/** Meaning of the address retained for the following memory half-cycle. */
		enum class VideoReadKind
		{
			_NONE,
			_MATRIX,
			_CHARACTER,
			_BITMAP,
			_IDLE
		};

		/** One pending memory read, shared by alternating fetch half-cycles. \n
			The address and destination slot are selected before data capture. \n
			Matrix identifies a bus read; capture enables select its buffer destinations. */
		struct PendingVideoRead final
		{
			PendingVideoRead ()
				: _kind (VideoReadKind::_NONE), _address (), _index (0)
							{ }

			VideoReadKind _kind;
			MCHEmul::Address _address;
			size_t _index;
		};

		/** Video fetch timing contract. \n
			At VideoFetchPhase::_GRAPHICS, capture the preceding graphics read
			and prepare the following attribute or character-code address. \n
			At VideoFetchPhase::_MATRIX, capture the preceding attribute or
			character-code read and prepare the following graphics address. \n
			A prepared address belongs to a pending read. Its later capture
			must not reconstruct that address from registers modified meanwhile. \n
			Attribute and character-code captures use distinct reload conditions
			and update only their corresponding retained buffer. \n
			The graphics reader consumes the character code available at its
			address-selection boundary, not a future buffer value. \n
			Capture uses the counter values preceding its increment. Counter
			updates and buffer transfers on the same edge must have explicit order. \n
			CPU double speed changes how much of this pulse is recovered by one
			CPU cycle; it does not change these relative fetch boundaries. \n
			FREEZE and the effective fetch windows qualify each operation. \n
			The scheduler executes these captures before coincident CPU writes.
			Row reloads retain the existing raster-cycle approximation. Output
			loads retained cells after emission at the sampled scroll boundary. */

		/** Current fetch integration status. \n
			H=432 opens address generation and reloads working positions;
			H=296 closes it without cancelling the last pending graphics read. \n
			The circular-buffer index advances at enabled matrix boundaries,
			independently of address reloads and the graphics working counter. \n
			Row-end work retains its legacy cycle-57 approximation. Retained cells
			feed the pixel shift register. Slow-boundary controls and retained border
			windows follow the selected implementation profile, not a new silicon measurement. */

		/** Normal horizontal traversal, with the existing phase alignment. \n
			Matrix-plane address positions: 435, 443, ..., 291. \n
			Matrix-plane capture positions: 439, 447, ..., 295. \n
			Graphics address positions: 439, 447, ..., 295. \n
			Graphics capture positions: 443, 451, ..., 299. \n
			Each enabled traversal contains 40 entries and wraps through H=0. \n
			These coordinates describe the normal sequence, not an index formula. \n
			FF1E writes, interrupted windows and FREEZE require retained window
			and counter state; do not reconstruct buffer indexes from H alone. */

		/** Persistent diagnostic state and the marker for one output edge. */
		struct EventsStatus final
		{
			static const unsigned short _NOROW = 0xffff;

			/** Events captured before presentation advances. */
			struct Marker final
			{
				static const unsigned int _BADLINE = 0x01;
				static const unsigned int _RASTER = 0x02;
				static const unsigned int _IRQ = 0x04;

				Marker ()
					: _events (0), _column (0), _row (0), _rasterLine (0),
					  _visible (false), _rowVisible (false),
					  _emitted (false), _afterVisible (false)
							{ }

				unsigned int _events;
				unsigned short _column, _row, _rasterLine;
				bool _visible, _rowVisible, _emitted, _afterVisible;
			};

			EventsStatus (size_t columns = 0)
				: _marker (), _irqRequestMarked (false),
				  _row (_NOROW), _rasterLine (0),
				  _rowFinished (false), _rasterPending (false),
				  _marks (columns, 0),
				  _earlyRasterLines
					{ _NOROW, _NOROW, _NOROW, _NOROW, _NOROW }
							{ }

			/** Resets diagnostics while reusing marker storage when geometry permits. */
			inline void reset (size_t columns, bool irqMarked = false);
			/** Starts a different presentation row and clears its point markers. */
			inline void beginRow (unsigned short row);
			/** Drops frame annotations without rearming a continuous IRQ request. */
			inline void finishFrame ();

			/** Events captured for the current output edge. */
			Marker _marker;
			bool _irqRequestMarked;
			unsigned short _row, _rasterLine;
			bool _rowFinished, _rasterPending;
			/** Point markers already drawn in this row; zero means no marker. */
			std::vector <unsigned char> _marks;
			/** Top labels wait until their five text rows are complete. */
			unsigned short _earlyRasterLines [5];
		};

		// Video output and diagnostic overlays.
		/** Selects the current shift-register pixel using effective controls and live colors. \n
			pixel identifies the phase-derived offset used by diagnostic annotations. \n
			inScreen is the retained horizontal border window, not presentation geometry. */
		void drawVisibleZone (unsigned int pixel, bool inScreen);
		/** Draws diagnostic markers over already rendered pixels. */
		void drawOtherEvents ();
		/** Draws an accepted raster event over its completed presentation row. */
		void drawRasterIRQLine
			(unsigned short row, unsigned short rasterLine);

		// Retained video reads and output preparation.
		/** Captures the preceding read and prepares the opposite memory half. */
		void clockVideoFetch (VideoFetchPhase phase);
		/** Retains a read after the preceding operation was consumed or cancelled. */
		inline void prepareVideoRead
			(VideoReadKind kind, const MCHEmul::Address& address, size_t index);
		/** Captures the pending read and reports its effective operation. */
		MCHEmul::UByte captureVideoRead (unsigned int targets);
		/** Selects a graphics address using the operands available at selection. */
		void prepareGraphicsRead
			(size_t index, const MCHEmul::UByte& character,
			 unsigned short position, unsigned char row, bool idle);
		/** Reads an already selected graphics address and restores temporary mapping. */
		MCHEmul::UByte readVideoGraphics
			(const MCHEmul::Address& address, bool useGraphicsROMSelection);

		/** Shifts the output byte or loads a retained cell at the scroll boundary. \n
			The retained horizontal window permits shifting beyond the normal screen rectangle. */
		void clockVideoOutput (bool halfEnd, bool inScreen, unsigned char scrollX);
		/** Resolves the current output bits without advancing the shift register. */
		unsigned int videoPixelColor () const;

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
		/** BA holds reads; writes may finish only before TED takes the bus. */
		inline bool CPUAccessBlocked () const;
		/** Compensates recovered wait time through the existing CPU stop mechanism. */
		void updateCPUStop (MCHEmul::CPU* cpu);
		/** Recovers a CPU cycle, optionally stopping before its access. */
		bool simulateCPUCycle
			(MCHEmul::CPU* cpu,
			 unsigned int batchStart, unsigned int batchEnd,
			 bool completeCycle = true);
		/** Visits control and dot edges directly using the retained recovery cursor. \n
			target is exclusive and identifies the pre-access edge or CPU-cycle end. \n
			Callers use quarterDots () - 9 or quarterDots (), preserving edge alignment. */
		bool advanceCPUQuarters
			(MCHEmul::CPU* cpu, unsigned int target,
			 unsigned int batchStart, unsigned int batchEnd);
		/** Processes the current control or dot-clock edge once. */
		bool processCPUQuarter
			(MCHEmul::CPU* cpu,
			 unsigned int batchStart, unsigned int batchEnd);
		/** Updates retained border windows before emitting the current dot. */
		inline void clockBorders ();
		/** Accepts the programmed graphics controls for subsequent output and fetches. */
		inline void latchVideoControls ();
		/** Updates BA/AEC arbitration after the current bus phase has completed. */
		void clockDMA (bool slowEnd);
		bool DMARequested () const
							{ return (_dmaState != DMAState::_IDLE); }
		bool DMAActive () const
							{ return (_dmaState == DMAState::_ACTIVE); }
		/** Selects timing without changing an already captured CPU period. */
		inline void selectConfiguration ();
		/** Applies presentation geometry at its next frame origin. */
		bool applyPresentationConfiguration ();
		/** Decodes the effective horizontal counter and internal line capture. */
		bool processRasterControl ();
		/** Advances the counters, respecting simultaneous CPU-load priorities. */
		void clockRasterDot (bool slowEnd, unsigned int writes);
		inline bool sampleRasterIRQ (bool matched, bool acknowledge);
		/** Returns zero outside the normal horizontal traversal. */
		inline unsigned short rasterCycle () const;
		/** Processes slow-pulse display, badline and row-end controls. */
		void beginTEDCycle ();
		/** Completes slow-pulse work and draws events before image delivery. */
		void finishTEDCycle (MCHEmul::CPU* cpu, unsigned int requestCycle);
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
		/** Divider selected by the current FF07.6 value. */
		unsigned int clockDivider () const
							{ return (_configuration -> _divider); }
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
		/** Updates the legacy diagnostic image from the effective counters. */
		inline void publishRasterPosition ();

		//-----
		std::string debugRasterAsString () const;
		std::string debugTimingAsString () const;
		/** Selected controls and retained slow-pulse gate; no CPU duration is inferred. */
		std::string debugClockAsString () const;
		std::string debugInternalAsString () const;
		std::string debugVideoOutputAsString () const;
		std::string debugTimersAsString () const;
		std::string debugInterruptAsString () const
							{ return (_TEDRegisters -> debugInterruptAsString ()); }
		DebugCycleContext debugCycleContext
			(const CPUCycleState& cS,
			 unsigned int batchStart, unsigned int batchEnd) const;
		void debugTEDCycle ();
		void debugCPUAboutToExecute
			(const MCHEmul::InstructionContextEventData* dt);
		void debugCPUAboutToExecute
			(const MCHEmul::InterruptContextEventData* dt);
		std::string debugCPUTransactionAsString () const;
		void debugCPUReadAdvance
			(MCHEmul::CPU* cpu, bool after) const;
		void debugCPUStop (MCHEmul::CPU* cpu, unsigned int remaining) const;
		void debugBadLine ();
		void debugReadingVideoMatrix
			(const MCHEmul::Address& address, size_t index,
			 const MCHEmul::UByte& value);
		void debugReadingVideoAttributes
			(const MCHEmul::Address& address, size_t index,
			 const MCHEmul::UByte& value);
		void debugReadingGraphics
			(const PendingVideoRead& pending, const MCHEmul::UByte& value);
		/** Logs a shift-register load before slow-boundary control acceptance. */
		void debugVideoOutputLoad () const;
		void debugTEDCycleEnd ();
		void debugFrozenTEDCycle () const;
		void debugCPUWrite
			(MCHEmul::CPU* cpu, unsigned int cycle,
			 const MCHEmul::SetMemoryCommand& command, bool after) const;
		void debugRasterEvent
			(const char* event, unsigned short sourceLine) const;
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
		/** Retained horizontal windows used by the CPU clock decision. */
		bool _externalFetchClock, _refreshClock;
		/** Arbitration window differs from the matrix-address generation window. */
		DMAState _dmaState;
		bool _dmaWindow;
		/** Latest transaction, retained after recovery for diagnostics. */
		CPUTransactionContext _cpuTransaction;
		/** CPU binding and buffered-write scheduling. */
		CPUTimedWrites _cpuTimedWrites;
		/** Recovered time and suspended CPU-cycle progress. */
		CPURecoveryState _cpuRecovery;
		/** Crystal clocks accumulated within the current slow sound pulse. \n
			Cleared when that pulse is queued; not an absolute TED clock. */
		unsigned int _pendingSoundMasterCycles;
		/** The format used to draw. It has to be the same that is used by the Screen object. */
		SDL_PixelFormat* _format;
		/** Derived graphics-cycle position retained at the beginning of the slow pulse. */
		unsigned short _cycleInRasterLine;
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
			This variable is cleared when the next internal video line is captured. */
		mutable bool _newBadLineCondition;

		/** Graphics fetch state and retained buffers. \n
			The ten-bit character reload counter and three-bit vertical
			subaddress are shared through TEDRegisters::GraphicalInfo. \n
			VC is the graphics working counter. Matrix/attribute address
			position and reload belong to VideoFetchState. \n
			Fetches follow the retained window and phase boundaries. The output
			consumes retained cell operands instead of reading these buffers live. */
		struct TEDGraphicInfo
		{
			TEDGraphicInfo ()
				: _VC (0), _VLMI (0),
				  _idleState (true),
				  _screenCodeData { },
				  _graphicData { },
				  _colorData { }
							{ }

			unsigned short _VC, _VLMI;
			bool _idleState; 

			// Implementation
			mutable std::array <MCHEmul::UByte, _GRAPHMAXCHARCOLUMNS> _screenCodeData;
			mutable std::array <MCHEmul::UByte, _GRAPHMAXCHARCOLUMNS> _graphicData;
			mutable std::array <MCHEmul::UByte, _GRAPHMAXCHARCOLUMNS> _colorData;
		};

		/** Independent matrix/attribute reload state. \n
			Address position and circular-buffer position are different counters. \n
			Position advances at matrix boundaries; row reload retains cycle 57. */
		struct VideoFetchState final
		{
			static const unsigned int _ATTRIBUTES = 0x01;
			static const unsigned int _MATRIX = 0x02;

			VideoFetchState ()
				: _position (0), _reload (0), _bufferIndex (0),
				  _scrollY (0), _displayActive (false),
				  _matrixLine (false), _window (false)
							{ }

			/** Returns buffer-capture enables, not the selected bus address. */
			unsigned int captureTargets (unsigned short videoLine) const
			{
				return
					(((_displayActive && videoLine < 203 &&
					   (videoLine & 0x07) == _scrollY)
						? _ATTRIBUTES : 0u) |
					 (_matrixLine ? _MATRIX : 0u));
			}

			/** Retains the preceding line condition before line/scroll updates. */
			void latchNextLine (unsigned short previousVideoLine)
							{ _matrixLine =
								(captureTargets (previousVideoLine) & _ATTRIBUTES) != 0; }

			/** Advances the RAM address counter after its qualified use. */
			void advancePosition ()
							{ _position = (_position + 1) & 0x03ff; }

			/** Advances buffer circulation independently of address reloads. */
			void advanceBuffer ()
							{ _bufferIndex =
								(_bufferIndex == _GRAPHMAXCHARCOLUMNS - 1)
									? 0 : _bufferIndex + 1; }

			/** Current ten-bit matrix/attribute address position. */
			unsigned short _position;
			/** Independent ten-bit address reload value. */
			unsigned short _reload;
			/** Circular-buffer position, advanced independently of address reloads. */
			unsigned short _bufferIndex;
			/** Scroll sampled at the qualified slow-clock boundary. */
			unsigned char _scrollY;
			/** Display enable retained for the fetch sequence. */
			bool _displayActive;
			/** Character-code capture condition retained from the preceding line. */
			bool _matrixLine;
			/** Address-generation and buffer-circulation window. */
			bool _window;
		};

		TEDGraphicInfo _tedGraphicInfo;
		/** Active reload conditions, circular index and phased address state. */
		VideoFetchState _videoFetchState;
		/** Address and destination retained until the next qualified capture. */
		PendingVideoRead _pendingVideoRead;
		/** Retained cell transfers, effective controls and pixel shifting. */
		VideoOutputState _videoOutput;

		/** Snapshot captured before writes and advancement of the current phase. */
		DebugCycleContext _debugCycleContext;
		EventsStatus _eventStatus;

		private:
		static const MCHEmul::Address _MEMORYPOSIDLE1, _MEMORYPOSIDLE2;
	};

	// ---
	inline void TED::setDrawOtherEvents (bool d)
	{
		if (_drawOtherEvents == d)
			return;

		_drawOtherEvents = d;

		// An existing request must not become a new diagnostic event on activation.
		_eventStatus.reset
			(_raster.visibleColumns (),
			 d && _TEDRegisters != nullptr && _TEDRegisters -> launchIRQ ());
	}

	// ---
	inline void TED::CPUTransactionContext::finish ()
	{
		_pending = false;
		_readAdvanceCycles = 0;
	}

	// ---
	inline void TED::CPUTransactionContext::advance
		(bool held, unsigned int recoveryCycle)
	{
		// A completed transaction can still recover waits before the next opcode.
		if (held || _logicalCycle == _busData -> _numberCycles)
		{
			if (_waitCycles == 0)
				_firstStopPosition = recoveryCycle - _startCycle;

			++_waitCycles;
		}
		else
			++_logicalCycle;
	}

	// ---
	inline void TED::CPUTimedWrites::clearCommands ()
	{
		_commands.clear ();
		_next = 0;
	}

	// ---
	inline void TED::CPURecoveryState::commitElapsedTime ()
	{
		_halfCarry =
			(_cycle._halfCarryIn + _cycle.halfTEDCycles ()) & 0x01;
		_pendingPacingCycles += _cycle.masterCycles ();
	}

	// ---
	inline unsigned int TED::CPURecoveryState::takePendingPacingCycles ()
	{
		const unsigned int result = _pendingPacingCycles;

		_pendingPacingCycles = 0;

		return (result);
	}

	// ---
	inline void TED::EventsStatus::reset
		(size_t columns, bool irqMarked)
	{
		_marker = { };
		_irqRequestMarked = irqMarked;
		_rasterLine = 0;

		finishFrame ();

		// Resizing to the current width preserves the existing allocation.
		_marks.resize (columns);
		std::fill (_marks.begin (), _marks.end (), 0);
	}

	// ---
	inline void TED::EventsStatus::beginRow (unsigned short row)
	{
		if (_row == row)
			return;

		_row = row;
		_rowFinished = false;
		_rasterPending = false;

		std::fill (_marks.begin (), _marks.end (), 0);
	}

	// ---
	inline void TED::EventsStatus::finishFrame ()
	{
		_row = _NOROW;
		_rowFinished = false;
		_rasterPending = false;

		for (size_t i = 0; i < 5; i++)
			_earlyRasterLines [i] = _NOROW;
	}

	// ---
	inline void TED::prepareVideoRead
		(VideoReadKind kind, const MCHEmul::Address& address, size_t index)
	{
		_pendingVideoRead._kind = kind;
		_pendingVideoRead._address = address;
		_pendingVideoRead._index = index;
	}

	// ---
	inline bool TED::CPUAccessBlocked () const
	{
		if (!_cpuTransaction._pending || !DMARequested ())
			return (false);

		if (DMAActive () ||
			_cpuTransaction._logicalCycle == _cpuTransaction._busData -> _numberCycles)
			return (true);

		// The next write is indexed by completed commands, not elapsed wait time.
		return (_cpuTimedWrites._next == _cpuTransaction._busData -> _numberWriteCycles ||
			_cpuTransaction._busData -> writeCycle (_cpuTimedWrites._next) !=
				_cpuTransaction._logicalCycle);
	}

	// ---
	inline void TED::clockBorders ()
	{
		const unsigned short line = _rasterPosition._videoLine,
			horizontal = _rasterPosition._horizontal;
		const bool rows25 = _TEDRegisters -> textDisplay25RowsActive ();
		const bool columns40 = _TEDRegisters -> textDisplay40ColumnsActive ();

		if (line == (rows25 ? 204 : 200))
			_videoOutput._verticalOpen = false;
		else if (line == (rows25 ? 4 : 8))
			_videoOutput._verticalOpen = true;

		// Vertical permission qualifies opening; it does not directly
		// overwrite the horizontal window already retained for this line.
		if (horizontal == (columns40 ? 320 : 312))
			_videoOutput._horizontalOpen = false;
		else if (horizontal == (columns40 ? 0 : 8) &&
			_videoFetchState._displayActive && _videoOutput._verticalOpen)
			_videoOutput._horizontalOpen = true;
	}

	// ---
	inline void TED::latchVideoControls ()
	{
		_videoOutput._control1 =
			(_TEDRegisters -> graphicExtendedColorTextModeActive () ? 0x40 : 0) |
			(_TEDRegisters -> graphicBitModeActive () ? 0x20 : 0);
		_videoOutput._control2 =
			(_TEDRegisters -> reverseVideoActive () ? 0 : 0x80) |
			(_TEDRegisters -> graphicMulticolorTextModeActive () ? 0x10 : 0);
	}

	// ---
	inline void TED::selectConfiguration ()
	{
		const Configuration* configuration =
			&_configurations [_TEDRegisters -> PALActive () ? 0 : 1];

		if (_configuration == configuration)
			return;

		_configuration = configuration;
		_soundFunction -> setClockDivider (_configuration -> _divider);
		notify (MCHEmul::Event (_VIDEOCONFIGURATIONCHANGED));
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
		const bool result =
			(_videoFetchState.captureTargets (_rasterPosition._videoLine) &
				VideoFetchState::_ATTRIBUTES) != 0 &&
			_lastBadLineScrollY != (int) _videoFetchState._scrollY;

		if (result)
			_lastBadLineScrollY = _videoFetchState._scrollY;

		return (result);
	}

	// ---
	inline bool TED::singleClockModeForCurrentCycle () const
	{
		return
			(_TEDRegisters -> freezeActive () ||
			 _TEDRegisters -> singleClockModeForced () ||
			 _refreshClock ||
			 (_videoFetchState._displayActive && _externalFetchClock));
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
		if (_cpuTimedWrites._enabled &&
			(cpu != _cpuTimedWrites._cpu ||
			 _cpuTransaction._pending ||
			 _cpuRecovery._cycle._accessPending ||
			 !_cpuTimedWrites._commands.empty () ||
			 _cpuRecovery._nextCycle != cpu -> clockCycles ()))
		{
			_error = MCHEmul::_CHIP_ERROR;

			return (false);
		}

		_cpuTransaction = CPUTransactionContext
			(cpu, &busData, interrupt, _cpuTimedWrites._enabled);

		return (true);
	}

	// ---
	inline unsigned int TED::pendingWriteCycle () const
	{
		assert (_cpuTransaction._busData != nullptr);
		assert (_cpuTimedWrites._next < _cpuTimedWrites._commands.size ());

		return
			(_cpuTransaction._startCycle +
			 (unsigned int) (_cpuTransaction._busData ->
				writeCycle (_cpuTimedWrites._next)) + _cpuTransaction._waitCycles);
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
