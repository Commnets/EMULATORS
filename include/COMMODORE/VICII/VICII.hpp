/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: VICII.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 15/05/2021 \n
 *	Description: The VICII Chip.
 *	Versions: 2.0 Powered by the information in the documentation and the tests of the emulation of the VICII in CMB64. \n
 *	Based on https://www.cebix.net/VIC-Article.txt.
 */

#ifndef __COMMODORE_VICII__
#define __COMMODORE_VICII__

#include <array>
#include <CORE/incs.hpp>
#include <COMMODORE/VICII/VICIIRegisters.hpp>

namespace COMMODORE
{
	/** 
		The chip that takes care of anything around the graphics in computers like 
		Commodore 64 or Commodore 128 when emulating CMB64. \n
		@see GraphicalChip. \n
		How it works in detail can be found in: https://www.cebix.net/VIC-Article.txt. \n
		\n
		Any case: \n
		Every horizontal raster line takes 64us (including horizontal retrace = 15us as PAL/NTSC standard definition). \n
		1/985.248 = 1,01497us per cycle in PAL. 64us/1,01497 = 63 cycles per raster line in PAL. \n
		1/1.023.000 = 0,977517us per cycle in NTSC. 64us/0,977517us = 64 cycles per raster line in NTSC. \n
		The VIC-II and the 6510 share the same visible cycle rate: PAL has 63
		VIC-II/CPU cycles per raster line and NTSC has 64. Internally, every
		cycle is split into bus phases. The VIC-II normally uses one phase for
		video memory activity while the CPU can use the other one; during bad
		lines or sprite DMA, the VIC-II can also steal the CPU-visible phase
		through BA/AEC. \n
		This emulator advances the VIC-II once per CPU cycle. It does not model
		the two hardware half-cycles as independent scheduler steps. Instead, it
		executes the g-access associated with phi1 before the c-access associated
		with phi2 in their shared cycles. Thus each c-access prepares the Video
		Matrix and Color RAM data consumed by the following g-access while
		preserving the CPU bus-stealing effect. \n
		Every emulated VIC-II cycle corresponds to 8 raster pixels when the beam
		is inside the visible area.
		\n
		VICII has a LP pin that latches the current raster beam position on a
		negative edge. The emulation maps this behaviour to the mouse: while the
		left mouse button is pressed, the mouse position is treated as the light-pen
		target position. The latch happens when the emulated raster beam reaches
		that visible mouse position, and only once per frame. \n
		The latched position is stored in VIC-II registers $13 and $14.
	*/
	class VICII : public MCHEmul::GraphicalChip
	{
		public:
		static const unsigned int _ID = 104;

		// Related with badlines...
		/** The position of the bad lines. */
		static const unsigned short _FIRSTBADLINE							= 0x30;
		static const unsigned short _LASTBADLINE							= 0xf7;
		/** Bauer bad-line BA/c-access start window. */
		static const unsigned short _BADLINE_START_FIRST_CYCLE				= 12;
		static const unsigned short _BADLINE_START_LAST_CYCLE				= 54;
		/** Window where a Bad Line Condition can switch the graphics
			sequencer from idle to screen/display state. */
		static const unsigned short _BADLINE_DISPLAY_FIRST_CYCLE			= 12;
		static const unsigned short _BADLINE_DISPLAY_LAST_CYCLE				= 57;
		/** Late Bad Line Condition window that can prevent idle entry at cycle 58. */
		static const unsigned short _BADLINE_IDLE_PREVENT_FIRST_CYCLE		= 54;
		static const unsigned short _BADLINE_IDLE_PREVENT_LAST_CYCLE		= 57;
		/** Effective c-access window. c-access in phi2 of cycles 15..54 prepares
			the matrix and color data consumed by g-access in the following cycle. */
		static const unsigned short _BADLINE_EFFECTIVE_CACCESS_FIRST_CYCLE	= 15;
		static const unsigned short _BADLINE_EFFECTIVE_CACCESS_LAST_CYCLE	= 54;
		/** Effective g-access window. g-access in phi1 of cycles 16..55 consumes
			the data prepared by the preceding c-access. */
		static const unsigned short _GRAPHIC_ACCESS_FIRST_CYCLE				= 16;
		static const unsigned short _GRAPHIC_ACCESS_LAST_CYCLE				= 55;

		/** VC and VCBASE are 10-bit video matrix counters. */
		static const unsigned short _VCMASK									= 0x03ff;

		/** Data about the size of the screen */
		static const unsigned short _GRAPHMAXCHARLINES						= 25;	// Not taking into account reductions in the size!
		static const unsigned short _GRAPHMAXCHARCOLUMNS					= 40;
		static const unsigned short _GRAPHMAXBITMAPCOLUMNS					= 320;	// Not taking into account double coulors!
		static const unsigned short _GRAPHMAXBITMAPROWS						= 200;

		// Some events.
		/** As the VICII only addresses 16k and some computers where it might be connected to, admits up to 64k,
			there is the possibility to change the bank. \n
			The unsigned ints associated to each must be consecutive for everything to work properly. \n
			NOTE: Don't tounch these values! */
		static const unsigned int _BANK0SET									= 200;
		static const unsigned int _BANK1SET									= 201;
		static const unsigned int _BANK2SET									= 202;
		static const unsigned int _BANK3SET									= 203;

		/** Specific classes for PAL & NTSC have been created giving this data as default. \n
			The VICII constructor receives a refeence to the the Color RAM, to simulate the access to the 
			color at the same time it acceses to the char info. \n
			The VICII constructor receives also info over the raster data, the memory view to use,
			The number of cycles of every raster line (different depending on the VICII version) 
			and additional attributes. */
		VICII (int intId, MCHEmul::PhysicalStorageSubset* cR, const MCHEmul::Address& cRA,
			const MCHEmul::RasterData& vd, const MCHEmul::RasterData& hd, 
			int vV, unsigned short cRL, const MCHEmul::Attributes& attrs = { });

		virtual ~VICII () override;

		// Managing lightpen
		/** To activate/desactivate the lightpen. */
		void setLightPenActive (bool a)
							{ _VICIIRegisters -> setLigthPenActive (a); 
							  _eventStatus._lightPenPositionLatched = false;}

		/** To draw or not to draw raster interrupt positions. */
		void setDrawRasterInterruptPositions (bool d)
							{ _drawRasterInterruptPositions = d; }
		/** To draw or not to draw an square rounding the sprites. 
			It is drawn when the sprite is active in the raster line where the VICII is. */
		void setDrawSpritesBorder (bool d)
							{ _drawSpritesBorder = d; }
		/** Whether to draw other events related with the raster of VICII. 
			Borders, bad lines mainly. */
		void setDrawOtherEvents (bool d)
							{ _drawOtherEvents = d; }

		/** To assign or to know the color RAM. 
			The VICII needs to know where the color RAM is, to access the nibbles with the color. */
		const MCHEmul::PhysicalStorageSubset* colorRAM () const
							{ return (_colorRAM); }
		MCHEmul::PhysicalStorageSubset* colorRAM ()
							{ return (_colorRAM); }
		const MCHEmul::Address& colorRAMAddreess () const
							{ return (_colorRAMAddress); }
		void setColorRAM (MCHEmul::PhysicalStorageSubset* cR, const MCHEmul::Address& cRA)
							{ _colorRAM = cR; _colorRAMAddress = cRA; }

		virtual unsigned short numberColumns () const override
							{ return (_raster.visibleColumns ()); }
		virtual unsigned short numberRows () const override
							{ return (_raster.visibleLines ()); }

		/** To get the number of cycles per raster line used in this chip. */
		unsigned short cyclesPerRasterLine () const
							{ return (_cyclesPerRasterLine); }

		/** To change and get the bank. */
		unsigned short bank () const
							{ return (_VICIIRegisters -> bank ()); }
		void setBank (unsigned char bk)
							{ _VICIIRegisters -> setBank (bk); }

		/** To get the raster info. */
		const MCHEmul::Raster& raster () const
							{ return (_raster); }

		virtual bool initialize () override;

		/** The raster is a critical thing in the behaviour of the VICII.
			The content of the register 0x12 (and the 0x11 bacause it contains high bit) is "real time".
			Any read instruction could read different values depending on the position of the raster 
			when that instruction happens. */
		virtual void CPUAboutToExecute (const MCHEmul::InstructionContextEventData* dt) override;
		/** Prepares VIC-II timing for an accepted interrupt launch sequence. */
		virtual void CPUAboutToExecute (const MCHEmul::InterruptContextEventData* dt) override;

		/** Simulates cycles in the VICII. \n
			It draws the border AFTER once graphics info has been drawn within the display zone. \n
			So sprites can be drawn behing the border and collisions could take place out of the visible zone. */
		virtual bool simulate (MCHEmul::CPU* cpu) override;

		/**
		  *	The name of the fields are: \n
		  * VICIIRegisters					= InfoStructure: Info about the registers. \n
		  * Raster							= InfoStructure: Info about the raster. \n
		  * VICIIInternal					= InfoStructure: Info about the internal registers of the VICII. \n
		  * DENSeenAtLine30					= Attribute: Whether the DEN signal has been seen at line 30. \n
		  * BadlineCondition				= Attribute: Whether the instantaneous Bad Line Condition is active. \n
		  * BadlinePreventedIdleThisLine	= Attribute: Whether a late Bad Line Condition has prevented idle entry. \n
		  * BadlineDetected					= Attribute: Whether a Bad Line Condition has been accepted during the display-state window. \n
		  * BadlineBARequested				= Attribute: Whether a BA-like bus request has been scheduled for the current bad line. \n
		  * BadlineBARequestCycle			= Attribute: First VICII internal cycle in which the scheduled
		  *		BA-like request is effective. \n
		  * BadlineFirstCAccessCycle			= Attribute: Number of the VICII internal cycle where the first
		  *		access to the character data happens. \n
		  * BadlineCAccess					= Attribute: Whether a bad-line c-access sequence is latched for the current raster line. \n
		  * BadlineCAccessAllowed			= Attribute: Whether the latched c-access sequence is allowed
		  *		to perform normal Video Matrix / Color RAM reads in this raster line. \n
		  * BadlineInvalidCAccessCycles		= Attribute: Number of initial invalid c-access attempts in the current raster line. \n
		  * BadlineCAccessStartCycle		= Attribute: VICII internal cycle where the bad-line c-access
		  *		sequence was latched. For late sequences, the first attempted c-access is reported separately
		  *		by BadlineFirstCAccessCycle. \n
		  * Cycle							= Attribute: Number of the VICII internal cycle where the raster beam is. \n
		  * LastVICDataRead					= Attribute: The last byte read by the VICII. \n
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		// To get snapshots of the memory...
		// They are used in some commands...
		/** content of the screen. */
		MCHEmul::UBytes screenMemorySnapShot (MCHEmul::CPU* cpu) const;
		/** The content of the color memory (always in the same position). */
		MCHEmul::UBytes colorMemorySnapShot (MCHEmul::CPU* cpu) const;
		/** The content of the bitmap memory. */
		MCHEmul::UBytes bitmapMemorySnapShot (MCHEmul::CPU* cpu) const;
		/** The data of the sprites memory. */
		MCHEmul::UBytes spritesMemorySnapShot (MCHEmul::CPU* cpu, 
			const std::vector <size_t>& sprs = { }) const;

		// To print out the form of the different graphic elements
		// taking into account the graphic mode active
		/** The sprites. */
		MCHEmul::Strings spritesDrawSnapshot (MCHEmul::CPU* cpu, 
			const std::vector <size_t>& sprs = { }) const;
		/** The characters. */
		MCHEmul::Strings charsDrawSnapshot (MCHEmul::CPU* cpu, 
			const std::vector <size_t>& chrs = { }) const;

		protected:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		/** A position in the VIC-II raster-cycle sequence relative to the
			beginning of the current raster line. \n
			Cycle 1 of the current line is represented by 1 and cycle 1 of the
			next line by cyclesPerRasterLine () + 1. Negative and zero values are
			used by sprite windows whose BA lead started in the previous line. */
		using CPURasterCycle = int;

		/** Maximum number of write cycles in a 6500 CPU transaction. \n
			Regular store instructions have one write, memory RMW instructions
			have two and BRK/IRQ/NMI entry has three stack writes. */
		static const size_t _MAXCPUTRANSACTIONWRITES = 3;

		/** Effective interval in which VIC-II bus activity can stop the CPU. \n
			BA is low from _firstBACycle through _lastCycle. A pending 6510 write
			can still finish before _firstAECCycle; a read stops as soon as BA is
			low. The interval uses inclusive cycle limits. */
		struct CPUStopWindow final
		{
			CPUStopWindow ()
				: _firstBACycle (0), _firstAECCycle (0), _lastCycle (0)
								{ }

			CPUStopWindow (CPURasterCycle fBA, CPURasterCycle fAEC, CPURasterCycle lC)
				: _firstBACycle (fBA), _firstAECCycle (fAEC), _lastCycle (lC)
								{ }

			bool BAActiveAt (CPURasterCycle c) const
								{ return (c >= _firstBACycle && c <= _lastCycle); }
			bool CPUOwnsBusAt (CPURasterCycle c) const
								{ return (c < _firstAECCycle || c > _lastCycle); }

			CPURasterCycle _firstBACycle;
			CPURasterCycle _firstAECCycle;
			CPURasterCycle _lastCycle;
		};

		using CPUStopWindows = std::vector <CPUStopWindow>;
		using CPUStopWindowSets = std::array <CPUStopWindows, 512>;

		/** Result of projecting one already executed CPU transaction over the
			current and next VIC-II stop windows. \n
			Besides the final instruction effect, the structure stores the number
			of elapsed CPU/VIC-II positions at which every write bus cycle can
			really complete after applying BA/AEC stalls. */
		struct CPUStopPrediction final
		{
			CPUStopPrediction ()
				: _valid (false), _stopRequired (false), _stopRequested (false),
				  _firstStopCycle (0), _instructionEffectCycle (0), _firstNormalCycle (0),
				  _cyclesToStop (0), _positionsToInstructionEffect (0),
				  _positionsToWriteEffects { 0, 0, 0 }
								{ }

			bool _valid;
			bool _stopRequired;
			bool _stopRequested;
			CPURasterCycle _firstStopCycle;
			CPURasterCycle _instructionEffectCycle;
			CPURasterCycle _firstNormalCycle;
			unsigned int _cyclesToStop;
			unsigned int _positionsToInstructionEffect;
			/** Positions elapsed from the beginning of the transaction to each
				effective write, indexed by the write ordinal in BusCycleData. */
			std::array <unsigned int, _MAXCPUTRANSACTIONWRITES>
				_positionsToWriteEffects;
		};

		/** Minimum immutable context required to recalculate a CPU transaction
			prediction after a late bad-line or sprite-DMA window change. \n
			The cycle structure and its metadata are owned by the instruction or
			interrupt definition. Retaining these pointers transfers no ownership. \n
			_startCycle can be adjusted when crossing a raster line, whereas
			_startCPUCycle remains the absolute CPU-clock origin of the transaction. */
		struct PendingCPUTransaction final
		{
			PendingCPUTransaction ()
				: _cycleStructure (nullptr), _busCycleData (nullptr),
				  _clockCycles (0), _startCycle (0), _startCPUCycle (0)
								{ }

			bool valid () const
								{ return (_cycleStructure != nullptr); }
			void reset ()
								{ _cycleStructure = nullptr; _busCycleData = nullptr;
								  _clockCycles = 0; _startCycle = 0;
								  _startCPUCycle = 0; }

			const MCHEmul::CycleStructure* _cycleStructure;
			const MCHEmul::BusCycleData* _busCycleData;
			unsigned int _clockCycles;
			CPURasterCycle _startCycle;
			unsigned int _startCPUCycle;
		};

		/** Invoked from initialize to create the right screen memory. \n
			It also creates the Palette used by CBM 64 (_format variable). */
		virtual MCHEmul::ScreenMemory* createScreenMemory () override;

		// Draw the graphics & Sprites in detail...
		/** A couple of variables defining the drawing context. \n
			Tjose are to simplify the use of some of the routines dedicated to draw graphics. */
		struct DrawContext
		{
			unsigned short _ICD;	// Initial Column of the Display (Not taken into account reductions in size).
			unsigned short _SC;		// Scroll X
			unsigned short _RC;		// Raster X position not adjusted
			unsigned short _RCA;	// Raster X position adjusted (Moves 8 by 8, so = Raster X >> 3 << 3)
			unsigned short _RR;		// Raster Y (From the beginning of the visible zone.
									// Moves 1 by 1. No adjusted needed because how the C64 screen is defined...
		};

		/** To simplify the way the result of a drawing text/bitmaps routines are managed. \n
			Any time a draw routine runs, 8 bits of info are calculated. \n
			These bits can be either foreground or background and their color are kept in 
			their respective variable _colorData. */
		struct DrawResult
		{
			/** The data used to detect the collisions with the graphics. */
			MCHEmul::UByte _collisionGraphicData;
			/** The data used to detect collisions with the sprites. */
			MCHEmul::UByte _collisionSpritesData [8];
			/** The color of the each pixel considered as background. */
			unsigned int _backgroundColorData [8];
			/** The color of the each pixel considered as foreground. */
			unsigned int _foregroundColorData [8];
			/** The color of each pixel if there was sprites behind!,
				and who is the owner (sprite number) of that pixel! */
			unsigned int _spriteColor [8];
			size_t _spriteColorOwner [8];
			/** To indicate whether it is or not consecuencia of a "bad mode",
				and the has to be everything draw in black, but taking into account the real graphics behaind. */
			bool _invalid;

			DrawResult ()
				: _collisionGraphicData (MCHEmul::UByte::_0),
				  _collisionSpritesData
						{ MCHEmul::UByte::_0, MCHEmul::UByte::_0, MCHEmul::UByte::_0, MCHEmul::UByte::_0, 
						  MCHEmul::UByte::_0, MCHEmul::UByte::_0, MCHEmul::UByte::_0, MCHEmul::UByte::_0 },
				  _backgroundColorData 
						{ MCHEmul::_U0, MCHEmul::_U0, MCHEmul::_U0, MCHEmul::_U0, 
						  MCHEmul::_U0, MCHEmul::_U0, MCHEmul::_U0, MCHEmul::_U0 },
				  _foregroundColorData 
						{ MCHEmul::_U0, MCHEmul::_U0, MCHEmul::_U0, MCHEmul::_U0, 
						  MCHEmul::_U0, MCHEmul::_U0, MCHEmul::_U0, MCHEmul::_U0 },
				  _spriteColor
						{ MCHEmul::_U0, MCHEmul::_U0, MCHEmul::_U0, MCHEmul::_U0, 
						  MCHEmul::_U0, MCHEmul::_U0, MCHEmul::_U0, MCHEmul::_U0 },
				  _spriteColorOwner
						{ MCHEmul::_S0, MCHEmul::_S0, MCHEmul::_S0, MCHEmul::_S0, 
						  MCHEmul::_S0, MCHEmul::_S0, MCHEmul::_S0, MCHEmul::_S0 },
				  _invalid (false)
							{ }
		};

		// -----
		// Support methods used by the VIC-II simulation pipeline...
		// --------------------------------------------------------------------
		// Per-cycle simulation pipeline.
		// These methods are directly invoked from simulate(), or are the main
		// entry points for each phase of the VIC-II cycle.
		// --------------------------------------------------------------------
		// Bad-line state management...
		/** To update the complete bad-line state for the current VIC-II cycle. */
		inline void treatBadLineStateAtCurrentCycle ();
		/** To reset the per-raster-line bad-line state. */
		inline void resetBadLineStateForNewRasterLine ();
		/** To update the DEN latch used to enable bad lines in the current frame. */
		inline void updateDENSeenAtLine30 ();
		/** To determine whether a bad line is possible in the current cycle. */
		inline bool isBadLineCondition () const;

		// Idle/screen state management...
		/** To determine whether the VIC-II internal graphics sequencer is in idle state. */
		bool idleStateActive () const
							{ return (_vicGraphicInfo._idleState); }
		/** To determine whether the VIC-II internal graphics sequencer is in screen/display state. */
		bool screenStateActive () const
							{ return (!idleStateActive ()); }
		/** To switch the VIC-II internal graphics sequencer to screen/display state. */
		void enterScreenState ()
							{ _vicGraphicInfo._idleState = false; }
		/** To switch the VIC-II internal graphics sequencer back to idle state. */
		void enterIdleState ()
							{ _vicGraphicInfo._idleState = true; }

		// Bus arbitration/BA-like stop requests...
		/** Creates the 512 immutable combinations of bad-line and sprite-DMA
			windows used by this concrete VIC-II model. */
		void initializeCPUStopWindowSets ();
		/** Creates one of the immutable stop-window combinations. */
		void buildCPUStopWindowSet
			(bool bL, unsigned char sM, CPUStopWindows& result) const;
		/** Adds the normal bad-line interval BA=12..54, AEC=15..54. */
		void addBadLineCPUStopWindow (CPUStopWindows& result) const;
		/** Adds the bus interval assigned to one sprite in the concrete video model. */
		virtual void addSpriteCPUStopWindow
			(size_t nS, CPUStopWindows& result) const = 0;
		/** Sorts and joins intervals whose BA-low portions form one effective stop. */
		void mergeCPUStopWindows (CPUStopWindows& result) const;
		/** Returns the table index made of the bad-line flag and eight DMA bits. */
		size_t cpuStopWindowSetIndex (bool bL, unsigned char sM) const
							{ return ((bL ? 0x0100 : 0) | sM); }
		/** Returns the current eight-bit sprite-DMA mask without allocating. */
		unsigned char spriteDMAMask () const;
		/** Projects which sprite slots will actually steal the bus on the next
			raster line. Sprites 3..7 fetch before the cycle-16 DMA termination,
			whereas sprites 0..2 fetch after it. */
		unsigned char projectedSpriteDMAMaskForNextRasterLine () const;
		/** Determines the regular bad-line condition for an arbitrary raster line. */
		bool badLineConditionForRasterLine (unsigned short rL) const;
		/** Selects immutable stop-window sets for the current and following lines. */
		void selectCPUStopWindowsForCurrentAndNextLine ();
		/** Replaces only the current-line bad-line part after a late transition. */
		void actualizeCPUStopWindowsAfterBadLineChange ();
		/** Locates the effective window containing a linear raster-cycle position. \n
			The returned copy is shifted to the current-line coordinate system. */
		bool CPUStopWindowAt
			(CPURasterCycle c, const CPUStopWindows& currentWindows,
			 const CPUStopWindows& nextWindows, CPUStopWindow& result) const;
		/** Projects a nominal CPU bus-cycle structure over two raster lines. \n
			A final implicit read represents the opcode fetch of the following
			instruction, allowing BA to stop the CPU immediately after the current
			instruction has produced its effect. */
		CPUStopPrediction calculateCPUStopPrediction
			(const MCHEmul::CycleStructure& cS, const MCHEmul::BusCycleData& bD,
			 unsigned int nC, CPURasterCycle startCycle,
			 const CPUStopWindows& currentWindows,
			 const CPUStopWindows& nextWindows) const;
		/** Selects and predicts the bus structure of the CPU transaction that
			is about to execute atomically. The referenced structures are owned
			by the corresponding instruction or interrupt definition. */
		void prepareCPUStopPrediction
			(const MCHEmul::CycleStructure* cS,
			 const MCHEmul::BusCycleData* bD, unsigned int nC,
			 unsigned int startCPUCycle);
		/** Recalculates the pending prediction after a window-set transition. \n
			Returns true only when the prediction was actually replaced. */
		bool recalculatePendingCPUStopPrediction ();
		/** Requests the single compensated CPU stop at the predicted raster cycle. */
		void requestPredictedCPUStopIfNeeded (MCHEmul::CPU* cpu, unsigned int cC);
		/** Extracts buffered writes targeting the VIC-II register subset. \n
			An already pending collection means that the CPU is still completing
			the same transaction and no new extraction is required. */
		void extractPendingRegisterWrites ();
		/** Executes the first pending VIC-II register write during the CPU phase
			of the current cycle, after the VIC-II bus activity represented by
			treatRasterBusCycle () and before the border and sprite comparator phase. \n
			The write keeps its predicted absolute CPU cycle; only its phase inside
			that cycle is represented explicitly. \n
			After execution the command is erased, making the next command the new
			element zero. Returns true when one write has been applied. \n
			A $d016 write updates the VIC-II CSEL comparator values immediately. The
			host-side horizontal display limits are reported through hDZC and remain
			deferred until the current eight-pixel slice has been drawn. */
		bool executePendingRegisterWriteAt (unsigned int cC, bool* hDZC);

		// Raster-cycle execution...
		/** Executes the VIC-II memory-bus activity and the internal sequencer
			state transitions associated with the current raster cycle. \n
			This phase is executed before the CPU write belonging to the same grouped
			cycle becomes visible. It includes sprite pointer/data accesses, c-accesses,
			g-accesses and the counter transitions directly associated with them. \n
			Border comparators are deliberately excluded and are evaluated later in
			the comparator phase of simulate(). */
		virtual void treatRasterBusCycle ();
		/** To treat the VIC-II cycle where VC is loaded from VCBASE and
			the graphic access indexes are reset for the current line. */
		inline void treatGraphicFetchStartCycle ();
		/** To compare sprite Y with the low 8 bits of the current raster line. */
		inline bool spriteYMatchesCurrentRaster (size_t nS) const;
		/** To treat the VIC-II cycle where RC is advanced, VCBASE can be updated,
			and the idle state can be restored. */
		inline void treatGraphicRowEndCycle ();
		/** To decide sprite DMA in cycles 55/56 before BA/RDY arbitration sees upcoming s-accesses. */
		inline void treatSpriteDMAStartAtCurrentCycle ();
		/** To apply the cycle-15 MCBASE +2 step when the Y-expansion flip-flop allows line advance. */
		inline void treatSpriteCounterCycle15 ();
		/** To apply the cycle-16 MCBASE +1 step and stop DMA after the 63 sprite bytes. */
		inline void treatSpriteCounterCycle16 ();
		/** To copy MCBASE to MC and enable sprite display at cycle 58 after DMA was decided earlier. */
		inline void treatSpriteDisplayStartCycle ();
		/** To latch the horizontal sprite position when the VIC-II X comparator
			reaches it. The comparison is performed even outside the visible zone,
			because border and blanking only mask the sprite output; they do not
			stop its horizontal sequencer. */
		inline void treatSpriteHorizontalStartAtCurrentCycle ();

		/** Last byte read by the VIC-II from the 8-bit memory data bus. */
		const MCHEmul::UByte& lastVICDataRead () const
							{ return (_lastVICDataRead); }

		// Graphics and bad-line accesses performed during raster-cycle execution...
		/** To treat a graphics access cycle. */
		inline void treatGraphicAccessCycle ();
		/** To treat a bad-line c-access cycle. */
		inline void treatBadLineCAccessCycle ();
		/** To determine the first attempted c-access cycle for the current bad-line sequence. \n
			The first attempts can be invalid DMA-delay/FLI accesses returning $ff.
			0 means that no c-access sequence has been defined. */
		inline unsigned short firstBadLineCAccessCycle () const;
		/** To determine whether the VIC-II is doing a bad-line c-access in this cycle. */
		inline bool isBadLineCAccessCycle () const;
		/** To determine whether the VIC-II performs a graphics data access
			in the current raster cycle. */
		bool isGraphicAccessCycle () const
							{ return (_cycleInRasterLine >= _GRAPHIC_ACCESS_FIRST_CYCLE &&
									  _cycleInRasterLine <= _GRAPHIC_ACCESS_LAST_CYCLE); }

		// Graphic access counters...
		/** To reset the graphics access counters at the beginning of a graphics fetch line. */
		inline void resetGraphicAccessCountersForCurrentLine ();
		/** To reset the video counters at the beginning of a new frame. */
		inline void resetGraphicCountersForNewFrame ();
		/** To advance the VIC-II video counters after a graphics access cycle. */
		inline void advanceGraphicAccessCounters ();

		// Raster movement and position-dependent events...
		/** To advance the internal raster position one VIC-II cycle.
			It also handles the transition to a new raster line. */
		inline void advanceRasterPosition ();
		/** To activate the raster IRQ flag at the VIC-II raster comparison
			cycle when the current raster line matches the programmed line. */
		inline void treatRasterIRQAtCurrentPosition ();
		/** To request a CPU IRQ if any VIC-II IRQ reason is currently active. */
		inline void requestIRQIfNeeded (MCHEmul::CPU* cpu, unsigned int cC);
		/** To notify once when the raster enters the first VBlank zone. */
		inline void notifyGraphicsReadyIfNeeded ();

		// Light pen...
		/** To treat light-pen detection at the current raster position. */
		inline void treatLightPenAtCurrentRasterPosition ();

		// --------------------------------------------------------------------
		// Drawing pipeline.
		// These methods are reached from drawVisibleZone().
		// --------------------------------------------------------------------
		/** Composes the visible eight-pixel slice using the raster and border state
			already evaluated by simulate (). \n
			This method performs rendering and collision processing only; it does not
			evaluate VIC-II border comparators. \n
			@param cpu	CPU used for collision IRQ notification and debug context. \n
			@param cv	Current horizontal position inside the visible raster area. \n
			@param rv	Current vertical position inside the visible raster area. \n
			@param cav	Horizontal position aligned to the beginning of the current
						eight-pixel slice. */
		void drawVisibleZone
			(MCHEmul::CPU* cpu, unsigned short cv,
			 unsigned short rv, unsigned short cav);

		// Border management.
		/** Evaluates the left and right horizontal main-border comparators for the
			current eight-pixel raster slice. \n
			The vertical border flip-flop must already have been updated by the
			final-cycle comparator phase in simulate(). This method only consumes
			that state when applying the left-comparator rule. \n
			The temporary left/right flags describe a partial transition for rendering;
			they are not VIC-II hardware flip-flops. */
		inline void actualizeMainBorderStatus (unsigned short cav);
		/** Evaluates the vertical border flip-flop at the final cycle of the current
			raster line. \n
			The comparison uses the RSEL-derived vertical limits and the DEN state that
			are effective after the CPU write phase of the grouped cycle. */
		inline void actualizeVerticalBorderStatus ();

		// Graphics, sprites and collision composition.
		/** Invoked from drawVisibleZone() to compose graphics and sprites and detect collisions. \n
		  *	@param dC	= The drawing context. \n
		  * @param sdCA = Whether sprite-data collisions are enabled. The vertical border
						  flip-flop disables the graphics-data output and these collisions. \n
		  * @param dTS	= Whether the priority-multiplexer output has to be copied to ScreenMemory.
						  Sprite sequencers and collision detection are processed regardless of this value.
		  *	@see DrawContext and DrawResult. */
		void drawGraphicsSpritesAndDetectCollisions (const DrawContext& dC, bool sdCA, bool dTS);
		/** To draw any text or bitmap graphic mode. \n
			The method receives the drawing context and returns a DrawResult. */
		DrawResult drawGraphics (const DrawContext& dC);

		// Character and bitmap drawing modes.
		/** Draws a monochrome character. */
		DrawResult drawMonoColorChar (int cb);
		/** Draws a multicolor character. \n
			The mode can also be used as an invalid mode. */
		DrawResult drawMultiColorChar (int cb, bool inv = false);
		/** Draws an extended-background character. */
		DrawResult drawMultiColorExtendedChar (int cb);
		/** Draws a monochrome bitmap. \n
			The mode can also be used as an invalid mode. */
		DrawResult drawMonoColorBitMap (int cb, bool inv = false);
		/** Draws a multicolor bitmap. \n
			The mode can also be used as an invalid mode. */
		DrawResult drawMultiColorBitMap (int cb, bool inv = false);

		// Sprite drawing.
		/** Draws one sprite over the already computed graphics result. */
		MCHEmul::UByte drawSpriteOver (size_t spr, unsigned int* d, size_t* dO);
		/** Draws a monochrome sprite line. */
		MCHEmul::UByte drawMonoColorSpriteOver (unsigned short c, unsigned short r, 
			size_t spr, unsigned int* d, size_t* dO);
		/** Draws a multicolor sprite line. */
		MCHEmul::UByte drawMultiColorSpriteOver (unsigned short c, unsigned short r, 
			size_t spr, unsigned int* d, size_t* dO);

		// Final drawing output and collision status.
		/** To move the computed graphics/sprite result to screen memory. */
		void drawResultToScreen (const DrawResult& cT, const DrawContext& dC);
		/** To detect collisions between graphics and sprites, and between sprites. 
			The second parameter is a byte which bits points out which sprites were or not drawn. \n
			The thors parameter defines whether the detection of the collision and data is or not active. */
		void detectCollisions (const DrawResult& cT, const MCHEmul::UByte& sD, bool sdCA);

		// Optional event visualization.
		/** To draw debug/event markers if _drawOtherEvents is active. */
		void drawOtherEvents (unsigned short cav, unsigned short rv);

		// --------------------------------------------------------------------
		// Memory reads performed by the raster-cycle pipeline.
		// --------------------------------------------------------------------
		// Character/color and graphics data.
		/** To get the logical index used to access the internal Video Matrix / Color RAM line. */
		inline size_t videoMatrixLineIndex () const;
		/** To get the emulator-side index used to access the 40-byte graphics buffers. */
		inline size_t graphicAccessIndex () const;
		/** To read Video Matrix and Color RAM during an effective normal bad-line c-access. */
		inline void readVideoMatrixAndColorRAM ();
		/** To read graphic data using the information previously fetched from
			Video Matrix / Color RAM, or from idle-state fixed addresses. \n
			_VC determines the real VIC-II memory position when needed.
			_GAccessIndex determines the emulator-side 40-byte buffer position. */
		inline void readGraphicalInfo ();

		// Sprite data.
		/** Legacy helper not used by the raster pipeline. \n
			Do not call from timing code because readSpriteData increments MC for DMA-active sprites. */
		inline void readSpritesData ();
		/** To read the sprite pointer and, when DMA is active, the three sprite data bytes. */
		inline bool readSpriteData (size_t nS);

		// --------------------------------------------------------------------
		// Different debug methods to simplify the internal code
		// --------------------------------------------------------------------
		// and to make simplier the modification in case it is needed...
		/** Debug special situations...
			Take care using this instructions _deepDebugFile could be == nullptr... */
		void debugDisconnected (MCHEmul::CPU* cpu);
		void debugVICIICycle (MCHEmul::CPU* cpu, unsigned int i);
		/** Records a buffered VIC-II register write when it becomes effective at
			its predicted absolute CPU cycle. */
		void debugVICIIRegisterWriteApplied
			(unsigned int cC, const MCHEmul::SetMemoryCommand* command,
			 size_t writeIndex, size_t pendingAfter);
		/** Returns the stop windows in a compact format suitable for deep debug. */
		std::string debugCPUStopWindowsAsString (const CPUStopWindows& windows) const;
		/** Returns the current CPU stop prediction in a compact format. */
		std::string debugCPUStopPredictionAsString () const;
		/** Returns the effective write positions of the current CPU prediction. */
		std::string debugCPUWriteEffectPositionsAsString () const;
		/** Records the prediction calculated for an instruction notification. */
		void debugCPUStopPrediction
			(const MCHEmul::InstructionContextEventData* dt);
		/** Records the prediction calculated for an interrupt notification. */
		void debugCPUStopPrediction
			(const MCHEmul::InterruptContextEventData* dt);
		/** Records why an already pending prediction has been recalculated. */
		void debugCPUStopPredictionRecalculated (const std::string& reason);
		/** Records the exact CPU cycle where a predicted stop is requested. */
		void debugCPUStopRequested (unsigned int cC) const;
		void debugBadLine ();
		void debugReadingSpriteInfo (size_t nS);
		void debugSpriteDrawFinishes (size_t nS);
		void debugSpriteDrawToStart (size_t nS);
		/** Records the data and counters targeted by the completed c-access,
			including whether the access returned DMA-delay/FLI fallback data. */
		void debugReadingVideoMatrix (bool invalidCAccess);
		/** Records the data and the counters used by the completed g-access.
			It must be called before advancing VC, VLMI and GAccessIndex. */
		void debugReadingGraphics ();
		/** Records both the raster position and the graphics-buffer interval
			selected to compose the current eight-pixel slice. */
		void debugDrawPixelAt (unsigned short cav, int cb);
		void debugDrawSpriteAt (size_t nS, unsigned short x, unsigned short r);
		// -----

		protected:
		/** The type of interrupt that are launched from this chip. */
		int _interruptId;
		/** A reference to the color RAM. */
		MCHEmul::PhysicalStorageSubset* _colorRAM;
		/** With the address it belongs to. */
		MCHEmul::Address _colorRAMAddress;
		/** The memory is used also as the set of registers of the chip. */
		COMMODORE::VICIIRegisters* _VICIIRegisters;
		/** The number of the memory view used to read the data. */
		int _VICIIView;
		/** The number of cycles per raster line as it depends on the type of Chip. */
		unsigned short _cyclesPerRasterLine;
		/** The difference with the PAL System. */
		unsigned short _incCyclesPerRasterLine;
		/** The raster. */
		MCHEmul::Raster _raster;
		/** To draw or not to draw lines at the positions where the raster interruptions are generated. */
		bool _drawRasterInterruptPositions;
		/** To draw or not to draw a border around the sprites active at the line where the raster is in. */
		bool _drawSpritesBorder;
		/** To draw or not other different events. */
		bool _drawOtherEvents;

		// Implementation
		/** When the CPU is not stopped (sometimes the VIC requires to stop it). \n 
			and a instruction is executed, the number of cycles that that instruction required, has to be taken into account
			to define what the VICII has to do. */
		unsigned int _lastCPUCycles;
		/** The format used to draw. It has to be the same that is used by the Screen object. */
		SDL_PixelFormat* _format;
		/** When a raster line is processed, it is necessary to know which cycle is being processed. 
			The number of max cycles is get from the method (@see) "cyclesPerRasterLine". */
		unsigned short _cycleInRasterLine;
		/** Last byte read by the VIC-II from the 8-bit memory data bus. \n
			It is updated by matrix, graphics, sprite and invalid DMA-delay/FLI
			accesses. It is not yet connected to CPU open-bus reads. */
		MCHEmul::UByte _lastVICDataRead;
		/** Low nibble of the opcode most recently notified by the CPU. \n
			In full-instruction mode it represents the CPU data bits D0-D3
			available when the following opcode fetch is stopped by BA. */
		MCHEmul::UByte _cpuOpcodeLowNibble;
		/** Immutable stop-window combinations indexed by bad-line state and the
			eight-bit sprite-DMA mask. They are built once at initialization. */
		CPUStopWindowSets _cpuStopWindowSets;
		/** Selected immutable windows for the current and following raster lines. \n
			The following-line set is required because sprite windows and delayed
			instructions can cross the horizontal raster boundary. */
		const CPUStopWindows* _currentCPUStopWindows;
		const CPUStopWindows* _nextCPUStopWindows;
		/** Reusable current-line storage used only for a bad line whose BA start
			differs from the normal precalculated cycle 12. */
		CPUStopWindows _adjustedCurrentCPUStopWindows;
		/** Actual sprite-DMA state most recently observed by the bus-arbitration
			pipeline. It is deliberately separate from the masks of raster-line
			slots because DMA can finish between the early and late sprite slots. */
		unsigned char _spriteDMAStateMask;
		/** Sprite-DMA slot masks represented by the selected current-line and
			next-line window sets. They are not simple copies of _DMAActive. */
		unsigned char _currentSpriteDMAMask;
		unsigned char _nextSpriteDMAMask;
		/** Minimal context retained only while its stop prediction can still be
			recalculated after a VIC-II timing-state transition. */
		PendingCPUTransaction _pendingCPUTransaction;
		/** Current projection of the pending CPU transaction over the stop windows. */
		CPUStopPrediction _pendingCPUStopPrediction;
		/** Buffered CPU writes targeting the VIC-II register subset and not yet
			applied at their predicted bus cycles. \n
			The first element is always the next command to execute. Executed
			commands are erased, so no separate consumption index is required. \n
			For a 6500 instruction targeting VIC-II registers, command order is the
			same as the write-cycle order stored in BusCycleData. */
		MCHEmul::SetMemoryCommands _pendingRegisterWrites;
		/** True when DEN has been active at least once during raster line $30. */
		bool _DENSeenAtLine30;
		/** True when a bad line has already been accepted in the current raster line. \n
			This avoids detecting the same bad line multiple times because simulate ()
			runs once per VIC-II cycle. */
		 bool _badLineAlreadyDetectedThisLine;
		/** True when the bad line condition is active in the current VIC - II cycle. */
		bool _badLineConditionActive;
		/** True when a Bad Line Condition has been seen in cycles 54-57 while
			RC == 7 in the current raster line. \n
			In that case the graphics sequencer must not return to idle state
			at cycle 58, even if the Bad Line Condition is no longer active
			exactly at cycle 58. */
		bool _badLinePreventedIdleThisLine;
		/** True when the BA-like CPU stop request for the current bad line
			has already been scheduled. */
		bool _badLineBAAlreadyRequested;
		/** First raster cycle in which the scheduled BA-like CPU stop request for
			the current bad line is effective. 0 means that no BA request has been
			scheduled for this line. */
		unsigned short _badLineBARequestCycle;
		/** True when a bad-line c-access sequence has been latched for the current raster line. \n
			This does not necessarily mean that real Video Matrix / Color RAM reads
			will be performed: some attempts can be invalid DMA-delay/FLI accesses
			returning $ff, and aborted sequences can be blocked by
			_badLineCAccessAllowedThisLine. */
		bool _badLineCAccessActive;
		/** True when the current bad-line c-access sequence is allowed to perform
			matrix/color access attempts in this raster line. \n
			Some of those attempts can be invalid DMA-delay/FLI accesses and therefore
			write $ff instead of reading real Video Matrix data. */
		bool _badLineCAccessAllowedThisLine;
		/** Number of initial c-access attempts that must read $ff instead of real
			Video Matrix data. \n
			This models the AEC/BA delay effect used by FLI / DMA delay / VSP. */
		unsigned short _badLineInvalidCAccessCycles;
		/** Color-data nibble latched when a DMA-delay/FLI c-access sequence starts. \n
			While AEC is still high, the VIC-II receives CPU D0-D3 through U16
			instead of valid Color RAM data. */
		MCHEmul::UByte _badLineInvalidColorData;
		/** Raster cycle where the current bad-line c-access sequence was latched. \n
			For a late sequence, its first attempted c-access can occur in the following
			cycle. 0 means that no c-access sequence is active in the current line. */
		unsigned short _badLineCAccessStartCycle;
		/** Whether the vertical raster has entered the last VBlank zone already. */
		bool _lastVBlankEntered;
		/** A temporal variable to indicate whether a lightpen position
			has already been latched in the current frame.
			This variable is reset at the beginning of every frame. */
		bool _lightPenFrameLatched;
		/** A very temporal variable to keep when the button is pressed. */
		bool _lightPenButtonPressed;

		/** Structure used to control how the graphics sequencer displays text and bitmap data. \n
		  *	Reference: https://www.cebix.net/VIC-Article.txt, sections 3.7.1 and 3.7.2. \n
		  *	The VIC-II graphics sequencer can be in either idle state or screen/display state. \n
		  *
		  * In display state:
		  * - g-accesses read character or bitmap graphics data;
		  * - VC and the logical VMLI/VLMI advance after each g-access;
		  * - bad-line c-access attempts can fill the internal 40-byte Video Matrix /
		  *   Color RAM line buffer either with real matrix/color data or, in DMA-delay /
		  *   FLI cases, with initial invalid $ff screen-code data and provisional color
		  *   data before normal matrix/color reads resume.
		  *
		  *	In idle state:
		  *	- no regular Video Matrix / Color RAM c-accesses are performed;
		  *	  the first attempted c-access of a late DMA-delay transition can
		  *	  occur before display state becomes visible to the g-access path;
		  *	- g-accesses still occur, but they read from the idle address:
		  *	  $3fff normally, or $39ff when ECM affects the address lines;
		  *	- the display output is produced from the idle graphics data and uses color 0.
		  *
		  *	This emulator separates two concepts:
		  *	- _VLMI is the logical Video Matrix Line Index used to address
		  *	  _screenCodeData and _colorData;
		  *	- _GAccessIndex is an emulator-side 0..39 buffer index used to address
		  *	  _graphicData, _screenCodeDrawData and _colorDrawData.
		  *
		  *	Relevant raster-cycle rules in this implementation:
		  * - At cycle 14, VC is loaded from VCBASE and the line access indexes are reset.
		  *   If a Bad Line Condition is active, RC is reset to 0 and c-access attempts
		  *   are allowed for this line. If the sequence was first latched at cycle 14,
		  *   the first attempts are treated as invalid FLI/DMA-delay accesses.
		  *   If the condition was latched earlier
		  *   but is no longer active at cycle 14, the c-access sequence is cancelled. 
		  *	- c-accesses occur in phi2 of cycles 15..54 and g-accesses in phi1 of
		  *	  cycles 16..55. In cycles containing both, g-access and its counter
		  *	  advance precede the c-access that prepares the following entry.
		  *	  _GAccessIndex advances on every g-access. VC and _VLMI advance only
		  *	  while the sequencer is in display state.
		  *	- When a late Bad Line Condition is accepted while the sequencer is
		  *	  idle, its first attempted c-access occurs after the g-access of that
		  *	  cycle, which therefore still uses idle state.
		  *	  Display state becomes active after that bus cycle, so VC and _VLMI
		  *	  start advancing with the following g-access.
		  *	- At cycle 58, if RC == 7, VCBASE is loaded from VC. The sequencer enters
		  *	  idle state only if there is no current Bad Line Condition and no late
		  *	  Bad Line Condition has prevented idle entry for this line.
		  *	- RC is incremented only if the sequencer remains in display state after
		  *	  the cycle-58 decision. */
		struct VICGraphicInfo
		{
			VICGraphicInfo ()
				: _VCBASE (0), _VC (0),
				  _RC (0),
				  _ROW (0), // This is the row where the raster is once it has been recognized by the VICII
				  _VLMI (0),
				  _GAccessIndex (0),
				  _idleState (true),
				  _ffVBorder (false),
				  _ffMBorder (false),
				  _ffLBorder (false), _ffRBorder (false),
				  _ffMBorderBegin (0), _ffMBorderPixels (0),
				  _screenCodeData (std::vector <MCHEmul::UByte> (40, MCHEmul::UByte::_0)),
				  _graphicData (std::vector <MCHEmul::UByte> (40, MCHEmul::UByte::_0)),
				  _colorData (std::vector <MCHEmul::UByte> (40, MCHEmul::UByte::_0)),
				  _screenCodeDrawData (std::vector <MCHEmul::UByte> (40, MCHEmul::UByte::_0)),
				  _colorDrawData (std::vector <MCHEmul::UByte> (40, MCHEmul::UByte::_0)),
				  _lastScreenCodeDataRead (MCHEmul::UByte::_0),
				  _lastGraphicDataRead (MCHEmul::UByte::_0),
				  _lastColorDataRead (MCHEmul::UByte::_0)
							{ }

			void emptyVideoMatrixAndColorRAMData ()
							{ _screenCodeData	= std::vector <MCHEmul::UByte> (40, MCHEmul::UByte::_0);
							  _colorData		= std::vector <MCHEmul::UByte> (40, MCHEmul::UByte::_0); }
			
			void emptyGraphicData ()
							{ _graphicData			= std::vector <MCHEmul::UByte> (40, MCHEmul::UByte::_0);
							  _screenCodeDrawData	= std::vector <MCHEmul::UByte> (40, MCHEmul::UByte::_0);
							  _colorDrawData		= std::vector <MCHEmul::UByte> (40, MCHEmul::UByte::_0); }


			/** 
				The name of the fields are: \n
				VCBASE			= Attribute: 10-bit video position counter base for the current character/bitmap row.
				VC				= Attribute: 10-bit video position counter. It advances by 1 during display-state g-accesses.
				VLMI			= Attribute: Logical Video Matrix Line Index. \n
				GAccessIndex	= Attribute: Emulator-side graphics access index within the 40-byte line buffer. \n
				RC				= Attribute: Line position within the graphics memory. From 0 to 7. \n
				ROW				= Attribute: Row number. Depending on the VICII type. \n
				IDLE			= Attribute: Whether the VICII is or not in idle state.
			  */
			MCHEmul::InfoStructure getInfoStructure () const;

			// Internal elements used to read the memory from the VICII
			// Read what they do at the header of the class. Very important counters...
			unsigned short _VCBASE, _VC;
			/** Logical Video Matrix Line Index. \n
				It advances only when the VIC-II is in screen/display state. \n
				It can diverge from _GAccessIndex when the VIC-II is in idle state. */
			unsigned short _VLMI;
			/** Graphics access index within the current 40-column graphics fetch line. \n
				This is not a real VIC-II register. It is an emulator-side index used to
				separate the horizontal graphics buffer position from the logical VLMI/VMLI. \n
				It is now the index used to access the emulator-side graphics buffers.
				It can diverge from _VLMI when the VIC-II is in idle state. */
			unsigned short _GAccessIndex;
			unsigned char _RC;
			/** Where the raster is from the VICII perspective. \n
				This counter gets updated just when the first cycle of the VICII happens. */
			unsigned short _ROW;
			/** Whether the VIC-II graphics sequencer is in idle state. \n
				The sequencer leaves idle state when a new Bad Line Condition is accepted. \n
				It can return to idle state at cycle 58 when RC == 7, provided that no
				current Bad Line Condition is active and no late Bad Line Condition in
				cycles 54..57 has prevented the idle transition for this raster line. \n
				Even in idle state, the graphics sequencer still produces display data
				from the idle g-access address, using color 0. */
			bool _idleState;

			// Internal elements used to manage the border...
			bool _ffVBorder; 
			bool _ffMBorder;
			/** Rendering-only flags describing a horizontal border transition inside
				the current eight-pixel raster slice. \n
				The simulation evaluates the hardware comparator before rendering, but
				pixels preceding and following its exact position need different border
				states. These flags are not VIC-II hardware flip-flops. */
			bool _ffLBorder, _ffRBorder;
			/** Beginning and length of the border portion inside the current visible
				eight-pixel slice. They are rendering data derived from the hardware
				comparator result. */
			unsigned short _ffMBorderBegin;
			unsigned char _ffMBorderPixels;

			// Implementation...
			// This one doesn't actually exist "in" the VICII chip, 
			// but is used when he left border has to be partially drawn.
			// After doing so, the _ffMBorder will become false...
			mutable MCHEmul::UBytes _screenCodeData;
			mutable MCHEmul::UBytes _graphicData; 
			mutable MCHEmul::UBytes _colorData;
			/** Video Matrix and Color RAM data aligned with _graphicData for drawing. */
			mutable MCHEmul::UBytes _screenCodeDrawData;
			mutable MCHEmul::UBytes _colorDrawData;
			/** The last info read. */
			mutable MCHEmul::UByte _lastScreenCodeDataRead;
			mutable MCHEmul::UByte _lastGraphicDataRead;
			mutable MCHEmul::UByte _lastColorDataRead;
		};

		VICGraphicInfo _vicGraphicInfo;

		/** 
		  * Structure used in controlling how sprites are managed: \n
		  *	Important things that happen during the raster line and affects the sprites... \n
		  *	As the raster moves the VICII has to decide	which sprite info to draw.\n
		  *	That is based on info gather in three major variables. \n
		  *	The rules to manipulate every value are decribed below attending to the cycle in the raster. \n
		  *	https://www.cebix.net/VIC-Article.txt. (point 3.8.1): \n
		  *	The vertical flipflop is set as long as 
		  *	the bit in the resgiter $d017 (vertical expansion) is cleared. \n
		  *	CYCLE 55 (PAL):			If the bit in the register $d017 is set, the _FF is inverted. \n
		  *	CYCLE 55 & 56 (PAL):	The VICII checks whether every sprite is on (bit at $d015) 
		  *							and the Y coordinate of the sprite matches the lower 8 bits of the raster. \n
		  *							If _DMA is still off, the is it is activated, _MCBASE is cleared,
		  *							and if the bit in the register $d017 is set the _FF is reset (put back to true). \n
		  *	CYCLE 58 (PAL):			_MCBASE is moved to MC. \n
		  *							If _DMA is on and (again) the Y coordinate matches the lower 8 bits of the raster 
		  *							the visualization of the Sprite is swithed on!. \n
		  *	CYCLEs ss:				If _DMA is switched the sprite data is accesed and the _MC is incremented in 3. \n
		  *	CYCLE visible:			If _displayActive is true, the sprite info is drawn. \n
		  *							The rules to dowble the X size are taken as the visualization of each comes. \n
		  *	CYCLE 15:				If the _FF is set, _MCBASE is incremented in 2. \n
		  *	CYCLE 16:				If the _FF is set, _MCBASE is incremented in 1. \n
		  *							If _MCBASE is 63 then _DMA is set to off. \n
		  * In this simulation DMA and display are separated. \n
		  * Sprite bytes are kept as a 3-byte line buffer. Horizontal shift
		  * position is derived from the circular distance to the X coordinate;
		  * the implementation still does not model the two VIC-II clock phases.
		  */
		struct VICSpriteInfo
		{
			VICSpriteInfo ()
				: _DMAActive (false), _displayActive (false),
				  _MCBASE (0), _MC (0), _expansionY (false),
				  _spriteBaseAddress ({ 0x00, 0x00 }, true),
				  _graphicsLineSprites (MCHEmul::UBytes::_E),
				  _drawing (false), _xS (0)
							{ }

			bool _DMAActive;		// VIC-II sprite DMA state. It controls sprite s-accesses and bus stealing.
			bool _displayActive;	// VIC-II sprite display state. It controls drawing and collisions.
			unsigned char _MCBASE;	// Base sprite data counter updated at cycles 15/16.
			unsigned char _MC;		// Sprite data counter used by the actual s-accesses.
			bool _expansionY;
			mutable MCHEmul::Address _spriteBaseAddress;
			mutable MCHEmul::UBytes _graphicsLineSprites;
			/** True once the horizontal sprite comparator has matched for the
				current sprite data line. */
			bool _drawing;
			/** Internal sprite X coordinate latched when the comparator matched.
				The internal horizontal coordinate system is shifted four pixels. */
			unsigned short _xS;
		};

		VICSpriteInfo _vicSpriteInfo [8];

		/** The events that can be drawn
			associated to the movement of the raster line. */
		struct EventsStatus
		{
			/** When the internal variable to indicate the status of the main border, changes. */
			MCHEmul::Pulse _ffVBorderChange;
			/** Same when what change is an auxiliar variable to support changes in the border. */
			MCHEmul::Pulse _ffMBorderChange;
			/** The bad line to hightlight. */
			unsigned short _badLine;
			// Managing the lightpen related events...
			/** A lightpen position has been latched. */
			bool _lightPenPositionLatched;
			/** The lightpen position latched is different than the previous one. */
			MCHEmul::OBool _lightPenPositionChanged;
		};

		mutable EventsStatus _eventStatus;

		private:
		static const MCHEmul::Address _MEMORYPOSIDLE1, _MEMORYPOSIDLE2;
	};

	// ---
	inline void VICII::treatBadLineStateAtCurrentCycle ()
	{
		const bool previousBadLineCondition = _badLineConditionActive;
		const bool previousCAccessActive = _badLineCAccessActive;
		const unsigned short previousCAccessStartCycle = _badLineCAccessStartCycle;

		updateDENSeenAtLine30 ();

		// Current Bad Line Condition for this exact VIC-II cycle.
		_badLineConditionActive = isBadLineCondition ();

		// Bauer: a Bad Line Condition in cycles 54..57 while RC == 7 prevents
		// the graphics sequencer from returning to idle state at cycle 58.
		if (_badLineConditionActive &&
			_vicGraphicInfo._RC == 7 &&
			_cycleInRasterLine >= _BADLINE_IDLE_PREVENT_FIRST_CYCLE &&
			_cycleInRasterLine <= _BADLINE_IDLE_PREVENT_LAST_CYCLE)
			_badLinePreventedIdleThisLine = true;

		// A Bad Line Condition can switch the sequencer to display state only
		// during the hardware acceptance window. Conditions before cycle 12
		// remain observable but must not alter VC, VCBASE, RC or the idle state.
		if (_badLineConditionActive &&
			!_badLineAlreadyDetectedThisLine &&
			_cycleInRasterLine >= _BADLINE_DISPLAY_FIRST_CYCLE &&
			_cycleInRasterLine <= _BADLINE_DISPLAY_LAST_CYCLE)
		{
			_badLineAlreadyDetectedThisLine = true;

			// DMA delay switches an idle sequencer to display state in the cycle
			// following recognition of the late Bad Line Condition.
			const bool lateDMAFromIdle =
				idleStateActive () &&
				_cycleInRasterLine > 14 &&
				_cycleInRasterLine <= _BADLINE_START_LAST_CYCLE;
			if (!lateDMAFromIdle)
				enterScreenState ();

			_eventStatus._badLine =
				_raster.vData ().currentVisiblePosition ();

			_IFDEBUG debugBadLine ();
		}

		// Latch a bad-line c-access sequence if the condition appears inside
		// Bauer's BA/c-access start window. This preserves the timing/state effect.
		// If the sequence is already valid at cycle 14, normal/FLI-like c-access
		// handling is decided there. If it appears after cycle 14, it is treated as
		// a late DMA-delay/VSP-like sequence: c-access attempts are allowed, but the
		// first ones return invalid $ff data.
		if (!_badLineCAccessActive &&
			_badLineConditionActive &&
			_cycleInRasterLine >= _BADLINE_START_FIRST_CYCLE &&
			_cycleInRasterLine <= _BADLINE_START_LAST_CYCLE)
		{
			_badLineCAccessActive = true;
			_badLineCAccessStartCycle = _cycleInRasterLine;

			// If the Bad Line Condition appears after cycle 14, this is a late
			// DMA-delay/VSP-like sequence. Cycle 14 has already passed, so the line
			// will not be validated by treatGraphicFetchStartCycle(). Allow c-access
			// attempts here and mark the first three as invalid.
			if (_cycleInRasterLine > 14)
			{
				_badLineCAccessAllowedThisLine = true;
				_badLineInvalidCAccessCycles = 3;
			}
		}

		if (previousBadLineCondition != _badLineConditionActive ||
			previousCAccessActive != _badLineCAccessActive ||
			previousCAccessStartCycle != _badLineCAccessStartCycle)
			actualizeCPUStopWindowsAfterBadLineChange ();
	}

	// ---
	inline void VICII::resetBadLineStateForNewRasterLine ()
	{
		_badLineAlreadyDetectedThisLine = false;
		_badLineConditionActive = false;
		_badLinePreventedIdleThisLine = false;
		_badLineBAAlreadyRequested = false;
		_badLineBARequestCycle = 0;
		_badLineCAccessActive = false;
		_badLineCAccessAllowedThisLine = false;
		_badLineInvalidCAccessCycles = 0;
		_badLineInvalidColorData = MCHEmul::UByte::_0;
		_badLineCAccessStartCycle = 0;
	}

	// ---
	inline void VICII::updateDENSeenAtLine30 ()
	{
		// The DEN condition for bad lines is latched during raster line $30.
		// It is not the instantaneous value of DEN at the end of the line.
		if (_vicGraphicInfo._ROW == 0 && _cycleInRasterLine == 1)
			_DENSeenAtLine30 = false;

		if (_vicGraphicInfo._ROW == _FIRSTBADLINE &&
			!_VICIIRegisters -> blankEntireScreen ())
			_DENSeenAtLine30 = true;
	}

	// ---
	inline bool VICII::isBadLineCondition () const
	{
		return (
			_DENSeenAtLine30 && // Bad lines are possible only if DEN was seen active
								// during raster line $30 in the current frame...
			_vicGraphicInfo._ROW >= _FIRSTBADLINE &&
			_vicGraphicInfo._ROW <= _LASTBADLINE &&
			(unsigned char) (_vicGraphicInfo._ROW & 0x07) /** The three last bits only */ ==
				_VICIIRegisters -> verticalScrollPosition ()); // aligned with the scrollY
	}

	// ---
	inline void VICII::treatGraphicFetchStartCycle ()
	{
		// At this cycle, the VIC-II reloads VC from VCBASE and starts a new
		// 40-position graphics fetch sequence for the current raster line.
		_vicGraphicInfo._VC = _vicGraphicInfo._VCBASE & _VCMASK;

		resetGraphicAccessCountersForCurrentLine ();

		// Cycle 14 is the decisive point for a normal bad-line matrix fetch.
		// If the Bad Line Condition is active here, the c-access sequence is allowed
		// for this line. A sequence first latched at cycle 14 is treated as FLI-like:
		// the first c-access attempts return $ff before normal matrix/color reads.
		// Bad lines that appear after cycle 14 are enabled later by
		// treatBadLineStateAtCurrentCycle() as DMA-delay/VSP-like sequences.
		_badLineCAccessAllowedThisLine = _badLineConditionActive;
		_badLineInvalidCAccessCycles = 0;

		if (_badLineConditionActive)
		{
			// A normal or FLI-like bad line is valid at cycle 14.
			// If the c-access sequence was first latched exactly at cycle 14,
			// emulate the FLI/DMA-delay effect: the first three c-access attempts
			// read $ff on D0-D7.
			if (_badLineCAccessActive &&
				_badLineCAccessStartCycle == 14)
				_badLineInvalidCAccessCycles = 3;

			_vicGraphicInfo._RC = 0;
		}
		else
		{
			// A bad-line c-access sequence could have been latched in cycles 12/13
			// and then aborted before cycle 14. The sequencer remains in display
			// state, but no normal matrix/color c-accesses continue.
			_badLineCAccessActive = false;
			_badLineCAccessAllowedThisLine = false;
			_badLineInvalidCAccessCycles = 0;
			_badLineCAccessStartCycle = 0;
		}

		actualizeCPUStopWindowsAfterBadLineChange ();
	}

	// ---
	inline void VICII::treatGraphicRowEndCycle ()
	{
		// At this cycle, if the last row of the current character/bitmap cell has
		// just been processed, VC becomes the new VCBASE for the next character row.
		if (_vicGraphicInfo._RC == 7)
		{
			_vicGraphicInfo._VCBASE = _vicGraphicInfo._VC & _VCMASK;
	
			// If no current Bad Line Condition is active, and no late Bad Line
			// Condition in cycles 54..57 has prevented the transition, the VIC-II
			// returns to idle state after completing the current 8-row block.
			if (!_badLineConditionActive &&
				!_badLinePreventedIdleThisLine)
				enterIdleState ();
		}
	
		// RC advances only if the sequencer remains in display state after the
		// cycle-58 decision. If idle state was entered, RC is preserved.
		if (screenStateActive ())
		{
			if (++_vicGraphicInfo._RC == 8)
				_vicGraphicInfo._RC = 0;
		}
	}

	// ---
	inline bool VICII::spriteYMatchesCurrentRaster (size_t nS) const
	{
		return ((unsigned char) (_vicGraphicInfo._ROW & 0xff) ==
			_VICIIRegisters -> spriteYCoord (nS));
	}

	// ---
	inline void VICII::treatSpriteDMAStartAtCurrentCycle ()
	{
		if (_cycleInRasterLine != 55 && _cycleInRasterLine != 56)
			return;

		for (size_t i = 0; i < 8; i++)
		{
			// Sprite DMA is checked in cycles 55/56 before BA/RDY arbitration.
			// The cycle-58 sprite data slot depends on this state already being known.
			if (_cycleInRasterLine == 55 &&
				_VICIIRegisters -> spriteDoubleHeight (i))
				_VICIIRegisters -> invertExpansionYFlipFlop (i);

			if (!_vicSpriteInfo [i]._DMAActive &&
				_VICIIRegisters -> spriteEnable (i) &&
				spriteYMatchesCurrentRaster (i))
			{
				_vicSpriteInfo [i]._DMAActive = true;
				_vicSpriteInfo [i]._MCBASE = 0;
				_vicSpriteInfo [i]._MC = 0;
				_vicSpriteInfo [i]._graphicsLineSprites = MCHEmul::UBytes::_E;
				_vicSpriteInfo [i]._drawing = false;

				// Starting DMA for a Y-expanded sprite resets the expansion flip-flop.
				if (_VICIIRegisters -> spriteDoubleHeight (i))
					_VICIIRegisters -> setExpansionYFlipFlop (i, false);
			}
		}
	}

	// ---
	inline void VICII::treatSpriteCounterCycle15 ()
	{
		for (size_t i = 0; i < 8; i++)
			if (_vicSpriteInfo [i]._DMAActive &&
				_VICIIRegisters -> expansionYFlipFlop (i))
				_vicSpriteInfo [i]._MCBASE += 2;
	}

	// ---
	inline void VICII::treatSpriteCounterCycle16 ()
	{
		for (size_t i = 0; i < 8; i++)
		{
			if (!_vicSpriteInfo [i]._DMAActive ||
				!_VICIIRegisters -> expansionYFlipFlop (i))
				continue;

			_vicSpriteInfo [i]._MCBASE++;

			if (_vicSpriteInfo [i]._MCBASE >= 63)
			{
				_vicSpriteInfo [i]._DMAActive = false;

				// Keep the current line buffer alive: the last sprite row can
				// still be displayed until cycle 58 observes DMA inactive.
				_IFDEBUG debugSpriteDrawFinishes (i);
			}
		}
	}

	// ---
	inline void VICII::treatSpriteDisplayStartCycle ()
	{
		for (size_t i = 0; i < 8; i++)
		{
			if (!_vicSpriteInfo [i]._DMAActive)
			{
				// Display is switched off at cycle 58 after DMA has already
				// finished, not at cycle 16 when MCBASE reaches 63.
				_vicSpriteInfo [i]._displayActive = false;

				continue;
			}

			// At cycle 58 the VIC-II copies MCBASE to MC. Display is enabled only
			// when the sprite Y coordinate still matches the current raster line.
			_vicSpriteInfo [i]._MC = _vicSpriteInfo [i]._MCBASE;

			if (spriteYMatchesCurrentRaster (i))
			{
				_vicSpriteInfo [i]._displayActive = true;
				_vicSpriteInfo [i]._drawing = false;
				_vicSpriteInfo [i]._xS = 0;
				_vicSpriteInfo [i]._expansionY =
					_VICIIRegisters -> spriteDoubleHeight (i);

				_IFDEBUG debugSpriteDrawToStart (i);
			}
		}
	}

	// ---
	inline void VICII::treatSpriteHorizontalStartAtCurrentCycle ()
	{
		for (size_t i = 0; i < 8; i++)
		{
			if (!_vicSpriteInfo [i]._displayActive ||
				_vicSpriteInfo [i]._drawing)
				continue;

			unsigned short x = _VICIIRegisters -> spriteXCoord (i) + 4;

			if (x >= _raster.currentColumn () &&
				x < (_raster.currentColumn () + _raster.step ()))
			{
				_vicSpriteInfo [i]._drawing = true;
				_vicSpriteInfo [i]._xS = x;

				_IFDEBUG debugDrawSpriteAt
					(i, x, _vicGraphicInfo._ROW);
			}
		}
	}

	// ---
	inline void VICII::treatGraphicAccessCycle ()
	{
		const bool gAccess = isGraphicAccessCycle ();
		const bool cAccess = isBadLineCAccessCycle ();
		if (!gAccess && !cAccess)
			return;

		// The scheduler advances once per CPU cycle, but the VIC-II bus pipeline
		// remains staggered. phi1 first performs the g-access prepared by the
		// preceding c-access. Its counter advance then selects the matrix entry
		// that phi2 prepares for the following cycle. Only c-access steals the
		// CPU-visible bus phase.
		memoryRef () -> setActiveView (_VICIIView);

		if (gAccess)
		{
			readGraphicalInfo ();

			// Record the counters and buffer index used by the completed g-access
			// before advancing them for the following graphics cycle.
			_IFDEBUG debugReadingGraphics ();

			advanceGraphicAccessCounters ();
		}

		if (cAccess)
			treatBadLineCAccessCycle ();

		memoryRef () -> setCPUView ();
	}

	// ---
	inline void VICII::treatBadLineCAccessCycle ()
	{
		if (!isBadLineCAccessCycle ())
			return;

		const unsigned short fCA = firstBadLineCAccessCycle ();

		const bool invalidCAccess =
			_badLineInvalidCAccessCycles > 0 &&
			_cycleInRasterLine >= fCA &&
			_cycleInRasterLine < (fCA + _badLineInvalidCAccessCycles);
		if (invalidCAccess)
		{
			const size_t vMLI = videoMatrixLineIndex ();

			// Latch CPU D0-D3 immediately before the first invalid c-access.
			// CPU notification has already advanced to the instruction reached
			// during the BA/AEC delay.
			if (_cycleInRasterLine == fCA)
				_badLineInvalidColorData = _cpuOpcodeLowNibble;

			// During the first invalid DMA-delay/FLI c-accesses, the VIC reads
			// $ff on D0-D7 instead of valid Video Matrix data.
			_vicGraphicInfo._lastScreenCodeDataRead =
				_lastVICDataRead =
				_vicGraphicInfo._screenCodeData [vMLI] = MCHEmul::UByte::_FF;

			// While AEC is still high, U16 connects CPU D0-D3 to the VIC-II
			// color-data inputs instead of selecting Color RAM.
			_vicGraphicInfo._lastColorDataRead =
				_vicGraphicInfo._colorData [vMLI] =
					_badLineInvalidColorData;

			_IFDEBUG debugReadingVideoMatrix (true);

			return;
		}

		readVideoMatrixAndColorRAM ();

		_IFDEBUG debugReadingVideoMatrix (false);

	}

	// ---
	inline unsigned short VICII::firstBadLineCAccessCycle () const
	{
		if (!_badLineCAccessActive ||
			_badLineCAccessStartCycle == 0)
			return (0);

		// Normal bad lines and FLI-like bad lines that are active by cycle 14
		// start their attempted c-accesses in phi2 of cycle 15.
		// Late DMA-delay/VSP sequences start attempting c-accesses in the cycle
		// immediately following recognition of the Bad Line Condition. The BA/AEC
		// delay makes the first attempts invalid; it does not postpone them.
		unsigned short result =
			(_badLineCAccessStartCycle <= 14)
				? _BADLINE_EFFECTIVE_CACCESS_FIRST_CYCLE
				: (unsigned short) (_badLineCAccessStartCycle + 1);
		if (result < _BADLINE_EFFECTIVE_CACCESS_FIRST_CYCLE)
			result = _BADLINE_EFFECTIVE_CACCESS_FIRST_CYCLE;

		return (result);
	}

	// ---
	inline bool VICII::isBadLineCAccessCycle () const
	{
		const unsigned short fCA = firstBadLineCAccessCycle ();
		return (
			_badLineCAccessAllowedThisLine &&
			fCA != 0 &&
			_cycleInRasterLine >= _BADLINE_EFFECTIVE_CACCESS_FIRST_CYCLE &&
			_cycleInRasterLine <= _BADLINE_EFFECTIVE_CACCESS_LAST_CYCLE &&
			_cycleInRasterLine >= fCA);
	}

	// ---
	inline void VICII::resetGraphicAccessCountersForCurrentLine ()
	{
		_vicGraphicInfo._VLMI = 0;
		_vicGraphicInfo._GAccessIndex = 0;
	}

	// ---
	inline void VICII::resetGraphicCountersForNewFrame ()
	{
		_vicGraphicInfo._VCBASE = _vicGraphicInfo._VC = 0;

		resetGraphicAccessCountersForCurrentLine ();
	}

	// ---
	inline void VICII::advanceGraphicAccessCounters ()
	{
		if (screenStateActive ())
		{
			_vicGraphicInfo._VC = 
				(_vicGraphicInfo._VC + 1) & _VCMASK;
			_vicGraphicInfo._VLMI++;
		}

		_vicGraphicInfo._GAccessIndex++;
	}

	// ---
	inline void VICII::advanceRasterPosition ()
	{
		// First move to the next raster cycle.
		_cycleInRasterLine++;

		// Move the horizontal raster.
		const bool cLine = _raster.hData ().add (1 * _raster.step ());

		// If the horizontal retrace has just been overpassed,
		// the vertical raster has to advance one line.
		if (_raster.hData ().retraceJustOverPassed ())
			_raster.vData ().next ();

		// Has the raster reached the end of the current line?
		if (cLine)
		{
			_cycleInRasterLine = 1;

			// Every piece of state selected below belongs to the new raster
			// line. ROW must be updated before evaluating bad-line conditions
			// or selecting the current stop-window set.
			_vicGraphicInfo._ROW = _raster.currentLine ();

			resetBadLineStateForNewRasterLine ();

			// At the first line of a frame, reset the video counters and allow
			// the light pen to latch a new position. On every other line, VC
			// starts from the current VCBASE value, but not RC
			if (_vicGraphicInfo._ROW == 0)
			{ 
				resetGraphicCountersForNewFrame ();
			
				// ...and also allow a new light pen latch in the new frame.
				_lightPenFrameLatched = false;
			}
			else 
			{
				// At the beginning of every other line, VC starts from VCBASE.
				_vicGraphicInfo._VC = _vicGraphicInfo._VCBASE & _VCMASK;
			}

			// Sprite DMA was decided during cycles 55/56. Once ROW represents
			// the new line, both immutable window sets can be selected without
			// inheriting the previous line's bad-line state.
			_currentSpriteDMAMask = _nextSpriteDMAMask;
			_spriteDMAStateMask = spriteDMAMask ();
			_nextSpriteDMAMask = projectedSpriteDMAMaskForNextRasterLine ();
			selectCPUStopWindowsForCurrentAndNextLine ();
			if (_pendingCPUTransaction.valid () &&
				!_pendingCPUStopPrediction._stopRequested)
			{
				// Cycle 1 of the former next line is now cycle 1 of the current
				// line. Keep the retained transaction origin in that coordinate
				// system before recalculating its prediction.
				_pendingCPUTransaction._startCycle -= _cyclesPerRasterLine;
				if (recalculatePendingCPUStopPrediction ())
					_IFDEBUG debugCPUStopPredictionRecalculated	("RasterLineChange");
			}
		}
	}

	// ---
	inline void VICII::treatRasterIRQAtCurrentPosition ()
	{
		const unsigned short cC =
			(_vicGraphicInfo._ROW == 0) ? 2 : 1;

		if (_cycleInRasterLine == cC &&
			_vicGraphicInfo._ROW == _VICIIRegisters -> IRQRasterLineAt ())
			_VICIIRegisters -> activateRasterIRQ ();
	}

	// ---
	inline void VICII::requestIRQIfNeeded (MCHEmul::CPU* cpu, unsigned int cC)
	{
		int cI = (int) _VICIIRegisters -> reasonIRQCode ();
		if (cI != 0)
			cpu -> requestInterrupt (
				_interruptId,
				cC,
				this,
				cI);
	}

	// ---
	inline void VICII::notifyGraphicsReadyIfNeeded ()
	{
		if (_raster.isInFirstVBlankZone ())
		{
			if (!_lastVBlankEntered)
			{
				_lastVBlankEntered = true;
	
				notify (MCHEmul::Event (_GRAPHICSREADY));
			}
		}
		else
			_lastVBlankEntered = false;
	}

	// ---
	inline void VICII::treatLightPenAtCurrentRasterPosition ()
	{
		if (!_VICIIRegisters -> lightPenActive () ||
			!_lightPenButtonPressed ||
			_lightPenFrameLatched ||
			!_VICIIRegisters -> isMouseInVisibleZone () ||
			!_raster.isInVisibleZone ())
			return; // Nothing to do really...

		unsigned short cv = 0;
		unsigned short rv = 0;
		_raster.currentVisiblePosition (cv, rv);
		const int mx = _VICIIRegisters -> mousePositionX ();
		const int my = _VICIIRegisters -> mousePositionY ();

		// The raster advances horizontally in 8-pixel blocks in this emulation.
		// And it is needed to compare visible thing with visible things...
		if (my != (int) rv || 
			!((mx >= (int) cv) && (mx < (int) (cv + _raster.step ()))))
			return;

		/** At this point the raster beam is passing through the mouse/light-pen
			position. The light pen detects the beam and drives LP low. \n
			Convert from visible mouse coordinate to the corresponding
			VIC-II raster coordinate. \n
			_raster.hData().currentPosition() is the current internal
			raster X position. mx - cv gives the pixel offset inside the
			current 8-pixel group. */
		unsigned char lxo, lyo;
		_VICIIRegisters -> lightPenPositionLatched (&lxo, &lyo); // Before changing this one...
		_VICIIRegisters -> latchLightPenPositionFromRaster
			((unsigned char) (((_raster.hData ().currentPosition () + 
				((unsigned short) mx - cv)) >> 1) & 0xff), // LPX is in 2-pixel units...
			 (unsigned char) (_raster.vData ().currentPosition () & 0xff));
		unsigned char lx, ly;
		_VICIIRegisters -> lightPenPositionLatched (&lx, &ly); // The new one...

		_eventStatus._lightPenPositionLatched = true;
		_eventStatus._lightPenPositionChanged = (lx != lxo) || (ly != lyo);

		_lightPenFrameLatched = true;

		_VICIIRegisters -> activateLightPenOnScreenIRQ ();
	}

	// ---
	inline void VICII::actualizeMainBorderStatus (unsigned short cav)
	{
		_vicGraphicInfo._ffMBorderBegin = cav;
		_vicGraphicInfo._ffMBorderPixels = (cav + 8) > _raster.visibleColumns () 
			? (_raster.visibleColumns () - cav) : 8;

		// The real VIC-II evaluates the horizontal comparators pixel by pixel,
		// while this simulation advances the raster by one eight-pixel VIC-II
		// cycle. currentColumn() is the internal horizontal raster coordinate and
		// the CSEL-dependent limits in VICIIRegisters use that same coordinate
		// system. The circular distance therefore predicts whether the exact
		// comparator value will be reached within the current cycle; it is not an
		// interval comparison replacing the hardware comparator.
		unsigned short pixelsPerRasterLine = _cyclesPerRasterLine << 3;
		unsigned short rightComparator = _VICIIRegisters -> maxRasterH ();
		unsigned short distanceToRightComparator =
			(rightComparator + pixelsPerRasterLine - _raster.currentColumn ()) %
			pixelsPerRasterLine;

		// Bauer rule 1 sets the main border flip-flop at the right comparator.
		// A transition has to be prepared only while the flip-flop is clear; if
		// it is already set, reaching the comparator produces no visible change.
		if (distanceToRightComparator < _raster.step () &&
			!_vicGraphicInfo._ffMBorder)
		{
			// cav is deliberately used only from this point onwards. It is the
			// aligned coordinate of the slice in ScreenMemory, whereas the border
			// comparison above belongs to the internal raster coordinate system.
			// Convert the comparator to the visible coordinate system and calculate
			// how many pixels after it must be covered by the border.
			unsigned short rightVisiblePosition =
				_raster.columnInVisibleZone (rightComparator);
			unsigned short sliceEnd = ((cav + _raster.step ()) >
				_raster.visibleColumns ())
				? _raster.visibleColumns ()
				: (cav + _raster.step ());

			// The complete eight-pixel slice is composed before the border layer is
			// applied. _ffRBorder postpones the actual main flip-flop transition
			// until drawVisibleZone() has drawn this partial slice. This preserves
			// the pixels preceding the comparator as graphics or sprite output.
			_vicGraphicInfo._ffRBorder = true;
			_vicGraphicInfo._ffMBorderBegin =
				(rightVisiblePosition < cav)
					? cav
					: ((rightVisiblePosition > sliceEnd)
						? sliceEnd
						: rightVisiblePosition);
			_vicGraphicInfo._ffMBorderPixels =
				sliceEnd - _vicGraphicInfo._ffMBorderBegin;
		}
		
		// Detect the left comparator in the same internal coordinate system used
		// for the right one. CSEL may move this comparator while a line is being
		// generated, so the value currently stored in VICIIRegisters is sampled
		// for this VIC-II cycle. The modular distance also handles the horizontal
		// raster wrap without involving the visible, aligned cav coordinate.
		unsigned short leftComparator = _VICIIRegisters -> minRasterH ();
		unsigned short distanceToLeftComparator =
			(leftComparator + pixelsPerRasterLine - _raster.currentColumn ()) %
			pixelsPerRasterLine;
		bool atLeft = distanceToLeftComparator < _raster.step ();

		if (atLeft)
		{
			// The vertical border flip-flop was evaluated at the final cycle of the
			// corresponding raster line. The left comparator only consumes that state:
			// graphics can be exposed when the vertical border is already open. Test
			// the flip-flop itself instead of inferring its value
			// from the normal vertical display interval: timed RSEL changes can keep
			// the vertical border open outside that geometrical interval. Likewise,
			// no temporary transition is needed if the main flip-flop is already
			// clear when the comparator is reached.
			if (!_vicGraphicInfo._ffVBorder &&
				_vicGraphicInfo._ffMBorder)
			{
				// Pixels before the left comparator still belong to the main border;
				// pixels from the comparator onwards expose graphics and sprites. cav
				// is used only for this rendering calculation after the hardware event
				// has been detected with currentColumn().
				unsigned short leftVisiblePosition =
					_raster.columnInVisibleZone (leftComparator);
				unsigned short sliceEnd = ((cav + _raster.step ()) >
					_raster.visibleColumns ())
					? _raster.visibleColumns ()
					: (cav + _raster.step ());

				// _ffLBorder postpones clearing the main flip-flop until the partial
				// border has been drawn by drawVisibleZone(). The clamping keeps the
				// pixel count valid at either end of the visible output slice.
				_vicGraphicInfo._ffLBorder = true;
				_vicGraphicInfo._ffMBorderBegin = cav;
				_vicGraphicInfo._ffMBorderPixels =
					(leftVisiblePosition < cav)
						? 0
						: ((leftVisiblePosition > sliceEnd)
							? (sliceEnd - cav)
							: (leftVisiblePosition - cav));
			}
		}

		// This is used later to draw events...if active!
		_eventStatus._ffMBorderChange.set (_vicGraphicInfo._ffMBorder);
	}

	// ---
	inline void VICII::actualizeVerticalBorderStatus ()
	{
		// The border appears when the first vertical raster border position is reached...
		if (_raster.vData ().currentPosition () == _VICIIRegisters -> maxRasterV ()) 
			_vicGraphicInfo._ffVBorder = true;
		// ...and disappears when the first vertical raster visible position is reached...
		if (_raster.vData ().currentPosition () == _VICIIRegisters -> minRasterV () &&
				!_VICIIRegisters -> blankEntireScreen ())
			_vicGraphicInfo._ffVBorder = false;

		// This is used later to draw events...if active!
		_eventStatus._ffVBorderChange.set (_vicGraphicInfo._ffVBorder);
	}

	// ---
	inline size_t VICII::videoMatrixLineIndex () const
	{
		assert (_vicGraphicInfo._VLMI < _GRAPHMAXCHARCOLUMNS);

		return ((size_t) _vicGraphicInfo._VLMI);
	}

	// ---
	inline size_t VICII::graphicAccessIndex () const
	{ 
		assert (_vicGraphicInfo._GAccessIndex < _GRAPHMAXCHARCOLUMNS); 
		
		return ((size_t) _vicGraphicInfo._GAccessIndex);
	}

	// ---
	inline void VICII::readVideoMatrixAndColorRAM ()
	{
		// _VC determines the real VIC-II memory position to read from.
		// _VLMI determines the position in the internal 40-byte matrix/color line.
		const size_t vMLI = videoMatrixLineIndex ();
		const size_t vC = (size_t) (_vicGraphicInfo._VC & _VCMASK);
	
		_vicGraphicInfo._lastScreenCodeDataRead = 
			_lastVICDataRead =
			_vicGraphicInfo._screenCodeData [vMLI] =
				memoryRef () -> value (_VICIIRegisters -> screenMemory () + vC);
		// In the invalid text mode, the bits 6 & 7 won't be used.
		// In invalid bitmap modes 1 & 2, this information won't be used later.
	
		// Color RAM is accessed directly by the VIC-II when fetching matrix data.
		_vicGraphicInfo._lastColorDataRead = 
			_vicGraphicInfo._colorData [vMLI] =
				_colorRAM -> valueDirect (_colorRAMAddress + vC) &
					(_VICIIRegisters -> invalidGraphicMode () ? 0x08 : 0x0f); 
		// In the invalid text mode, only the MG flag is relevant at this point.
	}

	// ---
	inline void VICII::readGraphicalInfo ()
	{
		// _GAccessIndex selects the emulator-side 40-byte graphics buffer slot
		// being filled in this graphics access cycle.
		const size_t gAI = graphicAccessIndex ();
		const unsigned short vC = _vicGraphicInfo._VC & _VCMASK;

		// In idle state, the VIC-II reads from a fixed address depending on the
		// active bank and graphic mode. The fetched byte is stored in the graphics
		// line buffer at the current graphics access index.
		if (idleStateActive ()) 
		{
			_vicGraphicInfo._screenCodeDrawData [gAI] = MCHEmul::UByte::_0;
			_vicGraphicInfo._colorDrawData [gAI] = MCHEmul::UByte::_0;

			_vicGraphicInfo._lastGraphicDataRead =
				_lastVICDataRead =
				_vicGraphicInfo._graphicData [gAI] = 
					_VICIIRegisters -> graphicExtendedColorTextModeActive () 
						? memoryRef () -> value (_MEMORYPOSIDLE1 + (bank () << 14))
						: memoryRef () -> value (_MEMORYPOSIDLE2 + (bank () << 14));
		}
		// In display state, VMLI selects the logical matrix/color entry while
		// GAccessIndex selects the horizontal output slot receiving that entry.
		else 
		{
			const size_t vMLI = videoMatrixLineIndex ();
			const MCHEmul::UByte& screenCodeData =
				_vicGraphicInfo._screenCodeData [vMLI];

			_vicGraphicInfo._screenCodeDrawData [gAI] = screenCodeData;
			_vicGraphicInfo._colorDrawData [gAI] =
				_vicGraphicInfo._colorData [vMLI];

			_vicGraphicInfo._lastGraphicDataRead =
				_lastVICDataRead =
				_vicGraphicInfo._graphicData [gAI] = _VICIIRegisters -> textMode () 
					? memoryRef () -> value (_VICIIRegisters -> charDataMemory () + 
						(((size_t) screenCodeData.value () &
							((_VICIIRegisters -> graphicExtendedColorTextModeActive () ||
							  _VICIIRegisters -> invalidGraphicMode ()) ? 0x3f : 0xff))
							/** In extended-background mode or invalid text mode,
								there are only 64 possible characters. */ << 3) + _vicGraphicInfo._RC)
					: memoryRef () -> value (_VICIIRegisters -> bitmapMemory () +
						(((size_t) (vC & 
							(_VICIIRegisters -> invalidGraphicMode () ? 0x033f : _VCMASK))) << 3) +
						(size_t) _vicGraphicInfo._RC);
		}

		// In invalid bitmap modes, bits 6 and 7 of the VC position are not taken
		// into account when forming the bitmap address.
	}

	// ---
	inline void VICII::readSpritesData ()
	{
		for (size_t i = 0; i < 8; i++) 
		{
			// Put it back to null, first...
			_vicSpriteInfo [i]._graphicsLineSprites = MCHEmul::UBytes::_E;
			// ...and update the info if the sprite is active...
			readSpriteData (i); 
		}
	}

	// ---
	inline bool VICII::readSpriteData (size_t nS)
	{
		bool result = false;

		memoryRef () -> setActiveView (_VICIIView);

		// At a sprite slot the VIC-II always performs the pointer p-access.
		// The following three data bytes are real s-accesses only while DMA is active.
		MCHEmul::UByte sprPtr =
			memoryRef () -> value (_VICIIRegisters -> spritePointersMemory () + nS);
		_lastVICDataRead = sprPtr;

		_vicSpriteInfo [nS]._spriteBaseAddress =
			_VICIIRegisters -> initAddressBank () + ((size_t) sprPtr.value () << 6);

		if (_vicSpriteInfo [nS]._DMAActive)
		{
			MCHEmul::UBytes sprData = std::move (MCHEmul::UBytes
				(memoryRef () -> bytes (_vicSpriteInfo [nS]._spriteBaseAddress +
					(size_t) _vicSpriteInfo [nS]._MC, 3)));

			if (sprData.size () > 0)
				_lastVICDataRead = sprData [sprData.size () - 1];

			_vicSpriteInfo [nS]._graphicsLineSprites = std::move (sprData);
			_vicSpriteInfo [nS]._MC += 3;

			// New sprite data starts a new 24-pixel shift sequence for this raster line.
			_vicSpriteInfo [nS]._drawing = false;

			result = true;
		}
		else
			_vicSpriteInfo [nS]._graphicsLineSprites = MCHEmul::UBytes::_E;

		memoryRef () -> setCPUView ();

		return (result);
	}

	/** The version for PAL systems. 
		It couldn't be final. */
	class VICII_PAL : public VICII
	{
		public:
		static const MCHEmul::RasterData _VRASTERDATA;
		static const MCHEmul::RasterData _HRASTERDATA;

		static const unsigned short _CYCLESPERRASTERLINE = 63;

		VICII_PAL (int intId, MCHEmul::PhysicalStorageSubset* cR, 
			const MCHEmul::Address& cRA, int vV);

		private:
		virtual void addSpriteCPUStopWindow
			(size_t nS, CPUStopWindows& result) const override;
		virtual void treatRasterBusCycle () override;
	};

	/** The version for NTSC systems. \n
		It couldn't be final. */
	class VICII_NTSC : public VICII
	{
		public:
		static const MCHEmul::RasterData _VRASTERDATA;
		static const MCHEmul::RasterData _HRASTERDATA;

		VICII_NTSC (int intId, MCHEmul::PhysicalStorageSubset* cR, 
			const MCHEmul::Address& cRA, int vV);

		private:
		virtual void addSpriteCPUStopWindow
			(size_t nS, CPUStopWindows& result) const override;
		virtual void treatRasterBusCycle () override;
	};
}

#endif
  
// End of the file
/*@}*/
